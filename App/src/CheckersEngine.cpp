#include "CheckersEngine.h"

#include <optional>
#include <string>
#include <expected>
#include <bit>
#include <cassert>

#include <fmt/base.h>

#include "Core/Utils/BitUtils.h"
#include "Enums/Sides.h"
#include "Enums/Pieces.h"
#include "Enums/Directions.h"
#include "CheckersTypes.h"

CheckersEngine::CheckersEngine()
{
	UpdatePossibleCaptures(current_team_);
}

void CheckersEngine::Print() const noexcept
{
	const auto side_to_play = current_team_ == Sides::kWhite ? "White" : "Black";
	fmt::print("\n\nIt's {}'s turn:", side_to_play);
	bb_manager_.Print();
}

Sides CheckersEngine::GetEnemySide() const noexcept
{
	return GetEnemySide(current_team_);
}

Sides CheckersEngine::GetEnemySide(Sides side) noexcept
{
	return side == Sides::kWhite ? Sides::kBlack : Sides::kWhite;
}

std::expected<GameState, std::string> CheckersEngine::MovePiece(size_t from, size_t to) noexcept
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

	if (piece_side != current_team_)
	{
		return std::unexpected("Invalid move: can't move an enemy piece.");
	}

	const auto movements = piece_type == Pieces::kPawn 
		? move_generator_.GetMovementsForPawn (from, current_team_)
		: move_generator_.GetMovementsForQueen(from, bb_manager_.GetBoard(GetEnemySide()), bb_manager_.GetBoard(current_team_));
	
	const auto captures = piece_type == Pieces::kPawn
		? available_pawn_captures_
		: available_queen_captures_;

	if (!core::utils::bits::IsBitSet(movements | captures, to))
	{
		return std::unexpected("Invalid move: unreachable or blocked square.");
	}

	const auto should_capture = captures != 0;
	if (should_capture && !core::utils::bits::IsBitSet(captures, to))
	{
		return std::unexpected("Invalid move: you are required to capture a piece.");
	}

	if (should_capture)
	{
		const auto capture_result = CapturePiece(from, to);
		if (!capture_result)
		{
			just_captured_piece_ = false;
			return std::unexpected(capture_result.error());
		}
		just_captured_piece_ = true;
	}

	const auto move_result = bb_manager_.MovePiece(from, to);
	if (!move_result)
	{
		return std::unexpected(move_result.error());
	}

	last_played_piece_to_ = to;
	
	return FinishTurn();
}

std::expected<checkers_types::bitboard, std::string> CheckersEngine::GetMoves(size_t at) const noexcept
{
	if (!bb_manager_.IsIndexOccupied(at))
	{
		return std::unexpected("Invalid index: there's no piece at the specified square.");
	}

	const auto side = bb_manager_.GetSideByIndex(at);
	if (!side.has_value())
	{
		return std::unexpected("Invalid index: there's no piece at the specified square.");
	}
	const auto enemy_side = GetEnemySide(side.value());

	const auto moves = bb_manager_.GetPieceTypeByIndex(at) == Pieces::kPawn
		? move_generator_.GetMovementsForPawn (at, side.value())
		: move_generator_.GetMovementsForQueen(at, bb_manager_.GetBoard(enemy_side), bb_manager_.GetBoard(side.value()));

	return moves;
}

std::expected<checkers_types::bitboard, std::string> CheckersEngine::GetCaptures(size_t at) const noexcept
{
	if (!bb_manager_.IsIndexOccupied(at))
	{
		return std::unexpected("Invalid index: there's no piece at the specified square.");
	}

	const auto side = bb_manager_.GetSideByIndex(at);
	if (!side.has_value())
	{
		return std::unexpected("Invalid index: there's no piece at the specified square.");
	}
	const auto enemy_side = GetEnemySide(side.value());

	const auto allies  = bb_manager_.GetBoard(side.value());
	const auto enemies = bb_manager_.GetBoard(enemy_side  );

	const auto captures = bb_manager_.GetPieceTypeByIndex(at) == Pieces::kPawn
		? move_generator_.GetCapturesForPawn(side.value(), allies, enemies, at)
		: move_generator_.GetCapturesForQueen(at, enemies, allies);

	return captures;
}

GameState CheckersEngine::FinishTurn() noexcept
{
	if (bb_manager_.GetBoard(Sides::kWhite) == 0)
	{
		game_over_ = true;
		return GameState::kBlackWon;
	}
	if (bb_manager_.GetBoard(Sides::kBlack) == 0)
	{
		game_over_ = true;
		return GameState::kWhiteWon;
	}

	const auto is_combo = CheckForCombos();
	if (!is_combo)
	{
		current_team_ = GetEnemySide();

		last_played_piece_to_ = {};
	}

	UpdatePossibleCaptures(current_team_);

	return GameState::kPlaying;
}

bool CheckersEngine::CheckForCombos() const noexcept
{
	if (!last_played_piece_to_.has_value() || !just_captured_piece_)
	{
		return false;
	}
	const auto type = bb_manager_.GetPieceTypeByIndex(last_played_piece_to_.value());

	const auto allies  = bb_manager_.GetBoard(current_team_);
	const auto enemies = bb_manager_.GetBoard(GetEnemySide());
	
	checkers_types::bitboard captures = type == Pieces::kPawn
		? move_generator_.GetCapturesForPawn(current_team_, allies, enemies, last_played_piece_to_.value())
		: move_generator_.GetCapturesForQueen(last_played_piece_to_.value(), enemies, allies);

	return captures != 0;
}

std::expected<void, std::string> CheckersEngine::CapturePiece(size_t from, size_t to) noexcept
{
	const auto piece_side = bb_manager_.GetSideByIndex     (from);
	const auto piece_type = bb_manager_.GetPieceTypeByIndex(from);

	const auto captures = piece_type == Pieces::kPawn ? available_pawn_captures_ : available_queen_captures_;

	if (!core::utils::bits::IsBitSet(captures, to))
	{
		return std::unexpected("Invalid capture: there's no available capture at that square.");
	}

	const auto dir_y = static_cast<int>(from) - static_cast<int>(to) > 0 ? VerticalDirections::kDown : VerticalDirections::kUp;
	std::optional<size_t> enemy_i{};
	if (piece_type == Pieces::kPawn)
	{
		enemy_i = move_generator_.GetEnemyIndexCapturedByPawn(from, to, dir_y);
	}
	if (piece_type == Pieces::kQueen)
	{
		enemy_i = move_generator_.GetEnemyIndexCapturedByQueen(from, to, dir_y, bb_manager_.GetBoard(GetEnemySide()));
	}

	if (!enemy_i.has_value())
	{
		return std::unexpected("Invalid capture: there's no enemy to capture.");
	}
	bb_manager_.RemovePiece(enemy_i.value());
}

void CheckersEngine::UpdatePossibleCaptures(Sides side) noexcept
{
	available_pawn_captures_  = 0;
	available_queen_captures_ = 0;

	const auto enemies = bb_manager_.GetBoard(GetEnemySide());

	auto allies = bb_manager_.GetBoard(current_team_);
	while (std::popcount(allies) != 0)
	{
		const auto i = std::countr_zero(allies);
		allies = core::utils::bits::ClearBit(allies, i);

		const auto type = bb_manager_.GetPieceTypeByIndex(i);
		assert(type.has_value());

		if (type.value() == Pieces::kPawn)
		{
			available_pawn_captures_  |= move_generator_.GetCapturesForPawn (current_team_, allies, enemies, i);
		}
		else
		{
			available_queen_captures_ |= move_generator_.GetCapturesForQueen(i, enemies, allies);
		}
	}
}