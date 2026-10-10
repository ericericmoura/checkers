#pragma once

#include <string>

namespace checkers
{

enum class Sides
{
	kWhite,
	kBlack,
	kCount
};

constexpr Sides GetOppositeSide(Sides side) noexcept
{
	return side == Sides::kWhite ? Sides::kBlack : Sides::kWhite;
}

constexpr std::string SideToString(Sides side) noexcept
{
	return side == Sides::kWhite ? "white" : "black";
}

} // namespace checkers
