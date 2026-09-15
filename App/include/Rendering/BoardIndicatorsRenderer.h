#pragma once

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "CheckersTypes.h"

class BoardIndicatorsRenderer : public sf::Transformable, public sf::Drawable
{
public:
	void UpdateMoves(
		sf::Vector2f board_position,
		checkers_types::bitboard movements
	);

	void UpdateCaptures(
		sf::Vector2f board_position,
		checkers_types::bitboard captures
	);

	void ClearMoves() noexcept;
	void ClearCaptures() noexcept;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::VertexArray movement_indicator_graphics_;
	sf::VertexArray capture_indicator_graphics_ ;

	static sf::VertexArray UpdateArray(
		sf::Vector2f board_position,
		checkers_types::bitboard bb,
		sf::Color color
	);
};

