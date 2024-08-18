#pragma once

#include "encoded_move.h"
#include "types.h"

#include <variant>

#ifndef NDEBUG
#include <iostream>
#endif


namespace move_type_v {

struct Common {
    Square source;
    Square dest;
    Piece piece;
    Colour colour;
};

struct Quiet {
    Common common;
};
struct Capture{
    Common common;
    Piece captured_piece;
};

struct DoublePawnPush {
    Common common;
    Square ep_square;
};

struct CastleKingSide {
    Common common;
};

struct CastleQueenSide {
    Common common;
};

struct EnPassant {
    Common common;
    Square pawn_square;
};

struct MovePromotion {
    Common common;
    Piece promotion_piece;
};

struct CapturePromotion {
    Common common;
    Piece captured_piece;
    Piece promotion_piece;
};

} // namespace move_type_v;

namespace move_action_v {

struct Make {};
struct UnMake {};

} // namespace move_action_v

using DecodedMove = std::variant<
    move_type_v::Quiet,
    move_type_v::Capture,
    move_type_v::DoublePawnPush,
    move_type_v::CastleKingSide,
    move_type_v::CastleQueenSide,
    move_type_v::EnPassant,
    move_type_v::MovePromotion,
    move_type_v::CapturePromotion
>;

using MoveAction = std::variant<
    move_action_v::Make,
    move_action_v::UnMake
>;

DecodedMove decode(const EncodedMove encoded_move);

#ifndef NDEBUG

namespace move_type_v {

std::ostream& operator<<(std::ostream& os, const Common &common);
std::ostream& operator<<(std::ostream& os, const Quiet &quiet);
std::ostream& operator<<(std::ostream& os, const Capture &cap);
std::ostream& operator<<(std::ostream& os, const DoublePawnPush &dpp);
std::ostream& operator<<(std::ostream& os, const CastleKingSide &cks);
std::ostream& operator<<(std::ostream& os, const CastleQueenSide &cqs);
std::ostream& operator<<(std::ostream& os, const EnPassant &ep);
std::ostream& operator<<(std::ostream& os, const MovePromotion &mp);
std::ostream& operator<<(std::ostream& os, const CapturePromotion &cp);

bool operator==(const Common &l, const Common &r);
bool operator==(const Quiet &l, const Quiet &r);
bool operator==(const Capture &l, const Capture &r);
bool operator==(const DoublePawnPush &l, const DoublePawnPush &r);
bool operator==(const CastleKingSide &l, const CastleKingSide &r);
bool operator==(const CastleQueenSide &l, const CastleQueenSide &r);
bool operator==(const EnPassant &l, const EnPassant &r);
bool operator==(const MovePromotion &l, const MovePromotion &r);
bool operator==(const CapturePromotion &l, const CapturePromotion &r);

bool operator!=(const Common &l, const Common &r);
bool operator!=(const Quiet &l, const Quiet &r);
bool operator!=(const Capture &l, const Capture &r);
bool operator!=(const DoublePawnPush &l, const DoublePawnPush &r);
bool operator!=(const CastleKingSide &l, const CastleKingSide &r);
bool operator!=(const CastleQueenSide &l, const CastleQueenSide &r);
bool operator!=(const EnPassant &l, const EnPassant &r);
bool operator!=(const MovePromotion &l, const MovePromotion &r);
bool operator!=(const CapturePromotion &l, const CapturePromotion &r);

} // namespace move_type_v

std::ostream& operator<<(std::ostream& os, const DecodedMove &move);

#endif // ifndef NDEBUG