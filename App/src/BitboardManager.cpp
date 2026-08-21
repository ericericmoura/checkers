#include "BitboardManager.h"

#include <optional>
#include <expected>
#include <string>

#include <fmt/base.h>
#include <fmt/color.h>

#include "Constants/CheckersConstants.h"
#include "Core/Utils/BitUtils.h"
#include "Enums/Pieces.h"
#include "Enums/Sides.h"
#include "CheckersTypes.h"

BitboardManager::BitboardManager() noexcept
{
	SetBoard(Sides::kWhite, Pieces::kPawn, 0xAA55);
	SetBoard(Sides::kBlack, Pieces::kPawn, 0xAA55000000000000);
}

void BitboardManager::Print() const noexcept
{
	fmt::print("\n");
	for (int rank = checkers_constants::row_count_ - 1; rank >= 0; --rank)
	{
		fmt::print("{}  ", rank + 1);
		for (int file = 0; file < checkers_constants::col_count_; ++file)
		{
			const auto index = file + rank * checkers_constants::col_count_;
			const auto side = GetSideByIndex(index);
			const auto type = GetPieceTypeByIndex(index);
			
			if (!side.has_value())
			{
				fmt::print("0 ");
				continue;
			}
			auto symbol = 'P';
			auto style  = side == Sides::kBlack 
				? fg(fmt::color::orange_red  ) | fmt::emphasis::bold
				: fg(fmt::color::light_yellow) | fmt::emphasis::bold;

			if (type == Pieces::kQueen)
			{
				symbol = 'Q';
			}

			fmt::print(style, "{} ", symbol);
		}
		fmt::print("\n");
	}
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
	board = core::utils::bits::ClearBit(board, i);
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
	board = core::utils::bits::ClearBit(board, from);
	board = core::utils::bits::SetBit  (board, to  );
	SetBoard(side.value(), type.value(), board);

	const auto row = to / checkers_constants::col_count_;

	bool should_promote = type == Pieces::kPawn 
		&& (side == Sides::kWhite && row == checkers_constants::row_count_ - 1) 
		|| (side == Sides::kBlack && row == 0);

	if (should_promote)
	{
		SetBoard(side.value(), type.value()  , core::utils::bits::ClearBit(GetBoard(side.value(), type.value())  , to));
		SetBoard(side.value(), Pieces::kQueen, core::utils::bits::SetBit  (GetBoard(side.value(), Pieces::kQueen), to));
	}
}

std::optional<Sides> BitboardManager::GetSideByIndex(size_t i) const noexcept
{
	if (core::utils::bits::IsBitSet(white_bb_, i))
	{
		return Sides::kWhite;
	}
	if (core::utils::bits::IsBitSet(black_bb_, i))
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
	if (core::utils::bits::IsBitSet(GetBoard(side.value(), Pieces::kPawn), i))
	{
		return Pieces::kPawn;
	}
	if (core::utils::bits::IsBitSet(GetBoard(side.value(), Pieces::kQueen), i))
	{
		return Pieces::kQueen;
	}
	return {};
}

bool BitboardManager::IsIndexOccupied(size_t i) const noexcept
{
	return core::utils::bits::IsBitSet(white_bb_, i) || core::utils::bits::IsBitSet(black_bb_, i);
}

checkers_types::bitboard BitboardManager::GetBoard(Sides side, Pieces piece) const noexcept
{
	if (side == Sides::kCount || piece == Pieces::kCount)
	{
		return {};
	}
	return bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece));
}

checkers_types::bitboard BitboardManager::GetBoard(Sides side) const noexcept
{
	if (side == Sides::kCount)
	{
		return {};
	}
	return side == Sides::kWhite ? white_bb_ : black_bb_;
}

void BitboardManager::SetBoard(Sides side, Pieces piece, checkers_types::bitboard board) noexcept
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