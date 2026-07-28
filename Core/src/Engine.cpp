#include "Core/Engine.h"

#include <cassert>
#include <memory>
#include <utility>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

#include "Core/Debugging/Logging.h"
#include "Core/Layer.h"

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
			if (!event.has_value())
			{
				continue;
			}
			if (event->is<sf::Event::Closed>())
			{
				Debugging::LogInfo("Exiting the main loop...");
				window_.close();
			}
			for (auto& layer : layer_stack_)
			{
				layer->HandleEvent(event.value());
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

	if (to_queued_layer_transition_ != nullptr)
	{
		Debugging::LogInfo("Transitioning layers...");		
		if (from_index_queued_layer_transition_ > layer_stack_.size())
		{
			Debugging::LogError("invalid index for layer transition.");
			return;
		}
		layer_stack_.at(from_index_queued_layer_transition_) = std::move(to_queued_layer_transition_);

		to_queued_layer_transition_         = nullptr;
		from_index_queued_layer_transition_ = 0;
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

void core::Engine::QueueLayerTransition(unsigned int from_index, std::unique_ptr<Layer> to_layer)
{
	if (from_index > layer_stack_.size())
	{
		Debugging::LogError("invalid index for layer transition.");
	}
	from_index_queued_layer_transition_ = from_index;
	to_queued_layer_transition_ = std::move(to_layer);
}

core::Engine& core::Engine::Get()
{
	assert(instance_);
	return *instance_;
}

void core::Engine::CreateWindow()
{
	if (!specification_.window_specification_.video_mode_.isValid())
	{
		Debugging::LogWarn("currently used video mode is invalid.");
	}

	window_.create(
		specification_.window_specification_.video_mode_,
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