#include <cassert>
#include <algorithm>


#include <llmr/renderer/painter.hpp>
#include <llmr/util/std.hpp>

#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/renderer/point_bucket.hpp>
#include <llmr/renderer/text_bucket.hpp>
#include <llmr/renderer/raster_bucket.hpp>

#include <llmr/map/map.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/geometry/glyph_atlas.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/style/style.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/util/raster.hpp>
#include <llmr/util/string.hpp>
#include <llmr/util/timer.hpp>
#include <llmr/util/time.hpp>
#include <llmr/util/clip_ids.hpp>

using namespace llmr;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

Painter::Painter(Map &map)
    : map(map) {
}

bool Painter::needsAnimation() const {
    return frameHistory.needsAnimation(300);
}

void Painter::setup() {
    util::timer timer("painter setup");

    setupShaders();

    assert(pointShader);
    assert(plainShader);
    assert(outlineShader);
    assert(lineShader);
    assert(linejoinShader);
    assert(patternShader);
    assert(rasterShader);
    assert(textShader);
    assert(dotShader);


    // Blending
    // We are blending new pixels on top of old pixels. Since we have depth testing
    // and are drawing opaque fragments first front-to-back, then translucent
    // fragments back-to-front, this shades the fewest fragments possible.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Set clear values
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClearStencil(0x0);

    // Stencil test
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void Painter::setupShaders() {
    plainShader = std::make_unique<PlainShader>();
    outlineShader = std::make_unique<OutlineShader>();
    lineShader = std::make_unique<LineShader>();
    linejoinShader = std::make_unique<LinejoinShader>();
    patternShader = std::make_unique<PatternShader>();
    pointShader = std::make_unique<PointShader>();
    rasterShader = std::make_unique<RasterShader>();
    textShader = std::make_unique<TextShader>();
    dotShader = std::make_unique<DotShader>();
}

void Painter::resize() {
    const TransformState &state = map.getState();
    if (gl_viewport != state.getFramebufferDimensions()) {
        gl_viewport = state.getFramebufferDimensions();
        assert(gl_viewport[0] > 0 && gl_viewport[1] > 0);
        glViewport(0, 0, gl_viewport[0], gl_viewport[1]);
    }
}

void Painter::setDebug(bool enabled) {
    debug = enabled;
}

void Painter::useProgram(uint32_t program) {
    if (gl_program != program) {
        glUseProgram(program);
        gl_program = program;
    }
}

void Painter::lineWidth(float lineWidth) {
    if (gl_lineWidth != lineWidth) {
        glLineWidth(lineWidth);
        gl_lineWidth = lineWidth;
    }
}

void Painter::depthMask(bool value) {
    if (gl_depthMask != value) {
        glDepthMask(value ? GL_TRUE : GL_FALSE);
        gl_depthMask = value;
    }
}

void Painter::changeMatrix() {
    // Initialize projection matrix
    matrix::ortho(projMatrix, 0, map.getState().getWidth(), map.getState().getHeight(), 0, 0, 1);

    // The extrusion matrix.
    matrix::identity(extrudeMatrix);
    matrix::multiply(extrudeMatrix, projMatrix, extrudeMatrix);
    matrix::rotate_z(extrudeMatrix, extrudeMatrix, map.getState().getAngle());

    // The native matrix is a 1:1 matrix that paints the coordinates at the
    // same screen position as the vertex specifies.
    matrix::identity(nativeMatrix);
    matrix::multiply(nativeMatrix, projMatrix, nativeMatrix);
}

void Painter::prepareClippingMask() {
    gl::start_group("clipping masks");
    useProgram(plainShader->program);
    glDisable(GL_DEPTH_TEST);
    depthMask(false);
    glDepthRange(1.0f, 1.0f);

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
}

void Painter::drawClippingMask(const mat4& matrix, const ClipID &clip) {
    plainShader->setMatrix(matrix);

    GLint id = clip.mask.to_ulong();
    GLuint mask = clipMask[clip.length];
    glStencilFunc(GL_ALWAYS, id, mask);
    glStencilMask(0xFF);

    Color background = map.getStyle().computed.background.color;
    background[0] *= map.getStyle().computed.background.opacity;
    background[1] *= map.getStyle().computed.background.opacity;
    background[2] *= map.getStyle().computed.background.opacity;
    background[3] *= map.getStyle().computed.background.opacity;

    plainShader->setColor(background);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}

void Painter::finishClippingMask() {
    glEnable(GL_DEPTH_TEST);
    depthMask(true);
    glStencilMask(0x0);
    gl::end_group();
}

void Painter::clear() {
    gl::group group("clear");
    glStencilMask(0xFF);
    depthMask(true);
#ifdef NVIDIA
    // We're painting in a way that allows us to skip clearing the color buffer.
    // On Tegra hardware, this is faster.
    glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
}

void Painter::startOpaquePass() {
    gl::start_group("opaque pass");
    pass = Opaque;
    glDisable(GL_BLEND);
    depthMask(true);
}

void Painter::startTranslucentPass() {
    gl::start_group("translucent pass");
    pass = Translucent;
    glEnable(GL_BLEND);
    depthMask(false);
}

void Painter::endPass() {
    gl::end_group();
}

void Painter::setStrata(float value) {
    strata = value;
}

void Painter::prepareTile(const Tile& tile) {
    matrix = tile.matrix;

    GLint id = (GLint)tile.clip.mask.to_ulong();
    GLuint mask = clipMask[tile.clip.length];
    glStencilFunc(GL_EQUAL, id, mask);
}

void Painter::renderTileLayer(const Tile& tile, const LayerDescription &layer_desc) {
    assert(tile.data);
    if (tile.data->hasData(layer_desc)) {
        gl::group group(util::sprintf<32>("render %d/%d/%d", tile.id.z, tile.id.y, tile.id.z));
        prepareTile(tile);
        tile.data->render(*this, layer_desc);
        frameHistory.record(map.getAnimationTime(), map.getState().getNormalizedZoom());
    }
}

void Painter::renderTileDebug(const Tile& tile) {
    gl::group group(util::sprintf<32>("debug %d/%d/%d", tile.id.z, tile.id.y, tile.id.z));
    assert(tile.data);
    if (debug) {
        prepareTile(tile);
        renderDebugText(tile.data->debugBucket);
        renderDebugFrame();
    }
}

void Painter::translateLayer(std::array<float, 2> translation, bool reverse) {
    if (translation[0] || translation[1]) {
        if (reverse) {
            translation[0] *= -1;
            translation[1] *= -1;
        }
        matrix::translate(matrix,
                          matrix,
                          translation[0] / map.getState().getPixelRatio(),
                          translation[1] / map.getState().getPixelRatio(),
                          0);
    }
}

void Painter::renderRaster(RasterBucket& bucket, const std::string& layer_name, const Tile::ID& /*id*/) {
    if (pass == Translucent) return;

    auto raster_properties = map.getStyle().computed.rasters;
    auto raster_properties_it = raster_properties.find(layer_name);
    if (raster_properties_it == raster_properties.end()) return;

    const RasterProperties& properties = raster_properties_it->second;
    if (!properties.enabled) return;

    depthMask(false);

    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
    rasterShader->setOpacity(1);
    // rasterShader->setOpacity(properties.opacity * tile_data->raster->opacity);

    glDepthRange(strata + strata_epsilon, 1.0f);
    bucket.drawRaster(*rasterShader, tileStencilBuffer, coveringRasterArray);

    depthMask(true);
}

void Painter::renderFill(FillBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (!bucket.hasData()) return;

    auto fill_properties = map.getStyle().computed.fills;
    auto fill_properties_it = fill_properties.find(layer_name);
    if (fill_properties_it == fill_properties.end()) return;

    const FillProperties& properties = fill_properties_it->second;
    if (!properties.enabled) return;

    Color fill_color = properties.fill_color;
    fill_color[0] *= properties.opacity;
    fill_color[1] *= properties.opacity;
    fill_color[2] *= properties.opacity;
    fill_color[3] *= properties.opacity;

    Color stroke_color = properties.stroke_color;
    stroke_color[0] *= properties.opacity;
    stroke_color[1] *= properties.opacity;
    stroke_color[2] *= properties.opacity;
    stroke_color[3] *= properties.opacity;

    bool outline = properties.antialias && properties.stroke_color != properties.fill_color;
    bool fringeline = properties.antialias && properties.stroke_color == properties.fill_color;
    if (fringeline) {
        outline = true;
        stroke_color = fill_color;
    }

    translateLayer(properties.translate);

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (outline && pass == Translucent) {
        useProgram(outlineShader->program);
        outlineShader->setMatrix(matrix);
        lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->setColor(stroke_color);

        // Draw the entire line
        outlineShader->setWorld({{
            static_cast<float>(map.getState().getFramebufferWidth()),
            static_cast<float>(map.getState().getFramebufferHeight())
        }});
        glDepthRange(strata, 1.0f);
        bucket.drawVertices(*outlineShader);
    } else if (fringeline) {
        // // We're only drawing to the first seven bits (== support a maximum of
        // // 127 overlapping polygons in one place before we get rendering errors).
        // glStencilMask(0x3F);
        // glClear(GL_STENCIL_BUFFER_BIT);

        // // Draw front facing triangles. Wherever the 0x80 bit is 1, we are
        // // increasing the lower 7 bits by one if the triangle is a front-facing
        // // triangle. This means that all visible polygons should be in CCW
        // // orientation, while all holes (see below) are in CW orientation.
        // glStencilFunc(GL_EQUAL, 0x80, 0x80);

        // // When we do a nonzero fill, we count the number of times a pixel is
        // // covered by a counterclockwise polygon, and subtract the number of
        // // times it is "uncovered" by a clockwise polygon.
        // glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    }

    if ((fill_color[3] >= 1.0f) == (pass == Opaque)) {
        auto &sprite = map.getStyle().sprite;
        if (properties.image.size() && sprite && sprite->isLoaded()) {
            // Draw texture fill
            ImagePosition imagePos = sprite->getPosition(properties.image, true);

            float factor = 8.0 / pow(2, map.getState().getIntegerZoom() - id.z);
            float mix = fmod(map.getState().getZoom(), 1.0);

            std::array<float, 2> imageSize = {{
                    imagePos.size.x * factor,
                    imagePos.size.y *factor
                }
            };

            std::array<float, 2> offset = {{
                    (float)fmod(id.x * 4096, imageSize[0]),
                    (float)fmod(id.y * 4096, imageSize[1])
                }
            };

            useProgram(patternShader->program);
            patternShader->setMatrix(matrix);
            patternShader->setOffset(offset);
            patternShader->setPatternSize(imageSize);
            patternShader->setPatternTopLeft({{ imagePos.tl.x, imagePos.tl.y }});
            patternShader->setPatternBottomRight({{ imagePos.br.x, imagePos.br.y }});
            patternShader->setColor(fill_color);
            patternShader->setMix(mix);
            sprite->raster.bind(true);

            // Draw the actual triangles into the color & stencil buffer.
            glDepthRange(strata + strata_epsilon, 1.0f);
            bucket.drawElements(*patternShader);
        } else {
            // Only draw the fill when it's either opaque and we're drawing opaque
            // fragments or when it's translucent and we're drawing translucent
            // fragments
            // Draw filling rectangle.
            useProgram(plainShader->program);
            plainShader->setMatrix(matrix);
            plainShader->setColor(fill_color);

            // Draw the actual triangles into the color & stencil buffer.
            glDepthRange(strata + strata_epsilon, 1.0f);
            bucket.drawElements(*plainShader);
        }
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline && pass == Translucent) {
        useProgram(outlineShader->program);
        outlineShader->setMatrix(matrix);
        lineWidth(2.0f); // This is always fixed and does not depend on the pixelRatio!

        outlineShader->setColor(fill_color);

        // Draw the entire line
        outlineShader->setWorld({{
            static_cast<float>(map.getState().getFramebufferWidth()),
            static_cast<float>(map.getState().getFramebufferHeight())
        }});

        glDepthRange(strata + strata_epsilon, 1.0f);
        bucket.drawVertices(*outlineShader);
    }

    translateLayer(properties.translate, true);
}

void Painter::renderLine(LineBucket& bucket, const std::string& layer_name, const Tile::ID& /*id*/) {
    // Abort early.
    if (pass == Opaque) return;
    if (!bucket.hasData()) return;

    auto line_properties = map.getStyle().computed.lines;
    auto line_properties_it = line_properties.find(layer_name);
    if (line_properties_it == line_properties.end()) return;

    const LineProperties& properties = line_properties_it->second;
    if (!properties.enabled) return;

    float width = properties.width;
    float offset = properties.offset / 2;

    // These are the radii of the line. We are limiting it to 16, which will result
    // in a point size of 64 on retina.
    float inset = fmin((fmax(-1, offset - width / 2 - 0.5) + 1), 16.0f);
    float outset = fmin(offset + width / 2 + 0.5, 16.0f);

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    float dash_length = properties.dash_array[0];
    float dash_gap = properties.dash_array[1];

    translateLayer(properties.translate);

    glDepthRange(strata, 1.0f);

    // We're only drawing end caps + round line joins if the line is > 2px. Otherwise, they aren't visible anyway.
    if (bucket.hasPoints() && outset > 1.0f) {
        useProgram(linejoinShader->program);
        linejoinShader->setMatrix(matrix);
        linejoinShader->setColor(color);
        linejoinShader->setWorld({{
                map.getState().getFramebufferWidth() * 0.5f,
                map.getState().getFramebufferHeight() * 0.5f
            }
        });
        linejoinShader->setLineWidth({{
                ((outset - 0.25f) * map.getState().getPixelRatio()),
                ((inset - 0.25f) * map.getState().getPixelRatio())
            }
        });

        float pointSize = ceil(map.getState().getPixelRatio() * outset * 2.0);
#if defined(GL_ES_VERSION_2_0)
        linejoinShader->setSize(pointSize);
#else
        glPointSize(pointSize);
#endif
        bucket.drawPoints(*linejoinShader);
    }

    // var imagePos = properties.image && imageSprite.getPosition(properties.image);
    bool imagePos = false;
    if (imagePos) {
        // var factor = 8 / Math.pow(2, painter.transform.zoom - params.z);

        // imageSprite.bind(gl, true);

        // //factor = Math.pow(2, 4 - painter.transform.zoom + params.z);
        // gl.switchShader(painter.linepatternShader, painter.translatedMatrix || painter.posMatrix, painter.extrudeMatrix);
        // shader = painter.linepatternShader;
        // glUniform2fv(painter.linepatternShader.u_pattern_size, [imagePos.size[0] * factor, imagePos.size[1] ]);
        // glUniform2fv(painter.linepatternShader.u_pattern_tl, imagePos.tl);
        // glUniform2fv(painter.linepatternShader.u_pattern_br, imagePos.br);
        // glUniform1f(painter.linepatternShader.u_fade, painter.transform.z % 1.0);

    } else {
        useProgram(lineShader->program);
        lineShader->setMatrix(matrix);
        lineShader->setExtrudeMatrix(extrudeMatrix);
        lineShader->setDashArray({{ dash_length, dash_gap }});
        lineShader->setLineWidth({{ outset, inset }});
        lineShader->setRatio(map.getState().getPixelRatio());
        lineShader->setColor(color);
        bucket.drawLines(*lineShader);
    }

    translateLayer(properties.translate, true);
}

void Painter::renderPoint(PointBucket& bucket, const std::string& layer_name, const Tile::ID& /*id&*/) {
    // Abort early.
    if (!bucket.hasData()) return;
    if (pass == Opaque) return;

    auto point_properties = map.getStyle().computed.points;
    auto point_properties_it = point_properties.find(layer_name);
    if (point_properties_it == point_properties.end()) return;

    const PointProperties& properties = point_properties_it->second;
    if (!properties.enabled) return;

    translateLayer(properties.translate);

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    auto &sprite = map.getStyle().sprite;
    ImagePosition imagePos;

    if (properties.image.length() && sprite && sprite->isLoaded()) {
        std::string sized_image = properties.image;
        if (properties.size) {
            sized_image.append("-");
            sized_image.append(std::to_string(static_cast<int>(std::round(properties.size))));
        }
        imagePos = sprite->getPosition(sized_image, false);
    }

    if (!imagePos.size) {
        useProgram(dotShader->program);
        dotShader->setMatrix(matrix);
        dotShader->setColor(color);

        const float pointSize = (properties.radius ? properties.radius * 2 : 8) * map.getState().getPixelRatio();
#if defined(GL_ES_VERSION_2_0)
            dotShader->setSize(pointSize);
#else
            glPointSize(pointSize);
            glEnable(GL_POINT_SPRITE);
#endif
        dotShader->setBlur((properties.blur ? properties.blur : 1.5) / pointSize);

        glDepthRange(strata, 1.0f);
        bucket.drawPoints(*dotShader);
    } else {
        useProgram(pointShader->program);
        pointShader->setMatrix(matrix);
        pointShader->setColor(color);

        pointShader->setImage(0);
        pointShader->setPointTopLeft({{ imagePos.tl.x, imagePos.tl.y }});
        pointShader->setPointBottomRight({{ imagePos.br.x, imagePos.br.y }});

        sprite->raster.bind(map.getState().isChanging());

        const float pointSize = (properties.size ? properties.size : (imagePos.size.x / map.getState().getPixelRatio())) * 1.4142135623730951 * map.getState().getPixelRatio();
#if defined(GL_ES_VERSION_2_0)
            pointShader->setSize(pointSize);
#else
            glPointSize(pointSize);
            glEnable(GL_POINT_SPRITE);
#endif

        glDepthRange(strata, 1.0f);
        bucket.drawPoints(*pointShader);
    }

    translateLayer(properties.translate, true);
}

void Painter::renderText(TextBucket& bucket, const std::string& layer_name, const Tile::ID& /*id*/) {
    // Abort early.
    if (pass == Opaque) return;
    if (!bucket.hasData()) return;

    auto text_properties = map.getStyle().computed.texts;
    auto text_properties_it = text_properties.find(layer_name);
    if (text_properties_it == text_properties.end()) return;

    const TextProperties& properties = text_properties_it->second;
    if (!properties.enabled) return;

    translateLayer(properties.translate);

    mat4 exMatrix;
    matrix::copy(exMatrix, projMatrix);
    if (bucket.geom_desc.path == TextPathType::Curve) {
        matrix::rotate_z(exMatrix, exMatrix, map.getState().getAngle());
    }

    const float rotate = properties.rotate;
    if (rotate != 0.0f) {
        matrix::rotate_z(exMatrix, exMatrix, rotate);
    }

    // If layerStyle.size > bucket.info.fontSize then labels may collide
    float fontSize = fmin(properties.size, bucket.geom_desc.font_size);
    matrix::scale(exMatrix, exMatrix, fontSize / 24.0f, fontSize / 24.0f, 1.0f);

    useProgram(textShader->program);
    textShader->setMatrix(matrix);
    textShader->setExtrudeMatrix(exMatrix);

    map.getGlyphAtlas().bind();
    textShader->setTextureSize({{static_cast<float>(map.getGlyphAtlas().width),
                                 static_cast<float>(map.getGlyphAtlas().height)}});

    textShader->setGamma(2.5f / fontSize / map.getState().getPixelRatio());

    // Convert the -pi..pi to an int8 range.
    float angle = round((map.getState().getAngle() + rotate) / M_PI * 128);

    // adjust min/max zooms for variable font sies
    float zoomAdjust = log(fontSize / bucket.geom_desc.font_size) / log(2);

    textShader->setAngle((int32_t)(angle + 256) % 256);
    textShader->setFlip(bucket.geom_desc.path == TextPathType::Curve ? 1 : 0);
    textShader->setZoom((map.getState().getNormalizedZoom() - zoomAdjust) * 10); // current zoom level

    // Label fading
    const time duration = 300_milliseconds;
    const time currentTime = util::now();

    std::deque<FrameSnapshot> &history = frameHistory.history;

    // Remove frames until only one is outside the duration, or until there are only three
    while (history.size() > 3 && history[1].timestamp + duration < currentTime) {
        history.pop_front();
    }

    if (history[1].timestamp + duration < currentTime) {
        history[0].z = history[1].z;
    }

    size_t frameLen = history.size();
    assert("there should never be less than three frames in the history" && frameLen >= 3);

    // Find the range of zoom levels we want to fade between
    float startingZ = history.front().z;
    const FrameSnapshot lastFrame = history.back();
    float endingZ = lastFrame.z;
    float lowZ = fmin(startingZ, endingZ);
    float highZ = fmax(startingZ, endingZ);

    // Calculate the speed of zooming, and how far it would zoom in terms of zoom levels in one duration
    float zoomDiff = endingZ - history[1].z,
        timeDiff = lastFrame.timestamp - history[1].timestamp;
    if (timeDiff > duration) timeDiff = 1;
    float fadedist = zoomDiff / (timeDiff / duration);

    if (isnan(fadedist)) fprintf(stderr, "fadedist should never be NaN\n");

    // At end of a zoom when the zoom stops changing continue pretending to zoom at that speed
    // bump is how much farther it would have been if it had continued zooming at the same rate
    float bump = (currentTime - lastFrame.timestamp) / duration * fadedist;

    textShader->setFadeDist(fadedist * 10);
    textShader->setMinFadeZoom(floor(lowZ * 10));
    textShader->setMaxFadeZoom(floor(highZ * 10));
    textShader->setFadeZoom((map.getState().getZoom() + bump) * 10);

    // We're drawing in the translucent pass which is bottom-to-top, so we need
    // to draw the halo first.
    if (properties.halo[3] > 0.0f) {
        textShader->setColor(properties.halo);
        textShader->setBuffer(properties.haloRadius);
        glDepthRange(strata, 1.0f);
        bucket.drawGlyphs(*textShader);
    }

    // Then, we draw the text over the halo
    textShader->setColor(properties.color);
    textShader->setBuffer((256.0f - 64.0f) / 256.0f);
    glDepthRange(strata + strata_epsilon, 1.0f);
    bucket.drawGlyphs(*textShader);

    translateLayer(properties.translate, true);
}

void Painter::renderDebugText(DebugBucket& bucket) {
    gl::group group("debug text");

    glDisable(GL_DEPTH_TEST);

    useProgram(plainShader->program);
    plainShader->setMatrix(matrix);

    // Draw white outline
    plainShader->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    lineWidth(4.0f * map.getState().getPixelRatio());
    bucket.drawLines(*plainShader);

#ifndef GL_ES_VERSION_2_0
    // Draw line "end caps"
    glPointSize(2);
    bucket.drawPoints(*plainShader);
#endif

    // Draw black text.
    plainShader->setColor(0.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(2.0f * map.getState().getPixelRatio());
    bucket.drawLines(*plainShader);

    glEnable(GL_DEPTH_TEST);
}

void Painter::renderDebugFrame() {
    gl::group group("debug frame");

    // Disable depth test and don't count this towards the depth buffer,
    // but *don't* disable stencil test, as we want to clip the red tile border
    // to the tile viewport.
    glDisable(GL_DEPTH_TEST);

    useProgram(plainShader->program);
    plainShader->setMatrix(matrix);

    // draw tile outline
    tileBorderArray.bind(*plainShader, tileBorderBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(1.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(4.0f * map.getState().getPixelRatio());
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)tileBorderBuffer.index());

    glEnable(GL_DEPTH_TEST);
}

void Painter::renderMatte() {
    gl::group group("matte");
    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_EQUAL, 0x0, 0xFF);

    // Color white = {{ 0.9, 0.9, 0.9, 1 }};
    Color white = {{ 1, 1, 0, 1 }};

    useProgram(plainShader->program);
    plainShader->setMatrix(nativeMatrix);

    // Draw the clipping mask
    matteArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(white);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());

    glEnable(GL_DEPTH_TEST);
}


void Painter::renderDebugText(const std::vector<std::string> &strings) {
    gl::group group("debug text");

    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF);

    useProgram(plainShader->program);
    plainShader->setMatrix(nativeMatrix);

    DebugFontBuffer debugFontBuffer;
    int line = 25;
    for (const std::string &str : strings) {
        debugFontBuffer.addText(str.c_str(), 10, line, 0.75);
        line += 20;
    }

    // draw debug info
    VertexArrayObject debugFontArray;
    debugFontArray.bind(*plainShader, debugFontBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    lineWidth(4.0f * map.getState().getPixelRatio());
    glDrawArrays(GL_LINES, 0, (GLsizei)debugFontBuffer.index());
#ifndef GL_ES_VERSION_2_0
    glPointSize(2);
    glDrawArrays(GL_POINTS, 0, (GLsizei)debugFontBuffer.index());
#endif
    plainShader->setColor(0.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(2.0f * map.getState().getPixelRatio());
    glDrawArrays(GL_LINES, 0, (GLsizei)debugFontBuffer.index());


    glEnable(GL_DEPTH_TEST);
}