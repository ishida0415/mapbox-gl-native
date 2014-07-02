#include <llmr/style/style_parser.hpp>
#include <llmr/style/style_layer_group.hpp>
#include <llmr/util/constants.hpp>
#include <llmr/util/std.hpp>
#include <csscolorparser/csscolorparser.hpp>

namespace llmr {

using JSVal = const rapidjson::Value&;

void StyleParser::parse(JSVal document) {
    if (document.HasMember("constants")) {
        parseConstants(document["constants"]);
    }

    if (document.HasMember("sources")) {
        parseSources(document["sources"]);
    }

    if (document.HasMember("layers")) {
        root = createLayers(document["layers"]);
        parseLayers();
    }

    if (document.HasMember("sprite")) {
        parseSprite(document["sprite"]);
    }
}

void StyleParser::parseConstants(JSVal value) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            std::string name { itr->name.GetString(), itr->name.GetStringLength() };
            // Discard constants that don't start with an @ sign.
            if (name.length() && name[0] == '@') {
                constants.emplace(std::move(name), &itr->value);
            }
        }
    } else {
        throw Style::exception("constants must be an object");
    }
}

JSVal StyleParser::replaceConstant(JSVal value) {
    if (value.IsString()) {
        auto it = constants.find({ value.GetString(), value.GetStringLength() });
        if (it != constants.end()) {
            return *it->second;
        }
    }

    return value;
}

#pragma mark - Parse Render Properties

template<> bool StyleParser::parseRenderProperty(JSVal value, std::string &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsString()) {
            target = { property.GetString(), property.GetStringLength() };
            return true;
        } else {
            fprintf(stderr, "[WARNING] '%s' must be a string\n", name);
        }
    }
    return false;
}

template<> bool StyleParser::parseRenderProperty(JSVal value, float &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsNumber()) {
            target = property.GetDouble();
            return true;
        } else {
            fprintf(stderr, "[WARNING] '%s' must be a number\n", name);
        }
    }
    return false;
}

template<> bool StyleParser::parseRenderProperty(JSVal value, uint16_t &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsUint()) {
            unsigned int value = property.GetUint();
            if (value > std::numeric_limits<uint16_t>::max()) {
                fprintf(stderr, "[WARNING] values for %s that are larger than %d are not supported\n", name, std::numeric_limits<uint16_t>::max());
                return false;
            }

            target = value;
            return true;
        } else {
            fprintf(stderr, "[WARNING] %s must be an unsigned integer\n", name);
        }
    }
    return false;
}

template<> bool StyleParser::parseRenderProperty(JSVal value, int32_t &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsInt()) {
            target = property.GetInt();
            return true;
        } else {
            fprintf(stderr, "[WARNING] %s must be an integer\n", name);
        }
    }
    return false;
}

template<> bool StyleParser::parseRenderProperty(JSVal value, vec2<float> &target, const char *name) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsArray()) {
            if (property.Size() >= 2) {
                target.x = property[(rapidjson::SizeType)0].GetDouble();
                target.y = property[(rapidjson::SizeType)1].GetDouble();
                return true;
            } else {
                fprintf(stderr, "[WARNING] %s must have at least two members\n", name);
            }
        } else {
            fprintf(stderr, "[WARNING] %s must be a n array of numbers\n", name);
        }
    }
    return false;
}

template<typename T, typename Parser>
bool StyleParser::parseRenderProperty(JSVal value, T &target, const char *name, Parser &parser) {
    if (value.HasMember(name)) {
        JSVal property = replaceConstant(value[name]);
        if (property.IsString()) {
            target = parser({ property.GetString(), property.GetStringLength() });
            return true;
        } else {
            fprintf(stderr, "[WARNING] %s must have one of the enum values\n", name);
        }
    }
    return false;
}


#pragma mark - Parse Sources

void StyleParser::parseSources(JSVal value) {
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            std::string name { itr->name.GetString(), itr->name.GetStringLength() };
            SourceType type = SourceType::Vector;
            std::string url;
            std::string glyphs;
            uint16_t tile_size = 512;
            int32_t min_zoom = 0;
            int32_t max_zoom = 22;

            parseRenderProperty(itr->value, type, "type", parseSourceType);
            parseRenderProperty(itr->value, url, "url");
            parseRenderProperty(itr->value, glyphs, "glyphs");
            parseRenderProperty(itr->value, tile_size, "tileSize");
            parseRenderProperty(itr->value, min_zoom, "minZoom");
            parseRenderProperty(itr->value, max_zoom, "maxZoom");

            sources.emplace(std::move(name), std::make_shared<Source>(type, url, glyphs, tile_size, min_zoom, max_zoom));
        }
    } else {
        throw Style::exception("sources must be an object");
    }
}

