#pragma once

#include <string>
#include <expected>

#include "CheckersTypes.h"

namespace checkers::position_mapping
{

size_t WorldPositionToBoardIndex(sf::Vector2f world_pos) noexcept;
size_t WorldPositionToBitboardIndex(sf::Vector2f world_pos) noexcept;

sf::Vector2f BitboardIndexToWorldPosition(size_t index) noexcept;

} // namespace checkers::position_mapping