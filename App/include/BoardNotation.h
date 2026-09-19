
#include <expected>

#include "Constants/CheckersConstants.h"

namespace checkers
{

static std::expected<std::string, std::string> BitboardIndexToNotation(size_t index)
{
	if (IsIndexOutOfBounds(index))
	{
		return std::unexpected("Invalid bitboard index: out of bounds.");
	}

	const auto row = index / checkers_constants::col_count_ + 1;
	const auto col = checkers_constants::files[index % checkers_constants::col_count_];

	return col + std::to_string(row);
}

} // namespace checkers
