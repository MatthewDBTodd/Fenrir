#pragma once

#include "attack_table.h"
#include "bitboard.h"
#include "types.h"

#include <bit>
#include <cstdint>

std::uint64_t king_attackers(const Bitboard &bb, const AttackTable &at, const Colour colour);
