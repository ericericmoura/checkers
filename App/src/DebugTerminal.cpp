#include <iostream>
#include <string>

#include "CheckersEngine.h"

int main()
{
	CheckersEngine checkers_engine_;
	checkers_engine_.Init();

	return 0;

	std::string input;
	while (true)
	{
		std::cout << checkers_engine_.ToString() << "\n";

		std::cout << "\nEnter command:\n";
		std::cout << "  Q: quit\n  Move example: a1b2\n-";
		std::cin >> input;

		if (input == "Q")
		{
			return 0;
		}

		checkers_engine_.ExecuteCommand(input);
	}
}