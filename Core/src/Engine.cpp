#include "Core/Engine.h"

#include <cassert>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

#include "Core/Debugging/Logging.h"

core::Engine::Engine(const EngineSpecification& engine_specification)
	: specification_(engine_specification)
{	
	assert(instance_ == nullptr);
	instance_ = this;
	
	Debugging::LogInfo("Initializing the engine...");
	Debugging::LogInfo("Creating the window...");
	CreateWindow();
	Debugging::LogInfo("Finish creating the window.");
	Debugging::LogInfo("Finish initializing the engine.");
}

void core::Engine::Run()
{
	Debugging::LogInfo("Starting the main loop...");
	sf::Clock time_{};
	while (window_.isOpen())
	{
		while (auto event = window_.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				Debugging::LogInfo("Exiting the main loop...");
				window_.close();
			}
			for (auto& layer : layer_stack_)
			{
				layer->HandleEvent(event);
			}
		}
		auto delta = time_.restart().asSeconds();
		for (auto& layer : layer_stack_)
		{
			layer->Update(delta);
		}

		window_.clear(specification_.window_specification_.background_color_);
		for (auto& layer : layer_stack_)
		{
			window_.draw(*layer);
		}
		window_.display();
	}

	if (queued_layers_for_deletion_.size() > 0)
	{
		for (auto index : queued_layers_for_deletion_)
		{
			RemoveLayer(index);
		}
	}
}

void core::Engine::Stop() noexcept
{
	window_.close();
}

void core::Engine::QueueLayerForDeletion(unsigned int index)
{
	queued_layers_for_deletion_.push_back(index);
}

void core::Engine::CreateWindow()
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

void core::Engine::RemoveLayer(unsigned int index)
{
	Debugging::LogInfo("Removing layer by id ´{}´...", index);
	if (index >= layer_stack_.size())
	{
		Debugging::LogError("Invalid layer id.");
	}
	layer_stack_.erase(layer_stack_.begin() + index);
}