#pragma mark - Parse Style Properties

Color parseColor(JSVal value) {
    if (!value.IsString()) {
        fprintf(stderr, "[WARNING] color value must be a string\n");
        return Color{{ 0, 0, 0, 0 }};
    }

    CSSColorParser::Color css_color = CSSColorParser::parse({ value.GetString(), value.GetStringLength() });

    // Premultiply the color.
    const float factor = css_color.a / 255;

    return Color{{(float)css_color.r * factor,
                  (float)css_color.g * factor,
                  (float)css_color.b * factor,
                  css_color.a}};
}

template <>
bool StyleParser::parseFunctionArgument(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    if (rvalue.IsBool()) {
        return rvalue.GetBool();
    } else if (rvalue.IsNumber()) {
        return rvalue.GetDouble();
    } else {
        fprintf(stderr, "[WARNING] function argument must be a boolean or numeric value");
        return false;
    }
}

template <>
float StyleParser::parseFunctionArgument(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    if (rvalue.IsNumber()) {
        return rvalue.GetDouble();
    } else {
        fprintf(stderr, "[WARNING] function argument must be a numeric value");
        return 0.0f;
    }
}

template <>
Color StyleParser::parseFunctionArgument(JSVal value) {
    JSVal rvalue = replaceConstant(value);
    return parseColor(rvalue);
}

template <typename T>
bool StyleParser::parseFunction(PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember("fn")) {
        fprintf(stderr, "[WARNING] function must specify a function name\n");
        return false;
    }

    JSVal value_fn = value["fn"];
    if (!value_fn.IsString()) {
        fprintf(stderr, "[WARNING] function must specify a function type\n");
        return false;
    }

    const std::string type { value_fn.GetString(), value_fn.GetStringLength() };

    if (type == "linear") {
        if (!value.HasMember("z")) {
            fprintf(stderr, "[WARNING] linear function must specify a base zoom level\n");
            return false;
        }
        if (!value.HasMember("val")) {
            fprintf(stderr, "[WARNING] linear function must specify a base value\n");
            return false;
        }
        const float z_base = parseFunctionArgument<float>(value["z"]);
        const T val = parseFunctionArgument<T>(value["val"]);
        const float slope = value.HasMember("slope") ? parseFunctionArgument<float>(value["slope"]) : 1;
        const T min = value.HasMember("min") ? parseFunctionArgument<T>(value["min"]) : T();
        const T max = value.HasMember("max") ? parseFunctionArgument<T>(value["max"]) : T();
        klass.set(key, LinearFunction<T>(val, z_base, slope, min, max));
        return true;
    }
    else if (type == "exponential") {
        if (!value.HasMember("z")) {
            fprintf(stderr, "[WARNING] exponential function must specify a base zoom level\n");
            return false;
        }
        if (!value.HasMember("val")) {
            fprintf(stderr, "[WARNING] exponential function must specify a base value\n");
            return false;
        }
        const float z_base = parseFunctionArgument<float>(value["z"]);
        const T val = parseFunctionArgument<T>(value["val"]);
        const float slope = value.HasMember("slope") ? parseFunctionArgument<float>(value["slope"]) : 1;
        const T min = value.HasMember("min") ? parseFunctionArgument<T>(value["min"]) : T();
        const T max = value.HasMember("max") ? parseFunctionArgument<T>(value["max"]) : T();
        klass.set(key, ExponentialFunction<T>(val, z_base, slope, min, max));
        return true;

    }
    else if (type == "stops") {
        if (!value.HasMember("stops")) {
            fprintf(stderr, "[WARNING] stops function must specify a stops array\n");
            return false;
        }

        JSVal value_stops = value["stops"];
        if (!value_stops.IsArray()) {
            fprintf(stderr, "[WARNING] stops function must specify a stops array\n");
            return false;
        }

        std::vector<std::pair<float, T>> stops;
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            JSVal stop = value[i];
            if (stop.IsObject()) {
                if (!stop.HasMember("z")) {
                    fprintf(stderr, "[WARNING] stop must have zoom level specification\n");
                    return false;
                }
                if (!stop.HasMember("val")) {
                    fprintf(stderr, "[WARNING] stop must have value specification\n");
                    return false;
                }

                JSVal z = stop["z"];
                if (!z.IsNumber()) {
                    fprintf(stderr, "[WARNING] zoom level in stop must be a number\n");
                    return false;
                }

                stops.emplace_back(z.GetDouble(), parseFunctionArgument<T>(stop["val"]));
            } else {
                fprintf(stderr, "[WARNING] function argument must be a numeric value\n");
                return false;
            }
        }

        klass.set(key, StopsFunction<T>(stops));
        return true;
    }
    else {
        fprintf(stderr, "[WARNING] function type '%s' is unknown\n", type.c_str());
    }

    return false;
}

