#pragma once

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "CheckersTypes.h"

class PiecesRenderer : public sf::Transformable, public sf::Drawable
{
public:
	PiecesRenderer();

	void Update(
		sf::Vector2u board_size, 
		sf::Vector2f board_position,
		checkers_types::bitboard white_pawns,
		checkers_types::bitboard white_queens,
		checkers_types::bitboard black_pawns,
		checkers_types::bitboard black_queens
	);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::VertexArray graphics_;

	void AppendPieces(
		sf::Vector2f board_position,
		checkers_types::bitboard white_bb,
		checkers_types::bitboard black_bb,
		size_t white_texture_index,
		size_t black_texture_index
	);
};
