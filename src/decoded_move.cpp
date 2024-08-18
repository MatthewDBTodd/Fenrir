#include "assert.h"
#include "decoded_move.h"
#include "direction.h"
#include "move_types.h"

static Square ep_square(const Square dest_square) {
    static constexpr std::uint64_t white_double_push_squares {
        (1ul << A4) | (1ul << B4) | (1ul << C4) | (1ul << D4) |
        (1ul << E4) | (1ul << F4) | (1ul << G4) | (1ul << H4)
    };

    static constexpr std::uint64_t black_double_push_squares {
        (1ul << A5) | (1ul << B5) | (1ul << C5) | (1ul << D5) |
        (1ul << E5) | (1ul << F5) | (1ul << G5) | (1ul << H5)
    };
    const std::uint64_t square_mask { from_square(dest_square) };

    BOOST_ASSERT((square_mask & white_double_push_squares) || 
           (square_mask & black_double_push_squares));
    
    (void)black_double_push_squares;

    if (square_mask & white_double_push_squares) {
        return from_mask(direction::south(square_mask));
    } else {
        return from_mask(direction::north(square_mask));
    }
}

static Square ep_pawn_square(const Square dest_square) {
    static constexpr std::uint64_t white_double_push_squares {
        (1ul << A3) | (1ul << B3) | (1ul << C3) | (1ul << D3) |
        (1ul << E3) | (1ul << F3) | (1ul << G3) | (1ul << H3)
    };

    static constexpr std::uint64_t black_double_push_squares {
        (1ul << A6) | (1ul << B6) | (1ul << C6) | (1ul << D6) |
        (1ul << E6) | (1ul << F6) | (1ul << G6) | (1ul << H6)
    };
    const std::uint64_t square_mask { from_square(dest_square) };

    BOOST_ASSERT((square_mask & white_double_push_squares) || 
           (square_mask & black_double_push_squares));
    
    (void)black_double_push_squares;

    if (square_mask & white_double_push_squares) {
        return from_mask(direction::north(square_mask));
    } else {
        return from_mask(direction::south(square_mask));
    }
}

DecodedMove decode(const EncodedMove encoded_move) {
    move_type_v::Common common {
        static_cast<Square>(encoded_move.source_square),
        static_cast<Square>(encoded_move.dest_square),
        static_cast<Piece>(encoded_move.piece),
        static_cast<Colour>(encoded_move.colour)
    };
    MoveType move_type { static_cast<MoveType>(encoded_move.move_type) };
    switch (move_type) {
        case MoveType::QUIET : 
            return move_type_v::Quiet { common };
        case MoveType::CAPTURE : {
            BOOST_ASSERT(encoded_move.captured_piece < NUM_PIECES);
            const Piece captured { static_cast<Piece>(encoded_move.captured_piece) }; 
            return move_type_v::Capture { common, captured };}
        case MoveType::DOUBLE_PAWN_PUSH : 
            return move_type_v::DoublePawnPush { common, ep_square(common.dest) };
        case MoveType::CASTLE_KINGSIDE : 
            return move_type_v::CastleKingSide { common };
        case MoveType::CASTLE_QUEENSIDE : 
            return move_type_v::CastleQueenSide { common };
        case MoveType::EN_PASSANT : 
            return move_type_v::EnPassant { common, ep_pawn_square(common.dest) };
        case MoveType::MOVE_PROMOTION : {
            BOOST_ASSERT(encoded_move.promoted_piece < NUM_PIECES);
            const Piece promoted { static_cast<Piece>(encoded_move.promoted_piece) };
            return move_type_v::MovePromotion { common, promoted };}
        case MoveType::CAPTURE_PROMOTION : {
            BOOST_ASSERT(encoded_move.captured_piece < NUM_PIECES);
            BOOST_ASSERT(encoded_move.promoted_piece < NUM_PIECES);
            const Piece promoted { static_cast<Piece>(encoded_move.promoted_piece) };
            const Piece captured { static_cast<Piece>(encoded_move.captured_piece) };
            return move_type_v::CapturePromotion { common, captured, promoted };}
        default : 
            throw std::invalid_argument("Unrecognised move type"); // should never happen
    }
}

