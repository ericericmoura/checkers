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

bool utils::checkers::IsIndexOutOfBounds(size_t index) noexcept
{
	return index < 0 || index >= checkers_constants::total_squares_;
}

std::string utils::checkers::BitboardToString(bitboard board, char symbol) noexcept
{
	std::string output = "\n";
	for (int rank = checkers_constants::row_count_ - 1; rank >= 0; --rank)
	{
		output += (rank + 1) + '0';
		output += "  ";
		for (int file = 0; file < checkers_constants::col_count_; ++file)
		{
			const auto index = file + rank * checkers_constants::col_count_;
			if (!core::utils::IsBitSet(board, index))
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

void utils::checkers::LogBitboardWithContrast(bitboard board, char symbol_to_highlight) noexcept
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

bitboard utils::checkers::pawn::GetPossibleMovesForPawn(Sides side, size_t index) noexcept
{
	bitboard result = 0;
	const auto sign = side == Sides::kWhite ? +1 : -1;
	result |= 0x1ull << index + 9 * static_cast<unsigned long long>(sign);
	result |= 0x1ull << index + 7 * static_cast<unsigned long long>(sign);
	return result;
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

bitboard utils::checkers::pawn::GetPawnCaptures(Sides side, bitboard black_bb, bitboard white_bb, bitboard pawns) noexcept
{
	const auto east_pawns = (pawns & ~(checkers_constants::file_h | checkers_constants::file_g));
	const auto west_pawns = (pawns & ~(checkers_constants::file_a | checkers_constants::file_b));

	auto opposite_side_bb = black_bb;
	auto north_east = east_pawns << 9;
	auto north_west = west_pawns << 7;
	auto side_text = "White";

	if (side == Sides::kBlack)
	{
		opposite_side_bb = white_bb;
		side_text = "Black";
		north_east = east_pawns >> 7;
		north_west = west_pawns >> 9;
	}

	const auto north_east_mask = north_east & opposite_side_bb;
	const auto north_west_mask = north_west & opposite_side_bb;

	bitboard captures = 0;

	auto moved_north_east_mask = north_east_mask << 9;
	auto moved_north_west_mask = north_west_mask << 7;

	if (side == Sides::kBlack)
	{
		moved_north_east_mask = north_east_mask >> 7;
		moved_north_west_mask = north_west_mask >> 9;
	}

	captures |= moved_north_east_mask & ~(black_bb | white_bb);
	captures |= moved_north_west_mask & ~(black_bb | white_bb);

	return captures;
}

bitboard utils::checkers::pawn::GetPawnCaptures(Sides side, bitboard black_bb, bitboard white_bb, bitboard pawns, size_t pawn_i) noexcept
{
	return GetPawnCaptures(side, black_bb, white_bb, pawns & (0x1ull << pawn_i));
}
