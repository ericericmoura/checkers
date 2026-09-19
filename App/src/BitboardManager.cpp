#include "BitboardManager.h"

#include <optional>
#include <expected>
#include <string>

#include <fmt/base.h>
#include <fmt/color.h>

#include "Constants/CheckersConstants.h"
#include "Enums/Pieces.h"
#include "Enums/Sides.h"

namespace checkers
{

BitboardManager::BitboardManager() noexcept
{
	SetBoard(Sides::kWhite, Pieces::kPawn, 0xAA55);
	SetBoard(Sides::kBlack, Pieces::kPawn, 0xAA55000000000000);
}

void BitboardManager::Print() const noexcept
{
	auto board = white_bb_ | white_bb_;

	board.DrainBits([this](size_t index) {
		const auto side = GetSideByIndex(index);
		const auto type = GetPieceTypeByIndex(index);

		if (!side.has_value())
		{
			fmt::print("0 ");
			return;
		}
		auto symbol = 'P';
		auto style = side == Sides::kBlack
			? fg(fmt::color::orange_red) | fmt::emphasis::bold
			: fg(fmt::color::light_yellow) | fmt::emphasis::bold;

		if (type == Pieces::kQueen)
		{
			symbol = 'Q';
		}

		fmt::print(style, "{} ", symbol);

		if (index % (checkers::constants::col_count_-1) == 0) fmt::print("\n");
	});
	fmt::print("\n   a b c d e f g h");
}

bool BitboardManager::RemovePiece(size_t i) noexcept
{
	const auto side = GetSideByIndex(i);
	const auto type = GetPieceTypeByIndex(i);
	if (!side.has_value() || !type.has_value())
	{
		return false;
	}
	auto board = GetBoard(side.value(), type.value());
	board.ClearBit(i);
	SetBoard(side.value(), type.value(), board);
	return true;
}

std::expected<void, std::string> BitboardManager::MovePiece(size_t from, size_t to) noexcept
{	
	if (!IsIndexOccupied(from))
	{
		return std::unexpected("Invalid request : non-existent piece.");
	}
	if (IsIndexOccupied(to))
	{
		return std::unexpected("Invalid request: can't move piece into an occupied square.");
	}
	const auto side = GetSideByIndex(from);
	const auto type = GetPieceTypeByIndex(from);
	if (!side.has_value() || !type.has_value())
	{
		return std::unexpected("Invalid request: non-existent piece.");
	}

	auto board = GetBoard(side.value(), type.value());
	board.ClearBit(from);
	board.SetBit  (to  );
	SetBoard(side.value(), type.value(), board);

	const auto row = to / checkers::constants::col_count_;

	bool should_promote = type == Pieces::kPawn 
		&& (side == Sides::kWhite && row == checkers::constants::row_count_ - 1) 
		|| (side == Sides::kBlack && row == 0);

	if (should_promote)
	{
		SetBoard(side.value(), type.value()  , GetBoard(side.value(), type.value()  ).ClearBit(to));
		SetBoard(side.value(), Pieces::kQueen, GetBoard(side.value(), Pieces::kQueen).SetBit  (to));
	}
}

std::optional<Sides> BitboardManager::GetSideByIndex(size_t i) const noexcept
{
	if (white_bb_.IsBitSet(i))
	{
		return Sides::kWhite;
	}
	if (black_bb_.IsBitSet(i))
	{
		return Sides::kBlack;
	}
	return {};
}

std::optional<Pieces> BitboardManager::GetPieceTypeByIndex(size_t i) const noexcept
{
	const auto side = GetSideByIndex(i);
	if (!side.has_value())
	{
		return {};
	}
	if (GetBoard(side.value(), Pieces::kPawn).IsBitSet(i))
	{
		return Pieces::kPawn;
	}
	if (GetBoard(side.value(), Pieces::kQueen).IsBitSet(i))
	{
		return Pieces::kQueen;
	}
	return {};
}

bool BitboardManager::IsIndexOccupied(size_t i) const noexcept
{
	return white_bb_.IsBitSet(i) || black_bb_.IsBitSet(i);
}

Bitboard BitboardManager::GetBoard(Sides side, Pieces piece) const noexcept
{
	if (side == Sides::kCount || piece == Pieces::kCount)
	{
		return {};
	}
	return bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece));
}

Bitboard BitboardManager::GetBoard(Sides side) const noexcept
{
	if (side == Sides::kCount)
	{
		return {};
	}
	return side == Sides::kWhite ? white_bb_ : black_bb_;
}

void BitboardManager::SetBoard(Sides side, Pieces piece, Bitboard board) noexcept
{
	if (side == Sides::kCount || piece == Pieces::kCount)
	{
		return;
	}
	const auto old_bb = GetBoard(side, piece);
	auto& side_board = side == Sides::kWhite ? white_bb_ : black_bb_;

	side_board ^= old_bb;
	bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece)) = board;
	side_board |= board;
}

} // namespace checkers