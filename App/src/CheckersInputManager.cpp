#include "CheckersInputManager.h"

#include <expected>
#include <string>
#include <optional>

#include "Utils/CheckersUtils.h"

void CheckersInputManager::SelectIndex(size_t index) noexcept
{
	selected_index_ = index;
}

void CheckersInputManager::TargetIndex(size_t index) noexcept
{
	target_index_ = index;
}

bool CheckersInputManager::IsIndexSelected() const noexcept
{
	return selected_index_.has_value();
}

std::expected<std::string, std::string> CheckersInputManager::GetCommand() noexcept
{
	if (!selected_index_.has_value() || !target_index_.has_value())
	{
		return std::unexpected("Incomplete command, either selected or target index are not set.");
	}

	const auto index_notation  = utils::checkers::BitboardIndexToNotation(selected_index_.value());
	const auto target_notation = utils::checkers::BitboardIndexToNotation(target_index_.value());

	if (!index_notation || !target_notation)
	{
		return std::unexpected("Invalid selected or target index: " + index_notation.error() + " | " + target_notation.error());
	}

	const auto command = index_notation.value() + target_notation.value();

	selected_index_ = {};
	target_index_   = {};

	return command;
}