template<> bool StyleParser::parseStyleProperty<std::string>(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    }

    JSVal rvalue = replaceConstant(value[property_name]);
    if (!rvalue.IsString()) {
        fprintf(stderr, "[WARNING] value of '%s' must be a string\n", property_name);
        return false;
    }

    klass.set(key, std::string { rvalue.GetString(), rvalue.GetStringLength() });
    return true;
}

template<> bool StyleParser::parseStyleProperty<TranslateAnchorType>(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    }

    JSVal rvalue = replaceConstant(value[property_name]);
    if (!rvalue.IsString()) {
        fprintf(stderr, "[WARNING] value of '%s' must be a string\n", property_name);
        return false;
    }

    klass.set(key, parseTranslateAnchorType({ rvalue.GetString(), rvalue.GetStringLength() }));
    return true;
}

template<> bool StyleParser::parseStyleProperty<RotateAnchorType>(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    }

    JSVal rvalue = replaceConstant(value[property_name]);
    if (!rvalue.IsString()) {
        fprintf(stderr, "[WARNING] value of '%s' must be a string\n", property_name);
        return false;
    }

    klass.set(key, parseRotateAnchorType({ rvalue.GetString(), rvalue.GetStringLength() }));
    return true;
}

template <> bool StyleParser::parseStyleProperty<PropertyTransition>(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    }
    JSVal elements = replaceConstant(value[property_name]);
    PropertyTransition transition;

    if (elements.IsObject()) {
        if (elements.HasMember("duration") && elements["duration"].IsNumber()) {
            transition.duration = elements["duration"].GetUint();
        }
        if (elements.HasMember("delay") && elements["delay"].IsNumber()) {
            transition.delay = elements["delay"].GetUint();
        }
    }

    if (transition.duration == 0 && transition.delay == 0) {
        return false;
    }

    klass.setTransition(key, std::move(transition));
    return true;
}

template<> bool StyleParser::parseStyleProperty<Function<bool>>(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    } else {
        JSVal rvalue = replaceConstant(value[property_name]);
        if (rvalue.IsObject()) {
            return parseFunction<bool>(key, klass, rvalue);
        } else if (rvalue.IsNumber()) {
            klass.set(key, bool(rvalue.GetDouble()));
            return true;
        } else if (rvalue.IsBool()) {
            klass.set(key, bool(rvalue.GetBool()));
            return true;
        } else {
            fprintf(stderr, "[WARNING] value of '%s' must be a boolean, or a boolean function\n", property_name);
            return false;
        }
    }
}

template<> bool StyleParser::parseStyleProperty<Function<float>>(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    } else {
        JSVal rvalue = replaceConstant(value[property_name]);
        if (rvalue.IsObject()) {
            return parseFunction<float>(key, klass, rvalue);
        } else if (rvalue.IsNumber()) {
            klass.set(key, float(rvalue.GetDouble()));
            return true;
        } else if (rvalue.IsBool()) {
            klass.set(key, float(rvalue.GetBool()));
            return true;
        } else {
            fprintf(stderr, "[WARNING] value of '%s' must be a number, or a number function\n", property_name);
            return false;
        }
    }
}

template<> bool StyleParser::parseStyleProperty<Function<Color>>(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value) {
    if (!value.HasMember(property_name)) {
        return false;
    } else {
        JSVal rvalue = replaceConstant(value[property_name]);
        if (rvalue.IsObject()) {
            return parseFunction<Color>(key, klass, rvalue);
        } else if (rvalue.IsString()) {
            klass.set(key, parseColor(rvalue));
            return true;
        } else {
            fprintf(stderr, "[WARNING] value of '%s' must be a color, or a color function\n", property_name);
            return false;
        }
    }
}


