#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <optional>

#include "Constants/CheckersConstants.h"
#include "Enums/Directions.h"
#include "Enums/Sides.h"

using bitboard = uint64_t;
using ray_list = std::array<std::array<bitboard, checkers_constants::col_count_* checkers_constants::row_count_>, static_cast<int>(DiagonalDirections::kCount)>;

namespace utils
{
namespace checkers
{

namespace pawn
{

bitboard GetPossibleMovesForPawn(Sides side, size_t index) noexcept;
std::optional<size_t> CapturePieceWithPawn(size_t from, size_t to, VerticalDirections dir_y) noexcept;
bitboard GetPawnCaptures(Sides side, bitboard black_bb, bitboard white_bb, bitboard pawns) noexcept;
bitboard GetPawnCaptures(Sides side, bitboard black_bb, bitboard white_bb, bitboard pawns, size_t pawn_i) noexcept;

} // namespace pawn

bool IsIndexOutOfBounds(size_t index) noexcept;

std::string BitboardToString(bitboard board, char symbol) noexcept;
void LogBitboardWithContrast(bitboard board, char symbol_to_highlight) noexcept;

} // namespace checkers
} // namespace utils