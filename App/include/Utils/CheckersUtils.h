#pragma once

#include <string>
#include <expected>

#include "CheckersTypes.h"

namespace utils
{
namespace checkers
{

size_t GetBoardIndexFromWorldPosition(sf::Vector2f world_pos) noexcept;

std::expected<std::string, std::string> BitboardIndexToNotation(size_t index) noexcept;

sf::Vector2f BitboardIndexToPosition(size_t index) noexcept;
size_t PositionToBitboardIndex(sf::Vector2f world_pos) noexcept;

bool IsIndexOutOfBounds(size_t index) noexcept;

std::string BitboardToString(checkers_types::bitboard board, char symbol) noexcept;
void LogBitboardWithContrast(checkers_types::bitboard board, char symbol_to_highlight) noexcept;

} // namespace checkers
} // namespace utils