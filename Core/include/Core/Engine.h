#pragma once

#include <string>
#include <optional>
#include <unordered_map>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>

#include "Debugging/Logging.h"

namespace core
{

class Layer;

/* Window specifications
* this controls the window's settings, such as title, icon, states (fullscreen or windowed), etc.
*/
struct WindowSpecification
{
	std::string   title_ = "";
	std::string   icon_texture_key_ = "";
	sf::Color     background_color_ = sf::Color::Black;

	sf::State	  window_state_ = sf::State::Windowed;
	sf::VideoMode video_mode_  = {};
};

/* Engine specifications
* this controls the engine's settings, such as fps limit and vsync
*/
struct EngineSpecification
{
	std::optional<unsigned int> framerate_limit{};

	bool vsync_enabled_   = false;
	bool production_mode_ = false;
	WindowSpecification window_specification_;
};

/**
* Main class from which the game is mainly run.
* Engine is a type of singleton, there can ever be one per application.
*
* It's responsible for loading the window and handling the main Game Loop.
*/
class Engine
{
public:
	Engine(const EngineSpecification& engine_specification);
	void Run();
	void Stop() noexcept;

	template <class TLayer, typename ... Args>
	requires(std::is_base_of_v<Layer, TLayer>)
	int PushLayer(Args&&... construct_args)
	{
		Debugging::LogInfo("Creating new layer with id `{}`...", current_layer_id_);
		layer_stack_.try_emplace(current_layer_id_, std::make_unique<TLayer>(current_layer_id_, std::forward<Args>(construct_args)...));
		return current_layer_id_++;
	}

	template <class TLayer>
	requires(std::is_base_of_v<Layer, TLayer>)
	TLayer* GetLayer(unsigned int id) const
	{
		auto it = layer_stack_.find(id);
		if (it == layer_stack_.end())
		{
			Debugging::LogWarn("No layer found with id ´{}´.", current_layer_id_);
			return nullptr;
		}
		auto type_pointer = dynamic_cast<TLayer*>(it->second.get());
		if (!type_pointer)
		{
			Debugging::LogWarn("Failed casting layer to the requested type.");
		}
		return type_pointer;
	}	

	void QueueLayerForDeletion(unsigned int id);
	void QueueLayerTransition(unsigned int from_id, std::unique_ptr<Layer> to_layer);

	void DeleteQueuedLayers    () noexcept;
	void TransitionQueuedLayers();

	sf::Vector2u GetCurrentDisplaySize() const noexcept;

	static sf::Time GetElapsedTime() noexcept;
	static Engine& Get();

private:
	inline static Engine* instance_;

	EngineSpecification specification_;

	sf::RenderWindow window_;

	std::unordered_map<int, std::unique_ptr<Layer>> layer_stack_{};
	unsigned int current_layer_id_ = 1;

	std::vector<unsigned int> queued_layers_for_deletion_{};

	std::unique_ptr<Layer> to_queued_layer_transition_ = nullptr;
	unsigned int from_id_queued_layer_transition_ = 0;

	sf::Time elapsed_time_{};

	void CreateWindow();

	void RemoveLayer(unsigned int index) noexcept;
};

} // namespace core	