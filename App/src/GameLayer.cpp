#include "GameLayer.h"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include "Core/Debugging/Logging.h"
#include <SFML/Window/Keyboard.hpp>

void GameLayer::HandleEvent(sf::Event event)
{
	if (auto key_event = event.getIf<sf::Event::KeyPressed>())
	{
		if (key_event->scancode == sf::Keyboard::Scancode::S)
		{
			core::Debugging::LogInfo("Pressed S. Saving the game!");
		}
	}
}

void GameLayer::Update(float delta)
{}

void GameLayer::draw(sf::RenderTarget & target, sf::RenderStates states) const
{}