template <typename T>
bool StyleParser::parseStyleProperty(const char *property_name, const std::vector<PropertyKey> &keys, ClassProperties &klass, JSVal value) {
    if (value.HasMember(property_name)) {
        JSVal rvalue = replaceConstant(value[property_name]);
        if (!rvalue.IsArray()) {
            throw Style::exception("array value must be an array");
        }

        if (rvalue.Size() != keys.size()) {
            throw Style::exception("array value has unexpected number of elements");
        }

        for (uint16_t i = 0; i < keys.size(); i++) {
            parseStyleProperty<T>(property_name, keys[i], klass, rvalue[(rapidjson::SizeType)i]);
        }
    }
    return true;
}

#pragma mark - Parse Layers

std::unique_ptr<StyleLayerGroup> StyleParser::createLayers(JSVal value) {
    if (value.IsArray()) {
        std::unique_ptr<StyleLayerGroup> group = std::make_unique<StyleLayerGroup>();
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            std::shared_ptr<StyleLayer> layer = createLayer(value[i]);
            if (layer) {
                group->layers.emplace_back(layer);
            }
        }
        return group;
    } else {
        throw Style::exception("layers must be an array");
    }
}

std::shared_ptr<StyleLayer> StyleParser::createLayer(JSVal value) {
    if (value.IsObject()) {
        if (!value.HasMember("id")) {
            fprintf(stderr, "[WARNING] layer must have an id\n");
            return nullptr;
        }

        JSVal id = value["id"];
        if (!id.IsString()) {
            fprintf(stderr, "[WARNING] layer id must be a string\n");
            return nullptr;
        }

        const std::string layer_id = { id.GetString(), id.GetStringLength() };

        if (existing_layer_ids.find(layer_id) != existing_layer_ids.end()) {
            fprintf(stderr, "[WARNING] duplicate layer id %s\n", layer_id.c_str());
            return nullptr;
        }

        // Parse styles already, as they can't be inherited anyway.
        std::map<ClassID, ClassProperties> styles;
        parseStyles(value, styles);

        // Parse Rasterization options, as they can't be inherited anyway.
        std::unique_ptr<const RasterizeProperties> rasterize;
        if (value.HasMember("rasterize")) {
            rasterize = parseRasterize(replaceConstant(value["rasterize"]));
        }

        std::shared_ptr<StyleLayer> layer = std::make_shared<StyleLayer>(
            layer_id, std::move(styles), std::move(rasterize));

        if (value.HasMember("layers")) {
            layer->layers = createLayers(value["layers"]);
        }

        // Store the layer ID so we can reference it later.
        layers.emplace(layer_id, std::pair<JSVal, std::shared_ptr<StyleLayer>> { value, layer });

        return layer;
    } else {
        fprintf(stderr, "[WARNING] layer must be an object\n");
        return nullptr;
    }
}

void StyleParser::parseLayers() {
    for (std::pair<const std::string, std::pair<JSVal, std::shared_ptr<StyleLayer>>> &pair : layers) {
        parseLayer(pair.second);
    }
}

void StyleParser::parseLayer(std::pair<JSVal, std::shared_ptr<StyleLayer>> &pair) {
    JSVal value = pair.first;
    std::shared_ptr<StyleLayer> &layer = pair.second;

    if (layer->bucket || layer->layers) {
        // Skip parsing this again. We already have a valid layer definition.
        return;
    }

    // Make sure we have not previously attempted to parse this layer.
    if (std::find(stack.begin(), stack.end(), layer.get()) != stack.end()) {
        fprintf(stderr, "[WARNING] layer reference of '%s' is circular\n", layer->id.c_str());
        return;
    }

    if (value.HasMember("ref")) {
        // This layer is referencing another layer. Inherit the bucket from that layer, if we
        // already parsed it.
        parseReference(replaceConstant(value["ref"]), layer);
    } else {
        // Otherwise, parse the source/source-layer/filter/render keys to form the bucket.
        parseBucket(value, layer);
    }
}

#pragma mark - Parse Styles

void StyleParser::parseStyles(JSVal value, std::map<ClassID, ClassProperties> &styles) {
    rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
    for (; itr != value.MemberEnd(); ++itr) {
        const std::string name { itr->name.GetString(), itr->name.GetStringLength() };

        if (name == "style") {
            parseStyle(replaceConstant(itr->value), styles[ClassID::Default]);
        } else if (name.compare(0, 6, "style.") == 0 && name.length() > 6) {
            const ClassID class_id = ClassDictionary::Lookup(name.substr(6));
            parseStyle(replaceConstant(itr->value), styles[class_id]);
        }
    }
}

