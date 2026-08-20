#pragma once

#include <cctype>      
#include <algorithm>   
#include <string_view> 
#include <ranges>
#include <expected>

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

inline std::expected<int, std::string> stoi(std::string_view str) noexcept
{
	int result{};

	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
	if (ec == std::errc::invalid_argument)
	{
		return std::unexpected("Error parsing value: not a number.");
	}
	else if (ec == std::errc::result_out_of_range)
	{
		return std::unexpected("Error parsing value: larger than an int.\n");
	}
	return result;
}

} // namespace string
} // namespace utils
