#include "Rendering/PiecesRenderer.h"

#include <bit>
#include <cassert>

#include "Constants/CheckersConstants.h"
#include "Utils/CheckersUtils.h"

PiecesRenderer::PiecesRenderer()
{
	constexpr auto pieces_per_side = 8;
	constexpr auto vertices_per_piece = 6;

	graphics_.setPrimitiveType(sf::PrimitiveType::Triangles);
	graphics_.resize(vertices_per_piece * pieces_per_side * pieces_per_side);
}

void PiecesRenderer::Update(
	sf::Vector2u board_size,
	sf::Vector2f board_position,
	checkers_types::bitboard white_pawns,
	checkers_types::bitboard white_queens,
	checkers_types::bitboard black_pawns,
	checkers_types::bitboard black_queens
)
{
	graphics_.clear();

	auto all_pawns = white_pawns | black_pawns;

	while (all_pawns != 0)
	{
		utils::checkers::LogBitboardWithContrast(all_pawns, 'A');

		// BIT BOARD INDEX 1 = A1 OR BOTTOM LEFT
		// GRAPHICAL CHESS BOARD INDEX 1 = TOP LEFT
		const auto index = std::countr_zero(all_pawns);
		all_pawns &= ~(0x1ull << index);

		utils::checkers::LogBitboardWithContrast(0x1ull << index, 'R');
		utils::checkers::LogBitboardWithContrast(all_pawns, 'A');

		const auto top    = (index / board_size.x) * checkers_constants::cell_size_ + checkers_constants::margin_.y;
		const auto left   = (index % board_size.x) * checkers_constants::cell_size_ + checkers_constants::margin_.x;
		const auto bottom = top  + checkers_constants::cell_size_;
		const auto right  = left + checkers_constants::cell_size_;

		const auto is_white = (white_pawns & (0x1ull << index)) != 0;

		const auto piece_color = is_white ? sf::Color::Red : sf::Color::Green;

		graphics_.append(sf::Vertex(sf::Vector2f(left , top   ), piece_color));
		graphics_.append(sf::Vertex(sf::Vector2f(right, top   ), piece_color));
		graphics_.append(sf::Vertex(sf::Vector2f(left , bottom), piece_color));

		graphics_.append(sf::Vertex(sf::Vector2f(right, top   ), piece_color));
		graphics_.append(sf::Vertex(sf::Vector2f(right, bottom), piece_color));
		graphics_.append(sf::Vertex(sf::Vector2f(left , bottom), piece_color));
	}
}

void PiecesRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(graphics_, states);
}
