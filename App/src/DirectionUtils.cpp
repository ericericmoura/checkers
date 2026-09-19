#include "Enums/Directions.h"

#include <cassert>

checkers::DiagonalDirections checkers::GetOppositeDirection(DiagonalDirections dir) noexcept
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

checkers::VerticalDirections checkers::GetVerticalDirection(DiagonalDirections dir) noexcept
{
    if (dir == DiagonalDirections::kNorthEast || dir == DiagonalDirections::kNorthWest)
    {
        return VerticalDirections::kUp;
    }
    return VerticalDirections::kDown;
}

checkers::DiagonalDirections checkers::GetDiagonalDirection(bool is_east, bool is_up) noexcept
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