#pragma once

#include <cstdint>
#include <string>
#include <expected>
#include <bit>
#include <concepts>

#include "Constants/CheckersConstants.h"

namespace checkers
{

using bitboard = uint64_t;

struct Bitboard
{
	bitboard bits_{};

	Bitboard() = default;
	Bitboard(bitboard bits) noexcept : bits_{ bits }
	{}

	template <typename Fn>
	requires std::invocable<Fn, size_t>
	void ForEachBitSet(Fn&& lambda) const
	{
		const bits = bits_;
		while (bits != 0)
		{
			size_t index = std::countr_zero(bits);
			ClearBit(index);

			lambda(index);
		}
	}

	template <typename Fn>
	requires std::invocable<Fn, size_t>
	void DrainBits(Fn&& lambda)
	{
		while (bits != 0)
		{
			size_t index = PopLowerBit();
			lambda(index);
		}
	}

	Bitboard operator& (Bitboard o) const { return Bitboard{ bits_ & o.bits_ }; }
	Bitboard operator| (Bitboard o) const { return Bitboard{ bits_ | o.bits_ }; }
	Bitboard operator^ (Bitboard o) const { return Bitboard{ bits_ ^ o.bits_ }; }
	Bitboard operator~ ()           const { return Bitboard{ ~bits_ }; }

	void operator^= (Bitboard o) { bits_ ^= o.bits_; }
	void operator|= (Bitboard o) { bits_ |= o.bits_; }
	void operator&= (Bitboard o) { bits_ &= o.bits_; }

	Bitboard operator>> (size_t i) { return bits_ >> i; }
	Bitboard operator<< (size_t i) { return bits_ << i; }

	bool operator==(Bitboard o) const { return bits_ == o.bits_; }
	void operator= (Bitboard o) { bits_ = o.bits_; }

	constexpr size_t Count() const
	{
		return std::popcount(bits_);
	}

	size_t PopLowerBit() 
	{
		size_t index = std::countr_zero(bits_);
		ClearBit(index);
	}

	Bitboard SetBit(size_t index) noexcept
	{
		bits_ = bits_ | (0x1ull << index);
		return bits_;
	}

	Bitboard ClearBit(size_t index) noexcept
	{
		bits_ = bits_ & ~(0x1ull << index);
		return bits_;
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
		for (int rank = checkers::constants::row_count_ - 1; rank >= 0; --rank)
		{
			output += static_cast<char>('0' + rank + 1);
			output += "  ";
			for (int file = 0; file < checkers::constants::col_count_; ++file)
			{
				const auto index = file + rank * checkers::constants::col_count_;
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
		return index >= checkers::constants::total_squares_;
	}
};

} // namespace checkers