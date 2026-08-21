#pragma once

#include <optional>
#include <string>
#include <expected>

#include "CheckersTypes.h"
#include "Enums/Pieces.h"
#include "Enums/Sides.h"

class BitboardManager
{
public:
	BitboardManager() noexcept;

	void Print() const noexcept;

	bool RemovePiece(size_t i) noexcept;
	std::expected<void, std::string> MovePiece(size_t from, size_t to) noexcept;

	std::optional<Sides>  GetSideByIndex     (size_t i) const noexcept;
	std::optional<Pieces> GetPieceTypeByIndex(size_t i) const noexcept;

	bool IsIndexOccupied(size_t i) const noexcept;

	checkers_types::bitboard GetBoard(Sides side, Pieces piece) const noexcept;
	checkers_types::bitboard GetBoard(Sides side) const noexcept;

private:
	checkers_types::bitboard_list bitboards_{};

	checkers_types::bitboard white_bb_{};
	checkers_types::bitboard black_bb_{};

	void SetBoard(Sides side, Pieces piece, checkers_types::bitboard board) noexcept;
};