#pragma once

#include "node_style.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

namespace node_mbgl {

class NodeMap : public Nan::ObjectWrap,
                public mbgl::FileSource {
public:
    struct RenderOptions;
    class RenderWorker;

    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(New);
    static NAN_METHOD(Load);
    static NAN_METHOD(Loaded);
    static NAN_METHOD(Render);
    static NAN_METHOD(Release);
    static NAN_METHOD(AddClass);
    static NAN_METHOD(SetLayoutProperty);
    static NAN_METHOD(SetPaintProperty);
    static NAN_METHOD(SetFilter);
    static NAN_METHOD(DumpDebugLogs);
    static NAN_METHOD(QueryRenderedFeatures);

    static void setProperty(const Nan::FunctionCallbackInfo<v8::Value>&, const PropertySetters&);

    void startRender(RenderOptions options);
    void renderFinished();

    void release();

    static RenderOptions ParseOptions(v8::Local<v8::Object>);
    static Nan::Persistent<v8::Function> constructor;

    NodeMap(v8::Local<v8::Object>);
    ~NodeMap();

    std::unique_ptr<mbgl::AsyncRequest> request(const mbgl::Resource&, Callback);

    mbgl::HeadlessView view;
    std::unique_ptr<mbgl::Map> map;

    std::exception_ptr error;
    mbgl::PremultipliedImage image;
    std::unique_ptr<Nan::Callback> callback;

    // Async for delivering the notifications of render completion.
    uv_async_t *async;

    bool loaded = false;
};

}
