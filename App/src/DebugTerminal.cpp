#include <iostream>
#include <string>
#include <expected>
#include <variant>

#include <fmt/base.h>

#include "Core/Utils/StringUtils.h"
#include "Core/Debugging/Logging.h"
#include "Utils/CheckersUtils.h"
#include "CheckersEngine.h"
#include "CommandParser.h"

int main()
{
	CheckersEngine engine{};

	std::string input{};
	while (true)
	{
#ifndef _DEBUG
		std::cout << "\033[H\033[2J" << std::flush;
#endif // !_DEBUG

		engine.Print();

		std::cout << "\nEnter command:\n";
		std::cout << " - quit\n - redo\n - undo\n - display-moves xy\n - display-captures xy\n - move xyza\n:";
		if (!std::getline(std::cin, input))
		{
			return 0;
		}
		if (utils::string::iequals(input, "quit"))
		{
			return 0;
		}

		command_parser::RunCommand(engine, input).transform(
			[](const auto& error_message)
			{
				core::debugging::LogError("Command failed: {}", error_message);
			}
		);
	}
}