void StyleParser::parseStyle(JSVal value, ClassProperties &klass) {
    using Key = PropertyKey;

    parseStyleProperty<Function<bool>>("fill-enabled", Key::FillEnabled, klass, value);
    parseStyleProperty<PropertyTransition>("transition-fill-enabled", Key::FillEnabled, klass, value);
    parseStyleProperty<Function<bool>>("fill-antialias", Key::FillAntialias, klass, value);
    parseStyleProperty<Function<float>>("fill-opacity", Key::FillOpacity, klass, value);
    parseStyleProperty<PropertyTransition>("transition-fill-opacity", Key::FillOpacity, klass, value);
    parseStyleProperty<Function<Color>>("fill-color", Key::FillColor, klass, value);
    parseStyleProperty<PropertyTransition>("transition-fill-color", Key::FillColor, klass, value);
    parseStyleProperty<Function<Color>>("fill-outline-color", Key::FillOutlineColor, klass, value);
    parseStyleProperty<PropertyTransition>("transition-fill-outline-color", Key::FillOutlineColor, klass, value);
    parseStyleProperty<Function<float>>("fill-translate", { Key::FillTranslateX, Key::FillTranslateY }, klass, value);
    parseStyleProperty<PropertyTransition>("transition-fill-translate", Key::FillTranslate, klass, value);
    parseStyleProperty<TranslateAnchorType>("fill-translate-anchor", Key::FillTranslateAnchor, klass, value);
    parseStyleProperty<std::string>("fill-image", Key::FillImage, klass, value);

    parseStyleProperty<Function<bool>>("line-enabled", Key::LineEnabled, klass, value);
    parseStyleProperty<PropertyTransition>("transition-line-enabled", Key::LineEnabled, klass, value);
    parseStyleProperty<Function<float>>("line-opacity", Key::LineOpacity, klass, value);
    parseStyleProperty<PropertyTransition>("transition-line-opacity", Key::LineOpacity, klass, value);
    parseStyleProperty<Function<Color>>("line-color", Key::LineColor, klass, value);
    parseStyleProperty<PropertyTransition>("transition-line-color", Key::LineColor, klass, value);
    parseStyleProperty<Function<float>>("line-translate", { Key::LineTranslateX, Key::LineTranslateY }, klass, value);
    parseStyleProperty<PropertyTransition>("transition-line-translate", Key::LineTranslate, klass, value);
    parseStyleProperty<TranslateAnchorType>("line-translate-anchor", Key::LineTranslateAnchor, klass, value);
    parseStyleProperty<Function<float>>("line-width", Key::LineWidth, klass, value);
    parseStyleProperty<PropertyTransition>("transition-line-width", Key::LineWidth, klass, value);
    parseStyleProperty<Function<float>>("line-offset", Key::LineOffset, klass, value);
    parseStyleProperty<PropertyTransition>("transition-line-offset", Key::LineOffset, klass, value);
    parseStyleProperty<Function<float>>("line-blur", Key::LineBlur, klass, value);
    parseStyleProperty<PropertyTransition>("transition-line-blur", Key::LineBlur, klass, value);
    parseStyleProperty<Function<float>>("line-dasharray", { Key::LineDashLand, Key::LineDashGap }, klass, value);
    parseStyleProperty<PropertyTransition>("transition-line-dasharray", Key::LineDashArray, klass, value);
    parseStyleProperty<std::string>("line-image", Key::LineImage, klass, value);

    parseStyleProperty<Function<bool>>("icon-enabled", Key::IconEnabled, klass, value);
    parseStyleProperty<PropertyTransition>("transition-icon-enabled", Key::IconEnabled, klass, value);
    parseStyleProperty<Function<float>>("icon-opacity", Key::IconOpacity, klass, value);
    parseStyleProperty<PropertyTransition>("transition-icon-opacity", Key::IconOpacity, klass, value);
    parseStyleProperty<Function<float>>("icon-rotate", Key::IconRotate, klass, value);
    parseStyleProperty<RotateAnchorType>("icon-rotate-anchor", Key::IconRotateAnchor, klass, value);

    parseStyleProperty<Function<bool>>("text-enabled", Key::TextEnabled, klass, value);
    parseStyleProperty<PropertyTransition>("transition-text-enabled", Key::TextEnabled, klass, value);
    parseStyleProperty<Function<float>>("text-opacity", Key::TextOpacity, klass, value);
    parseStyleProperty<PropertyTransition>("transition-text-opacity", Key::TextOpacity, klass, value);
    parseStyleProperty<Function<float>>("text-size", Key::TextSize, klass, value);
    parseStyleProperty<PropertyTransition>("transition-text-size", Key::TextSize, klass, value);
    parseStyleProperty<Function<Color>>("text-color", Key::TextColor, klass, value);
    parseStyleProperty<PropertyTransition>("transition-text-color", Key::TextColor, klass, value);
    parseStyleProperty<Function<Color>>("text-halo-color", Key::TextHaloColor, klass, value);
    parseStyleProperty<PropertyTransition>("transition-text-halo-color", Key::TextHaloColor, klass, value);
    parseStyleProperty<Function<float>>("text-halo-width", Key::TextHaloWidth, klass, value);
    parseStyleProperty<PropertyTransition>("transition-text-halo-width", Key::TextHaloWidth, klass, value);
    parseStyleProperty<Function<float>>("text-halo-blur", Key::TextHaloBlur, klass, value);
    parseStyleProperty<PropertyTransition>("transition-text-halo-blur", Key::TextHaloBlur, klass, value);

    parseStyleProperty<Function<bool>>("composite-enabled", Key::CompositeEnabled, klass, value);
    parseStyleProperty<PropertyTransition>("transition-composite-enabled", Key::CompositeEnabled, klass, value);
    parseStyleProperty<Function<float>>("composite-opacity", Key::CompositeOpacity, klass, value);
    parseStyleProperty<PropertyTransition>("transition-composite-opacity", Key::CompositeOpacity, klass, value);

    parseStyleProperty<Function<bool>>("raster-enabled", Key::RasterEnabled, klass, value);
    parseStyleProperty<PropertyTransition>("transition-raster-enabled", Key::RasterEnabled, klass, value);
    parseStyleProperty<Function<float>>("raster-opacity", Key::RasterOpacity, klass, value);
    parseStyleProperty<PropertyTransition>("transition-raster-opacity", Key::RasterOpacity, klass, value);
    parseStyleProperty<Function<float>>("raster-spin", Key::RasterSpin, klass, value);
    parseStyleProperty<PropertyTransition>("transition-raster-spin", Key::RasterSpin, klass, value);
    parseStyleProperty<Function<float>>("raster-brightness-low", Key::RasterBrightnessLow, klass, value);
    parseStyleProperty<PropertyTransition>("transition-raster-brightness-low", Key::RasterBrightnessLow, klass, value);
    parseStyleProperty<Function<float>>("raster-brightness-high", Key::RasterBrightnessHigh, klass, value);
    parseStyleProperty<PropertyTransition>("transition-raster-brightness-high", Key::RasterBrightnessHigh, klass, value);
    parseStyleProperty<Function<float>>("raster-saturation", Key::RasterSaturation, klass, value);
    parseStyleProperty<PropertyTransition>("transition-raster-saturation", Key::RasterSaturation, klass, value);
    parseStyleProperty<Function<float>>("raster-contrast", Key::RasterContrast, klass, value);
    parseStyleProperty<PropertyTransition>("transition-raster-contrast", Key::RasterContrast, klass, value);
    parseStyleProperty<Function<float>>("raster-fade", Key::RasterFade, klass, value);
    parseStyleProperty<PropertyTransition>("transition-raster-fade", Key::RasterFade, klass, value);

    parseStyleProperty<Function<Color>>("background-color", Key::BackgroundColor, klass, value);
}

