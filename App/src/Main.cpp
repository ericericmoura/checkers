
#include <SFML/Graphics/Color.hpp>

#include "Core/Engine.h"

int main()
{
	Core::EngineSpecification engine_specs{};
	engine_specs.vsync_enabled_ = false;
	engine_specs.window_specification_.icon_texture_key_ = RESOURCES_PATH "icon.png";
	engine_specs.window_specification_.title_ = "C++ Checkers";
	engine_specs.window_specification_.background_color_ = sf::Color::Red;
	Core::Engine engine_{engine_specs};

	engine_.Run();
}