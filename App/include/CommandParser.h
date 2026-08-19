#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <variant>

struct CommandMove 
{
	size_t move_from_ = {};
	size_t move_to_   = {};
};

struct CommandRedo 
{
	static constexpr std::string_view kKey = "redo";
};

struct CommandUndo 
{
	static constexpr std::string_view kKey = "undo";
};

using Command = std::variant<CommandMove, CommandRedo, CommandUndo>;

namespace CommandParser
{

std::expected<Command, std::string> ParseCommand(std::string_view command) noexcept;

} // namespace CommandParser