// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

#pragma once

#include "layer.hpp"
#include "../transition_options.hpp"
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class HeatmapLayer : public Layer {
public:
    using SuperTag = Layer;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/HeatmapLayer"; };

    static void registerNative(jni::JNIEnv&);

    HeatmapLayer(jni::JNIEnv&, jni::String&, jni::String&);

    HeatmapLayer(mbgl::Map&, mbgl::style::HeatmapLayer&);

    HeatmapLayer(mbgl::Map&, std::unique_ptr<mbgl::style::HeatmapLayer>);

    ~HeatmapLayer();

    // Properties

    jni::Local<jni::Object<jni::ObjectTag>> getHeatmapRadius(jni::JNIEnv&);
    void setHeatmapRadiusTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getHeatmapRadiusTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHeatmapWeight(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHeatmapIntensity(jni::JNIEnv&);
    void setHeatmapIntensityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getHeatmapIntensityTransition(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHeatmapColor(jni::JNIEnv&);

    jni::Local<jni::Object<jni::ObjectTag>> getHeatmapOpacity(jni::JNIEnv&);
    void setHeatmapOpacityTransition(jni::JNIEnv&, jlong duration, jlong delay);
    jni::Local<jni::Object<TransitionOptions>> getHeatmapOpacityTransition(jni::JNIEnv&);
    jni::Local<jni::Object<Layer>> createJavaPeer(jni::JNIEnv&);

}; // class HeatmapLayer

} // namespace android
} // namespace mbgl
