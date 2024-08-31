#pragma once

#include "move_types.h"
#include "types.h"

#include <cstdint>

#ifdef FENRIR_TEST
#include <iostream>
#endif

struct EncodedMove {
    EncodedMove(const MoveType move_type, const Square source, const Square dest,
                const Piece piece, const Colour colour, const Piece captured,
                const Piece promoted);
    std::uint32_t move_type : 3;
    std::uint32_t source_square : 6;
    std::uint32_t dest_square : 6;
    std::uint32_t piece : 3;
    std::uint32_t colour : 2;
    std::uint32_t captured_piece : 3;
    std::uint32_t promoted_piece : 3;
};

bool operator==(const EncodedMove l, const EncodedMove r);
bool operator!=(const EncodedMove l, const EncodedMove r);
std::ostream& operator<<(std::ostream& os, const EncodedMove move);
