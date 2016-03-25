#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/map/map_data.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;

TEST(MapContext, DoubleStyleLoad) {
    util::RunLoop loop;

    std::shared_ptr<HeadlessDisplay> display = std::make_shared<HeadlessDisplay>();
    HeadlessView view(display, 1, 512, 512);
    StubFileSource fileSource;

    util::Thread<MapContext> context({"Map", util::ThreadType::Map, util::ThreadPriority::Regular},
        view, fileSource, MapMode::Continuous, GLContextMode::Unique, view.getPixelRatio());

    context.invokeSync(&MapContext::setStyleJSON, "", "");
    context.invokeSync(&MapContext::setStyleJSON, "", "");
    context.invokeSync(&MapContext::cleanup);
}
