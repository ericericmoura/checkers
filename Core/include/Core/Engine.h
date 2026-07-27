#pragma once

#include <string>
#include <optional>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>

namespace Core
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

private:
	inline static Engine* instance_;

	EngineSpecification specification_;

	sf::RenderWindow window_;

	void CreateWindow();
};

} // namespace Core	