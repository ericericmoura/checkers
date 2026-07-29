#include "GameLayer.h"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Core/Debugging/Logging.h"
#include "Core/Layer.h"
#include "GameHudLayer.h"
#include "MainMenuLayer.h"

GameLayer::GameLayer(unsigned int id) noexcept
	: Layer(id)
{}

void GameLayer::HandleEvent(sf::Event event)
{
	if (auto key_event = event.getIf<sf::Event::KeyPressed>())
	{
		if (key_event->scancode == sf::Keyboard::Scancode::F1)
		{
			core::Debugging::LogInfo("Transitioning to main menu...");
			TransitionTo<MainMenuLayer>();
		}
		else if (key_event->scancode == sf::Keyboard::Scancode::Escape && core::Engine::Get().GetLayer<GameHudLayer>(pause_menu_screen_id_) == nullptr)
		{
			core::Debugging::LogInfo("Creating pause screen...");
			pause_menu_screen_id_ = core::Engine::Get().PushLayer<GameHudLayer>();
		}
	}
}

void GameLayer::Update(float delta)
{}

void GameLayer::draw(sf::RenderTarget & target, sf::RenderStates states) const
{}
