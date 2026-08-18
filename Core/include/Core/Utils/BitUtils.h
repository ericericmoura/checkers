#pragma once

#include <type_traits>		

namespace core
{
namespace utils
{

template <typename T>
concept UnsignedIntegral = std::is_unsigned_v<T> && std::is_integral_v<T>;

template<UnsignedIntegral T>
constexpr T SetBit(T integer, size_t index)
{
	return integer | (0x1ull << index);
}

template<UnsignedIntegral T>
constexpr T ClearBit(T integer, size_t index)
{
	return integer & ~(0x1ull << index);
}

template<UnsignedIntegral T>
constexpr T GetBit(T integer, size_t index)
{
	return integer & (0x1ull << index);
}

template<UnsignedIntegral T>
constexpr bool IsBitSet(T integer, size_t index)
{
	return GetBit(integer, index) > 0;
}

} // namespace utils
} // namespace core
