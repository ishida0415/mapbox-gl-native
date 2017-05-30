#include <mbgl/renderer/sources/render_geojson_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/geojson_tile.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

namespace mbgl {

using namespace style;

RenderGeoJSONSource::RenderGeoJSONSource(Immutable<style::GeoJSONSource::Impl> impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

const style::GeoJSONSource::Impl& RenderGeoJSONSource::impl() const {
    return static_cast<const style::GeoJSONSource::Impl&>(*baseImpl);
}

bool RenderGeoJSONSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderGeoJSONSource::update(Immutable<style::Source::Impl> baseImpl_,
                                 const std::vector<Immutable<Layer::Impl>>& layers,
                                 const bool needsRendering,
                                 const bool needsRelayout,
                                 const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    GeoJSONData* data_ = impl().getData();

    if (!data_) {
        return;
    }

    if (data_ != data) {
        data = data_;
        tilePyramid.cache.clear();

        for (auto const& item : tilePyramid.tiles) {
            static_cast<GeoJSONTile*>(item.second.get())->updateData(data->getTile(item.first.canonical));
        }
    }

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::GeoJSON,
                       util::tileSize,
                       impl().getZoomRange(),
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<GeoJSONTile>(tileID, impl().id, parameters, data->getTile(tileID.canonical));
                       });
}

void RenderGeoJSONSource::startRender(algorithm::ClipIDGenerator& generator, const mat4& projMatrix, const mat4& clipMatrix, const TransformState& transform) {
    generator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(projMatrix, clipMatrix, transform);
}

void RenderGeoJSONSource::finishRender(Painter& painter) {
    tilePyramid.finishRender(painter);
}

std::map<UnwrappedTileID, RenderTile>& RenderGeoJSONSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderGeoJSONSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                           const TransformState& transformState,
                                           const style::Style& style,
                                           const RenderedQueryOptions& options) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, style, options);
}

std::vector<Feature> RenderGeoJSONSource::querySourceFeatures(const SourceQueryOptions& options) const {
    return tilePyramid.querySourceFeatures(options);
}

void RenderGeoJSONSource::setCacheSize(size_t size) {
    tilePyramid.setCacheSize(size);
}

void RenderGeoJSONSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderGeoJSONSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
