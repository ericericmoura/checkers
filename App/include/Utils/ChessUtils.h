#pragma once

#include <cstdint>
#include <string>
#include <array>

#include "Constants/ChessConstants.h"
#include "Enums/Directions.h"

using bitboard = uint64_t;
using ray_list = std::array<std::array<bitboard, chess_constants::col_count_* chess_constants::row_count_>, static_cast<int>(DiagonalDirections::kCount)>;

namespace utils
{

std::string BitboardToString(bitboard board, char symbol) noexcept;

void LogBitboardWithContrast(bitboard board, char symbol_to_highlight) noexcept;

} // namespace utils