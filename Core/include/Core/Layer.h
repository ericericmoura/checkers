#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

namespace core
{

class Layer : public sf::Drawable
{
public:
	virtual void HandleEvent(sf::Event event) {};
	virtual void Update(float delta) {};

	// Inherited via sf::Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;
};

} // namespace core
