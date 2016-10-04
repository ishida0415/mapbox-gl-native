#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

class RasterShader;
class StaticRasterVertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    RasterBucket(PremultipliedImage&&);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void drawRaster(RasterShader&, StaticRasterVertexBuffer&, VertexArrayObject&, gl::Context&);

private:
    PremultipliedImage image;
    optional<gl::Texture> texture;
};

} // namespace mbgl
