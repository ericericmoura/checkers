#pragma once

#include <string>
#include <optional>
#include <expected>

#include "Enums/Sides.h"
#include "CheckersTypes.h"
#include "BitboardManager.h"
#include "MoveGenerator.h"

enum class GameState
{
	kPlaying,
	kFinished,
	kBlackWon,
	kWhiteWon
};

class CheckersEngine
{
public:
	void Print() const noexcept;

	std::expected<GameState, std::string> MovePiece(size_t from, size_t to) noexcept;

	std::expected<checkers_types::bitboard, std::string> GetMoves(size_t at) const noexcept;

	Sides GetEnemySide() const noexcept;
	static Sides GetEnemySide(Sides side) noexcept;

private:
	BitboardManager bb_manager_{};
	MoveGenerator   move_generator_{};

	bool game_over_ = false;
	bool just_captured_piece_ = false;

	Sides current_team_ = Sides::kBlack;

	checkers_types::bitboard available_pawn_captures_ = 0;
	checkers_types::bitboard available_queen_captures_ = 0;

	std::optional<size_t> last_played_piece_to_ = {};

	GameState FinishTurn() noexcept;
	bool CheckForCombos() const noexcept;

	std::expected<void, std::string> CapturePiece(size_t from, size_t to) noexcept;

	void UpdatePossibleCaptures(Sides side) noexcept;
};