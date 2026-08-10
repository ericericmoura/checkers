#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <optional>

#include "Constants/ChessConstants.h"
#include "Enums/Directions.h"
#include "Enums/Sides.h"

using bitboard = uint64_t;
using ray_list = std::array<std::array<bitboard, chess_constants::col_count_* chess_constants::row_count_>, static_cast<int>(DiagonalDirections::kCount)>;

namespace utils
{
namespace pawn
{

bitboard GetPossibleMovesForPawn(Sides side, size_t index) noexcept;
std::optional<size_t> CapturePieceWithPawn(size_t from, size_t to, VerticalDirections dir_y) noexcept;
bitboard GetPawnCaptures(Sides side, bitboard black_bb, bitboard white_bb, bitboard pawns) noexcept;

} // namespace pawn

std::string BitboardToString(bitboard board, char symbol) noexcept;

void LogBitboardWithContrast(bitboard board, char symbol_to_highlight) noexcept;

} // namespace utils