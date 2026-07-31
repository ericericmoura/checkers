#include "CheckersEngine.h"

#include <optional>
#include <string>

#include "Core/Utils/BitUtils.h"
#include "Core/Debugging/Logging.h"
#include "Enums/Sides.h"
#include "Enums/Pieces.h"
#include "Constants/ChessConstants.h"

void CheckersEngine::InitBoards() noexcept
{
	SetBoard(Sides::White, Pieces::Pawn, 0xAA55);
	SetBoard(Sides::Black, Pieces::Pawn, 0xAA55000000000000);
}

std::string CheckersEngine::ToString() const noexcept
{
	std::string output = "";
	for (size_t rank = 0; rank < chess_constants::row_count_; ++rank)
	{
		output += rank + 1 - '0';
		output += "  ";
		for (size_t file = chess_constants::col_count_; file >= 0; --file)
		{
			const auto index = file + rank * chess_constants::col_count_;
			const auto piece_side = GetSideByIndex(index);
			auto symbol = '0';
			if (piece_side.has_value())
			{
				symbol = piece_side.value() == Sides::White ? 'W' : 'B';
			}
			output += symbol;
			output += " ";
		}
	}
	output += "    a b c d e f g h";
	return output;
}

std::optional<Sides> CheckersEngine::GetSideByIndex(size_t i) const noexcept
{
	if (core::utils::IsBitSet(white_bb, i))
	{
		return Sides::White;
	}
	if (core::utils::IsBitSet(black_bb, i))
	{
		return Sides::Black;
	}
	return {};
}

void CheckersEngine::SetBoard(Sides side, Pieces piece, bitboard board) noexcept
{
	const auto old_bb = GetBoard(side, piece);
	auto& side_board = side == Sides::White ? white_bb : black_bb;

	side_board ^= old_bb;
	bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece)) = board;
	side_board |= board;
}

bitboard CheckersEngine::GetBoard(Sides side, Pieces piece) const noexcept
{
	return bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece));
}
