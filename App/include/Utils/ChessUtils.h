#pragma once

#include <cstdint>
#include <string>

using bitboard = uint64_t;

namespace utils
{

std::string BitboardToString(bitboard board, char symbol) noexcept;

} // namespace utils