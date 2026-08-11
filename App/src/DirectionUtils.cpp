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
    return DiagonalDirections::kNorthEast;
}

VerticalDirections utils::directions::GetVerticalDirection(DiagonalDirections dir) noexcept
{
    if (dir == DiagonalDirections::kNorthEast || dir == DiagonalDirections::kNorthWest)
    {
        return VerticalDirections::kUp;
    }
    return VerticalDirections::kDown;
}

DiagonalDirections utils::directions::GetDiagonalDirection(bool is_east, bool is_up) noexcept
{
    if (is_east && is_up)
    {
        return DiagonalDirections::kNorthEast;
    }
    if (!is_east && is_up)
    {
        return DiagonalDirections::kNorthWest;
    }
    if (is_east && !is_up)
    {
        return DiagonalDirections::kSouthEast;
    }
    return DiagonalDirections::kSouthWest;
}