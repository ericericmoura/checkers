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