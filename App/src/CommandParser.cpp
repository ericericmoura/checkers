#include "CommandParser.h"

#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <charconv>

#include "Core/Utils/StringUtils.h"
#include "Constants/CheckersConstants.h"
#include "Utils/CheckersUtils.h"

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

std::expected<Command, std::string> CommandParser::ParseCommand(std::string_view cmd) noexcept
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
	return GetCoordinatesFromNotation(cmd)
		.transform([](const std::pair<size_t, size_t>& coords) 
			{ 
				return CommandMove(coords.first, coords.second);
			});
}