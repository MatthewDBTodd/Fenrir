#pragma once

#include <cstdint>

#ifdef FENRIR_TEST
#include <iostream>
#endif

struct EncodedMove {
    std::uint32_t move_type : 3;
    std::uint32_t source_square : 6;
    std::uint32_t dest_square : 6;
    std::uint32_t piece : 3;
    std::uint32_t colour : 2;
    std::uint32_t captured_piece : 3;
    std::uint32_t promoted_piece : 3;
};

#ifdef FENRIR_TEST

inline std::ostream& operator<<(std::ostream& os, const EncodedMove move) {
    os << "{ TYPE: " << move.move_type << " SRC: " << move.source_square << 
    " DEST: " << move.dest_square << " PIECE: " << move.piece << " COLOUR: " <<
    move.colour << " CAPTURED_PIECE: " << move.captured_piece << 
    " PROMOTED_PIECES: " << move.promoted_piece << " }";

    return os;
}

#endif