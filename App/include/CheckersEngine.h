#pragma once

#include <string>
#include <optional>
#include <expected>

#include "Enums/Sides.h"
#include "Types/Bitboard.h"
#include "BitboardManager.h"
#include "MoveGenerator.h"

namespace checkers
{

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
	Bitboard GetBoard(Sides side, Pieces piece) const noexcept;
	
	void Print() const noexcept;

	std::expected<GameState, std::string> MovePiece(size_t from, size_t to) noexcept;

	std::expected<Bitboard, std::string> GetMoves   (size_t at) const noexcept;
	std::expected<Bitboard, std::string> GetCaptures(size_t at) const noexcept;

	Bitboard GetCaptures() const noexcept;

	Sides GetEnemySide  () const noexcept;
	Sides GetCurrentTeam() const noexcept;

	bool IsIndexOccupied(size_t i) const noexcept;

private:
	BitboardManager bb_manager_{};
	MoveGenerator   move_generator_{};

	bool game_over_ = false;
	bool just_captured_piece_ = false;

	Sides current_team_ = Sides::kWhite;

	Bitboard available_pawn_captures_ = 0;
	Bitboard available_queen_captures_ = 0;

	std::optional<size_t> last_played_piece_to_ = {};

	GameState FinishTurn() noexcept;
	bool CheckForCombos() const noexcept;

	std::expected<void, std::string> CapturePiece(size_t from, size_t to) noexcept;

	void UpdatePossibleCaptures(Sides side) noexcept;
};

} // namespace checkers