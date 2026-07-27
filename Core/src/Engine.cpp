#include "Core/Engine.h"

#include <cassert>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

#include "Core/Debugging/Logger.h"

Core::Engine::Engine(const EngineSpecification& engine_specification)
	: specification_(engine_specification)
{	
	assert(instance_ == nullptr);
	instance_ = this;
	
	Debugging::LogInfo("Initializing the engine...");
	Debugging::LogInfo("Creating window...");
	CreateWindow();
	Debugging::LogInfo("Finish creating the window.");
	Debugging::LogInfo("Finish initializing the engine.");
}

void Core::Engine::Run()
{
	Debugging::LogInfo("Starting main loop...");
	sf::Clock time_{};
	while (window_.isOpen())
	{
		while (auto event = window_.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				Debugging::LogInfo("Exiting main loop...");
				window_.close();
			}
		}
		window_.clear(specification_.window_specification_.background_color_);
		window_.display();
	}
}

void Core::Engine::Stop() noexcept
{
	window_.close();
}

void Core::Engine::CreateWindow()
{
	if (!specification_.window_specification_.mode_.isValid())
	{
		Debugging::LogWarn("currently used video mode is invalid.");
	}

	window_.create(
		specification_.window_specification_.mode_,
		specification_.window_specification_.title_,
		specification_.window_specification_.window_state_
	);
	if (specification_.framerate_limit.has_value())
	{
		window_.setFramerateLimit(specification_.framerate_limit.value());
	}
	window_.setVerticalSyncEnabled(specification_.vsync_enabled_);

	sf::Image image_{ specification_.window_specification_.icon_texture_key_ };
	window_.setIcon(image_);
}
