#include "Core/Engine.h"

#include <cassert>
#include <memory>
#include <utility>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include "Core/Debugging/Logging.h"
#include "Core/Layer.h"

core::Engine::Engine(const EngineSpecification& engine_specification)
	: specification_(engine_specification)
{	
	assert(instance_ == nullptr);
	instance_ = this;
	
	debugging::LogInfo("Initializing the engine...");
	debugging::LogInfo("Creating the window...");
	CreateWindow();
	debugging::LogInfo("Finish creating the window.");
	debugging::LogInfo("Finish initializing the engine.");
}

void core::Engine::Run()
{
	debugging::LogInfo("Starting the main loop...");
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
				debugging::LogInfo("Exiting the main loop...");
				window_.close();
			}
			for (auto& [id, layer] : layer_stack_)
			{
				layer->HandleEvent(event.value());
			}
		}
		auto delta_time = time_.restart();
		elapsed_time_ += delta_time;
		auto delta = delta_time.asSeconds();

		for (auto& [id, layer] : layer_stack_)
		{
			layer->Update(delta);
		}

		window_.clear(specification_.window_specification_.background_color_);
		for (auto& [id, layer] : layer_stack_)
		{
			window_.draw(*layer);
		}
		window_.display();

		DeleteQueuedLayers();
		TransitionQueuedLayers();
	}
}

void core::Engine::Stop() noexcept
{
	window_.close();
}

void core::Engine::QueueLayerForDeletion(unsigned int id)
{
	queued_layers_for_deletion_.push_back(id);
}

void core::Engine::QueueLayerTransition(unsigned int from_id, std::unique_ptr<Layer> to_layer)
{
	if (!layer_stack_.contains(from_id))
	{
		debugging::LogError("invalid id `{}` for layer transition.", from_id);
	}
	from_id_queued_layer_transition_ = from_id;
	to_queued_layer_transition_      = std::move(to_layer);
}

void core::Engine::DeleteQueuedLayers() noexcept
{
	if (queued_layers_for_deletion_.size() == 0)
	{
		return;
	}
	for (auto id : queued_layers_for_deletion_)
	{
		RemoveLayer(id);		
	}
	queued_layers_for_deletion_.clear();
}

void core::Engine::TransitionQueuedLayers()
{
	if (to_queued_layer_transition_ == nullptr)
	{
		return;
	}
	if (!layer_stack_.contains(from_id_queued_layer_transition_))
	{
		debugging::LogError("invalid id `{}` for layer transition.", from_id_queued_layer_transition_);
		return;
	}
	layer_stack_[from_id_queued_layer_transition_] = std::move(to_queued_layer_transition_);

	to_queued_layer_transition_      = nullptr;
	from_id_queued_layer_transition_ = 0;
}

sf::Vector2u core::Engine::GetCurrentDisplaySize() const noexcept
{
	return specification_.window_specification_.video_mode_.size;
}

sf::Time core::Engine::GetElapsedTime() noexcept
{
	return Get().elapsed_time_;
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
		debugging::LogWarn("currently used video mode is invalid.");
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

void core::Engine::RemoveLayer(unsigned int id) noexcept
{
	debugging::LogInfo("Removing layer by id ´{}´...", id);		
	if (!layer_stack_.contains(id))
	{
		debugging::LogError("No layer found for id `{}`.", id);
		return;
	}
	layer_stack_.erase(id);	
}