#ifndef MBGL_MAP_MAP
#define MBGL_MAP_MAP

#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/util/ptr.hpp>
#include <cstdint>
#include <atomic>
#include <iosfwd>
#include <set>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace mbgl {

class GlyphStore;
class LayerDescription;
class Sprite;
class Style;
class StyleLayer;
class StyleLayerGroup;
class StyleSource;
class Texturepool;
class FileSource;
class View;

class Map : private util::noncopyable {
    typedef void (*stop_callback)(void *);

public:
    explicit Map(View &view);
    ~Map();

    // Start the map render thread. It is asynchronous.
    void start(bool start_paused = false);

    // Stop the map render thread. This call will block until the map rendering thread stopped.
    // The optional callback function will be invoked repeatedly until the map thread is stopped.
    // The callback function should wait until it is woken up again by view.notify(), otherwise
    // this will be a busy waiting loop. The optional data parameter will be passed to the callback
    // function.
    void stop(stop_callback cb = nullptr, void *data = nullptr);

    // Pauses the render thread. The render thread will stop running but will not be terminated and will not lose state until resumed.
    void pause(bool wait_for_pause = false);

    // Resumes a paused render thread
    void resume();

    // Runs the map event loop. ONLY run this function when you want to get render a single frame
    // with this map object. It will *not* spawn a separate thread and instead block until the
    // frame is completely rendered.
    void run();

    // Triggers a lazy rerender: only performs a render when the map is not clean.
    void rerender();

    // Forces a map update: always triggers a rerender.
    void update();

    // Triggers a cleanup that releases resources.
    void cleanup();

    // Releases resources immediately
    void terminate();

    // Controls buffer swapping.
    bool needsSwap();
    void swapped();

    // Size
    void resize(uint16_t width, uint16_t height, float ratio = 1);
    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fb_width, uint16_t fb_height);

    // Styling
    void setAppliedClasses(const std::vector<std::string> &classes);
    void toggleClass(const std::string &name);
    const std::vector<std::string> &getAppliedClasses() const;
    void setDefaultTransitionDuration(uint64_t duration_milliseconds = 0);
    uint64_t getDefaultTransitionDuration();
    void setStyleURL(const std::string &url);
    void setStyleJSON(std::string newStyleJSON, const std::string &base = "");
    std::string getStyleJSON() const;
    void setAccessToken(std::string access_token);
    std::string getAccessToken() const;

    // Transition
    void cancelTransitions();

    // Position
    void moveBy(double dx, double dy, double duration = 0);
    void setLonLat(double lon, double lat, double duration = 0);
    void getLonLat(double &lon, double &lat) const;
    void startPanning();
    void stopPanning();
    void resetPosition();

    // Scale
    void scaleBy(double ds, double cx = -1, double cy = -1, double duration = 0);
    void setScale(double scale, double cx = -1, double cy = -1, double duration = 0);
    double getScale() const;
    void setZoom(double zoom, double duration = 0);
    double getZoom() const;
    void setLonLatZoom(double lon, double lat, double zoom, double duration = 0);
    void getLonLatZoom(double &lon, double &lat, double &zoom) const;
    void resetZoom();
    void startScaling();
    void stopScaling();
    double getMinZoom() const;
    double getMaxZoom() const;

    // Rotation
    void rotateBy(double sx, double sy, double ex, double ey, double duration = 0);
    void setBearing(double degrees, double duration = 0);
    void setBearing(double degrees, double cx, double cy);
    double getBearing() const;
    void resetNorth();
    void startRotating();
    void stopRotating();

    // Debug
    void setDebug(bool value);
    void toggleDebug();
    bool getDebug() const;

    // Call this when the network reachability changed.
    void setReachability(bool status);

    inline const TransformState &getState() const { return state; }
    inline timestamp getTime() const { return animationTime; }

private:
    util::ptr<Sprite> getSprite();
    uv::worker& getWorker();

    // Checks if render thread needs to pause
    void check_for_pause();

    // Setup
    void setup();

    void updateTiles();
    void updateSources();
    void updateSources(const util::ptr<StyleLayerGroup> &group);

    // Prepares a map render by updating the tiles we need for the current view, as well as updating
    // the stylesheet.
    void prepare();

    // Unconditionally performs a render with the current map state.
    void render();

    bool async = false;
    std::unique_ptr<uv::loop> loop;
    std::unique_ptr<uv::worker> workers;
    std::unique_ptr<uv::thread> thread;
    std::unique_ptr<uv::async> async_terminate;
    std::unique_ptr<uv::async> async_render;
    std::unique_ptr<uv::async> async_cleanup;

    bool terminating = false;
    bool pausing = false;
    bool is_paused = false;
    std::mutex mutex_run;
    std::condition_variable cond_run;
    std::mutex mutex_pause;
    std::condition_variable cond_pause;

    // If cleared, the next time the render thread attempts to render the map, it will *actually*
    // render the map.
    std::atomic_flag is_clean = ATOMIC_FLAG_INIT;

    // If this flag is cleared, the current back buffer is ready for being swapped with the front
    // buffer (i.e. it has rendered data).
    std::atomic_flag is_swapped = ATOMIC_FLAG_INIT;

    // This is cleared once the current front buffer has been presented and the back buffer is
    // ready for rendering.
    std::atomic_flag is_rendered = ATOMIC_FLAG_INIT;

    // Stores whether the map thread has been stopped already.
    std::atomic_bool is_stopped;

    View &view;

#ifndef NDEBUG
    const unsigned long main_thread;
    unsigned long map_thread = -1;
#endif

    Transform transform;
    TransformState state;

    util::ptr<FileSource> fileSource;

    util::ptr<Style> style;
    GlyphAtlas glyphAtlas;
    util::ptr<GlyphStore> glyphStore;
    SpriteAtlas spriteAtlas;
    util::ptr<Sprite> sprite;
    util::ptr<Texturepool> texturepool;

    Painter painter;

    std::string styleURL;
    std::string styleJSON = "";
    std::string accessToken = "";

    std::atomic_uint_fast64_t defaultTransitionDuration;

    util::ptr<std::vector<std::string>> appliedClasses;
    std::mutex appliedClassesMutex;

    bool debug = false;
    timestamp animationTime = 0;

    std::set<util::ptr<StyleSource>> activeSources;
};

}

#endif
