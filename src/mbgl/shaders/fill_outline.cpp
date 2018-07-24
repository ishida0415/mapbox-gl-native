// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/fill_outline.hpp>
#include <mbgl/util/compression.hpp>

#include <cstdint>

namespace mbgl {
namespace shaders {

const char* fill_outline::name = "fill_outline";
const char* fill_outline::vertexSource = [] () {
    static const uint8_t compressed[] = {
        0x78, 0xda, 0x8d, 0x93, 0xcf, 0x4f, 0x83, 0x30,
        0x14, 0xc7, 0xef, 0xfc, 0x15, 0x2f, 0xd9, 0x85,
        0x29, 0x41, 0xb6, 0xec, 0x46, 0x3c, 0x78, 0x31,
        0x7a, 0xf0, 0x47, 0x34, 0x9e, 0x5f, 0xea, 0x80,
        0xed, 0xc5, 0xae, 0x25, 0xac, 0x85, 0x2d, 0xc6,
        0xff, 0xdd, 0x16, 0xb0, 0x16, 0x08, 0x46, 0x2e,
        0x90, 0xd7, 0x4f, 0x5f, 0x3f, 0xfd, 0xb6, 0x30,
        0xa5, 0x2a, 0x7a, 0xd7, 0x2a, 0x87, 0x3a, 0xdf,
        0xae, 0x81, 0x61, 0x29, 0x8f, 0x69, 0x10, 0x68,
        0x41, 0x85, 0xac, 0x0e, 0x70, 0x60, 0x6a, 0x03,
        0x1a, 0xcd, 0xab, 0xa2, 0x53, 0xea, 0xca, 0x2d,
        0xab, 0xb1, 0x91, 0x15, 0xcf, 0x0c, 0x5d, 0xb3,
        0xea, 0x4c, 0x62, 0xd7, 0x95, 0xeb, 0xbe, 0x45,
        0xb0, 0xa0, 0x42, 0x64, 0x79, 0x01, 0x77, 0x37,
        0xaf, 0xf8, 0xf6, 0x78, 0x7f, 0xfb, 0xf4, 0xf2,
        0x80, 0x1a, 0xa5, 0x56, 0x9c, 0x44, 0x8e, 0x5b,
        0xc9, 0x65, 0xe5, 0x1a, 0x72, 0xd9, 0x94, 0x50,
        0x70, 0xc9, 0x94, 0x51, 0x18, 0x20, 0xa8, 0xd2,
        0x80, 0x39, 0xc9, 0x3d, 0xed, 0xf6, 0xa5, 0x5d,
        0x67, 0x33, 0xe6, 0x52, 0x67, 0xe1, 0x31, 0x23,
        0x62, 0x91, 0xf3, 0x63, 0xee, 0xd6, 0xf4, 0x38,
        0x8d, 0x13, 0x52, 0x64, 0x54, 0xcc, 0x6f, 0xa2,
        0x64, 0x5b, 0x52, 0xe7, 0x19, 0xfd, 0x6e, 0x70,
        0x28, 0xde, 0x12, 0x7d, 0xc4, 0x3d, 0xf0, 0x6b,
        0xec, 0x4d, 0x77, 0x63, 0x43, 0x57, 0x8f, 0xd0,
        0xe8, 0x31, 0xbd, 0x65, 0x2d, 0x29, 0x33, 0x67,
        0x45, 0x22, 0x5c, 0xc2, 0x67, 0x00, 0xe6, 0xf9,
        0x57, 0xfa, 0x16, 0x1c, 0x54, 0xe0, 0x1a, 0xb4,
        0x30, 0xdd, 0x3f, 0xf0, 0x40, 0x27, 0xb4, 0xd1,
        0x84, 0xa3, 0x98, 0xa3, 0xe9, 0xf9, 0x2c, 0x7f,
        0x64, 0x6d, 0xbb, 0xb9, 0xf0, 0x6d, 0xe7, 0xb9,
        0x90, 0xff, 0xf2, 0xed, 0x83, 0x6e, 0x4d, 0xbb,
        0xef, 0x89, 0xe3, 0x3a, 0x74, 0x91, 0x46, 0x7e,
        0xfc, 0x03, 0xaf, 0x69, 0xc4, 0x9d, 0xd1, 0x24,
        0x4a, 0x0b, 0xef, 0x38, 0x3e, 0xcb, 0x23, 0x29,
        0x92, 0xa2, 0xa5, 0xba, 0xeb, 0x0f, 0x17, 0xd0,
        0x27, 0x62, 0x2e, 0x78, 0x04, 0x49, 0x04, 0x2b,
        0xb3, 0x84, 0xe5, 0xdb, 0x2b, 0x6f, 0xc8, 0xd0,
        0x9b, 0x18, 0x9f, 0xce, 0x70, 0xe5, 0x77, 0x8a,
        0x1b, 0xb8, 0x84, 0x55, 0x9c, 0x2c, 0x4d, 0x79,
        0x1d, 0x27, 0xa6, 0x9b, 0xfb, 0x81, 0xbe, 0x82,
        0x6f, 0xd3, 0x48, 0x35, 0x91
    };
    static std::string decompressed = util::decompress(std::string(reinterpret_cast<const char*>(compressed), sizeof(compressed)));
    return decompressed.c_str();
}();
const char* fill_outline::fragmentSource = [] () {
    static const uint8_t compressed[] = {
        0x78, 0xda, 0x85, 0x90, 0x5f, 0x4f, 0x83, 0x30,
        0x14, 0xc5, 0xdf, 0xfb, 0x29, 0x6e, 0xb2, 0x17,
        0x58, 0x26, 0x41, 0xe3, 0x1b, 0xf1, 0x61, 0x99,
        0x5b, 0xdc, 0x83, 0xc3, 0x30, 0x75, 0x8f, 0x4d,
        0x03, 0x05, 0x9a, 0x94, 0xb6, 0x81, 0x0e, 0x25,
        0xc6, 0xef, 0x6e, 0x3b, 0xfe, 0x84, 0x6d, 0xa2,
        0x7d, 0xbc, 0xfd, 0x9d, 0x73, 0xee, 0xb9, 0x68,
        0xc6, 0x52, 0x91, 0xd0, 0x14, 0x9e, 0x96, 0x7b,
        0xfc, 0xb6, 0xdb, 0x6e, 0xc2, 0xe8, 0x19, 0x1f,
        0xb1, 0x3c, 0x6a, 0xce, 0x04, 0xc5, 0xb1, 0xe4,
        0xb2, 0x44, 0x35, 0x29, 0x1b, 0x26, 0x32, 0xc8,
        0x59, 0x96, 0x2b, 0xa8, 0x69, 0x7c, 0x0f, 0x67,
        0x40, 0x80, 0x66, 0x94, 0x57, 0x14, 0x1d, 0x05,
        0x4b, 0x65, 0x59, 0x8c, 0xb9, 0x0b, 0x2b, 0x4b,
        0x8a, 0x84, 0xa5, 0x08, 0xa1, 0x89, 0x60, 0x45,
        0x62, 0xa6, 0x9b, 0x21, 0x92, 0xcb, 0x0f, 0x05,
        0x29, 0x97, 0x44, 0x43, 0xf7, 0x75, 0x19, 0x36,
        0x22, 0x06, 0xf9, 0x28, 0xa6, 0x37, 0x32, 0xdb,
        0xdc, 0x41, 0x8d, 0x95, 0xac, 0x02, 0x33, 0x94,
        0x2c, 0x81, 0x82, 0x30, 0xe1, 0xb8, 0xf0, 0x85,
        0xc0, 0x3c, 0xbb, 0xce, 0x7f, 0x67, 0xb0, 0xdc,
        0xd4, 0x09, 0xe0, 0x61, 0xba, 0xea, 0x1f, 0xfe,
        0x5d, 0x5b, 0x4b, 0x5c, 0x37, 0x6d, 0x3d, 0xaf,
        0x1a, 0x59, 0xb8, 0xe5, 0x12, 0x56, 0x69, 0x03,
        0x71, 0x2a, 0x32, 0x9d, 0x3b, 0xa7, 0x72, 0x70,
        0x03, 0x19, 0xc7, 0x9b, 0x92, 0x64, 0x2b, 0x29,
        0xcb, 0xc4, 0xfb, 0x6c, 0xdc, 0x60, 0xa4, 0x20,
        0x5c, 0xe5, 0xc4, 0x48, 0x6e, 0x3d, 0xdf, 0x90,
        0x55, 0x21, 0xa5, 0xce, 0x2b, 0x4d, 0x95, 0xe3,
        0x7b, 0xfe, 0xc2, 0x4e, 0x17, 0x27, 0xd3, 0x4e,
        0x33, 0x38, 0xb5, 0x05, 0xcf, 0x0b, 0xcf, 0xc1,
        0x69, 0xdd, 0xe6, 0xfd, 0xbe, 0x46, 0xd5, 0xd7,
        0x0c, 0xdf, 0xd7, 0xd1, 0x63, 0xb4, 0x3c, 0xe0,
        0xed, 0x6e, 0xff, 0xb2, 0x5e, 0xbd, 0x86, 0xd1,
        0x6f, 0x86, 0xf6, 0x8c, 0x8e, 0x09, 0x75, 0x87,
        0x76, 0xdf, 0xe8, 0x07, 0x2a, 0x68, 0xd6, 0xbe
    };
    static std::string decompressed = util::decompress(std::string(reinterpret_cast<const char*>(compressed), sizeof(compressed)));
    return decompressed.c_str();
}();

} // namespace shaders
} // namespace mbgl
