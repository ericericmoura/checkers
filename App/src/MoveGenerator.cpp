#include "MoveGenerator.h"

#include <optional>
#include <bit>

#include "Core/Utils/BitUtils.h"
#include "Enums/Directions.h"
#include "Enums/Sides.h"
#include "Constants/CheckersConstants.h"
#include "Utils/DirectionUtils.h"
#include "Utils/CheckersUtils.h"
#include "CheckersTypes.h"

MoveGenerator::MoveGenerator() noexcept
{
	CacheDiagonalRays();
}

checkers_types::bitboard MoveGenerator::GetMovementsForQueen(size_t i, checkers_types::bitboard enemies, checkers_types::bitboard allies) const noexcept
{
	const auto blockers = enemies | allies;
	checkers_types::bitboard attacks = 0;
	for (const auto dir : utils::directions::kDiagonalDirections)
	{
		attacks |= GetMaskedRayMovements(dir, i, blockers);
	}
	return attacks;
}

checkers_types::bitboard MoveGenerator::GetCapturesForQueen(size_t i, checkers_types::bitboard enemies, checkers_types::bitboard allies) const noexcept
{
	const auto blockers = enemies | allies;
	checkers_types::bitboard result = 0;
	for (const auto dir : utils::directions::kDiagonalDirections)
	{
		result |= GetMaskedRayCaptures(dir, i, blockers);
	}
	return result;
}

checkers_types::bitboard MoveGenerator::GetMovementsForPawn(size_t i, Sides side) noexcept
{
	if (utils::checkers::IsIndexOutOfBounds(i))
	{
		return {};
	}
	const auto pawn_bb = 0x1ull << i;
	const auto pawn_east_excluded_bb = pawn_bb & ~(checkers_constants::file_h);
	const auto pawn_west_excluded_bb = pawn_bb & ~(checkers_constants::file_a);

	checkers_types::bitboard result = 0;
	result |= MovePawnForward(side, pawn_east_excluded_bb, side == Sides::kBlack ? 7 : 9);
	result |= MovePawnForward(side, pawn_west_excluded_bb, side == Sides::kBlack ? 9 : 7);
	return result;
}

checkers_types::bitboard MoveGenerator::GetCapturesForPawns(Sides side, checkers_types::bitboard allies, checkers_types::bitboard enemies, checkers_types::bitboard pawns) noexcept
{
	const auto east_capable_pawns = (pawns & ~(checkers_constants::file_h | checkers_constants::file_g));
	const auto west_capable_pawns = (pawns & ~(checkers_constants::file_a | checkers_constants::file_b));	

	const auto jumped_east = MovePawnForward(side, east_capable_pawns, side == Sides::kBlack ? 7 : 9) & enemies;
	const auto jumped_west = MovePawnForward(side, west_capable_pawns, side == Sides::kBlack ? 9 : 7) & enemies;

	const auto empty_squares = ~(allies | enemies);

	const auto landed_east = MovePawnForward(side, jumped_east, side == Sides::kBlack ? 7 : 9) & empty_squares;
	const auto landed_west = MovePawnForward(side, jumped_west, side == Sides::kBlack ? 9 : 7) & empty_squares;

	return landed_east & landed_west;
}

checkers_types::bitboard MoveGenerator::GetCapturesForPawn(Sides side, checkers_types::bitboard allies, checkers_types::bitboard enemies, size_t i) noexcept
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

std::optional<size_t> MoveGenerator::GetEnemyIndexCapturedByQueen(size_t from, size_t to, VerticalDirections dir_y, checkers_types::bitboard enemies) const noexcept
{
	const auto is_east = from % 8 < to % 8;

	const auto dir = utils::directions::GetDiagonalDirection(is_east, dir_y == VerticalDirections::kUp);
	const auto blockers = diagonal_rays_[static_cast<int>(dir)][from] & enemies;
	if (std::popcount(blockers) == 0)
	{
		return {};
	}
	size_t enemy_i = {};
	if (dir_y == VerticalDirections::kUp)
	{
		enemy_i = std::countr_zero(blockers);
	}
	else
	{
		enemy_i = static_cast<size_t>(checkers_constants::total_squares_) - 1 - std::countl_zero(blockers);
	}
	return enemy_i;
}

