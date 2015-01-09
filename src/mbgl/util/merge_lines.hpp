#ifndef MBGL_UTIL_MERGELINES
#define MBGL_UTIL_MERGELINES

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <mbgl/renderer/symbol_bucket.hpp>


#include <iostream>
namespace mbgl {
namespace util {

unsigned int mergeFromRight(
        std::vector<std::vector<std::vector<Coordinate>>> &geometries,
        std::map<std::string,unsigned int> &rightIndex,
        std::map<std::string,unsigned int>::iterator left,
        std::string &rightKey,
        std::vector<std::vector<Coordinate>> &geom) {

    unsigned int index = left->second;
    rightIndex.erase(left);
    rightIndex[rightKey] = index;
    geometries[index][0].pop_back();
    geometries[index][0].insert(geometries[index][0].end(), geom[0].begin(), geom[0].end());
    geom[0].clear();
    return index;
}

unsigned int mergeFromLeft(
        std::vector<std::vector<std::vector<Coordinate>>> &geometries,
        std::map<std::string,unsigned int> &leftIndex,
        std::string &leftKey,
        std::map<std::string,unsigned int>::iterator right,
        std::vector<std::vector<Coordinate>> &geom) {

    unsigned int index = right->second;
    leftIndex.erase(right);
    leftIndex[leftKey] = index;
    geom[0].pop_back();
    geom[0].insert(geom[0].end(), geometries[index][0].begin(), geometries[index][0].end());
    geometries[index][0].clear();
    std::swap(geometries[index][0], geom[0]);
    return index;
}

std::string getKey(const std::u32string &text, const std::vector<std::vector<Coordinate>> &geom, bool onRight) {
    const Coordinate &coord = onRight ? geom[0].back() : geom[0].front();
    std::ostringstream key;
    for (const char32_t &c : text) {
        key << (char)c;
    }
    key << ":" << coord.x << ":" << coord.y;
    return key.str();
}


void mergeLines(const std::vector<SymbolFeature> &features, std::vector<std::vector<std::vector<Coordinate>>> &geometries) {

    std::map<std::string,unsigned int> leftIndex;
    std::map<std::string,unsigned int> rightIndex;
    
    if (features.size()) getKey(features[0].label, geometries[0], false);

    for (unsigned int k = 0; k < features.size(); k++) {
        const SymbolFeature &feature = features[k];
        std::vector<std::vector<Coordinate>> &geometry = geometries[k];

        if (!feature.label.length()) {
            continue;
        }

        std::string leftKey = getKey(feature.label, geometry, false);
        std::string rightKey = getKey(feature.label, geometry, true);

        auto left = rightIndex.find(leftKey);
        auto right = leftIndex.find(rightKey);

        if ((left != rightIndex.end()) && (right != leftIndex.end()) && (left->second != right->second)) {
            // found lines with the same text adjacent to both ends of the current line, merge all three
            unsigned int j = mergeFromLeft(geometries, leftIndex, leftKey, right, geometry);
            unsigned int i = mergeFromRight(geometries, rightIndex, left, rightKey, geometries[j]);

            leftIndex.erase(leftIndex.find(leftKey));
            rightIndex.erase(rightIndex.find(rightKey));
            rightIndex[getKey(feature.label, geometries[i], true)] = i;

        } else if (left != rightIndex.end()) {
            // found mergeable line adjacent to the start of the current line, merge
            mergeFromRight(geometries, rightIndex, left, rightKey, geometry);

        } else if (right != leftIndex.end()) {
            // found mergeable line adjacent to the end of the current line, merge
            mergeFromLeft(geometries, leftIndex, leftKey, right, geometry);

        } else {
            // no adjacent lines, add as a new item
            leftIndex[leftKey] = k;
            rightIndex[rightKey] = k;
        }

    }

}

} // end namespace util
} // end namespace mbgl

#endif
