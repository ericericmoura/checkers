#include "Utils/DirectionUtils.h"

#include <cassert>

#include "Enums/Directions.h"

DiagonalDirections utils::directions::GetOpositeDirection(DiagonalDirections dir) noexcept
{
    assert(dir != DiagonalDirections::kCount);
    if (dir == DiagonalDirections::kNorthEast)
    {
        return DiagonalDirections::kSouthWest;
    }
    if (dir == DiagonalDirections::kNorthWest)
    {
        return DiagonalDirections::kSouthEast;
    }
    if (dir == DiagonalDirections::kSouthEast)
    {
        return DiagonalDirections::kNorthWest;
    }
    if (dir == DiagonalDirections::kSouthWest)
    {
        return DiagonalDirections::kNorthEast;
    }
    return DiagonalDirections::kNorthWest;
}
