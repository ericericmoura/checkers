#pragma once

#include <expected>
#include <string>
#include <optional>

#include "CheckersEngine.h"

namespace checkers
{

namespace rendering
{

class PiecesRenderer;

} // namespace rendering

namespace interaction
{

class CheckersInputManager
{
public:
	std::expected<void, std::string> SelectIndex(size_t index, bool is_occupied = true) noexcept;

	void DeselectIndex() noexcept;
	bool IsIndexSelected() const noexcept;

	std::optional<size_t> GetSelectedIndex() const noexcept;

	std::expected<void, std::string> MoveSelectedIndexTo(CheckersEngine& engine, size_t target_index) noexcept;

	std::expected<Bitboard, std::string> GetMovementsForSelectedIndex(CheckersEngine& engine) noexcept;

	std::expected<std::string, std::string> GetCommand(size_t target_index) noexcept;

private:
	std::optional<size_t> selected_index_ = {};
};

} // namespace interaction

} // namespace checkers