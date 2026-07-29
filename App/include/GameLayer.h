#pragma once

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include "Core/Layer.h"

class GameLayer : public core::Layer
{
public:
	GameLayer(unsigned int id) noexcept;

	// Inherited via core::Layer
	void HandleEvent(sf::Event event) override;
	void Update(float delta) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	unsigned int pause_menu_screen_id_ = 0;
};