#include "Utils/ChessUtils.h"

#include <string>
#include <iostream>

#include <fmt/base.h>
#include <fmt/color.h>

#include "Constants/ChessConstants.h"
#include "Core/Utils/BitUtils.h"

std::string utils::BitboardToString(bitboard board, char symbol) noexcept
{
	std::string output = "\n";
	for (int rank = chess_constants::row_count_ - 1; rank >= 0; --rank)
	{
		output += (rank + 1) + '0';
		output += "  ";
		for (int file = 0; file < chess_constants::col_count_; ++file)
		{
			const auto index = file + rank * chess_constants::col_count_;
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

void utils::LogBitboardWithContrast(bitboard board, char symbol_to_highlight) noexcept
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