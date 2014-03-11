#include <llmr/map/vector_tile.hpp>
#include <llmr/style/bucket_description.hpp>

#include <algorithm>
#include <iostream>

using namespace llmr;

VectorTile::VectorTile() {}


VectorTile::VectorTile(pbf tile) {
    while (tile.next()) {
        if (tile.tag == 3) { // layer
            VectorTileLayer layer(tile.message());
            layers.emplace(layer.name, std::forward<VectorTileLayer>(layer));
        } else if (tile.tag == 4) { // face
            VectorTileFace face(tile.message());
            faces.emplace(face.name, std::forward<VectorTileFace>(face));
        } else {
            tile.skip();
        }
    }
}

VectorTile& VectorTile::operator=(VectorTile && other) {
    if (this != &other) {
        layers.swap(other.layers);
    }
    return *this;
}

VectorTileFace::VectorTileFace(pbf face) {
    while (face.next()) {
        if (face.tag == 1) { // family
            family = face.string();
        } else if (face.tag == 2) { // style
            style = face.string();
        } else if (face.tag == 5) { // glyphs
            glyphs.emplace_back(face.message());
        } else {
            face.skip();
        }
    }
    name = family + " " + style;
}


std::ostream& llmr::operator<<(std::ostream& os, const VectorTileFace& face) {
    os << "Face: " << face.name << std::endl;
    for (const VectorTileGlyph& glyph : face.glyphs) {
        os << "  - " << glyph;
    }
    return os;
}


VectorTileGlyph::VectorTileGlyph(pbf glyph) {
    while (glyph.next()) {
        if (glyph.tag == 1) { // id
            id = glyph.varint();
        } else if (glyph.tag == 2) { // bitmap
            bitmap = glyph.string();
        } else if (glyph.tag == 3) { // width
            width = glyph.varint();
        } else if (glyph.tag == 4) { // height
            height = glyph.varint();
        } else if (glyph.tag == 5) { // left
            left = glyph.svarint();
        } else if (glyph.tag == 6) { // top
            top = glyph.svarint();
        } else if (glyph.tag == 7) { // advance
            advance = glyph.varint();
        } else {
            glyph.skip();
        }
    }
}

std::ostream& llmr::operator<<(std::ostream& os, const VectorTileGlyph& glyph) {
    os << "Glyph " << glyph.id << ": " << glyph.width << "x" << glyph.height <<
          " (" << glyph.left << "/" << glyph.top << ") +" << glyph.advance << std::endl;
    return os;
}

VectorTileLayer::VectorTileLayer(pbf layer) : data(layer) {
    while (layer.next()) {
        if (layer.tag == 1) { // name
            name = layer.string();
        } else if (layer.tag == 3) { // keys
            keys.emplace_back(layer.string());
        } else if (layer.tag == 4) { // values
            values.emplace_back(std::move(parseValue(layer.message())));
        } else if (layer.tag == 5) { // extent
            extent = layer.varint();
        } else if (layer.tag == 7) { // faces
            std::string face = layer.string();
        } else if (layer.tag == 8) { // labels
            layer.skip();
        } else if (layer.tag == 9) { // stacks
            std::string stack = layer.string();
        } else {
            layer.skip();
        }
    }
}

FilteredVectorTileLayer::FilteredVectorTileLayer(const VectorTileLayer& layer, const BucketDescription& bucket_desc)
    : layer(layer),
      bucket_desc(bucket_desc) {
    // If we filter further by field/value, parse the key/value indices first
    // because protobuf doesn't mandate a particular key order.
    if (bucket_desc.source_field.size()) {
        // Find out what key/value IDs we need.
        auto key_it = std::find(layer.keys.begin(), layer.keys.end(), bucket_desc.source_field);
        if (key_it != layer.keys.end()) {
            key = (int32_t)(key_it - layer.keys.begin());
        }

        for (const Value& source_value : bucket_desc.source_value) {
            auto value_it = std::find(layer.values.begin(), layer.values.end(), source_value);
            if (value_it != layer.values.end()) {
                values.insert((uint32_t)(value_it - layer.values.begin()));
            }
        }
    }
}

FilteredVectorTileLayer::iterator FilteredVectorTileLayer::begin() const {
    if (bucket_desc.source_field.size() && (key < 0 || values.empty())) {
        // There are no valid values that we could possibly find. Abort early.
        return end();
    } else {
        return iterator(*this, layer.data);
    }
}

FilteredVectorTileLayer::iterator FilteredVectorTileLayer::end() const {
    return iterator(*this, pbf(layer.data.end, 0));
}

FilteredVectorTileLayer::iterator::iterator(const FilteredVectorTileLayer& filter, const pbf& data)
    : filter(filter),
      feature(pbf()),
      data(data) {
    operator++();
}

void FilteredVectorTileLayer::iterator::operator++() {
    valid = false;

    while (data.next(2)) { // feature
        feature = data.message();
        pbf feature_pbf = feature;
        BucketType type = BucketType::None;

        bool skip = false;
        while (!skip && feature_pbf.next()) {
            if (feature_pbf.tag == 2) { // tags
                if (filter.key < 0) {
                    // We want to parse the entire layer anyway
                    feature_pbf.skip();
                } else {
                    // We only want to parse some features.
                    skip = true;

                    // tags are packed varints. They should have an even length.
                    pbf tags_pbf = feature_pbf.message();
                    while (tags_pbf) {
                        uint32_t tag_key = tags_pbf.varint();
                        if (tags_pbf) {
                            uint32_t tag_val = tags_pbf.varint();
                            // Now check if the tag_key/tag_val pair is included
                            // in the set of key/values that we are looking for.
                            if (filter.key == tag_key && filter.values.find(tag_val) != filter.values.end()) {
                                skip = false;
                                break; // feature_pbf loop
                            }
                        } else {
                            // This should not happen; otherwise the vector tile
                            // is invalid.
                            throw std::runtime_error("uneven number of feature tag ids");
                        }
                    }
                }
            } else if (feature_pbf.tag == 3) { // geometry type
                switch (feature_pbf.varint()) {
                    case 1 /* Point */:      type = BucketType::Point; break;
                    case 2 /* LineString */: type = BucketType::Line; break;
                    case 3 /* Polygon */:    type = BucketType::Fill; break;
                    default:                 type = BucketType::None; break;
                }

                if (type != filter.bucket_desc.type) {
                    break; // feature_pbf loop early
                }
            } else {
                feature_pbf.skip();
            }
        }

        if (!skip && type == filter.bucket_desc.type) {
            valid = true;
            return; // data loop
        } else {
            valid = false;
        }
    }
}

bool FilteredVectorTileLayer::iterator::operator!=(const iterator& other) const {
    return !(data.data == other.data.data && data.end == other.data.end && valid == other.valid);
}

const pbf& FilteredVectorTileLayer::iterator:: operator*() const {
    return feature;
}
