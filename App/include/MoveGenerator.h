#pragma once

#include <optional>

#include "Enums/Directions.h"
#include "Enums/Sides.h"
#include "CheckersTypes.h"

class MoveGenerator
{
public:
	MoveGenerator() noexcept;

	checkers_types::bitboard GetMovementsForQueen(size_t i, checkers_types::bitboard enemies, checkers_types::bitboard allies) const noexcept;
	checkers_types::bitboard GetCapturesForQueen (size_t i, checkers_types::bitboard enemies, checkers_types::bitboard allies) const noexcept;

	static checkers_types::bitboard GetMovementsForPawn(size_t i, Sides side) noexcept;
	static checkers_types::bitboard GetCapturesForPawns(Sides side, checkers_types::bitboard allies, checkers_types::bitboard enemies, checkers_types::bitboard pawns) noexcept;
	static checkers_types::bitboard GetCapturesForPawn (Sides side, checkers_types::bitboard allies, checkers_types::bitboard enemies, size_t i) noexcept;

private:
	checkers_types::ray_list diagonal_rays_{};

	checkers_types::bitboard GetMaskedRayMovements(DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept;
	checkers_types::bitboard GetMaskedRayCaptures (DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept;

	static checkers_types::bitboard MovePawnForward(Sides side, checkers_types::bitboard pawn, size_t shift) noexcept;

	static size_t GetFirstBlockerIndex(checkers_types::bitboard board, bool is_above) noexcept;

	std::optional<checkers_types::bitboard> CastRay(DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept;
	checkers_types::bitboard GetRay(DiagonalDirections dir, size_t i) const noexcept;
	
	void CacheDiagonalRays() noexcept;	
	static checkers_types::bitboard GenerateDiagonalRays(DiagonalDirections  dir, size_t index) noexcept;
};