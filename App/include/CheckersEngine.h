#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <optional>

#include "Enums/Sides.h"
#include "Enums/Pieces.h"

using bitboard          = uint64_t;
using bitboardSidesList = std::array<bitboard, static_cast<size_t>(Pieces::Count)>;
using bitboardList      = std::array<bitboardSidesList, static_cast<size_t>(Sides::Count)>;

struct PieceInfo
{
	Sides team = Sides::White;
};

class CheckersEngine
{
public:
	void InitBoards() noexcept;
	
	std::string ToString() const noexcept;
	
	// This is the API for communicating with the engine
	// Input example: cmd = "e1f2" // move a pawn from e1 to f2
	// Input example: cmd = "e1xg3" // move a pawn from e1 to g3 and capture a piece at f2
	void ReceiveCommand(std::string cmd) noexcept;

private:
	bitboardList bitboards_{};
	bitboard white_bb{};
	bitboard black_bb{};

	void RemovePieceAt(Sides side, size_t i);
	void AddPieceAt(Sides side, Pieces piece, size_t i);

	std::optional<Sides> GetSideByIndex(size_t i) const noexcept;
	
	bitboard GetBoard(Sides side, Pieces piece) const noexcept;

	void SetBoard(Sides side, Pieces piece, bitboard board) noexcept;
};