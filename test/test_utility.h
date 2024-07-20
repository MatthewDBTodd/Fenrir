#pragma once

#include "types.h"

#include <cstdint>

struct AttackSquaresTestCase {
    const Square square;
    const std::uint64_t attack_mask;
};

