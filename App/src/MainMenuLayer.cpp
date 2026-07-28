#include "MainMenuLayer.h"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Core/Layer.h"
#include "Core/Debugging/Logging.h"
#include "GameLayer.h"

MainMenuLayer::MainMenuLayer(unsigned int id) noexcept
	: Layer(id)
{
	background_.setSize({ 1920, 1080 });
	background_.setPosition({ 0, 0 });
	background_.setFillColor({0, 0, 0, 155});	
}

void MainMenuLayer::HandleEvent(sf::Event event)
{
	if (auto key_event = event.getIf<sf::Event::KeyPressed>())
	{
		if (key_event->scancode == sf::Keyboard::Scancode::S)
		{
			core::Debugging::LogInfo("Transitioning to game layer!");
			TransitionTo<GameLayer>();
		}
	}
}

void MainMenuLayer::Update(float delta)
{}

void MainMenuLayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(background_, states);
}