std::unique_ptr<const RasterizeProperties> StyleParser::parseRasterize(JSVal value) {
    // TODO
    return nullptr;
}

void StyleParser::parseReference(JSVal value, std::shared_ptr<StyleLayer> &layer) {
    if (!value.IsString()) {
        fprintf(stderr, "[WARNING] layer ref of '%s' must be a string\n", layer->id.c_str());
        return;
    }
    const std::string ref { value.GetString(), value.GetStringLength() };
    auto it = layers.find(ref);
    if (it == layers.end()) {
        fprintf(stderr, "[WARNING] layer '%s' references unknown layer %s\n", layer->id.c_str(), ref.c_str());
        // We cannot parse this layer further.
        return;
    }

    // Recursively parse the referenced layer.
    stack.push_front(layer.get());
    parseLayer(it->second);
    stack.pop_front();

    std::shared_ptr<StyleLayer> reference = it->second.second;
    if (reference->layers) {
        fprintf(stderr, "[WARNING] layer '%s' references composite layer\n", layer->id.c_str());
        // We cannot parse this layer further.
        return;
    }

    layer->bucket = reference->bucket;
}

#pragma mark - Parse Bucket

void StyleParser::parseBucket(JSVal value, std::shared_ptr<StyleLayer> &layer) {
    layer->bucket = std::make_shared<StyleBucket>();

    // We name the buckets according to the layer that defined it.
    layer->bucket->name = layer->id;

    if (value.HasMember("source")) {
        JSVal value_source = replaceConstant(value["source"]);
        if (value_source.IsString()) {
            const std::string source_name = { value_source.GetString(), value_source.GetStringLength() };
            auto source_it = sources.find(source_name);
            if (source_it != sources.end()) {
                layer->bucket->source = source_it->second;
            } else {
                fprintf(stderr, "[WARNING] can't find source '%s' required for layer '%s'\n",
                        source_name.c_str(), layer->id.c_str());
            }
        } else {
            fprintf(stderr, "[WARNING] source of layer '%s' must be a string\n", layer->id.c_str());
        }
    }

    if (value.HasMember("source-layer")) {
        JSVal value_source_layer = replaceConstant(value["source-layer"]);
        if (value_source_layer.IsString()) {
            layer->bucket->source_layer = { value_source_layer.GetString(), value_source_layer.GetStringLength() };
        } else {
            fprintf(stderr, "[WARNING] source-layer of layer '%s' must be a string\n", layer->id.c_str());
        }
    }

    if (value.HasMember("filter")) {
        JSVal value_filter = replaceConstant(value["filter"]);
        layer->bucket->filter = parseFilter(value_filter);
    }

    if (value.HasMember("render")) {
        JSVal value_render = replaceConstant(value["render"]);
        parseRender(value_render, layer);
    }
}