#ifndef NDEBUG

namespace move_type_v {

std::ostream& operator<<(std::ostream& os, const Common &common) {
    os << "SOURCE: " << common.source << " DEST: " << common.dest << " PIECE: " <<
    common.piece << " COLOUR: " << common.colour << " ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Quiet &quiet) {
    os << "{ QUIET MOVE :: " << quiet.common << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Capture &cap) {
    os << "{ CAPTURE :: " << cap.common << "CAPTURED_PIECE: " << cap.captured_piece << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const DoublePawnPush &dpp) {
    os << "{ DOUBLE PAWN PUSH :: " << dpp.common << "EP_SQUARE: " << dpp.ep_square << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CastleKingSide &cks) {
    os << "{ CASTLE KING SIDE :: " << cks.common << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CastleQueenSide &cqs) {
    os << "{ CASTLE QUEEN SIDE :: " << cqs.common << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const EnPassant &ep) {
    os << "{ EN-PASSANT :: " << ep.common << " PAWN_SQUARE: " << ep.pawn_square << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const MovePromotion &mp) {
    os << "{ MOVE PROMOTION :: " << mp.common << " PROMOTION_PIECE: " << mp.promotion_piece << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CapturePromotion &cp) {
    os << "{ CAPTURE PROMOTION :: " << cp.common << " CAPTURED_PIECE: " << cp.captured_piece <<
    " PROMOTION_PIECE: " << cp.promotion_piece << " }";
    return os;
}

bool operator==(const Common &l, const Common &r) {
    return l.source == r.source
        && l.dest == r.dest
        && l.piece == r.piece
        && l.colour == r.colour;
}

bool operator==(const Quiet &l, const Quiet &r) {
    return l.common == r.common;
}

bool operator==(const Capture &l, const Capture &r) {
    return l.common == r.common && l.captured_piece == r.captured_piece;
}

bool operator==(const DoublePawnPush &l, const DoublePawnPush &r) {
    return l.common == r.common && l.ep_square == r.ep_square;
}

bool operator==(const CastleKingSide &l, const CastleKingSide &r) {
    return l.common == r.common;
}

bool operator==(const CastleQueenSide &l, const CastleQueenSide &r) {
    return l.common == r.common;
}

bool operator==(const EnPassant &l, const EnPassant &r) {
    return l.common == r.common && l.pawn_square == r.pawn_square;
}

bool operator==(const MovePromotion &l, const MovePromotion &r) {
    return l.common == r.common && l.promotion_piece == r.promotion_piece;
}

bool operator==(const CapturePromotion &l, const CapturePromotion &r) {
    return l.common == r.common 
        && l.captured_piece == r.captured_piece
        && l.promotion_piece == r.promotion_piece;
}

bool operator!=(const Common &l, const Common &r) {
    return !(l == r);
}

bool operator!=(const Quiet &l, const Quiet &r) {
    return !(l == r);
}
bool operator!=(const Capture &l, const Capture &r) {
    return !(l == r);
}
bool operator!=(const DoublePawnPush &l, const DoublePawnPush &r) {
    return !(l == r);
}
bool operator!=(const CastleKingSide &l, const CastleKingSide &r) {
    return !(l == r);
}
bool operator!=(const CastleQueenSide &l, const CastleQueenSide &r) {
    return !(l == r);
}
bool operator!=(const EnPassant &l, const EnPassant &r) {
    return !(l == r);
}
bool operator!=(const MovePromotion &l, const MovePromotion &r) {
    return !(l == r);
}
bool operator!=(const CapturePromotion &l, const CapturePromotion &r) {
    return !(l == r);
}

} // namespace move_type_v

std::ostream& operator<<(std::ostream& os, const DecodedMove &move) {
    std::visit([&os](auto&& variant) {
        os << variant;
    }, move);
    return os;
}

#endif // ifndef NDEBUG