#pragma once

#include <algorithm>
#include <cstdint>

#include <nonstd/optional.hpp>

#include "chainerx/error.h"

namespace chainerx {

class Slice {
public:
    Slice(nonstd::optional<int64_t> start, nonstd::optional<int64_t> stop, nonstd::optional<int64_t> step)
        : start_{start}, stop_{stop}, step_{step.value_or(1)} {
        if (step_ == 0) {
            throw DimensionError{"Step must not be zero."};
        }
    }
    Slice() = default;
    explicit Slice(int64_t stop) : Slice{0, stop, 1} {}
    Slice(int64_t start, int64_t stop) : Slice{start, stop, 1} {}

    const nonstd::optional<int64_t>& start() const { return start_; }

    const nonstd::optional<int64_t>& stop() const { return stop_; }

    int64_t step() const { return step_; }

    int64_t GetStart(int64_t dim) const {
        if (start_.has_value()) {
            if (*start_ < 0) {
                return std::max(int64_t{0}, *start_ + dim);
            }
            return std::min(*start_, dim - 1);
        }
        return step_ > 0 ? 0 : dim - 1;
    }

    int64_t GetStop(int64_t dim) const {
        if (stop_.has_value()) {
            if (*stop_ < 0) {
                return std::max(int64_t{-1}, *stop_ + dim);
            }
            return std::min(*stop_, dim);
        }
        return step_ > 0 ? dim : -1;
    }

    // Returns the number of elements after slicing an axis of length dim.
    int64_t GetLength(int64_t dim) const {
        // TODO(hvy): Round according to step sign, nicely.
        return std::max(int64_t{0}, (GetStop(dim) - GetStart(dim) + (step_ > 0 ? -1 : 1)) / step_ + 1);
    }

private:
    nonstd::optional<int64_t> start_;
    nonstd::optional<int64_t> stop_;
    int64_t step_{1};
};

}  // namespace chainerx
