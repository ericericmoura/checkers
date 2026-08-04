#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <optional>
#include <utility>		

#include "Constants/ChessConstants.h"
#include "Enums/Sides.h"
#include "Enums/Pieces.h"
#include "Enums/Directions.h"

using bitboard      = uint64_t;
using bitboard_list = std::array<std::array<bitboard, static_cast<size_t>(Pieces::kCount)>, static_cast<size_t>(Sides::kCount)>;
using ray_list      = std::array<std::array<bitboard, chess_constants::col_count_* chess_constants::row_count_>, static_cast<int>(DiagonalDirections::kCount)>;

class CheckersEngine
{
public:
	void Init() noexcept;
	void Print() const noexcept;
	void ExecuteCommand(std::string cmd) noexcept;

private:
	bitboard_list bitboards_{};
	bitboard white_bb_{};
	bitboard black_bb_{};
	ray_list diagonal_rays_{};

	Sides current_team_ = Sides::kWhite;

	bitboard available_captures_ = 0;

	void FinishTurn() noexcept;

	bool CapturePiece(size_t from, size_t to) noexcept;
	bool MovePiece(size_t from, size_t to) noexcept;

	std::optional<Sides>  GetSideByIndex (size_t i) const noexcept;
	std::optional<Pieces> GetPieceTypeByIndex(size_t i) const noexcept;

	bool IsIndexOccupied(size_t i) const noexcept;
	
	bitboard GetBoard(Sides side, Pieces piece) const noexcept;

	void SetBoard(Sides side, Pieces piece, bitboard board) noexcept;
	
	std::pair<std::string, std::string> SplitCommand(std::string cmd) const noexcept;
	std::optional<size_t> GetIndexFromNotation(std::string notation) const noexcept;

	bitboard GetPossibleMovements(Sides side, Pieces type, size_t i) const noexcept;
	bitboard GetPossibleMovementsForQueen(Sides side, size_t i) const noexcept;

	bitboard GetMaskedRayAttacks(DiagonalDirections dir, size_t i, bitboard blockers) const noexcept;

	bitboard CheckForCaptures(Sides side) noexcept;

	void CacheDiagonalRays() noexcept;
	bitboard GenerateDiagonalRays(DiagonalDirections dir, size_t index) const noexcept;
};