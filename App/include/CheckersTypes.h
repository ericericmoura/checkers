#pragma once

#include <cstdint>
#include <array>

#include "Constants/CheckersConstants.h"
#include "Enums/Directions.h"
#include "Enums/Pieces.h"
#include "Enums/Sides.h"

namespace checkers_types
{

using bitboard = uint64_t;
using ray_list = std::array<std::array<bitboard, checkers_constants::total_squares_>, static_cast<int>(DiagonalDirections::kCount)>;
using bitboard_list = std::array<std::array<bitboard, static_cast<size_t>(Pieces::kCount)>, static_cast<size_t>(Sides::kCount)>;

} // namespace checkers_types