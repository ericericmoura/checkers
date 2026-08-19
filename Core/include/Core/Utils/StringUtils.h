#pragma once

#include <cctype>      
#include <algorithm>   
#include <string_view> 
#include <ranges>

namespace utils
{
namespace string
{

inline bool ichar_equals(char a, char b) noexcept
{
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}

inline bool iequals(std::string_view lhs, std::string_view rhs) noexcept
{
    return std::ranges::equal(lhs, rhs, ichar_equals);
}

} // namespace string
} // namespace utils
