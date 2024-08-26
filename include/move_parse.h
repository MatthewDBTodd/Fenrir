#pragma once

#include "encoded_move.h"

#include <optional>
#include <string_view>

class Board;

std::optional<EncodedMove> parse_move_input(std::string_view input, const Board &board);

std::string move_to_string(const EncodedMove move);