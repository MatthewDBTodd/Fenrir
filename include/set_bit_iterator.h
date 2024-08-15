#pragma once

#include <cstdint>
#include <iterator>

class SetBits {
public:
    explicit SetBits(const std::uint64_t mask) :
        mask(mask)
    {}

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::uint64_t;
        using difference_type = std::ptrdiff_t;
        using pointer = std::uint64_t*;
        using reference = std::uint64_t&;

        explicit Iterator(const std::uint64_t current) :
            current(current),
            lowest_set_bit(current & -current)
        {}

        value_type operator*() {
            return lowest_set_bit;
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
                && lowest_set_bit == other.lowest_set_bit;
        }
    private:
        void next() {
            current ^= lowest_set_bit;
            lowest_set_bit = (current & -current);
        }

        std::uint64_t current;
        std::uint64_t lowest_set_bit;
    };

    Iterator begin() const {
        return Iterator(mask);
    }

    Iterator end() const {
        return Iterator(0ul);
    }
private:
    const std::uint64_t mask;
};
