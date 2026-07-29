#pragma once

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "Core/Layer.h"

class MainMenuLayer : public core::Layer
{
public:
	MainMenuLayer(unsigned int id) noexcept;

	// Inherited via core::Layer
	void HandleEvent(sf::Event event) override;
	void Update(float delta) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::RectangleShape background_{};

	sf::Text title_text_;
};