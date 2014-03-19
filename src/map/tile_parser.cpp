#include <llmr/map/tile_parser.hpp>

#include <llmr/style/style.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/renderer/point_bucket.hpp>
#include <llmr/renderer/text_bucket.hpp>

using namespace llmr;


TileParser::TileParser(const std::string& data, TileData& tile, const Style& style, GlyphAtlas& glyphAtlas)
    : data(pbf((const uint8_t *)data.data(), data.size())),
      tile(tile),
      style(style),
      glyphAtlas(glyphAtlas),
      placement(tile.id.z) {
    parseGlyphs();
    parseStyleLayers(style.layers);
}

bool TileParser::obsolete() const {
    return tile.state == TileData::State::obsolete;
}

void TileParser::parseGlyphs() {
    for (const std::pair<std::string, const VectorTileFace> pair : data.faces) {
        const std::string &name = pair.first;
        const VectorTileFace &face = pair.second;

        GlyphPositions &glyphs = faces[name];
        for (const VectorTileGlyph &glyph : face.glyphs) {
            const Rect<uint16_t> rect =
                glyphAtlas.addGlyph((uint64_t)tile.id, name, glyph);
            glyphs.emplace_back(Glyph{rect, glyph.metrics});
        }
    }
}

void TileParser::parseStyleLayers(const std::vector<LayerDescription>& layers) {
    for (const LayerDescription& layer_desc : layers) {
        // Cancel early when parsing.
        if (obsolete()) {
            return;
        }

        if (layer_desc.child_layer.size()) {
            // This is a layer group.
            // TODO: create framebuffer
            parseStyleLayers(layer_desc.child_layer);
            // TODO: render framebuffer on previous framebuffer
        } else {
            // This is a singular layer. Check if this bucket already exists. If not,
            // parse this bucket.
            auto bucket_it = tile.buckets.find(layer_desc.bucket_name);
            if (bucket_it == tile.buckets.end()) {
                auto bucket_it = style.buckets.find(layer_desc.bucket_name);
                if (bucket_it != style.buckets.end()) {
                    // Only create the new bucket if we have an actual specification
                    // for it.
                    std::shared_ptr<Bucket> bucket = createBucket(bucket_it->second);
                    if (bucket) {
                        // Bucket creation might fail because the data tile may not
                        // contain any data that falls into this bucket.
                        tile.buckets[layer_desc.bucket_name] = bucket;
                    }
                } else {
                    // There is no proper specification for this bucket, even though
                    // it is referenced in the stylesheet.
                    fprintf(stderr, "Stylesheet specifies bucket %s, but it is not defined\n", layer_desc.bucket_name.c_str());
                }
            }
        }
    }
}

std::shared_ptr<Bucket> TileParser::createBucket(const BucketDescription& bucket_desc) {
    auto layer_it = data.layers.find(bucket_desc.source_layer);
    if (layer_it != data.layers.end()) {
        const VectorTileLayer& layer = layer_it->second;
        if (bucket_desc.type == BucketType::Fill) {
            return createFillBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Line) {
            return createLineBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Point) {
            return createPointBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Text) {
            return createTextBucket(layer, bucket_desc);
        } else {
            throw std::runtime_error("unknown bucket type");
        }
    } else {
        // The layer specified in the bucket does not exist. Do nothing.
    }

    return nullptr;
}

template <class Bucket>
void TileParser::addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        if (obsolete()) return;

        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
            }
        }
    }
}

std::shared_ptr<Bucket> TileParser::createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<FillBucket> bucket = std::make_shared<FillBucket>(
        tile.fillVertexBuffer, tile.triangleElementsBuffer, tile.lineElementsBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return obsolete() ? nullptr : bucket;
}

std::shared_ptr<Bucket> TileParser::createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<LineBucket> bucket = std::make_shared<LineBucket>(
        tile.lineVertexBuffer, tile.triangleElementsBuffer, tile.pointElementsBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return obsolete() ? nullptr : bucket;
}

std::shared_ptr<Bucket> TileParser::createPointBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::shared_ptr<PointBucket> bucket = std::make_shared<PointBucket>(
        tile.pointVertexBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return obsolete() ? nullptr : bucket;
}

std::shared_ptr<Bucket> TileParser::createTextBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    // Determine the correct text stack.
    if (!layer.shaping.size()) {
        return nullptr;
    }

    // TODO: currently hardcoded to use the first font stack.
    const std::map<Value, Shaping>& shaping = layer.shaping.begin()->second;

    const FaceGlyphPositions& const_faces = faces;

    IndexedFaceGlyphPositions faces;
    for (const std::string& face : layer.faces) {
        auto it = const_faces.find(face);
        if (it == const_faces.end()) {
            // This layer references an unknown face.
            return nullptr;
        }
        faces.push_back(&it->second);
    }

    std::shared_ptr<TextBucket> bucket = std::make_shared<TextBucket>(
        tile.textVertexBuffer, tile.triangleElementsBuffer, bucket_desc, placement);

    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (const pbf& feature_pbf : filtered_layer) {
        if (obsolete()) return nullptr;
        bucket->addFeature({ feature_pbf, layer }, faces, shaping);
    }

    return bucket;
}
