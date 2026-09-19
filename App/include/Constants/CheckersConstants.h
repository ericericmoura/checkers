#pragma once

#include <cstdint>
#include <array>

#include <SFML/System/Vector2.hpp>

namespace checkers::constants
{

constexpr int cell_size_ = 16;
constexpr int row_count_ = 8;
constexpr int col_count_ = 8;
constexpr int total_squares_ = row_count_ * col_count_;

constexpr sf::Vector2u margin_ = {7, 7};

constexpr uint64_t file_a = 0x0101010101010101;
constexpr uint64_t file_b = 0x0202020202020202;
constexpr uint64_t file_g = 0x4040404040404040;
constexpr uint64_t file_h = 0x8080808080808080;

constexpr std::array<char, 8> files = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

} // namespace checkers::constants