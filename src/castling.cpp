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