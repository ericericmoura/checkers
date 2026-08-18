#pragma once

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

class BoardRenderer : public sf::Transformable, public sf::Drawable
{
public:
	BoardRenderer();

	static sf::Vector2u GetBoardSize() noexcept;

	// Inherited via sf::Drawable
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Sprite board_sprite_;
};