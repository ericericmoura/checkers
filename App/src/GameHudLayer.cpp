#include "GameHudLayer.h"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Core/Layer.h"
#include "Core/Debugging/Logging.h"
#include "Core/FontStore.h"
#include "GameLayer.h"
#include "MainMenuLayer.h"
#include "Constants/ResourcesConstants.h"

GameHudLayer::GameHudLayer(unsigned int id) noexcept
	: Layer(id)
	, paused_text_(*core::FontStore::Get().GetFont(resources_constants::pixel_font_key_.data()))
{
	created_at_ = core::Engine::GetElapsedTime();

	background_.setSize({ 1920, 1080 });
	background_.setPosition({ 0, 0 });
	background_.setFillColor({ 0, 0, 0, 155 });

	paused_text_.setString("Paused!");
	paused_text_.setPosition({ 1920/2.f, 1080/2.f });
	paused_text_.setCharacterSize(30);		
}

void GameHudLayer::HandleEvent(sf::Event event)
{
	if (auto key_event = event.getIf<sf::Event::KeyPressed>())
	{
		if (key_event->scancode == sf::Keyboard::Scancode::Escape && (core::Engine::GetElapsedTime() - created_at_).asSeconds() > invincibility_delay_)
		{
			core::debugging::LogInfo("Closing pause screen...");
			DestroySelf();
		}
		else if (key_event->scancode == sf::Keyboard::Scancode::F1)
		{			
			DestroySelf();
		}
	}
}

void GameHudLayer::Update(float delta)
{}

void GameHudLayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(background_ , states);
	target.draw(paused_text_, states);
}
