#pragma once

#include <expected>
#include <string>
#include <optional>

#include "CheckersTypes.h"
#include "CheckersEngine.h"

class PiecesRenderer;

class CheckersInputManager
{
public:
	void SelectIndex(size_t index) noexcept;

	void DeselectIndex() noexcept;
	bool IsIndexSelected() const noexcept;
	
	std::optional<size_t> GetSelectedIndex() const noexcept;

	std::expected<void, std::string> MoveSelectedIndexTo(CheckersEngine& engine, size_t target_index) noexcept;

	std::expected<Bitboard, std::string> GetMovementsForSelectedIndex(CheckersEngine& engine) noexcept;
	Bitboard GetCaptures(CheckersEngine& engine) noexcept;

	std::expected<std::string, std::string> GetCommand(size_t target_index) noexcept;

private:
	std::optional<size_t> selected_index_ = {};
};