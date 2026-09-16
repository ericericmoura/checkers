#pragma once

#include <type_traits>		

namespace core
{
namespace utils
{
namespace screen
{

constexpr sf::Vector2u ResizeToMatchRatio(sf::Vector2u size, sf::Vector2u target_size_for_ratio)
{	
	float height_ratio = static_cast<float>(target_size_for_ratio.y) / target_size_for_ratio.x;
	float width_ratio  = static_cast<float>(target_size_for_ratio.x) / target_size_for_ratio.y;
	if (size.y * width_ratio <= size.x)
	{
		size.x = size.y * width_ratio;
	}
	else if (size.x * height_ratio <= size.y)
	{
		size.y = size.x * height_ratio;
	}
	return size;
}

} // namespace bits
} // namespace utils
} // namespace core
