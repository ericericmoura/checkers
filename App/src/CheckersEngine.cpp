#include "CheckersEngine.h"

#include <optional>
#include <string>
#include <utility>
#include <cassert>
#include <bit>

#include <fmt/base.h>
#include <fmt/color.h>

#include "Core/Utils/BitUtils.h"
#include "Core/Debugging/Logging.h"
#include "Constants/ChessConstants.h"
#include "Utils/ChessUtils.h"
#include "Enums/Sides.h"
#include "Enums/Pieces.h"
#include "Enums/Directions.h"

void CheckersEngine::Init() noexcept
{
	SetBoard(Sides::kWhite, Pieces::kPawn, 0xAA55);
	SetBoard(Sides::kBlack, Pieces::kPawn, 0xAA55100044000000);
	SetBoard(Sides::kWhite, Pieces::kQueen, 0x1 << 8);
	SetBoard(Sides::kWhite, Pieces::kQueen, 0x800000);

	CacheDiagonalRays();
}

void CheckersEngine::Print() const noexcept
{
	fmt::print("\n");
	for (int rank = chess_constants::row_count_ - 1; rank >= 0; --rank)
	{
		fmt::print("{}  ", rank + 1);
		for (int file = 0; file < chess_constants::col_count_; ++file)
		{
			const auto index = file + rank * chess_constants::col_count_;
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

void CheckersEngine::ExecuteCommand(std::string cmd) noexcept
{
	if (cmd.length() != 4)
	{
		core::debugging::LogError("Invalid checkers command.");
		return;
	}
	const auto commands = SplitCommand(cmd);
	
	const auto first_i  = GetIndexFromNotation(commands.first );
	const auto second_i = GetIndexFromNotation(commands.second);	

	MovePiece(first_i, second_i);
}

void CheckersEngine::MovePiece(size_t from, size_t to) noexcept
{	
	if (IsIndexOccupied(to))
	{
		core::debugging::LogError("Can't move piece into an occupied square.");
		return;
	}
	const auto side = GetSideByIndex(from);
	const auto type = GetPieceTypeByIndex(from);
	if (!side.has_value() || !type.has_value())
	{
		core::debugging::LogError("Can't move non-existent piece.");
		return;
	}
	const auto movements = GetPossibleMovements(side.value(), type.value(), from);
	if (!core::utils::IsBitSet(movements, to))
	{
		core::debugging::LogError("Invalid movement.");
		return;
	}
	auto board = GetBoard(side.value(), type.value());
	board = core::utils::ClearBit(board, from);
	board = core::utils::SetBit  (board, to  );
	SetBoard(side.value(), type.value(), board);
}

std::optional<Sides> CheckersEngine::GetSideByIndex(size_t i) const noexcept
{
	if (core::utils::IsBitSet(white_bb_, i))
	{
		return Sides::kWhite;
	}
	if (core::utils::IsBitSet(black_bb_, i))
	{
		return Sides::kBlack;
	}
	return {};
}

std::optional<Pieces> CheckersEngine::GetPieceTypeByIndex(size_t i) const noexcept
{
	const auto side = GetSideByIndex(i);
	if (!side.has_value())
	{
		return {};
	}
	const auto pawn_board  = bitboards_.at(static_cast<size_t>(side.value())).at(static_cast<size_t>(Pieces::kPawn ));
	if (core::utils::IsBitSet(pawn_board, i))
	{
		return Pieces::kPawn;
	}
	const auto queen_board = bitboards_.at(static_cast<size_t>(side.value())).at(static_cast<size_t>(Pieces::kQueen));
	if (core::utils::IsBitSet(queen_board, i))
	{
		return Pieces::kQueen;
	}
	return {};
}

bool CheckersEngine::IsIndexOccupied(size_t i) const noexcept
{
	return core::utils::IsBitSet(white_bb_, i) || core::utils::IsBitSet(black_bb_, i);
}

void CheckersEngine::SetBoard(Sides side, Pieces piece, bitboard board) noexcept
{
	const auto old_bb = GetBoard(side, piece);
	auto& side_board = side == Sides::kWhite ? white_bb_ : black_bb_;

	side_board ^= old_bb;
	bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece)) = board;
	side_board |= board;
}

bitboard CheckersEngine::GetBoard(Sides side, Pieces piece) const noexcept
{
	return bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece));
}

std::pair<std::string, std::string> CheckersEngine::SplitCommand(std::string cmd) const noexcept
{
	std::pair<std::string, std::string> result{};
	result.first  = cmd.substr(0, 2);
	result.second = cmd.substr(2, 2);
	return result;
}

