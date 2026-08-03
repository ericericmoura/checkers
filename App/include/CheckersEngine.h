#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <optional>
#include <utility>		

#include "Enums/Sides.h"
#include "Enums/Pieces.h"
#include <SFML/System/Vector2.hpp>

using bitboard          = uint64_t;
using bitboardSidesList = std::array<bitboard, static_cast<size_t>(Pieces::Count)>;
using bitboardList      = std::array<bitboardSidesList, static_cast<size_t>(Sides::Count)>;

class CheckersEngine
{
public:
	void InitBoards() noexcept;
	
	std::string ToString() const noexcept;
	
	// This is the API for communicating with the engine
	// Input example: cmd = "e1f2" // move a pawn from e1 to f2
	// Input example: cmd = "e1xg3" // move a pawn from e1 to g3 and capture a piece at f2
	void ExecuteCommand(std::string cmd) noexcept;

private:
	bitboardList bitboards_{};
	bitboard white_bb{};
	bitboard black_bb{};

	Sides current_team_{};

	void MovePiece(size_t from, size_t to) noexcept;

	std::optional<Sides>  GetSideByIndex (size_t i) const noexcept;
	std::optional<Pieces> GetPieceTypeByIndex(size_t i) const noexcept;

	bool IsIndexOccupied(size_t i) const noexcept;
	
	bitboard GetBoard(Sides side, Pieces piece) const noexcept;

	void SetBoard(Sides side, Pieces piece, bitboard board) noexcept;
	
	std::pair<std::string, std::string> SplitCommand(std::string cmd) const noexcept;
	size_t GetIndexFromNotation(std::string notation) const noexcept;
};