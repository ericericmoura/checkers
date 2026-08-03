#include "CheckersEngine.h"

#include <optional>
#include <string>
#include <utility>

#include <SFML/System/Vector2.hpp>

#include "Core/Utils/BitUtils.h"
#include "Core/Debugging/Logging.h"
#include "Constants/ChessConstants.h"
#include "Enums/Sides.h"
#include "Enums/Pieces.h"

void CheckersEngine::InitBoards() noexcept
{
	SetBoard(Sides::White, Pieces::Pawn, 0xAA55);
	SetBoard(Sides::Black, Pieces::Pawn, 0x55AA000000000000);
}

std::string CheckersEngine::ToString() const noexcept
{
	std::string output = "\n";
	for (int rank = chess_constants::row_count_-1; rank >= 0; --rank)
	{
		output += (rank + 1) + '0';
		output += "  ";
		for (int file = 0; file < chess_constants::col_count_; ++file)
		{
			const auto index = file + rank * chess_constants::col_count_;
			const auto piece_side = GetSideByIndex(index);
			auto symbol = '0';
			if (piece_side.has_value())
			{
				symbol = piece_side.value() == Sides::White ? 'W' : 'B';
			}
			output += symbol;
			output += " ";
		}
		output += "\n";
	}
	output += "\n   a b c d e f g h";
	return output;
}

void CheckersEngine::ExecuteCommand(std::string cmd) noexcept
{
	if (cmd.length() != 4)
	{
		core::debugging::LogError("Invalid checkers command.");
		return;
	}
	const auto commands = SplitCommand(cmd);
	
	const auto first_i  = GetIndexFromNotation(commands.first );
	const auto second_i = GetIndexFromNotation(commands.second);	

	MovePiece(first_i, second_i);
}

void CheckersEngine::MovePiece(size_t from, size_t to) noexcept
{	
	if (IsIndexOccupied(to))
	{
		core::debugging::LogError("Can't move piece into an occupied square.");
		return;
	}
	const auto side = GetSideByIndex(from);
	const auto type = GetPieceTypeByIndex(from);
	if (!side.has_value() || !type.has_value())
	{
		core::debugging::LogError("Can't move non-existent piece.");
		return;
	}
	auto board = GetBoard(side.value(), type.value());
	board = core::utils::ClearBit(board, from);
	board = core::utils::SetBit  (board, to  );
	SetBoard(side.value(), type.value(), board);
}

std::optional<Sides> CheckersEngine::GetSideByIndex(size_t i) const noexcept
{
	if (core::utils::IsBitSet(white_bb, i))
	{
		return Sides::White;
	}
	if (core::utils::IsBitSet(black_bb, i))
	{
		return Sides::Black;
	}
	return {};
}

std::optional<Pieces> CheckersEngine::GetPieceTypeByIndex(size_t i) const noexcept
{
	const auto side = GetSideByIndex(i);
	if (!side.has_value())
	{
		return {};
	}
	const auto pawn_board  = bitboards_.at(static_cast<size_t>(side.value())).at(static_cast<size_t>(Pieces::Pawn ));
	if (core::utils::IsBitSet(pawn_board, i))
	{
		return Pieces::Pawn;
	}
	const auto queen_board = bitboards_.at(static_cast<size_t>(side.value())).at(static_cast<size_t>(Pieces::Queen));
	if (core::utils::IsBitSet(queen_board, i))
	{
		return Pieces::Pawn;
	}
	return {};
}

bool CheckersEngine::IsIndexOccupied(size_t i) const noexcept
{
	return core::utils::IsBitSet(white_bb, i) || core::utils::IsBitSet(black_bb, i);
}

void CheckersEngine::SetBoard(Sides side, Pieces piece, bitboard board) noexcept
{
	const auto old_bb = GetBoard(side, piece);
	auto& side_board = side == Sides::White ? white_bb : black_bb;

	side_board ^= old_bb;
	bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece)) = board;
	side_board |= board;
}

std::pair<std::string, std::string> CheckersEngine::SplitCommand(std::string cmd) const noexcept
{
	std::pair<std::string, std::string> result{};
	result.first  = cmd.substr(0, 2);
	result.second = cmd.substr(2, 2);
	return result;
}

size_t CheckersEngine::GetIndexFromNotation(std::string notation) const noexcept
{
	size_t result = 0;
	result  = static_cast<std::size_t>(notation.at(0) - 'a');
	result += static_cast<std::size_t>(notation.at(1) - '0' - 1) * chess_constants::col_count_;
	return result;
}

bitboard CheckersEngine::GetBoard(Sides side, Pieces piece) const noexcept
{
	return bitboards_.at(static_cast<size_t>(side)).at(static_cast<size_t>(piece));
}