size_t CheckersEngine::GetIndexFromNotation(std::string notation) const noexcept
{
	const auto file = static_cast<std::size_t>(notation.at(1) - '0' - 1);
	const auto rank = static_cast<std::size_t>(notation.at(0) - 'a');

	bool out_of_bounds = file > chess_constants::row_count_ || rank > chess_constants::col_count_ || file < 0 || rank < 0;
	assert(!out_of_bounds);

	size_t result = rank;
	result += file * chess_constants::col_count_;
	return result;
}

bitboard CheckersEngine::GetPossibleMovements(Sides side, Pieces type, size_t i) const noexcept
{
	bitboard result = 0;
	if (type == Pieces::kPawn)
	{		
		const auto sign = side == Sides::kWhite ? +1 : -1;
		result |= 0x1ull << i + 9 * static_cast<unsigned long long>(sign);
		result |= 0x1ull << i + 7 * static_cast<unsigned long long>(sign);
		return result;
	}
	if (type == Pieces::kQueen)
	{
		return GetPossibleMovementsForQueen(side, i);
	}
	core::debugging::LogError("Invalid piece type.");
	return result;
}

bitboard CheckersEngine::GetPossibleMovementsForQueen(Sides side, size_t i) const noexcept
{
	const auto blockers = black_bb_ | white_bb_;
	
	bitboard attacks = 0;
	attacks |= GetMaskedRayAttacks(DiagonalDirections::kNorthWest, i, blockers);
	attacks |= GetMaskedRayAttacks(DiagonalDirections::kNorthEast, i, blockers);
	attacks |= GetMaskedRayAttacks(DiagonalDirections::kSouthWest, i, blockers);
	attacks |= GetMaskedRayAttacks(DiagonalDirections::kSouthEast, i, blockers);

	utils::LogBitboardWithContrast(attacks, 'R');

	return attacks;
}

bitboard CheckersEngine::GetMaskedRayAttacks(DiagonalDirections dir, size_t i, bitboard blockers) const noexcept
{
	bitboard attacks = 0;
	
	const auto rays = diagonal_rays_[static_cast<int>(dir)][i];
	const auto mask = rays & blockers;

	attacks |= rays;
	if (mask == 0 || std::popcount(mask) <= 1)
	{
		return attacks;
	}
	size_t blocker_index = std::countr_zero(mask);
	if (dir == DiagonalDirections::kNorthEast || dir == DiagonalDirections::kNorthWest)
	{
		blocker_index = static_cast<size_t>(chess_constants::col_count_ * chess_constants::row_count_) - std::countl_zero(mask);
	}

	attacks &= ~diagonal_rays_[static_cast<int>(dir)][blocker_index];
	return attacks;
}

void CheckersEngine::CacheDiagonalRays() noexcept
{
	for (size_t i = 0; i < 64; ++i)
	{
		const auto row = i / 8;
		const auto col = i % 8;

		diagonal_rays_[static_cast<int>(DiagonalDirections::kNorthWest)][i] |= GenerateDiagonalRays(DiagonalDirections::kNorthWest, i);
		diagonal_rays_[static_cast<int>(DiagonalDirections::kNorthEast)][i] |= GenerateDiagonalRays(DiagonalDirections::kNorthEast, i);
		diagonal_rays_[static_cast<int>(DiagonalDirections::kSouthWest)][i] |= GenerateDiagonalRays(DiagonalDirections::kSouthWest, i);
		diagonal_rays_[static_cast<int>(DiagonalDirections::kSouthEast)][i] |= GenerateDiagonalRays(DiagonalDirections::kSouthEast, i);
	}
}

bitboard CheckersEngine::GenerateDiagonalRays(DiagonalDirections dir, size_t index) const noexcept
{
	bitboard result = 0;

	auto file = index / chess_constants::col_count_;
	auto rank = index % chess_constants::col_count_;

	while (true)
	{
		if (   (dir == DiagonalDirections::kNorthWest && !(rank < chess_constants::col_count_ - 1 && file < chess_constants::row_count_ - 1))
			|| (dir == DiagonalDirections::kNorthEast && !(rank > 0 && file < chess_constants::row_count_ - 1))
			|| (dir == DiagonalDirections::kSouthEast && !(rank < chess_constants::col_count_ - 1 && file > 0))
			|| (dir == DiagonalDirections::kSouthWest && !(rank > 0 && file > 0)))
		{
			break;
		}

		switch (dir)
		{
			case DiagonalDirections::kNorthWest: file++; rank++; break;  // was rank--
			case DiagonalDirections::kNorthEast: file++; rank--; break;  // was rank++
			case DiagonalDirections::kSouthEast: file--; rank++; break;
			case DiagonalDirections::kSouthWest: file--; rank--; break;
		}

		const auto square = rank + file * chess_constants::col_count_;
		result |= 0x1ull << square;		
	}
	return result;
}