#include "MoveGenerator.h"

#include <optional>
#include <bit>
#include <cmath>

#include "Core/Debugging/Logging.h"
#include "Enums/Directions.h"
#include "Enums/Sides.h"
#include "Constants/CheckersConstants.h"

namespace checkers
{

MoveGenerator::MoveGenerator() noexcept
{
	CacheDiagonalRays();
}

Bitboard MoveGenerator::GetMovementsForQueen(size_t i, Bitboard enemies, Bitboard allies) const noexcept
{
	const auto blockers = enemies | allies;
	Bitboard attacks = 0;
	for (const auto dir : kDiagonalDirections)
	{
		attacks |= GetMaskedRayMovements(dir, i, blockers);
	}
	return attacks;
}

Bitboard MoveGenerator::GetCapturesForQueen(size_t i, Bitboard enemies, Bitboard allies) const noexcept
{
	Bitboard result = 0;
	for (const auto dir : kDiagonalDirections)
	{
		result |= GetMaskedRayCaptures(dir, i, enemies, allies);
	}
	return result;
}

Bitboard MoveGenerator::GetMovementsForPawn(size_t i, Sides side) noexcept
{
	if (Bitboard::IsIndexOutOfBounds(i))
	{
		return {};
	}
	const auto pawn_bb = 0x1ull << i;
	const auto pawn_east_excluded_bb = pawn_bb & ~(checkers::constants::file_h);
	const auto pawn_west_excluded_bb = pawn_bb & ~(checkers::constants::file_a);

	Bitboard result = 0;
	result |= MovePawnForward(side, pawn_east_excluded_bb, side == Sides::kBlack ? 7 : 9);
	result |= MovePawnForward(side, pawn_west_excluded_bb, side == Sides::kBlack ? 9 : 7);
	return result;
}

Bitboard MoveGenerator::GetCapturesForPawns(Sides side, Bitboard allies, Bitboard enemies, Bitboard pawns) noexcept
{
	const auto east_capable_pawns = (pawns & ~(checkers::constants::file_h | checkers::constants::file_g));
	const auto west_capable_pawns = (pawns & ~(checkers::constants::file_a | checkers::constants::file_b));	

	const auto jumped_east = MovePawnForward(side, east_capable_pawns, side == Sides::kBlack ? 7 : 9) & enemies;
	const auto jumped_west = MovePawnForward(side, west_capable_pawns, side == Sides::kBlack ? 9 : 7) & enemies;

	const auto empty_squares = ~(allies | enemies);

	const auto landed_east = MovePawnForward(side, jumped_east, side == Sides::kBlack ? 7 : 9) & empty_squares;
	const auto landed_west = MovePawnForward(side, jumped_west, side == Sides::kBlack ? 9 : 7) & empty_squares;

	return landed_east | landed_west;
}

Bitboard MoveGenerator::GetCapturesForPawn(Sides side, Bitboard allies, Bitboard enemies, size_t i) noexcept
{
	return GetCapturesForPawns(side, allies, enemies, 0x1ull << i);
}

std::optional<size_t> MoveGenerator::GetEnemyIndexCapturedByPawn(size_t from, size_t to, VerticalDirections dir_y) noexcept
{
	const auto from_col = from % 8;
	const auto from_row = from / 8;
	const auto to_col = to % 8;
	const auto to_row = to / 8;

	const auto distance_y = std::abs(static_cast<int>(from_col) - static_cast<int>(to_col));
	const auto distance_x = std::abs(static_cast<int>(from_row) - static_cast<int>(to_row));

	if (distance_y != 2 || distance_x != 2)
	{
		return {};
	}

	const auto is_east = from_col > to_col;
	const auto is_west = from_col < to_col;

	std::optional<size_t> enemy_i{};
	if (dir_y == VerticalDirections::kUp && is_west)
	{
		enemy_i = from + 9;
	}
	if (dir_y == VerticalDirections::kUp && is_east)
	{
		enemy_i = from + 7;
	}
	if (dir_y == VerticalDirections::kDown && is_west)
	{
		enemy_i = from - 7;
	}
	if (dir_y == VerticalDirections::kDown && is_east)
	{
		enemy_i = from - 9;
	}
	return enemy_i;
}

std::optional<size_t> MoveGenerator::GetEnemyIndexCapturedByQueen(size_t from, size_t to, VerticalDirections dir_y, Bitboard enemies) const noexcept
{
	const auto is_east = from % 8 < to % 8;

	const auto dir = GetDiagonalDirection(is_east, dir_y == VerticalDirections::kUp);
	const auto blockers = diagonal_rays_[static_cast<int>(dir)][from] & enemies;
	if (std::popcount(blockers.bits_) == 0)
	{
		return {};
	}
	size_t enemy_i = {};
	if (dir_y == VerticalDirections::kUp)
	{
		enemy_i = std::countr_zero(blockers.bits_);
	}
	else
	{
		enemy_i = static_cast<size_t>(checkers::constants::total_squares_) - 1 - std::countl_zero(blockers.bits_);
	}
	return enemy_i;
}

Bitboard MoveGenerator::GetMaskedRayMovements(DiagonalDirections dir, size_t i, Bitboard blockers) const noexcept
{
	const auto mask = CastRay(dir, i, blockers);
	if (!mask.has_value())
	{
		return GetRay(dir, i);
	}
	const auto vertical_dir = GetVerticalDirection(dir);
	auto first_blocker_index = GetFirstBlockerIndex(mask.value(), vertical_dir == VerticalDirections::kUp);

	auto movements = GetRay(dir, i);
	movements &= ~diagonal_rays_[static_cast<int>(dir)][first_blocker_index];
	movements &= ~blockers;
	return movements;
}

Bitboard MoveGenerator::GetMaskedRayCaptures(DiagonalDirections dir, size_t i, Bitboard enemies, Bitboard allies) const noexcept
{
	const auto blockers = enemies | allies;

	// Cast a ray from the user into the specific direction
	const auto mask = CastRay(dir, i, blockers);
	if (!mask.has_value())
	{
		return {};
	}
	const auto vertical_dir = GetVerticalDirection(dir);

	const auto first_blocker_index = GetFirstBlockerIndex(mask.value(), vertical_dir == VerticalDirections::kUp);

	if ((Bitboard((0x1ull << first_blocker_index)) & allies) != 0)
	{
		return {};
	}

	// If there's only one blocker, return
	if (std::popcount(mask.value().bits_) == 1)
	{
		return GetRay(dir, first_blocker_index) & ~blockers;
	}
	
	auto mask_copy = mask.value();
	mask_copy.ClearBit(first_blocker_index);

	// Get the second blocker	
	const auto second_blocker_index = GetFirstBlockerIndex(mask_copy, vertical_dir == VerticalDirections::kUp);

	// Add the rays from the first blocker
	// and remove the rays past the second blocker
	auto captures = GetRay(dir, first_blocker_index) & ~GetRay(dir, second_blocker_index);
	// Remove the blockers
	return captures & ~blockers;
}

Bitboard MoveGenerator::MovePawnForward(Sides side, Bitboard pawn, size_t shift) noexcept
{
	return side == Sides::kBlack ? pawn >> shift : pawn << shift;
}

size_t MoveGenerator::GetFirstBlockerIndex(Bitboard board, bool is_above) noexcept
{
	return is_above
		? std::countr_zero(board.bits_)
		: static_cast<size_t>(checkers::constants::total_squares_ - 1) - std::countl_zero(board.bits_);
}

std::optional<Bitboard> MoveGenerator::CastRay(DiagonalDirections dir, size_t i, Bitboard blockers) const noexcept
{
	const auto rays = GetRay(dir, i);
	if (blockers == 0)
	{
		return {};
	}
	const auto mask = rays & blockers;
	if (mask == 0)
	{
		return {};
	}
	return mask;
}

Bitboard MoveGenerator::GetRay(DiagonalDirections dir, size_t i) const noexcept
{
	return diagonal_rays_[static_cast<int>(dir)][i];
}

void MoveGenerator::CacheDiagonalRays() noexcept
{
	for (size_t i = 0; i < checkers::constants::total_squares_; ++i)
	{
		for (const auto dir : kDiagonalDirections)
		{
			diagonal_rays_[static_cast<int>(dir)][i] |= GenerateDiagonalRays(dir, i);
		}
	}
}

Bitboard MoveGenerator::GenerateDiagonalRays(DiagonalDirections dir, size_t index) noexcept
{
	Bitboard result = 0;

	auto file = index % checkers::constants::col_count_;
	auto rank = index / checkers::constants::col_count_;

	while (true)
	{
		if (   (dir == DiagonalDirections::kNorthWest && (file <= 0 || rank >= checkers::constants::row_count_-1))
			|| (dir == DiagonalDirections::kNorthEast && (rank >= checkers::constants::row_count_ -1 || file >= checkers::constants::col_count_-1))
			|| (dir == DiagonalDirections::kSouthEast && (rank <= 0 || file >= checkers::constants::col_count_-1))
			|| (dir == DiagonalDirections::kSouthWest && (rank <= 0 || file <= 0)))
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

		const auto square = file + rank * checkers::constants::col_count_;
		result |= 0x1ull << square;		
	}
	return result;
}

} // namespace checkers