#pragma once

#include <cstdint>
#include <string>
#include <expected>
#include <bit>

#include "Constants/CheckersConstants.h"

namespace checkers
{

using bitboard = uint64_t;

struct Bitboard
{
	bitboard bits_{};

	Bitboard() = default;
	explicit Bitboard(bitboard bits) noexcept : bits_{ bits }
	{}

	Bitboard operator& (Bitboard o) { return Bitboard{ bits_ & o.bits_ }; }
	Bitboard operator| (Bitboard o) { return Bitboard{ bits_ | o.bits_ }; }
	Bitboard operator~ ()           { return Bitboard{ ~bits_ }; }
	bool     operator==(Bitboard o) { return bits_ == o.bits_; }
	void     operator= (Bitboard o) { bits_ = o.bits_; }

	constexpr size_t Count() const
	{
		return std::popcount(bits_);
	}

	size_t PopLowerBit() 
	{
		size_t index = std::countr_zero(bits_);
		ClearBit(index);
	}

	void SetBit(size_t index) noexcept
	{
		bits_ = bits_ | (0x1ull << index);
	}

	void ClearBit(size_t index) noexcept
	{
		bits_ = bits_ & ~(0x1ull << index);
	}

	constexpr bitboard GetBit(size_t index) const noexcept
	{
		return bits_ & (0x1ull << index);
	}

	constexpr bool IsBitSet(size_t index) const noexcept
	{
		return GetBit(index) != 0;
	}	

	std::string ToString(char symbol) const
	{
		std::string output = "\n";
		for (int rank = checkers_constants::row_count_ - 1; rank >= 0; --rank)
		{
			output += static_cast<char>('0' + rank + 1);
			output += "  ";
			for (int file = 0; file < checkers_constants::col_count_; ++file)
			{
				const auto index = file + rank * checkers_constants::col_count_;
				if (!IsBitSet(index))
				{
					output += '0';
					output += " ";
					continue;
				}
				output += symbol;
				output += " ";
			}
			output += "\n";
		}
		output += "\n   a b c d e f g h";
		return output;
	}

	static constexpr bool IsIndexOutOfBounds(size_t index) noexcept
	{
		return index >= checkers_constants::total_squares_;
	}
};

} // namespace checkers