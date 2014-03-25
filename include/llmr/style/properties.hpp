#ifndef LLMR_STYLE_PROPERTIES
#define LLMR_STYLE_PROPERTIES

#include <array>
#include <vector>
#include <string>
#include <limits>

namespace llmr {

typedef std::array<float, 4> Color;

enum class Winding {
    EvenOdd = 1,
    NonZero = 2
};

// enum LineCap {
//     Round = 1
// };

// enum LineJoin {
//     Butt = 1,
//     Bevel = 2
// };

enum class Property {
    Null = 1,
    Constant = 2,
    Stops = 3,
    Linear = 4
};

namespace functions {

float null(float z, const std::vector<float>&);
bool null(float z, const std::vector<bool>&);

float constant(float z, const std::vector<float>& values);
bool constant(float z, const std::vector<bool>& values);

float stops(float z, const std::vector<float>& values);
bool stops(float z, const std::vector<bool>& values);

float linear(float z, const std::vector<float>& values);
bool linear(float z, const std::vector<bool>& values);

float exponential(float z, const std::vector<float>& values);
bool exponential(float z, const std::vector<bool>& values);

}


template <typename T>
struct FunctionProperty {
    typedef T (*fn)(float z, const std::vector<T>& values);

    fn function;
    std::vector<T> values;

    inline FunctionProperty() : function(&functions::null) {}
    inline FunctionProperty(T value) : function(&functions::constant), values(1, value) {}
    inline T operator()(float z) const { return function(z, values); }
};

struct PointClass {
    FunctionProperty<bool> enabled = true;
    FunctionProperty<float> size;
    Color color = {{ 0, 0, 0, 1 }};
    FunctionProperty<float> opacity = 1;
    std::string image;
};

struct PointProperties {
    bool enabled = true;
    float size = 0;
    Color color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0;
    std::string image;
};

struct LineClass {
    FunctionProperty<bool> enabled = true;
    FunctionProperty<float> width;
    FunctionProperty<float> offset;
    Color color = {{ 0, 0, 0, 1 }};
    FunctionProperty<float> opacity = 1;
};

struct LineProperties {
    bool enabled = true;
    float width = 0;
    float offset = 0;
    Color color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0;
};

struct FillClass {
    FunctionProperty<bool> enabled = true;
    Winding winding = Winding::NonZero;
    FunctionProperty<bool> antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, std::numeric_limits<float>::infinity() }};
    FunctionProperty<float> opacity = 1;
    std::string image;
};

struct FillProperties {
    bool enabled = true;
    Winding winding = Winding::NonZero;
    bool antialias = true;
    Color fill_color = {{ 0, 0, 0, 1 }};
    Color stroke_color = {{ 0, 0, 0, 1 }};
    float opacity = 1.0;
    std::string image;
};

struct TextClass {
    FunctionProperty<bool> enabled = true;
    Color color = {{ 0, 0, 0, 1 }};
    Color halo = {{ 1, 1, 1, 0.75 }};
    FunctionProperty<float> haloRadius = 0.25f;
    FunctionProperty<float> size = 12.0f;
    FunctionProperty<float> rotate = 0.0f;
    FunctionProperty<bool> alwaysVisible = false;
};

struct TextProperties {
    bool enabled = true;
    Color color = {{ 0, 0, 0, 1 }};
    Color halo = {{ 1, 1, 1, 0.75 }};
    float haloRadius = 0.25f;
    float size = 12.0f;
    float rotate = 0.0f;
    bool alwaysVisible = false;
};

struct BackgroundClass {
    Color color = {{ 1, 1, 1, 1 }};
};


struct BackgroundProperties {
    Color color = {{ 1, 1, 1, 1 }};
};


}

#endif
