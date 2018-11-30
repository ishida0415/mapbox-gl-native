// This file is generated. Do not edit.

#pragma once

#include <mbgl/layermanager/layer_factory.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class BackgroundLayer : public Layer {
public:
    BackgroundLayer(const std::string& layerID);
    ~BackgroundLayer() final;

    // Dynamic properties
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Paint properties

    static PropertyValue<Color> getDefaultBackgroundColor();
    PropertyValue<Color> getBackgroundColor() const;
    void setBackgroundColor(PropertyValue<Color>);
    void setBackgroundColorTransition(const TransitionOptions&);
    TransitionOptions getBackgroundColorTransition() const;

    static PropertyValue<std::string> getDefaultBackgroundPattern();
    PropertyValue<std::string> getBackgroundPattern() const;
    void setBackgroundPattern(PropertyValue<std::string>);
    void setBackgroundPatternTransition(const TransitionOptions&);
    TransitionOptions getBackgroundPatternTransition() const;

    static PropertyValue<float> getDefaultBackgroundOpacity();
    PropertyValue<float> getBackgroundOpacity() const;
    void setBackgroundOpacity(PropertyValue<float>);
    void setBackgroundOpacityTransition(const TransitionOptions&);
    TransitionOptions getBackgroundOpacityTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    BackgroundLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style

class BackgroundLayerFactory : public LayerFactory {
protected:
    const style::LayerTypeInfo* getTypeInfo() const noexcept final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept final;
    std::unique_ptr<RenderLayer> createRenderLayer(Immutable<style::Layer::Impl>) noexcept final;
};

} // namespace mbgl
