#include "castling.h"

std::optional<CastlingRights> CastlingRights::from_fen(std::string_view castling) {
    std::uint8_t castling_mask {};
    if (castling == "-") {
        return CastlingRights(castling_mask);
    }

    if (castling.size() > 4) {
        return std::nullopt;
    }

    for (const char c : castling) {
        switch (c) {
            case 'K': castling_mask |= mask(WHITE, KING); break;
            case 'Q': castling_mask |= mask(WHITE, QUEEN); break;
            case 'k': castling_mask |= mask(BLACK, KING); break;
            case 'q': castling_mask |= mask(BLACK, QUEEN); break;
            default: return std::nullopt;
        }
    }

    return CastlingRights(castling_mask);
}

void CastlingRights::update_castling(const DecodedMove &move) {
    std::visit(*this, move);
}

void CastlingRights::operator()(const move_type_v::Quiet &quiet) {
    // While the can_castle calls aren't strictly needed, the hope is it will
    // help the branch predictor once the castling has been invalidated as it will 
    // always be false
    if (can_castle(quiet.common.colour) && quiet.common.piece == KING) {
        invalidate_both(quiet.common.colour);
    }

    // While this technically invalidates castling if a piece other than a rook
    // moves from the rook starting squares, that wouldn't be possible if the rook
    // has already been captured or moved from that square earlier so actually that
    // can't happen UNLESS you're playing chess-960. But I've not been programming
    // this engine with chess-960 even being considered, I doubt I'll ever get to
    // that. 
    if (can_castle(WHITE, QUEEN) && quiet.common.source == A1) {
        invalidate(WHITE, QUEEN);
    } else if (can_castle(WHITE, KING) && quiet.common.source == H1) {
        invalidate(WHITE, KING);
    } else if (can_castle(BLACK, QUEEN) && quiet.common.source == A8) {
        invalidate(BLACK, QUEEN);
    } else if (can_castle(BLACK, KING) && quiet.common.source == H8) {
        invalidate(BLACK, KING);
    }
}

void CastlingRights::operator()(const move_type_v::Capture &cap) {
    if (can_castle(cap.common.colour) && cap.common.piece == KING) {
        invalidate_both(cap.common.colour);
    }

    check_capture(cap.common.dest);
}

void CastlingRights::operator()(const move_type_v::CastleKingSide &cks) {
    invalidate_both(cks.common.colour);
}

void CastlingRights::operator()(const move_type_v::CastleQueenSide &cqs) {
    invalidate_both(cqs.common.colour);
}

void CastlingRights::operator()(const move_type_v::CapturePromotion &cp) {
    check_capture(cp.common.dest);
}

void CastlingRights::check_capture(const Square dest_square) {
    if (can_castle(WHITE, QUEEN) && dest_square == A1) {
        invalidate(WHITE, QUEEN);
    } else if (can_castle(WHITE, KING) && dest_square == H1) {
        invalidate(WHITE, KING);
    } else if (can_castle(BLACK, QUEEN) && dest_square == A8) {
        invalidate(BLACK, QUEEN);
    } else if (can_castle(BLACK, KING) && dest_square == H8) {
        invalidate(BLACK, KING);
    }
}