#include "MainMenuLayer.h"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Core/Layer.h"
#include "COre/FontStore.h"
#include "Core/Debugging/Logging.h"
#include "GameLayer.h"
#include "Constants/ResourcesConstants.h"

MainMenuLayer::MainMenuLayer(unsigned int id) noexcept
	: Layer(id)
	, title_text_(*core::FontStore::Get().GetFont(resources_constants::pixel_font_key_.data()))
{
	background_.setSize({ 1920, 1080 });
	background_.setPosition({ 0, 0 });
	background_.setFillColor({0, 0, 0, 255});	

	title_text_.setString("CHECKERS");
	title_text_.setPosition({ 1920 / 2.f, 200 });
	title_text_.setCharacterSize(120);
}

void MainMenuLayer::HandleEvent(sf::Event event)
{
	if (auto key_event = event.getIf<sf::Event::KeyPressed>())
	{
		if (key_event->scancode == sf::Keyboard::Scancode::Enter)
		{
			core::debugging::LogInfo("Transitioning to the game...");
			TransitionTo<GameLayer>();
		}
	}
}

void MainMenuLayer::Update(float delta)
{}

void MainMenuLayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(background_ , states);
	target.draw(title_text_, states);
}
