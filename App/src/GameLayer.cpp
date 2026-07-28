#include "GameLayer.h"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Core/Debugging/Logging.h"
#include "Core/Layer.h"
#include "MainMenuLayer.h"

GameLayer::GameLayer(unsigned int id) noexcept
	: Layer(id)
{}

void GameLayer::HandleEvent(sf::Event event)
{
	if (auto key_event = event.getIf<sf::Event::KeyPressed>())
	{
		if (key_event->scancode == sf::Keyboard::Scancode::S)
		{
			core::Debugging::LogInfo("Transitioning to main menu!");
			TransitionTo<MainMenuLayer>();
		}
	}
}

void GameLayer::Update(float delta)
{}

void GameLayer::draw(sf::RenderTarget & target, sf::RenderStates states) const
{}
