#include "CheckersEngine.h"

#include <optional>
#include <string>
#include <utility>
#include <bit>
#include <expected>
#include <variant>

#include <fmt/base.h>
#include <fmt/color.h>

#include "Core/Utils/BitUtils.h"
#include "Core/Debugging/Logging.h"
#include "Constants/CheckersConstants.h"
#include "Enums/Sides.h"
#include "Enums/Pieces.h"
#include "Enums/Directions.h"
#include "Utils/CheckersUtils.h"
#include "Utils/DirectionUtils.h"
#include "CommandParser.h"
#include <CheckersTypes.h>

void CheckersEngine::Print() const noexcept
{
	const auto side_to_play = current_team_ == Sides::kWhite ? "White" : "Black";
	fmt::print("\nIt's {}'s turn:", side_to_play);
	bb_manager_.Print();
}

std::expected<void, std::string> CheckersEngine::ExecuteCommand(std::string cmd) noexcept
{
	if (game_over_)
	{
		return std::unexpected("The game's over.");
	}
	return command_parser::ParseCommand(cmd).transform(
		[this](const auto& command)
		{
			if (auto value = std::get_if<CommandMove>(&command))
			{
				MovePiece(value->move_from_, value->move_to_);
			}
		}
	);
}

//void CheckersEngine::FinishTurn() noexcept
//{
//	if (black_bb_ == 0)
//	{
//		fmt::println("CONGRATS! Black won the game!");
//		game_over_ = true;
//		return;
//	}
//	if (white_bb_ == 0)
//	{
//		fmt::println("CONGRATS! White won the game!");
//		game_over_ = true;
//		return;
//	}
//
//	available_pawn_captures_  = 0;
//	available_queen_captures_ = 0;
//
//	const auto is_combo = CheckForCombos();
//	if (!is_combo)
//	{
//		current_team_ = current_team_ == Sides::kWhite
//			? Sides::kBlack
//			: Sides::kWhite;
//
//		last_played_piece_to_ = {};
//	}
//
//	UpdatePossibleCaptures(current_team_);	
//}
//
//bool CheckersEngine::CheckForCombos() const noexcept
//{
//	if (!last_played_piece_to_.has_value() || !just_captured_piece_)
//	{
//		return false;
//	}
//	const auto type = GetPieceTypeByIndex(last_played_piece_to_.value());
//	bitboard captures = 0;
//	if (type == Pieces::kPawn)
//	{
//		captures = utils::pawn::GetPawnCaptures(current_team_, black_bb_, white_bb_, GetBoard(current_team_, Pieces::kPawn), last_played_piece_to_.value());
//	}
//	else
//	{
//		captures = GetCapturesForQueen(current_team_, last_played_piece_to_.value());
//	}
//	return captures != 0;
//}
//
//bool CheckersEngine::CapturePiece(size_t from, size_t to) noexcept
//{
//	const auto side = GetSideByIndex     (from);
//	const auto type = GetPieceTypeByIndex(from);
//
//	const auto available_captures = type == Pieces::kPawn ? available_pawn_captures_ : available_queen_captures_;
//
//	if (!core::utils::IsBitSet(available_captures, to))
//	{
//		return false;
//	}
//
//	const auto dir_y = static_cast<int>(from) - static_cast<int>(to) > 0 ? VerticalDirections::kDown : VerticalDirections::kUp;	
//
//	std::optional<size_t> enemy_i{};
//	if (type == Pieces::kPawn)
//	{
//		enemy_i = utils::pawn::CapturePieceWithPawn(from, to, dir_y);
//	}
//	if (type == Pieces::kQueen)
//	{
//		enemy_i = CapturePieceWithQueen(from, to, dir_y);
//	}
//
//	if (!enemy_i.has_value())
//	{
//		return false;
//	}
//	const auto enemy_side = side.value() == Sides::kWhite ? Sides::kBlack : Sides::kWhite;
//	const auto enemy_type = GetPieceTypeByIndex(enemy_i.value());
//
//	if (!enemy_type.has_value())
//	{
//		return false;
//	}
//
//	auto board = GetBoard(enemy_side, enemy_type.value());
//	board = core::utils::ClearBit(board, enemy_i.value());
//
//	SetBoard(enemy_side, enemy_type.value(), board);
//	return true;
//}
//
//std::optional<size_t> CheckersEngine::CapturePieceWithQueen(size_t from, size_t to, VerticalDirections dir_y) const noexcept
//{
//	const auto is_east = from%8 < to%8;
//
//	const auto dir = utils::directions::GetDiagonalDirection(is_east, dir_y == VerticalDirections::kUp);
//
//	const auto opposite_side_bb = current_team_ == Sides::kBlack ? white_bb_ : black_bb_;
//	const auto blockers         = diagonal_rays_[static_cast<int>(dir)][from] & opposite_side_bb;
//
//	if (std::popcount(blockers) == 0)
//	{
//		return {};
//	}
//
//	size_t enemy_i = {};
//	if (dir_y == VerticalDirections::kUp)
//	{
//		enemy_i = std::countr_zero(blockers);
//	}
//	else
//	{
//		enemy_i = static_cast<size_t>(checkers_constants::total_squares_) - 1 - std::countl_zero(blockers);
//	}
//	return enemy_i;
//}

