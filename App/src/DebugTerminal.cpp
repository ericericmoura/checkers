#include <iostream>
#include <string>

#include "CheckersEngine.h"

int main()
{
	CheckersEngine checkers_engine_;
	checkers_engine_.Init();

	std::string input;
	while (true)
	{
#ifndef _DEBUG
		std::cout << "\033[H\033[2J" << std::flush;
#endif // !_DEBUG

		checkers_engine_.Print();
		std::cout << "\nEnter command:\n";
		std::cout << "  Q: quit\n  Move example: a1b2\n-";
		if (!(std::cin >> input))
		{
			return 0;
		}

		if (input == "Q")
		{
			return 0;
		}

		checkers_engine_.ExecuteCommand(input);
	}
}