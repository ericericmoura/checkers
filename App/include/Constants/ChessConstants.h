#pragma once

#include <unordered_map>

#include <SFML/System/Vector2.hpp>

namespace chess_constants
{

constexpr int cell_size_ = 22;
constexpr int row_count_ = 8;
constexpr int col_count_ = 8;
constexpr sf::Vector2u margin_ = {40, 40};

} // namespace chess_constants