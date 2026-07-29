#pragma once

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "Core/Layer.h"

class GameHudLayer : public core::Layer
{
public:
	GameHudLayer(unsigned int id) noexcept;

	// Inherited via core::Layer
	void HandleEvent(sf::Event event) override;
	void Update(float delta) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::RectangleShape background_{};
	sf::Text paused_text_;	
	
	float invincibility_delay_ = 0.3f;

	sf::Time created_at_{};
};