#pragma once

#include <optional>

#include "CheckersTypes.h"
#include "Enums/Directions.h"

class MoveGenerator
{
public:
	MoveGenerator() noexcept;

	checkers_types::bitboard GetMovementsForQueen(size_t i, checkers_types::bitboard enemies, checkers_types::bitboard allies) const noexcept;
	checkers_types::bitboard GetCapturesForQueen (size_t i, checkers_types::bitboard enemies, checkers_types::bitboard allies) const noexcept;

private:
	checkers_types::ray_list diagonal_rays_{};

	checkers_types::bitboard GetMaskedRayMovements(DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept;
	checkers_types::bitboard GetMaskedRayCaptures (DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept;

	std::optional<checkers_types::bitboard> CastRay(DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept;
	checkers_types::bitboard GetRay(DiagonalDirections dir, size_t i) const noexcept;
	
	void CacheDiagonalRays() noexcept;	
	static checkers_types::bitboard GenerateDiagonalRays(DiagonalDirections  dir, size_t index) noexcept;
};