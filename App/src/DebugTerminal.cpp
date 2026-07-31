#include <iostream>

#include "CheckersEngine.h"

int main()
{
	CheckersEngine checkers_engine_;
	checkers_engine_.InitBoards();
	std::cout << checkers_engine_.ToString() << "\n";
}