#ifndef LLMR_STYLE_SPRITE
#define LLMR_STYLE_SPRITE

#include <map>
#include <string>
#include <mutex>
#include <memory>
#include <atomic>

#include <llmr/util/raster.hpp>
#include <llmr/util/vec.hpp>

namespace llmr {

class Map;

class SpritePosition {
public:
    explicit SpritePosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio = 1);

    uint16_t x = 0, y = 0;
    uint16_t width = 0, height = 0;
    uint8_t pixelRatio = 1;
};

class ImagePosition {
public:
    explicit ImagePosition() {}
    explicit ImagePosition(const vec2<uint16_t>& size, vec2<float> tl, vec2<float> br);

    vec2<uint16_t> size = { 0, 0 };
    vec2<float> tl = { 0, 0 };
    vec2<float> br = { 0, 0 };
};


class Sprite : public std::enable_shared_from_this<Sprite> {
public:
    Sprite(Map &map, float pixelRatio = 1);

    void load(const std::string& base_url);

    ImagePosition getPosition(const std::string& name, bool repeating = false);

    bool isLoaded() const;

public:
    const float pixelRatio;
    Raster raster;

private:
    void asyncParseJSON();
    void asyncParseImage();

    static void parseJSON(std::shared_ptr<Sprite> &sprite);
    static void parseImage(std::shared_ptr<Sprite> &sprite);
    static void complete(std::shared_ptr<Sprite> &sprite);

private:
    Map &map;
    std::string body;
    std::string image;
    std::atomic<bool> loaded;
    std::map<std::string, SpritePosition> pos;
};

}

#endif