FilterExpression StyleParser::parseFilter(JSVal value) {
    return parseFilter(value, value.IsArray() ? FilterExpression::Operator::Or : FilterExpression::Operator::And);
}

FilterExpression StyleParser::parseFilter(JSVal value, FilterExpression::Operator op) {
    FilterExpression expression(op);
    if (value.IsObject()) {
        rapidjson::Value::ConstMemberIterator itr = value.MemberBegin();
        for (; itr != value.MemberEnd(); ++itr) {
            const std::string name { itr->name.GetString(), itr->name.GetStringLength() };
            if (name == "&") {
                expression.add(parseFilter(replaceConstant(itr->value), FilterExpression::Operator::And));
            } else if (name == "|") {
                expression.add(parseFilter(replaceConstant(itr->value), FilterExpression::Operator::Or));
            } else if (name == "^") {
                expression.add(parseFilter(replaceConstant(itr->value), FilterExpression::Operator::Xor));
            } else if (name == "!") {
                expression.add(parseFilter(replaceConstant(itr->value), FilterExpression::Operator::Nor));
            } else if (name == "$type") {
                JSVal type = replaceConstant(itr->value);
                if (type.IsString()) {
                    expression.setGeometryType(parseGeometryType({ type.GetString(), type.GetStringLength() }));
                }
            } else {
                FilterComparison comparison(name);
                JSVal filterValue = replaceConstant(itr->value);
                if (filterValue.IsObject()) {
                    rapidjson::Value::ConstMemberIterator itr = filterValue.MemberBegin();
                    for (; itr != filterValue.MemberEnd(); ++itr) {
                        comparison.add(
                            parseFilterComparisonOperator({ itr->name.GetString(), itr->name.GetStringLength() }),
                            parseValues(replaceConstant(itr->value))
                        );
                    }
                } else if (filterValue.IsArray()) {
                    comparison.add(FilterComparison::Operator::In, parseValues(filterValue));
                } else {
                    comparison.add(FilterComparison::Operator::Equal, std::forward_list<Value>({ parseValue(filterValue) }));
                }
                expression.add(comparison);
            }
        }
    } else if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); i++) {
            expression.add(parseFilter(replaceConstant(value[i])));
        }
    } else {
        fprintf(stderr, "[WARNING] expression must be either an array or an object\n");
    }

    return expression;
}

Value StyleParser::parseValue(JSVal value) {
    switch (value.GetType()) {
        case rapidjson::kNullType:
        case rapidjson::kFalseType:
            return false;

        case rapidjson::kTrueType:
            return true;

        case rapidjson::kStringType:
            return std::string { value.GetString(), value.GetStringLength() };

        case rapidjson::kNumberType:
            if (value.IsUint64()) return value.GetUint64();
            if (value.IsInt64()) return value.GetInt64();
            return value.GetDouble();

        default:
            return false;
    }
}

