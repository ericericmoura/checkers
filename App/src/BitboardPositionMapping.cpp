#include "BitboardPositionMapping.h"

#include <SFML/System/Vector2.hpp>

#include "Constants/CheckersConstants.h"

size_t checkers::position_mapping::WorldPositionToBoardIndex(sf::Vector2f world_pos) noexcept
{
	const auto board_pos = world_pos - sf::Vector2f(checkers::constants::margin_);
	const auto row = std::floor(board_pos.y / checkers::constants::cell_size_);
	const auto col = std::floor(board_pos.x / checkers::constants::cell_size_);
	return static_cast<size_t>(row * checkers::constants::col_count_ + col);
}

size_t checkers::position_mapping::WorldPositionToBitboardIndex(sf::Vector2f world_pos) noexcept
{
	const auto index = WorldPositionToBoardIndex(world_pos);

	auto row = index / checkers::constants::col_count_;
	row = checkers::constants::row_count_ - 1 - row;

	const auto col = index % checkers::constants::col_count_;

	return size_t(row * checkers::constants::col_count_ + col);
}

sf::Vector2f checkers::position_mapping::BitboardIndexToWorldPosition(size_t index) noexcept
{
	const auto row = (index / checkers::constants::col_count_);
	const auto col = (index % checkers::constants::col_count_);

	const auto top_offset = static_cast<float>((checkers::constants::cell_size_ * (checkers::constants::row_count_ - 1)) - (checkers::constants::cell_size_ * row) + checkers::constants::margin_.y);
	const auto left_offset = static_cast<float>((checkers::constants::cell_size_ * col) + checkers::constants::margin_.x);
	return sf::Vector2f({ left_offset, top_offset });
}
