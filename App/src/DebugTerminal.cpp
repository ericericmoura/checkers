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
		checkers_engine_.Print();

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