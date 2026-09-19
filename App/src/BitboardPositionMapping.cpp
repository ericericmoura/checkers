#include "BitboardPositionMapping.h"

size_t checkers::WorldPositionToBoardIndex(sf::Vector2f world_pos) noexcept
{
	const auto board_pos = world_pos - sf::Vector2f(checkers_constants::margin_);
	const auto row = std::floor(board_pos.y / checkers_constants::cell_size_);
	const auto col = std::floor(board_pos.x / checkers_constants::cell_size_);
	return static_cast<size_t>(row * checkers_constants::col_count_ + col);
}

size_t checkers::WorldPositionToBitboardIndex(sf::Vector2f world_pos) noexcept
{
	const auto index = GetBoardIndexFromWorldPosition(world_pos);

	auto row = index / checkers_constants::col_count_;
	row = checkers_constants::row_count_ - 1 - row;

	const auto col = index % checkers_constants::col_count_;

	return size_t(row * checkers_constants::col_count_ + col);
}

sf::Vector2f checkers::BitboardIndexToWorldPosition(size_t index) noexcept
{
	const auto row = (index / checkers_constants::col_count_);
	const auto col = (index % checkers_constants::col_count_);
	if (typeid(checkers_types::bitboard) == typeid(uint64_t))
	{
		const auto top_offset = static_cast<float>((checkers_constants::cell_size_ * (checkers_constants::row_count_ - 1)) - (checkers_constants::cell_size_ * row) + checkers_constants::margin_.y);
		const auto left_offset = static_cast<float>((checkers_constants::cell_size_ * col) + checkers_constants::margin_.x);
		return sf::Vector2f({ left_offset, top_offset });
	}
	assert(false);
	return sf::Vector2f();
}