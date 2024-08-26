#include "encoded_move.h"

EncodedMove::EncodedMove(const MoveType type, const Square source, const Square dest,
                         const Piece piece_type, const Colour friendly_colour, const Piece captured,
                         const Piece promoted) :
        move_type { static_cast<std::uint32_t>(type) },
        source_square { static_cast<std::uint32_t>(source) },
        dest_square { static_cast<std::uint32_t>(dest) },
        piece { static_cast<std::uint32_t>(piece_type) },
        colour { static_cast<std::uint32_t>(friendly_colour) },
        captured_piece { static_cast<std::uint32_t>(captured) },
        promoted_piece { static_cast<std::uint32_t>(promoted) }
{}


bool operator==(const EncodedMove l, const EncodedMove r) {
    return l.move_type == r.move_type
        && l.source_square == r.source_square
        && l.dest_square == r.dest_square
        && l.piece == r.piece
        && l.colour == r.colour
        && l.captured_piece == r.captured_piece
        && l.promoted_piece == r.promoted_piece;
}

bool operator!=(const EncodedMove l, const EncodedMove r) {
    return !(l == r);
}

#ifdef FENRIR_TEST

std::ostream& operator<<(std::ostream& os, const EncodedMove move) {
    os << "{ TYPE: " << move.move_type << " SRC: " << move.source_square << 
    " DEST: " << move.dest_square << " PIECE: " << move.piece << " COLOUR: " <<
    move.colour << " CAPTURED_PIECE: " << move.captured_piece << 
    " PROMOTED_PIECES: " << move.promoted_piece << " }";

    return os;
}

#endif // FENRIR_TEST
