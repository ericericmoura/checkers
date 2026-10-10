#include <iostream>
#include <string>
#include <expected>
#include <variant>

#include <fmt/base.h>

#include "Core/Utils/StringUtils.h"
#include "Core/Debugging/Logging.h"
#include "Debugging/BitboardLogging.h"
#include "CheckersEngine.h"
#include "CommandParser.h"

int main()
{
	checkers::CheckersEngine engine{};

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

		checkers::command_parser::ParseCommand(input)
			.transform(
				[&engine](const auto& command)
				{
					if (auto value = std::get_if<checkers::CommandMove>(&command))
					{
						const auto result = engine.MovePiece(value->move_from_, value->move_to_);
						if (!result)
						{
							core::debugging::LogError("{}\n", result.error());
						}
						return;
					}
					if (auto value = std::get_if<checkers::CommandDisplayMoves>(&command))
					{
						const auto piece_moves = engine.GetMoves(value->piece_index_);
						if (!piece_moves)
						{
							core::debugging::LogError("{}\n", piece_moves.error());
							return;
						}
						fmt::print("\nMoves for piece at index {}:", value->piece_index_);
						checkers::debugging::LogBitboardWithContrast(piece_moves.value(), 'M');
						return;
					}
					if (auto value = std::get_if<checkers::CommandDisplayCaptures>(&command))
					{
						const auto piece_captures = engine.GetCaptures(value->piece_index_);
						if (!piece_captures)
						{
							core::debugging::LogError("{}\n", piece_captures.error());
							return;
						}
						fmt::print("\nCaptures for piece at index {}:", value->piece_index_);
						checkers::debugging::LogBitboardWithContrast(piece_captures.value(), 'C');
						return;
					}
					core::debugging::LogError("Invalid command: not found.");
				}
			).or_else(
				[](const auto& error_message) -> std::expected<void, std::string>
				{
					core::debugging::LogError("{}\n", error_message);
					return {};
				}
			);
	}
}