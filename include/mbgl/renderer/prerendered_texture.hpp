#ifndef MBGL_RENDERER_PRERENDERED_TEXTURE
#define MBGL_RENDERER_PRERENDERED_TEXTURE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/style/rasterize_properties.hpp>
//#include <mbgl/renderer/raster_bucket.hpp>

namespace mbgl {

class Painter;

class PrerenderedTexture : private util::noncopyable {
public:
    PrerenderedTexture(const RasterBucket &properties);
    ~PrerenderedTexture();

    void bindTexture();
    void bindFramebuffer();
    void unbindFramebuffer();

    inline GLuint getTexture() const { return texture; }

    void blur(Painter& painter, uint16_t passes);

public:
    const RasterBucket properties;

private:
    GLint previous_fbo = 0;
    GLuint fbo = 0;
    GLuint texture = 0;
};

}

#endif
