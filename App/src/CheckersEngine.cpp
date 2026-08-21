#include "CheckersEngine.h"

#include <optional>
#include <string>
#include <expected>
#include <variant>

#include <fmt/base.h>

#include "Core/Utils/BitUtils.h"
#include "Enums/Sides.h"
#include "Enums/Pieces.h"
#include "Enums/Directions.h"
#include "CommandParser.h"
#include "CheckersTypes.h"

void CheckersEngine::Print() const noexcept
{
	const auto side_to_play = current_team_ == Sides::kWhite ? "White" : "Black";
	fmt::print("\nIt's {}'s turn:", side_to_play);
	bb_manager_.Print();
}

std::expected<GameState, std::string> CheckersEngine::ExecuteCommand(std::string cmd) noexcept
{
	return command_parser::ParseCommand(cmd).and_then(
		[this](const auto& command) -> std::expected<GameState, std::string>
		{
			if (auto value = std::get_if<CommandMove>(&command))
			{
				if (game_over_) return std::unexpected("Invalid move: the game's already over.");

				return MovePiece(value->move_from_, value->move_to_);
			}
			return game_over_ ? GameState::kFinished : GameState::kPlaying;
		}
	);
}

Sides CheckersEngine::GetEnemySide() const noexcept
{
	return current_team_ == Sides::kWhite ? Sides::kBlack : Sides::kWhite;
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
	
	if (!core::utils::bits::IsBitSet(movements, to))
	{
		return std::unexpected("Invalid move: unreachable or blocked square.");
	}

	const auto captures = piece_type == Pieces::kPawn
		? available_pawn_captures_
		: available_queen_captures_;

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

	bb_manager_.MovePiece(from, to);

	last_played_piece_to_ = to;
	
	return FinishTurn();
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

	available_pawn_captures_  = 0;
	available_queen_captures_ = 0;

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
	const auto allies  = bb_manager_.GetBoard(current_team_);
	const auto enemies = bb_manager_.GetBoard(GetEnemySide());

	available_pawn_captures_  = move_generator_.GetCapturesForPawn (current_team_, allies, enemies, last_played_piece_to_.value());
	available_queen_captures_ = move_generator_.GetCapturesForQueen(last_played_piece_to_.value(), enemies, allies);
}