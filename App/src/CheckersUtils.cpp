#include "Utils/CheckersUtils.h"

#include <string>
#include <iostream>
#include <expected>

#include <fmt/base.h>
#include <fmt/color.h>

#include "Core/Utils/BitUtils.h"
#include "Constants/CheckersConstants.h"
#include "CheckersTypes.h"

size_t utils::checkers::GetBoardIndexFromWorldPosition(sf::Vector2f world_pos) noexcept
{
	const auto board_pos = world_pos - sf::Vector2f(checkers_constants::margin_);
	const auto row = std::floor(board_pos.y / checkers_constants::cell_size_);
	const auto col = std::floor(board_pos.x / checkers_constants::cell_size_);
	return static_cast<size_t>(row * checkers_constants::col_count_ + col);	
}

std::expected<std::string, std::string> utils::checkers::BitboardIndexToNotation(size_t index) noexcept
{
	if (index >= checkers_constants::total_squares_)
	{
		return std::unexpected("Invalid bitboard index: out of bounds.");
	}
	
	const auto row = index / checkers_constants::col_count_ + 1;
	const auto col = checkers_constants::files[index % checkers_constants::col_count_];

	return std::string(col + std::to_string(row));
}

sf::Vector2f utils::checkers::BitboardIndexToPosition(size_t index) noexcept
{
	const auto row = (index / checkers_constants::col_count_);
	const auto col = (index % checkers_constants::col_count_);
	if (typeid(checkers_types::bitboard) == typeid(uint64_t))
	{
		const auto top_offset  = static_cast<float>((checkers_constants::cell_size_ * (checkers_constants::row_count_ - 1)) - (checkers_constants::cell_size_ * row) + checkers_constants::margin_.y);
		const auto left_offset = static_cast<float>((checkers_constants::cell_size_ * col) + checkers_constants::margin_.x);
		return sf::Vector2f({ left_offset, top_offset });
	}
	assert(false);
	return sf::Vector2f();
}

size_t utils::checkers::PositionToBitboardIndex(sf::Vector2f world_pos) noexcept
{
	const auto index = GetBoardIndexFromWorldPosition(world_pos);
	
	auto row = index / checkers_constants::col_count_;
	row = checkers_constants::row_count_ - 1 - row;

	const auto col = index % checkers_constants::col_count_;

	return size_t(row * checkers_constants::col_count_ + col);
}

bool utils::checkers::IsIndexOutOfBounds(size_t index) noexcept
{
	return index < 0 || index >= checkers_constants::total_squares_;
}

std::string utils::checkers::BitboardToString(checkers_types::bitboard board, char symbol) noexcept
{
	std::string output = "\n";
	for (int rank = checkers_constants::row_count_ - 1; rank >= 0; --rank)
	{
		output += (rank + 1) + '0';
		output += "  ";
		for (int file = 0; file < checkers_constants::col_count_; ++file)
		{
			const auto index = file + rank * checkers_constants::col_count_;
			if (!core::utils::bits::IsBitSet(board, index))
			{
				output += '0';
				output += " ";
				continue;
			}
			output += symbol;
			output += " ";
		}
		output += "\n";
	}
	output += "\n   a b c d e f g h";
	return output;
}

void utils::checkers::LogBitboardWithContrast(checkers_types::bitboard board, char symbol_to_highlight) noexcept
{
	const auto textboard = BitboardToString(board, symbol_to_highlight);
	std::cout << "\n";
	for (const auto& letter : textboard)
	{
		if (letter == symbol_to_highlight)
		{
			fmt::print(fg(fmt::color::orange) | fmt::emphasis::italic, "{}", letter);
			continue;
		}
		std::cout << letter;
	}
}