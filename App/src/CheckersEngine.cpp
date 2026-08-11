#include "CheckersEngine.h"

#include <optional>
#include <string>
#include <utility>
#include <bit>

#include <fmt/base.h>
#include <fmt/color.h>

#include "Core/Utils/BitUtils.h"
#include "Core/Debugging/Logging.h"
#include "Constants/ChessConstants.h"
#include "Enums/Sides.h"
#include "Enums/Pieces.h"
#include "Enums/Directions.h"
#include "Utils/ChessUtils.h"
#include "Utils/DirectionUtils.h"

void CheckersEngine::Init() noexcept
{
	SetBoard(Sides::kWhite, Pieces::kQueen, 0x1ull << 50);
	SetBoard(Sides::kBlack, Pieces::kQueen, 0x1ull << 22 | 0x1ull << 15 +7*3);

	CacheDiagonalRays();

	FinishTurn();
}

void CheckersEngine::Print() const noexcept
{
	const auto side_to_play = current_team_ == Sides::kWhite ? "White" : "Black";
	fmt::print("\n\n{} to play.", side_to_play);

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

	if (!first_i.has_value() || !second_i.has_value())
	{
		core::debugging::LogError("Invalid checkers command.");
		return;
	}

	if (available_pawn_captures_ != 0 && !core::utils::IsBitSet(available_pawn_captures_, second_i.value()))
	{
		core::debugging::LogError("You are required to capture a piece.");
		return;
	}

	bool success_moving = MovePiece(first_i.value(), second_i.value());	
	if (!success_moving)
	{
		return;
	}
	FinishTurn();
}

void CheckersEngine::FinishTurn() noexcept
{
	current_team_ = current_team_ == Sides::kWhite
		? Sides::kBlack
		: Sides::kWhite;

	UpdatePossibleCaptures(current_team_);

	fmt::print("\n\nAvailable Queen Captures for team {}", current_team_ == Sides::kBlack ? "black" : "white");
	utils::LogBitboardWithContrast(available_queen_captures_, 'C');
}

bool CheckersEngine::CapturePiece(size_t from, size_t to) noexcept
{
	const auto side = GetSideByIndex     (from);
	const auto type = GetPieceTypeByIndex(from);

	const auto available_captures = type == Pieces::kPawn ? available_pawn_captures_ : available_queen_captures_;

	if (!core::utils::IsBitSet(available_captures, to))
	{
		return false;
	}

	const auto dir_y = static_cast<int>(from) - static_cast<int>(to) > 0 ? VerticalDirections::kDown : VerticalDirections::kUp;	

	std::optional<size_t> enemy_i{};
	if (type == Pieces::kPawn)
	{
		enemy_i = utils::pawn::CapturePieceWithPawn(from, to, dir_y);
	}
	if (type == Pieces::kQueen)
	{
		enemy_i = CapturePieceWithQueen(from, to, dir_y);
	}

	if (!enemy_i.has_value())
	{
		return false;
	}
	const auto enemy_side = side.value() == Sides::kWhite ? Sides::kBlack : Sides::kWhite;
	const auto enemy_type = GetPieceTypeByIndex(enemy_i.value());

	if (!enemy_type.has_value())
	{
		return false;
	}

	auto board = GetBoard(enemy_side, enemy_type.value());
	board = core::utils::ClearBit(board, enemy_i.value());

	SetBoard(enemy_side, enemy_type.value(), board);
	return true;
}

std::optional<size_t> CheckersEngine::CapturePieceWithQueen(size_t from, size_t to, VerticalDirections dir_y) const noexcept
{
	const auto is_east = from%8 < to%8;

	const auto dir = utils::directions::GetDiagonalDirection(is_east, dir_y == VerticalDirections::kUp);

	const auto opposite_side_bb = current_team_ == Sides::kBlack ? white_bb_ : black_bb_;
	const auto blockers         = diagonal_rays_[static_cast<int>(dir)][from] & opposite_side_bb;

	if (std::popcount(blockers) == 0)
	{
		return {};
	}

	size_t enemy_i = {};
	if (dir_y == VerticalDirections::kUp)
	{
		enemy_i = static_cast<size_t>(chess_constants::total_squares_) - std::countl_zero(blockers);
	}
	else
	{
		enemy_i = std::countr_zero(blockers);
	}
	return enemy_i;
}

