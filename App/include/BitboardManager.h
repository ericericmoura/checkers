#pragma once

#include <optional>
#include <string>
#include <expected>

#include "Enums/Pieces.h"
#include "Enums/Sides.h"

#include "Types/Bitboard.h"

namespace checkers
{

using bitboard_list = std::array<std::array<Bitboard, static_cast<size_t>(Pieces::kCount)>, static_cast<size_t>(Sides::kCount)>;

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

	Bitboard GetBoard(Sides side, Pieces piece) const noexcept;
	Bitboard GetBoard(Sides side) const noexcept;

private:
	bitboard_list bitboards_{};

	Bitboard white_bb_{};
	Bitboard black_bb_{};

	void SetBoard(Sides side, Pieces piece, Bitboard board) noexcept;
};

} // namespace checkers