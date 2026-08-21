#include <iostream>
#include <string>
#include <variant>

#include <fmt/base.h>

#include "Core/Utils/StringUtils.h"
#include "Core/Debugging/Logging.h"
#include "Utils/CheckersUtils.h"
#include "Enums/Sides.h"
#include "CheckersEngine.h"

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
		std::cout << " - quit\n - redo\n - undo\n - display-moves xy\n - display-captures xy\n:";
		if (!std::getline(std::cin, input))
		{
			return 0;
		}

		if (utils::string::iequals(input, "quit"))
		{
			return 0;
		}
		
		auto result = engine.ExecuteCommand(input);
		if (!result)
		{
			core::debugging::LogError("{}\n", result.error());
			continue;
		}		
	}
}