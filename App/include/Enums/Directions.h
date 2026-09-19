#pragma once

#include <array>

namespace checkers
{

enum class DiagonalDirections : int
{
	kNorthWest,
	kNorthEast,
	kSouthWest,
	kSouthEast,
	kCount
};

enum class Directions : int
{
	kNorth,
	kEast,
	kSouth,
	kWest,
	kCount
};

enum class VerticalDirections
{
	kUp,
	kDown
};

DiagonalDirections GetOppositeDirection (DiagonalDirections dir  ) noexcept;
VerticalDirections GetVerticalDirection(DiagonalDirections dir   ) noexcept;
DiagonalDirections GetDiagonalDirection(bool is_east, bool is_up ) noexcept;

constexpr std::array<DiagonalDirections, 4> kDiagonalDirections = 
{
    DiagonalDirections::kNorthWest, DiagonalDirections::kNorthEast,
    DiagonalDirections::kSouthWest, DiagonalDirections::kSouthEast
};


} // namespace checkers
