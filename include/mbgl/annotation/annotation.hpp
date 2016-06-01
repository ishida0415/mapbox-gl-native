#pragma once

#include <mbgl/style/types.hpp>

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/variant.hpp>

#include <cstdint>
#include <vector>
#include <string>

namespace mbgl {

using AnnotationID = uint32_t;
using AnnotationIDs = std::vector<AnnotationID>;

class SymbolAnnotation {
public:
    Geometry<double> geometry;
    std::string icon;
};

class LineAnnotation {
public:
    Geometry<double> geometry;
    float opacity = 1;
    float width = 1;
    Color color = {{ 0, 0, 0, 1 }};
};

class FillAnnotation {
public:
    Geometry<double> geometry;
    float opacity = 1;
    Color color = {{ 0, 0, 0, 1 }};
    Color outlineColor = {{ 0, 0, 0, -1 }};
};

// An annotation whose type and properties are sourced from a style layer.
class StyleSourcedAnnotation {
public:
    Geometry<double> geometry;
    std::string layerID;
};

using Annotation = variant<
    SymbolAnnotation,
    LineAnnotation,
    FillAnnotation,
    StyleSourcedAnnotation>;

} // namespace mbgl
