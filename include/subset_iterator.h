#pragma once

#include <cstdint>
#include <iterator>

namespace utility {

// Iterator to iterate over all subsets of a uint64_t bitmask
class Subsets {
public:
    explicit Subsets(const std::uint64_t mask) :
        mask(mask)
    {}

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::uint64_t;
        using difference_type = std::ptrdiff_t;
        using pointer = std::uint64_t*;
        using reference = std::uint64_t&;

        Iterator(const std::uint64_t current, const std::uint64_t mask, 
                 const bool finished) :
            current(current),
            mask(mask),
            finished(finished)
        {}

        value_type operator*() {
            return current;
        }

        // prefix
        Iterator& operator++() {
            next();
            return *this;
        }

        // postfix
        Iterator operator++(int) {
            Iterator tmp { *this };
            next();
            return tmp;
        }

        bool operator==(const Iterator &other) const {
            return current == other.current 
                && mask == other.mask
                && finished == other.finished;
        }

        bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }
    private:
        std::uint64_t current {};
        std::uint64_t mask {};
        bool finished {};

        void next() {
            current = (current - mask) & mask;
            if (current == 0) {
                finished = true;
            }
        }
    };

    Iterator begin() const {
        return Iterator(0ull, mask, false);
    }

    Iterator end() const {
        return Iterator(0ull, mask, true);
    }
private:
    const std::uint64_t mask;
};

} // namespace utility