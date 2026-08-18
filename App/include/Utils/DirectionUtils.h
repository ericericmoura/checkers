#pragma once

#include "Enums/Directions.h"

namespace utils
{
namespace directions
{

DiagonalDirections GetOpositeDirection (DiagonalDirections dir  ) noexcept;
VerticalDirections GetVerticalDirection(DiagonalDirections dir  ) noexcept;
DiagonalDirections GetDiagonalDirection(bool is_east, bool is_up) noexcept;

} // namespace directions
} // namespace utils