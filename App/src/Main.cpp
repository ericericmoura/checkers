
#include <SFML/Graphics/Color.hpp>

#include "Core/Engine.h"
#include "GameLayer.h"

int main()
{
	core::EngineSpecification engine_specs{};
	engine_specs.vsync_enabled_ = false;
	engine_specs.window_specification_.icon_texture_key_ = RESOURCES_PATH "icon.png";
	engine_specs.window_specification_.title_ = "C++ Checkers";
	engine_specs.window_specification_.background_color_ = sf::Color::Red;
	
	core::Engine engine{engine_specs};
	engine.PushLayer<GameLayer>();
	engine.Run();
}