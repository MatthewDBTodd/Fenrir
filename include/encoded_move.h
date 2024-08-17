#pragma once

#include <cstdint>

struct EncodedMove {
    std::uint32_t move_type : 3;
    std::uint32_t source_square : 6;
    std::uint32_t dest_square : 6;
    std::uint32_t piece : 3;
    std::uint32_t colour : 2;
    std::uint32_t captured_piece : 3;
    std::uint32_t promoted_piece : 3;
};
