#ifndef LLMR_STYLE_FILTER_COMPARISON_PRIVATE
#define LLMR_STYLE_FILTER_COMPARISON_PRIVATE

#include "filter_comparison.hpp"

namespace llmr {

template <typename Extractor>
inline bool FilterComparison::compare(const Extractor &extractor) const {
    const std::vector<Value> values = extractor.getValues(field);

    // All instances are ANDed together.
    for (const Instance &instance : instances) {
        if (!instance.compare(values)) {
            return false;
        }
    }
    return true;
}

}

#endif
