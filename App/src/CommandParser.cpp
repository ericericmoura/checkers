#include "CommandParser.h"

#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <charconv>

#include "Core/Utils/StringUtils.h"
#include "Core/Debugging/Logging.h"
#include "Constants/CheckersConstants.h"
#include "Utils/CheckersUtils.h"
#include "CheckersEngine.h"

namespace
{

std::pair<std::string_view, std::string_view> SplitCommand(std::string_view cmd) noexcept
{
	return { cmd.substr(0, 2), cmd.substr(2, 2) };
}

std::expected<size_t, std::string> GetIndexFromNotation(std::string_view notation)
{
	if (notation.length() != 2)
	{
		return std::unexpected("Invalid notation: the notation must match standards. Ex.: a1b2");
	}
	if (notation.at(1) < '0' || notation.at(1) > '9' ||
		notation.at(0) < 'a' || notation.at(0) > 'h')
	{
		return std::unexpected("Invalid notation: invalid coordinates.");
	}

	const auto rank = static_cast<std::size_t>(notation.at(1) - '0' - 1);
	const auto file = static_cast<std::size_t>(notation.at(0) - 'a');

	size_t index = file + rank * checkers_constants::col_count_;
	if (utils::checkers::IsIndexOutOfBounds(index))
	{
		return std::unexpected("Invalid notation: coordinates out of bounds.");
	}
	return index;
}

std::expected<std::pair<size_t, size_t>, std::string> GetCoordinatesFromNotation(std::string_view notation) noexcept
{
	const auto [from_str, to_str] = SplitCommand(notation);

	return GetIndexFromNotation(from_str).and_then(
		[&](size_t first_i)
		{
			return GetIndexFromNotation(to_str).and_then(
				[&](size_t second_i)
				{
					return std::expected<std::pair<size_t, size_t>, std::string>(std::in_place, first_i, second_i);
				});
		});
}

std::expected<std::string_view, std::string> GetArgsFromCommand(std::string_view input, std::string_view command_key)
{	
	if (input.length() <= command_key.length() + 1)
	{
		return std::unexpected("Invalid command: no arguments were provided.");
	}	
	return input.substr(command_key.length() + 1, input.length());
}

std::expected<Command, std::string> ParseCommandWithArguments(std::string_view cmd)
{
	if (cmd.starts_with(CommandMove::kKey))
	{
		return GetArgsFromCommand(cmd, CommandMove::kKey).and_then(
			[](const auto& args)
			{
				return GetCoordinatesFromNotation(args).and_then(
					[](std::pair<size_t, size_t> indexes)
					{
						return std::expected<Command, std::string>(std::in_place, CommandMove{ indexes.first, indexes.second });
					});
			});
	}
	if (cmd.starts_with(CommandDisplayMoves::kKey))
	{
		return GetArgsFromCommand(cmd, CommandDisplayMoves::kKey).and_then(
			[](const auto& args) 
			{
				return GetIndexFromNotation(args).and_then(
					[](size_t i)
					{
						return std::expected<Command, std::string>(std::in_place, CommandDisplayMoves{ i });
					});
			});
	}
	if (cmd.starts_with(CommandDisplayCaptures::kKey))
	{
		return GetArgsFromCommand(cmd, CommandDisplayCaptures::kKey).and_then(
			[](const auto& args)
			{
				return GetIndexFromNotation(args).and_then(
					[](size_t i)
					{
						return std::expected<Command, std::string>(std::in_place, CommandDisplayCaptures{ i });
					});
			});
	}
	return std::unexpected("Non-existent command.");
}

} // anonymous namespace

std::expected<void, std::string> command_parser::RunCommand(CheckersEngine& engine, std::string cmd) noexcept
{
	const auto command = command_parser::ParseCommand(cmd);

	if (!command)
	{
		return std::unexpected(command.error());
	}

	if (auto value = std::get_if<CommandMove>(&command.value()))
	{
		const auto result = engine.MovePiece(value->move_from_, value->move_to_);
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return {};
	}
	if (auto value = std::get_if<CommandDisplayMoves>(&command.value()))
	{
		const auto piece_moves = engine.GetMoves(value->piece_index_);
		if (!piece_moves)
		{
			return std::unexpected(piece_moves.error());
		}
		return {};
	}
	if (auto value = std::get_if<CommandDisplayCaptures>(&command.value()))
	{
		const auto piece_captures = engine.GetCaptures(value->piece_index_);
		if (!piece_captures)
		{
			return std::unexpected(piece_captures.error());
		}
	}
	return {};
}

std::expected<Command, std::string> command_parser::ParseCommand(std::string_view cmd) noexcept
{
	if (cmd.length() > 4)
	{
		return ParseCommandWithArguments(cmd);
	}
	if (cmd.length() != 4)
	{
		return std::unexpected("Non-existent command.");
	}
	if (utils::string::iequals(cmd, CommandRedo::kKey))
	{
		return CommandRedo();
	}
	if (utils::string::iequals(cmd, CommandUndo::kKey))
	{
		return CommandUndo();
	}
	return std::unexpected("Non-existent command.");
}