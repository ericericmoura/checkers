#pragma once

#include <string>
#include <optional>

#include "Enums/Directions.h"
#include "CheckersTypes.h"

namespace utils
{
namespace checkers
{

namespace pawn
{

std::optional<size_t> CapturePieceWithPawn(size_t from, size_t to, VerticalDirections dir_y) noexcept;

} // namespace pawn

bool IsIndexOutOfBounds(size_t index) noexcept;

std::string BitboardToString(checkers_types::bitboard board, char symbol) noexcept;
void LogBitboardWithContrast(checkers_types::bitboard board, char symbol_to_highlight) noexcept;

} // namespace checkers
} // namespace utils