Sides CheckersEngine::GetEnemySide() const noexcept
{
	return current_team_ == Sides::kWhite ? Sides::kBlack : Sides::kWhite;
}

std::expected<void, std::string> CheckersEngine::MovePiece(size_t from, size_t to) noexcept
{	
	if (last_played_piece_to_.has_value() && last_played_piece_to_.value() != from)
	{
		return std::unexpected("Invalid move: you are in a combo, you must move the previous piece.");
	}

	const auto piece_type = bb_manager_.GetPieceTypeByIndex(from);
	const auto piece_side = bb_manager_.GetSideByIndex(from);

	if (!piece_side.has_value() || !piece_type.has_value())
	{
		return std::unexpected("Invalid piece: it's either non-existent or invalid.");
	}

	const auto captures = piece_type == Pieces::kPawn 
		? available_pawn_captures_ 
		: available_queen_captures_;
	
	const auto should_capture = captures != 0;	
	if (should_capture && !core::utils::bits::IsBitSet(captures, to))
	{
		return std::unexpected("Invalid move: you are required to capture a piece.");
	}		

	if (piece_side != current_team_)
	{
		return std::unexpected("Invalid move: can't move an enemy piece.");
	}

	const auto movements = piece_type == Pieces::kPawn 
		? move_generator_.GetMovementsForPawn (from, current_team_)
		: move_generator_.GetMovementsForQueen(from, bb_manager_.GetBoard(GetEnemySide()), bb_manager_.GetBoard(current_team_));
	
	if (!core::utils::bits::IsBitSet(movements, to))
	{
		return std::unexpected("Invalid move: unreachable or blocked square.");
	}

	if (should_capture)
	{
		just_captured_piece_ = CapturePiece(from, to);
	}
	if (should_capture && !just_captured_piece_)
	{
		return std::unexpected("Invalid capture.");
	}	
	//auto board = GetBoard(side.value(), type.value());
	//board = core::utils::ClearBit(board, from);
	//board = core::utils::SetBit  (board, to  );
	//SetBoard(side.value(), type.value(), board);

	//const auto row = to / checkers_constants::col_count_;

	//if (type == Pieces::kPawn && ((side == Sides::kWhite && row == checkers_constants::row_count_ - 1) || (side == Sides::kBlack && row == 0)))
	//{
	//	SetBoard(side.value(), type.value()  , core::utils::ClearBit(GetBoard(side.value(), type.value())  , to));
	//	SetBoard(side.value(), Pieces::kQueen, core::utils::SetBit  (GetBoard(side.value(), Pieces::kQueen), to));
	//}

	//just_captured_piece_ = captured;

	last_played_piece_to_ = to;
	FinishTurn();
}

//
//void CheckersEngine::UpdatePossibleCaptures(Sides side) noexcept
//{
//	available_pawn_captures_  = utils::pawn::GetPawnCaptures(side, black_bb_, white_bb_, GetBoard(side, Pieces::kPawn));
//	
//	UpdateQueenCaptures(side);
//}