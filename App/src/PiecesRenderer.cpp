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
}

void PiecesRenderer::Update(
	sf::Vector2u board_size,
	sf::Vector2f board_position,
	Bitboard white_pawns,
	Bitboard white_queens,
	Bitboard black_pawns,
	Bitboard black_queens
)
{
	graphics_.clear();
	AppendPieces(board_position, white_pawns , black_pawns , 0, 2);
	AppendPieces(board_position, white_queens, black_queens, 1, 3);
}

void PiecesRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture    = core::BitmapStore::Get().GetTexture(resources_constants::checkers_pieces_texture_key_.data());
	target.draw(graphics_, states);
}

void PiecesRenderer::AppendPieces(sf::Vector2f board_position, Bitboard white_bb, Bitboard black_bb, size_t white_texture_index, size_t black_texture_index)
{
	auto all_pieces = white_bb | black_bb;

	while (all_pieces != 0)
	{
		const auto index = std::countr_zero(all_pieces);
		all_pieces &= ~(0x1ull << index);

		const auto piece_position = utils::checkers::BitboardIndexToPosition(index);

		const auto top = piece_position.y;
		const auto left = piece_position.x;
		const auto bottom = top + checkers::constants::cell_size_;
		const auto right = left + checkers::constants::cell_size_;

		const auto is_white = (white_bb & (0x1ull << index)) != 0;

		const auto texture_index = is_white ? white_texture_index : black_texture_index;

		const auto tx_top = 0;
		const auto tx_left = checkers::constants::cell_size_ * texture_index;
		const auto tx_bottom = tx_top + checkers::constants::cell_size_ - 1;
		const auto tx_right = tx_left + checkers::constants::cell_size_ - 1;

		graphics_.append(sf::Vertex(sf::Vector2f(left , top   ), sf::Color::White, sf::Vector2f(tx_left , tx_top    )));
		graphics_.append(sf::Vertex(sf::Vector2f(right, top   ), sf::Color::White, sf::Vector2f(tx_right, tx_top    )));
		graphics_.append(sf::Vertex(sf::Vector2f(left , bottom), sf::Color::White, sf::Vector2f(tx_left , tx_bottom )));

		graphics_.append(sf::Vertex(sf::Vector2f(right, top   ), sf::Color::White, sf::Vector2f(tx_right, tx_top    )));
		graphics_.append(sf::Vertex(sf::Vector2f(right, bottom), sf::Color::White, sf::Vector2f(tx_right, tx_bottom )));
		graphics_.append(sf::Vertex(sf::Vector2f(left , bottom), sf::Color::White, sf::Vector2f(tx_left , tx_bottom )));
	}
}
