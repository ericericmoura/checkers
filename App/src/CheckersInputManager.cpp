#include "CheckersInputManager.h"

#include <expected>
#include <string>
#include <optional>
#include <format>

#include "Rendering/PiecesRenderer.h"
#include "Rendering/BoardRenderer.h"
#include "Core/Debugging/Logging.h"
#include "CheckersEngine.h"
#include "BoardNotation.h"
#include "Core/Engine.h"

namespace checkers::interaction
{

std::expected<void, std::string> CheckersInputManager::SelectIndex(size_t index, bool is_occupied) noexcept
{
	if (IsIndexSelected())
	{
		return std::unexpected("Invalid selection: an index is already selected.");
	}
	if (!is_occupied)
	{		
		return std::unexpected("Invalid selection: there's no piece at index " + std::to_string(index) + ".");
	}
	selected_index_ = index;
}

void CheckersInputManager::DeselectIndex() noexcept
{
	selected_index_ = {};
}

bool CheckersInputManager::IsIndexSelected() const noexcept
{
	return selected_index_.has_value();
}

std::optional<size_t> CheckersInputManager::GetSelectedIndex() const noexcept
{
	return selected_index_;
}

std::expected<void, std::string> CheckersInputManager::MoveSelectedIndexTo(CheckersEngine& engine, size_t target_index) noexcept
{
	if (!selected_index_)
	{
		return std::unexpected(std::string("No index selected."));
	}

	core::debugging::LogInfo("Moving piece from {} to {}", selected_index_.value(), target_index);

	const auto result = engine.MovePiece(selected_index_.value(), target_index);
	if (!result)
	{
		return std::unexpected(result.error());
	}
}

std::expected<Bitboard, std::string> CheckersInputManager::GetMovementsForSelectedIndex(CheckersEngine& engine) noexcept
{
	if (!selected_index_)
	{
		return std::unexpected(std::string("No index selected."));
	}
	const auto piece_moves = engine.GetMoves(selected_index_.value());
	if (!piece_moves)
	{
		return std::unexpected(piece_moves.error());
	}
	return piece_moves.value();
}

std::expected<std::string, std::string> CheckersInputManager::GetCommand(size_t target_index) noexcept
{
	if (!selected_index_.has_value())
	{
		return std::unexpected("Incomplete command, either selected or target index are not set.");
	}

	const auto index_notation = BitboardIndexToNotation(selected_index_.value());
	const auto target_notation = BitboardIndexToNotation(target_index);

	if (!index_notation || !target_notation)
	{
		return std::unexpected("Invalid selected or target index: " + index_notation.error() + " | " + target_notation.error());
	}

	const auto command = index_notation.value() + target_notation.value();

	selected_index_ = {};

	return command;
}

} // namespace checkers::interaction