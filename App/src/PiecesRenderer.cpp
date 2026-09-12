#include "Rendering/PiecesRenderer.h"

#include <bit>
#include <cassert>

#include "Core/BitmapStore.h"
#include "Constants/CheckersConstants.h"
#include "Constants/ResourcesConstants.h"
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
		const auto index = std::countr_zero(all_pawns);
		all_pawns &= ~(0x1ull << index);

		const auto row = (index / checkers_constants::col_count_);
		const auto col = (index % checkers_constants::col_count_);

		const auto top_offset  = (checkers_constants::cell_size_ * (checkers_constants::row_count_-1)) - (checkers_constants::cell_size_ * row) + checkers_constants::margin_.y;
		const auto left_offset = (checkers_constants::cell_size_ * col) + checkers_constants::margin_.x;

		const auto top    = top_offset;
		const auto left   = left_offset;
		const auto bottom = top  + checkers_constants::cell_size_;
		const auto right  = left + checkers_constants::cell_size_;

		const auto is_white = (white_pawns & (0x1ull << index)) != 0;

		const auto texture_index = is_white ? 0 : 2;

		const auto tx_top    = 0;
		const auto tx_left   = checkers_constants::cell_size_ * texture_index;
		const auto tx_bottom = tx_top  + checkers_constants::cell_size_-1;
		const auto tx_right  = tx_left + checkers_constants::cell_size_-1;

		graphics_.append(sf::Vertex(sf::Vector2f(left , top   ), sf::Color::White, sf::Vector2f(tx_left   , tx_top   )));
		graphics_.append(sf::Vertex(sf::Vector2f(right, top   ), sf::Color::White, sf::Vector2f(tx_right  , tx_top   )));
		graphics_.append(sf::Vertex(sf::Vector2f(left , bottom), sf::Color::White, sf::Vector2f(tx_left   , tx_bottom)));

		graphics_.append(sf::Vertex(sf::Vector2f(right, top   ), sf::Color::White, sf::Vector2f(tx_right  , tx_top   )));
		graphics_.append(sf::Vertex(sf::Vector2f(right, bottom), sf::Color::White, sf::Vector2f(tx_right  , tx_bottom)));
		graphics_.append(sf::Vertex(sf::Vector2f(left , bottom), sf::Color::White, sf::Vector2f(tx_left   , tx_bottom)));
	}
}

void PiecesRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture    = core::BitmapStore::Get().GetTexture(resources_constants::checkers_pieces_texture_key_.data());
	target.draw(graphics_, states);
}
