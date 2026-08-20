#include "MoveGenerator.h"

#include <optional>
#include <bit>

#include "Core/Utils/BitUtils.h"
#include "Constants/CheckersConstants.h"
#include "Utils/DirectionUtils.h"
#include "Enums/Directions.h"
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

size_t MoveGenerator::GetFirstBlockerIndex(checkers_types::bitboard board, bool is_above) const noexcept
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