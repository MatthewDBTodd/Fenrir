#pragma once

#include "decoded_move.h"

#include <optional>
#include <string_view>

class Board;

std::optional<DecodedMove> parse_move_input(std::string_view input, const Board &board);