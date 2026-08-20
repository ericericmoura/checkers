#include "Utils/CheckersUtils.h"

#include <string>
#include <iostream>
#include <cmath>
#include <optional>

#include <fmt/base.h>
#include <fmt/color.h>

#include "Core/Utils/BitUtils.h"
#include "Constants/CheckersConstants.h"
#include "Enums/Sides.h"
#include "Enums/Directions.h"
#include "CheckersTypes.h"

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

std::optional<size_t> utils::checkers::pawn::CapturePieceWithPawn(size_t from, size_t to, VerticalDirections dir_y) noexcept
{
	const auto from_col = from % 8;
	const auto from_row = from / 8;
	const auto to_col   = to   % 8;
	const auto to_row   = to   / 8;

	const auto distance_y = std::abs(static_cast<int>(from_col) - static_cast<int>(to_col));
	const auto distance_x = std::abs(static_cast<int>(from_row) - static_cast<int>(to_row));

	if (distance_y != 2 || distance_x != 2)
	{
		return {};
	}

	const auto is_east = from_col > to_col;
	const auto is_west = from_col < to_col;

	std::optional<size_t> enemy_i{};
	if (dir_y == VerticalDirections::kUp && is_west)
	{
		enemy_i = from + 9;
	}
	if (dir_y == VerticalDirections::kUp && is_east)
	{
		enemy_i = from + 7;
	}
	if (dir_y == VerticalDirections::kDown && is_west)
	{
		enemy_i = from - 7;
	}
	if (dir_y == VerticalDirections::kDown && is_east)
	{
		enemy_i = from - 9;
	}
	return enemy_i;
}