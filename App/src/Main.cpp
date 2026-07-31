
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "Core/Engine.h"
#include "GameLayer.h"	
#include "Constants/WindowConstants.h"
#include "Constants/ResourcesConstants.h"

int main()
{
	core::EngineSpecification engine_specs{};
	engine_specs.vsync_enabled_ = false;
	engine_specs.window_specification_.icon_texture_key_ = resources_constants::window_icon_texture_key_;
	engine_specs.window_specification_.title_ = window_constants::title_;
	engine_specs.window_specification_.background_color_ = window_constants::background_color_;
	engine_specs.window_specification_.video_mode_ = sf::VideoMode::getDesktopMode();
	
	core::Engine engine{engine_specs};
	engine.PushLayer<GameLayer>();
	engine.Run();
}