std::forward_list<Value> StyleParser::parseValues(JSVal value) {
    std::forward_list<Value> values;
    if (value.IsArray()) {
        auto it = values.before_begin();
        for (rapidjson::SizeType i = 0; i < value.Size(); i++) {
            it = values.emplace_after(it, parseValue(replaceConstant(value[i])));
        }
    } else {
        values.emplace_front(parseValue(value));
    }
    return values;
}

void StyleParser::parseRender(JSVal value, std::shared_ptr<StyleLayer> &layer) {
    if (!value.IsObject()) {
        fprintf(stderr, "[WARNING] render property of layer '%s' must be an object\n", layer->id.c_str());
        return;
    }

    StyleBucket &bucket = *layer->bucket;

    if (value.HasMember("type")) {
        JSVal type = value["type"];
        if (type.IsString()) {
            bucket.type = parseBucketType({ type.GetString(), type.GetStringLength() });
        }
    }

    switch (bucket.type) {
    case BucketType::Fill: {
        bucket.render = StyleBucketFill{};
        StyleBucketFill &render = bucket.render.get<StyleBucketFill>();

        parseRenderProperty(value, render.winding, "fill-winding", parseWindingType);
    } break;

    case BucketType::Line: {
        bucket.render = StyleBucketLine{};
        StyleBucketLine &render = bucket.render.get<StyleBucketLine>();

        parseRenderProperty(value, render.cap, "line-cap", parseCapType);
        parseRenderProperty(value, render.join, "line-join", parseJoinType);
        parseRenderProperty(value, render.miter_limit, "line-miter-limit");
        parseRenderProperty(value, render.round_limit, "line-round-limit");
    } break;

    case BucketType::Icon: {
        bucket.render = StyleBucketIcon{};
        StyleBucketIcon &render = bucket.render.get<StyleBucketIcon>();

        parseRenderProperty(value, render.size, "icon-size");
        parseRenderProperty(value, render.icon, "icon-image");
        parseRenderProperty(value, render.spacing, "icon-spacing");
        parseRenderProperty(value, render.padding, "icon-padding");
        if (parseRenderProperty(value, render.translate, "icon-translate")) {
            render.translate.x *= 24;
            render.translate.y *= -24;
        }
        parseRenderProperty(value, render.translate_anchor, "icon-translate-anchor", parseTranslateAnchorType);
    } break;

    case BucketType::Text: {
        bucket.render = StyleBucketText{};
        StyleBucketText &render = bucket.render.get<StyleBucketText>();

        parseRenderProperty(value, render.field, "text-field");
        parseRenderProperty(value, render.path, "text-path", parseTextPathType);
        parseRenderProperty(value, render.font, "text-font");
        parseRenderProperty(value, render.max_size, "text-max-size");
        if (parseRenderProperty(value, render.max_width, "text-max-width")) {
            render.max_width *= 24; // em
        }
        if (parseRenderProperty(value, render.line_height, "text-line-height")) {
            render.line_height *= 24; // em
        }
        if (parseRenderProperty(value, render.letter_spacing, "text-letter-spacing")) {
            render.letter_spacing *= 24; // em
        }
        parseRenderProperty(value, render.alignment, "text-alignment", parseAlignmentType);
        parseRenderProperty(value, render.vertical_alignment, "text-vertical-alignment", parseVerticalAlignmentType);
        parseRenderProperty(value, render.max_angle_delta, "text-max-angle");
        parseRenderProperty(value, render.min_distance, "text-min-distance");
        parseRenderProperty(value, render.rotate, "text-rotate");
        parseRenderProperty(value, render.slant, "text-slant");
        parseRenderProperty(value, render.padding, "text-padding");
        if (parseRenderProperty(value, render.translate, "text-translate")) {
            render.translate.x *= 24;
            render.translate.y *= -24;
        }
        parseRenderProperty(value, render.translate_anchor, "text-translate-anchor", parseTranslateAnchorType);
    } break;

    case BucketType::Raster: {
        bucket.render = StyleBucketRaster{};
    } break;

    default:
        fprintf(stderr, "[WARNING] bucket type of layer '%s' is invalid\n", layer->id.c_str());
        break;
    }
}

void StyleParser::parseSprite(JSVal value) {
    if (value.IsString()) {
        sprite = { value.GetString(), value.GetStringLength() };
    }
}


}
