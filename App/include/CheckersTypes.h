#pragma once

#include <cstdint>
#include <array>

#include "Constants/CheckersConstants.h"
#include "Enums/Directions.h"

namespace checkers_types
{

using bitboard = uint64_t;
using ray_list = std::array<std::array<bitboard, checkers_constants::total_squares_>, static_cast<int>(DiagonalDirections::kCount)>;

} // namespace types