checkers_types::bitboard MoveGenerator::GetMaskedRayMovements(DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept
{
	const auto mask = CastRay(dir, i, blockers);
	if (!mask.has_value())
	{
		return GetRay(dir, i);
	}
	const auto vertical_dir = utils::directions::GetVerticalDirection(dir);
	auto first_blocker_index = GetFirstBlockerIndex(mask.value(), vertical_dir == VerticalDirections::kUp);

	auto movements = GetRay(dir, i);
	movements &= ~diagonal_rays_[static_cast<int>(dir)][first_blocker_index];
	movements &= ~blockers;
	return movements;
}

checkers_types::bitboard MoveGenerator::GetMaskedRayCaptures(DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept
{
	// Cast a ray from the user into the specific direction
	const auto mask = CastRay(dir, i, blockers);
	if (!mask.has_value())
	{
		return {};
	}
	const auto vertical_dir = utils::directions::GetVerticalDirection(dir);

	const auto first_blocker_index = GetFirstBlockerIndex(mask.value(), vertical_dir == VerticalDirections::kUp);

	// If there's only one blocker, return
	if (std::popcount(mask.value()) == 1)
	{
		return GetRay(dir, first_blocker_index) & ~blockers;
	}
	
	const auto mask_copy = core::utils::bits::ClearBit(mask.value(), first_blocker_index);

	// Get the second blocker	
	const auto second_blocker_index = GetFirstBlockerIndex(mask_copy, vertical_dir == VerticalDirections::kUp);

	// Add the rays from the first blocker
	// and remove the rays past the second blocker
	auto captures = GetRay(dir, first_blocker_index) & ~GetRay(dir, second_blocker_index);
	// Remove the blockers
	return captures & ~blockers;
}

checkers_types::bitboard MoveGenerator::MovePawnForward(Sides side, checkers_types::bitboard pawn, size_t shift) noexcept
{
	return side == Sides::kBlack ? pawn >> shift : pawn << shift;
}

size_t MoveGenerator::GetFirstBlockerIndex(checkers_types::bitboard board, bool is_above) noexcept
{
	return is_above
		? static_cast<size_t>(checkers_constants::total_squares_ - 1) - std::countl_zero(board)
		: std::countr_zero(board);
}

std::optional<checkers_types::bitboard> MoveGenerator::CastRay(DiagonalDirections dir, size_t i, checkers_types::bitboard blockers) const noexcept
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

checkers_types::bitboard MoveGenerator::GetRay(DiagonalDirections dir, size_t i) const noexcept
{
	return diagonal_rays_[static_cast<int>(dir)][i];
}

void MoveGenerator::CacheDiagonalRays() noexcept
{
	for (size_t i = 0; i < checkers_constants::total_squares_; ++i)
	{
		for (const auto dir : utils::directions::kDiagonalDirections)
		{
			diagonal_rays_[static_cast<int>(dir)][i] |= GenerateDiagonalRays(dir, i);
		}
	}
}

checkers_types::bitboard MoveGenerator::GenerateDiagonalRays(DiagonalDirections dir, size_t index) noexcept
{
	checkers_types::bitboard result = 0;

	auto file = index % checkers_constants::col_count_;
	auto rank = index / checkers_constants::col_count_;

	while (true)
	{
		if (   (dir == DiagonalDirections::kNorthWest && (file <= 0 || rank >= checkers_constants::row_count_-1))
			|| (dir == DiagonalDirections::kNorthEast && (rank >= checkers_constants::row_count_ -1 || file >= checkers_constants::col_count_-1))
			|| (dir == DiagonalDirections::kSouthEast && (rank <= 0 || file >= checkers_constants::col_count_-1))
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

		const auto square = file + rank * checkers_constants::col_count_;
		result |= 0x1ull << square;		
	}
	return result;
}