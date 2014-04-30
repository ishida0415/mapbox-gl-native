#include <llmr/map/tile_data.hpp>
#include <llmr/map/tile_parser.hpp>
#include <llmr/map/map.hpp>

#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/point_buffer.hpp>
#include <llmr/geometry/text_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/util/raster.hpp>
#include <llmr/util/string.hpp>

#include <llmr/util/uv.hpp>

using namespace llmr;

TileData::TileData(Tile::ID id, Map &map, const std::string url,  const bool is_raster)
    : id(id),
      state(State::initial),
      raster(),
      url(url),
      is_raster(is_raster),
      map(map) {
    // Initialize tile debug coordinates
    const std::string str = util::sprintf<32>("%d/%d/%d", id.z, id.x, id.y);
    debugFontBuffer.addText(str.c_str(), 50, 200, 5);
}

TileData::~TileData() {
    map.getGlyphAtlas().removeGlyphs((uint64_t)id);
    cancel();
}

const std::string TileData::toString() const {
    return util::sprintf<32>("[tile %d/%d/%d]", id.z, id.x, id.y);
}

void TileData::request() {
    state = State::loading;

    // Note: Somehow this feels slower than the change to request_http()
    std::weak_ptr<TileData> weak_tile = shared_from_this();
    req = platform::request_http(url, [weak_tile](platform::Response *res) {
        std::shared_ptr<TileData> tile = weak_tile.lock();
        if (!tile || tile->state == State::obsolete) {
            // noop. Tile is obsolete and we're now just waiting for the refcount
            // to drop to zero for destruction.
        } else if (res->code == 200) {
            tile->state = State::loaded;

            tile->data.swap(res->body);

            // Schedule tile parsing in another thread
            tile->scheduleParsing();
        } else {
            fprintf(stderr, "[%s] tile loading failed: %d, %s\n", tile->url.c_str(), res->code, res->error_message.c_str());
        }
    }, map.getLoop());
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
        platform::cancel_request_http(req.lock());
    }
}

void TileData::scheduleParsing() {
    // We're creating a new work request. The work request deletes itself after it executed
    // the after work handler
    new uv::work<std::shared_ptr<TileData>>(
        map.getLoop(),
        [](std::shared_ptr<TileData> &tile) { tile->parse(); },
        [](std::shared_ptr<TileData> &tile) { tile->map.update(); },
        shared_from_this());
}

void TileData::parse() {
    if (state != State::loaded) {
        return;
    }

    try {
        // Parsing creates state that is encapsulated in TileParser. While parsing,
        // the TileParser object writes results into this objects. All other state
        // is going to be discarded afterwards.
        TileParser parser(data, *this, map.getStyle(), map.getGlyphAtlas(), is_raster);
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return;
    }

    if (state != State::obsolete) {
        state = State::parsed;
    }
}
