#pragma once

#include <optional>

#include "Enums/Directions.h"
#include "Enums/Sides.h"

#include "Types/Bitboard.h"

namespace checkers
{

using ray_list = std::array<std::array<Bitboard, checkers_constants::total_squares_>, static_cast<int>(DiagonalDirections::kCount)>;

class MoveGenerator
{
public:
	MoveGenerator() noexcept;

	Bitboard GetMovementsForQueen(size_t i, Bitboard enemies, Bitboard allies) const noexcept;
	Bitboard GetCapturesForQueen (size_t i, Bitboard enemies, Bitboard allies) const noexcept;

	static Bitboard GetMovementsForPawn(size_t i, Sides side) noexcept;
	static Bitboard GetCapturesForPawns(Sides side, Bitboard allies, Bitboard enemies, Bitboard pawns) noexcept;
	static Bitboard GetCapturesForPawn (Sides side, Bitboard allies, Bitboard enemies, size_t i) noexcept;

	static std::optional<size_t> GetEnemyIndexCapturedByPawn(size_t from, size_t to, VerticalDirections dir_y) noexcept;
	std::optional<size_t> GetEnemyIndexCapturedByQueen(size_t from, size_t to, VerticalDirections dir_y, Bitboard enemies) const noexcept;

private:
	ray_list diagonal_rays_{};

	Bitboard GetMaskedRayMovements(DiagonalDirections dir, size_t i, Bitboard blockers) const noexcept;
	Bitboard GetMaskedRayCaptures (DiagonalDirections dir, size_t i, Bitboard enemies, Bitboard allies) const noexcept;

	static Bitboard MovePawnForward(Sides side, Bitboard pawn, size_t shift) noexcept;

	static size_t GetFirstBlockerIndex(Bitboard board, bool is_above) noexcept;

	std::optional<Bitboard> CastRay(DiagonalDirections dir, size_t i, Bitboard blockers) const noexcept;
	Bitboard GetRay(DiagonalDirections dir, size_t i) const noexcept;
	
	void CacheDiagonalRays() noexcept;		

	static Bitboard GenerateDiagonalRays(DiagonalDirections  dir, size_t index) noexcept;
};

} // namespace checkers