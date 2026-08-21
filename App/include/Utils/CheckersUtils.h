#pragma once

#include <string>
#include <optional>

#include "Enums/Directions.h"
#include "CheckersTypes.h"

namespace utils
{
namespace checkers
{

bool IsIndexOutOfBounds(size_t index) noexcept;

std::string BitboardToString(checkers_types::bitboard board, char symbol) noexcept;
void LogBitboardWithContrast(checkers_types::bitboard board, char symbol_to_highlight) noexcept;

} // namespace checkers
} // namespace utils