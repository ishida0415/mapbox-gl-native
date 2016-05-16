#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>

#include <mapbox/geometry/envelope.hpp>

#include <cassert>
#include <string>

namespace mbgl {

FeatureIndex::FeatureIndex()
    : grid(util::EXTENT, 16, 0) {
}

void FeatureIndex::insert(const GeometryCollection& geometries,
                          std::size_t index,
                          const std::string& sourceLayerName,
                          const std::string& bucketName) {
    for (const auto& ring : geometries) {
        grid.insert(IndexedSubfeature { index, sourceLayerName, bucketName, sortIndex++ },
                    mapbox::geometry::envelope(ring));
    }
}

static bool vectorContains(const std::vector<std::string>& vector, const std::string& s) {
    return std::find(vector.begin(), vector.end(), s) != vector.end();
}

static bool vectorsIntersect(const std::vector<std::string>& vectorA, const std::vector<std::string>& vectorB) {
    for (const auto& a : vectorA) {
        if (vectorContains(vectorB, a)) {
            return true;
        }
    }
    return false;
}

static bool topDown(const IndexedSubfeature& a, const IndexedSubfeature& b) {
    return a.sortIndex > b.sortIndex;
}

static bool topDownSymbols(const IndexedSubfeature& a, const IndexedSubfeature& b) {
    return a.sortIndex < b.sortIndex;
}

void FeatureIndex::query(
        std::unordered_map<std::string, std::vector<Feature>>& result,
        const GeometryCollection& queryGeometry,
        const float bearing,
        const double tileSize,
        const double scale,
        const optional<std::vector<std::string>>& filterLayerIDs,
        const GeometryTile& geometryTile,
        const Style& style) const {

    mapbox::geometry::box<int16_t> box = mapbox::geometry::envelope(queryGeometry);

    const float pixelsToTileUnits = util::EXTENT / tileSize / scale;
    const int16_t additionalRadius = std::min<int16_t>(util::EXTENT, std::ceil(style.getQueryRadius() * pixelsToTileUnits));
    std::vector<IndexedSubfeature> features = grid.query({ box.min - additionalRadius, box.max + additionalRadius });

    std::sort(features.begin(), features.end(), topDown);
    size_t previousSortIndex = std::numeric_limits<size_t>::max();
    for (const auto& indexedFeature : features) {

        // If this feature is the same as the previous feature, skip it.
        if (indexedFeature.sortIndex == previousSortIndex) continue;
        previousSortIndex = indexedFeature.sortIndex;

        addFeature(result, indexedFeature, queryGeometry, filterLayerIDs, geometryTile, style, bearing, pixelsToTileUnits);
    }

    // query symbol features
    assert(collisionTile);
    std::vector<IndexedSubfeature> symbolFeatures = collisionTile->queryRenderedSymbols(box, scale);
    std::sort(symbolFeatures.begin(), symbolFeatures.end(), topDownSymbols);
    for (const auto& symbolFeature : symbolFeatures) {
        addFeature(result, symbolFeature, queryGeometry, filterLayerIDs, geometryTile, style, bearing, pixelsToTileUnits);
    }
}

void FeatureIndex::addFeature(
    std::unordered_map<std::string, std::vector<Feature>>& result,
    const IndexedSubfeature& indexedFeature,
    const GeometryCollection& queryGeometry,
    const optional<std::vector<std::string>>& filterLayerIDs,
    const GeometryTile& geometryTile,
    const Style& style,
    const float bearing,
    const float pixelsToTileUnits) const {

    auto& layerIDs = bucketLayerIDs.at(indexedFeature.bucketName);
    if (filterLayerIDs && !vectorsIntersect(layerIDs, *filterLayerIDs)) {
        return;
    }

    auto sourceLayer = geometryTile.getLayer(indexedFeature.sourceLayerName);
    assert(sourceLayer);

    auto geometryTileFeature = sourceLayer->getFeature(indexedFeature.index);
    assert(geometryTileFeature);

    for (const auto& layerID : layerIDs) {
        if (filterLayerIDs && !vectorContains(*filterLayerIDs, layerID)) {
            continue;
        }

        auto styleLayer = style.getLayer(layerID);
        if (!styleLayer) {
            continue;
        }

        if (!styleLayer->is<SymbolLayer>()) {
            auto geometries = geometryTileFeature->getGeometries();
            if (!styleLayer->queryIntersectsGeometry(queryGeometry, geometries, bearing, pixelsToTileUnits)) continue;
        }

        Feature feature { mapbox::geometry::point<double>() };
        feature.properties = geometryTileFeature->getProperties();
        feature.id = geometryTileFeature->getID();

        result[layerID].push_back(std::move(feature));
    }
}

optional<GeometryCollection> FeatureIndex::translateQueryGeometry(
        const GeometryCollection& queryGeometry,
        const std::array<float, 2>& translate,
        const TranslateAnchorType anchorType,
        const float bearing,
        const float pixelsToTileUnits) {
    if (translate[0] == 0 && translate[1] == 0) {
        return {};
    }

    GeometryCoordinate translateVec(translate[0] * pixelsToTileUnits, translate[1] * pixelsToTileUnits);
    if (anchorType == TranslateAnchorType::Viewport) {
        translateVec = util::rotate(translateVec, -bearing);
    }

    GeometryCollection translated;
    for (const auto& ring : queryGeometry) {
        translated.emplace_back();
        auto& translatedRing = translated.back();
        for (const auto& p : ring) {
            translatedRing.push_back(p - translateVec);
        }
    }
    return translated;
}

void FeatureIndex::addBucketLayerName(const std::string& bucketName, const std::string& layerID) {
    bucketLayerIDs[bucketName].push_back(layerID);
}

void FeatureIndex::setCollisionTile(std::unique_ptr<CollisionTile> collisionTile_) {
    collisionTile = std::move(collisionTile_);
}

} // namespace mbgl
