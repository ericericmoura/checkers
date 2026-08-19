#include <iostream>
#include <string>
#include <variant>

#include "Core/Utils/StringUtils.h"
#include "CommandParser.h"

int main()
{
	std::string input;
	while (true)
	{
#ifndef _DEBUG
		std::cout << "\033[H\033[2J" << std::flush;
#endif // !_DEBUG

		std::cout << "\nEnter command:\n";
		std::cout << "  Quit\n  Redo\n  Undo\n:";
		if (!(std::cin >> input))
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
			std::cout << "\nfrom: " << value->move_from_ << ", to: " << value->move_to_ << "\n";
		}
		else if (auto value = std::get_if<CommandRedo>(&parse_result.value()))
		{
			std::cout << "\nRedoing last action!\n";
		}
		else if (auto value = std::get_if<CommandUndo>(&parse_result.value()))
		{
			std::cout << "\nUndoing last action!\n";
		}
	}
}