bool CheckersEngine::MovePiece(size_t from, size_t to) noexcept
{	
	if (IsIndexOccupied(to))
	{
		core::debugging::LogError("Can't move piece into an occupied square.");
		return false;
	}
	const auto side = GetSideByIndex(from);
	const auto type = GetPieceTypeByIndex(from);
	if (!side.has_value() || !type.has_value() || side != current_team_)
	{
		core::debugging::LogError("Can't move piece. It's either a non-existent or enemy piece.");
		return false;
	}
	const auto movements = GetPossibleMovements(side.value(), type.value(), from);
	const auto available_captures = type == Pieces::kPawn ? available_pawn_captures_ : available_queen_captures_;
	if (!core::utils::IsBitSet(movements | available_captures, to))
	{
		core::debugging::LogError("Invalid movement.");
		return false;
	}
	auto captured = true;
	if (available_captures != 0)
	{
		captured = CapturePiece(from, to);
	}
	if (!captured)
	{
		core::debugging::LogError("Invalid capture.");
		return false;
	}
	auto board = GetBoard(side.value(), type.value());
	board = core::utils::ClearBit(board, from);
	board = core::utils::SetBit  (board, to  );
	SetBoard(side.value(), type.value(), board);
	return true;
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

std::optional<size_t> CheckersEngine::GetIndexFromNotation(std::string notation) const noexcept
{
	if (notation.at(1) < '0' || notation.at(1) > '9' ||
		notation.at(0) < 'a' || notation.at(0) > 'h')
	{		
		return {};
	}

	const auto rank = static_cast<std::size_t>(notation.at(1) - '0' - 1);
	const auto file = static_cast<std::size_t>(notation.at(0) - 'a');

	bool out_of_bounds = rank > chess_constants::row_count_ || file > chess_constants::col_count_ || rank < 0 || file < 0;
	if (out_of_bounds)
	{
		return {};
	}

	size_t result = file;
	result += rank * chess_constants::col_count_;
	return result;
}

bitboard CheckersEngine::GetPossibleMovements(Sides side, Pieces type, size_t i) const noexcept
{
	bitboard result = 0;
	if (type == Pieces::kPawn)
	{		
		return utils::pawn::GetPossibleMovesForPawn(side, i);
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
	auto blockers = black_bb_ | white_bb_;
	
	bitboard attacks = 0;
	attacks |= GetMaskedRayAttacks(DiagonalDirections::kNorthWest, i, blockers);
	attacks |= GetMaskedRayAttacks(DiagonalDirections::kNorthEast, i, blockers);
	attacks |= GetMaskedRayAttacks(DiagonalDirections::kSouthWest, i, blockers);
	attacks |= GetMaskedRayAttacks(DiagonalDirections::kSouthEast, i, blockers);

	fmt::print("Queen Attacks:");
	utils::LogBitboardWithContrast(attacks, 'A');

	return attacks;
}

bitboard CheckersEngine::GetMaskedRayAttacks(DiagonalDirections dir, size_t i, bitboard blockers) const noexcept
{
	bitboard attacks = 0;
	
	const auto rays = diagonal_rays_[static_cast<int>(dir)][i];
	const auto mask = rays & blockers;

	attacks |= rays;
	attacks &= ~blockers;
	if (mask == 0 || std::popcount(mask) <= 1)
	{
		return attacks;
	}

	auto mask_copy = mask;
	size_t first_blocker_index  = static_cast<size_t>(chess_constants::total_squares_) - 1 - std::countl_zero(mask);
	mask_copy = core::utils::ClearBit(mask, first_blocker_index);
	size_t second_blocker_index = static_cast<size_t>(chess_constants::total_squares_) - 1 - std::countl_zero(mask_copy);

	if (dir == DiagonalDirections::kNorthEast || dir == DiagonalDirections::kNorthWest)
	{
		first_blocker_index = std::countr_zero(mask);
		mask_copy = core::utils::ClearBit(mask, first_blocker_index);
		second_blocker_index = std::countr_zero(mask_copy);
	}
	return attacks & ~diagonal_rays_[static_cast<int>(dir)][second_blocker_index];
}

bitboard CheckersEngine::GetMaskedRayCaptures(DiagonalDirections dir, size_t i, bitboard blockers) const noexcept
{
	bitboard captures = 0;

	// Cast a ray from the user into the specific direction
	const auto rays = diagonal_rays_[static_cast<int>(dir)][i];

	fmt::print("\n\nRays: ");
	utils::LogBitboardWithContrast(rays, 'R');

	fmt::print("\n\nBlockers: ");
	utils::LogBitboardWithContrast(blockers, 'B');

	// Mask out any enemies that are hit
	const auto mask = rays & blockers;
	if (mask == 0)
	{
		return 0;
	}
	const auto vertical_dir = utils::directions::GetVerticalDirection(dir);

	auto mask_copy = mask;

	auto second_blocker_index = vertical_dir == VerticalDirections::kDown
		? std::countr_zero(mask_copy)
		: chess_constants::total_squares_ - 1 - std::countl_zero(mask_copy);
	// If there's only one blocker, return
	if (std::popcount(mask) == 1)
	{
		return diagonal_rays_[static_cast<int>(dir)][second_blocker_index] & ~blockers;
	}
	auto first_blocker_index  = 0;
	do
	{
		// Get the second blocker (farthest)
		second_blocker_index = vertical_dir == VerticalDirections::kDown
			? std::countr_zero(mask_copy)
			: chess_constants::total_squares_ - 1 - std::countl_zero(mask_copy);
		// Remove the second blocker
		mask_copy = core::utils::ClearBit(mask_copy, second_blocker_index);
		// Get the first blocker
		first_blocker_index = vertical_dir == VerticalDirections::kDown
			? std::countr_zero(mask_copy)
			: chess_constants::total_squares_ - 1 - std::countl_zero(mask_copy);
	} while (std::popcount(mask_copy) > 1);	

	// Add the rays from the first blocker
	captures |=  diagonal_rays_[static_cast<int>(dir)][first_blocker_index];
	// Remove the rays past the second blocker
	captures &= ~diagonal_rays_[static_cast<int>(dir)][second_blocker_index];
	captures &= ~blockers;

	return captures;
}

void CheckersEngine::UpdatePossibleCaptures(Sides side) noexcept
{
	available_pawn_captures_  = utils::pawn::GetPawnCaptures(side, black_bb_, white_bb_, GetBoard(side, Pieces::kPawn));
	
	available_queen_captures_ = GetQueenCaptures(side);
}

bitboard CheckersEngine::GetQueenCaptures(Sides side) const noexcept
{
	bitboard result = 0;

	const auto blockers = side == Sides::kWhite ? black_bb_ : white_bb_;

	auto queen_bb = GetBoard(side, Pieces::kQueen);
	while (queen_bb > 0)
	{
		const auto i = std::countr_zero(queen_bb);
		queen_bb = core::utils::ClearBit(queen_bb, i);
		//result |= GetMaskedRayCaptures(DiagonalDirections::kNorthEast, i, blockers);
		result |= GetMaskedRayCaptures(DiagonalDirections::kNorthWest, i, blockers);
		//result |= GetMaskedRayCaptures(DiagonalDirections::kSouthEast, i, blockers);
		//result |= GetMaskedRayCaptures(DiagonalDirections::kSouthWest, i, blockers);
	}
	return result;
}

void CheckersEngine::CacheDiagonalRays() noexcept
{
	for (size_t i = 0; i < chess_constants::total_squares_; ++i)
	{
		diagonal_rays_[static_cast<int>(DiagonalDirections::kNorthWest)][i] |= GenerateDiagonalRays(DiagonalDirections::kNorthWest, i);
		diagonal_rays_[static_cast<int>(DiagonalDirections::kNorthEast)][i] |= GenerateDiagonalRays(DiagonalDirections::kNorthEast, i);
		diagonal_rays_[static_cast<int>(DiagonalDirections::kSouthWest)][i] |= GenerateDiagonalRays(DiagonalDirections::kSouthWest, i);
		diagonal_rays_[static_cast<int>(DiagonalDirections::kSouthEast)][i] |= GenerateDiagonalRays(DiagonalDirections::kSouthEast, i);
	}
}

bitboard CheckersEngine::GenerateDiagonalRays(DiagonalDirections dir, size_t index) const noexcept
{
	bitboard result = 0;

	auto file = index % chess_constants::col_count_;
	auto rank = index / chess_constants::col_count_;

	while (true)
	{
		if (   (dir == DiagonalDirections::kNorthWest && !(rank < chess_constants::row_count_ - 1 && file > 0))
			|| (dir == DiagonalDirections::kNorthEast && !(rank < chess_constants::row_count_ - 1 && file < chess_constants::col_count_ - 1))
			|| (dir == DiagonalDirections::kSouthEast && !(rank > 0 && file < chess_constants::col_count_ - 1))
			|| (dir == DiagonalDirections::kSouthWest && !(rank > 0 && file > 0)))
		{
			break;
		}

		switch (dir)
		{
			case DiagonalDirections::kNorthWest: file--; rank++; break; 
			case DiagonalDirections::kNorthEast: file++; rank++; break; 
			case DiagonalDirections::kSouthEast: file++; rank--; break;
			case DiagonalDirections::kSouthWest: file--; rank--; break;
		}

		const auto square = file + rank * chess_constants::col_count_;
		result |= 0x1ull << square;		
	}
	return result;
}
