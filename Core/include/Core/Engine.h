#pragma once

#include <string>
#include <optional>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>

#include "Debugging/Logging.h"
#include "Layer.h"

namespace core
{

/* Window specifications
* this controls the window's settings, such as title, icon, states (fullscreen or windowed), etc.
*/
struct WindowSpecification
{
	std::string   title_ = "";
	std::string   icon_texture_key_ = "";
	sf::Color     background_color_ = sf::Color::Black;

	sf::State	  window_state_ = sf::State::Windowed;
	sf::VideoMode mode_  = {};
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
		Debugging::LogInfo("Creating new layer at index {}...", current_layer_index_);
		layer_stack_.push_back(std::make_unique<TLayer>(std::forward<Args>(construct_args)...));
		return current_layer_index_++;
	}

	template <class TLayer>
	requires(std::is_base_of_v<Layer, TLayer>)
	TLayer* GetLayer(int index)
	{
		auto pointer = layer_stack_.at(index).get();
		if (!pointer)
		{
			Debugging::LogError("No layer found on index ´{}´.", current_layer_index_);
			return pointer;
		}
		auto type_pointer = dynamic_cast<TLayer*>(pointer);
		if (!type_pointer)
		{
			Debugging::LogError("Failed casting layer to the requested type.");
		}
		return type_pointer;
	}	

	void QueueLayerForDeletion(unsigned int index);

private:
	inline static Engine* instance_;

	EngineSpecification specification_;

	sf::RenderWindow window_;

	std::vector<std::unique_ptr<Layer>> layer_stack_{};
	int current_layer_index_ = 0;

	std::vector<unsigned int> queued_layers_for_deletion_{};

	void CreateWindow();

	void RemoveLayer(unsigned int index);
};

} // namespace core	