#pragma once

#include <array>

#include "Enums/Directions.h"

namespace utils
{
namespace directions
{

DiagonalDirections GetOpositeDirection (DiagonalDirections dir  ) noexcept;
VerticalDirections GetVerticalDirection(DiagonalDirections dir  ) noexcept;
DiagonalDirections GetDiagonalDirection(bool is_east, bool is_up) noexcept;

constexpr std::array<DiagonalDirections, 4> kDiagonalDirections = 
{
    DiagonalDirections::kNorthWest, DiagonalDirections::kNorthEast,
    DiagonalDirections::kSouthWest, DiagonalDirections::kSouthEast
};


} // namespace directions
} // namespace utils