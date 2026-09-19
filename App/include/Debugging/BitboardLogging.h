
#pragma once

#include <iostream>

#include <fmt/base.h>
#include <fmt/color.h>

#include "CheckersTypes.h"
#include "Types/Bitboard.h"

namespace checkers::debugging
{

void LogBitboardWithContrast(Bitboard board, char symbol_to_highlight) noexcept
{
	const auto textboard = board.ToString(symbol_to_highlight);
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

} // namespace checkers::debugging
