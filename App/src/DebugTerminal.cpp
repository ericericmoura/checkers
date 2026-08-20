#include <iostream>
#include <string>
#include <variant>

#include <fmt/base.h>

#include "Core/Utils/StringUtils.h"
#include "CommandParser.h"

#include "Utils/CheckersUtils.h"
#include "MoveGenerator.h"

int main()
{
	MoveGenerator mv_gen{};

	std::string input{};
	while (true)
	{
#ifndef _DEBUG
		std::cout << "\033[H\033[2J" << std::flush;
#endif // !_DEBUG

		std::cout << "\nEnter command:\n";
		std::cout << " - quit\n - redo\n - undo\n - display-moves xy\n - display-captures xy\n:";
		if (!std::getline(std::cin, input))
		{
			return 0;
		}

		if (utils::string::iequals(input, "quit"))
		{
			return 0;
		}
		
		auto parse_result = CommandParser::ParseCommand(input);
		if (!parse_result)
		{
			std::cout << "\n" << parse_result.error() << "\n";
			continue;
		}	
		if (auto value = std::get_if<CommandMove>(&parse_result.value()))
		{
			fmt::println("\nFrom: {}, to: {}", value->move_from_, value->move_to_);
		}
		else if (auto value = std::get_if<CommandRedo>(&parse_result.value()))
		{
			fmt::println("\nRedoing last action!");
		}
		else if (auto value = std::get_if<CommandUndo>(&parse_result.value()))
		{
			fmt::println("\nUndoing last action!");
		}
		else if (auto value = std::get_if<CommandDisplayMoves>(&parse_result.value()))
		{			
			fmt::println("\nDisplaying moves:");
			utils::checkers::LogBitboardWithContrast(mv_gen.GetMovementsForQueen(value->piece_index_, 0, 0), 'M');
		}		
		else if (auto value = std::get_if<CommandDisplayCaptures>(&parse_result.value()))
		{
			fmt::println("\nBlockers:");
			utils::checkers::LogBitboardWithContrast(0xA0000A000A00, 'B');

			fmt::println("\nDisplaying captures:");
			utils::checkers::LogBitboardWithContrast(mv_gen.GetCapturesForQueen(value->piece_index_, 0xA0000A000A00, 0), 'M');
		}
	}
}