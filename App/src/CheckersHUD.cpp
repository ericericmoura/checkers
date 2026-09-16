#include "CheckersHUD.h"

#include "Core/FontStore.h"
#include "Core/Engine.h"
#include "Constants/ResourcesConstants.h"

CheckersHUD::CheckersHUD() 
	: error_text_       (*core::FontStore::Get().GetFont(resources_constants::pixel_font_key_.data()))
	, current_team_text_(*core::FontStore::Get().GetFont(resources_constants::pixel_font_key_.data()))
{
	auto display_size = core::Engine::Get().GetCurrentDisplaySize();

	hud_camera_.setSize  (sf::Vector2f(display_size));
	hud_camera_.setCenter(sf::Vector2f(display_size / 2u));

	error_text_.setCharacterSize(30);
	error_text_.setPosition({ 5.f, 5.f });
	error_text_.setFillColor(sf::Color::Red);
	HideErrorText();

	current_team_text_.setCharacterSize(40);
	UpdateCurrentTeamText(Sides::kWhite);
}

void CheckersHUD::UpdateCurrentTeamText(Sides current_team) noexcept
{
	current_team_text_.setString(std::format("Current Team: {}", current_team == Sides::kWhite ? "White" : "Black"));
	const auto x_center = core::Engine::Get().GetCurrentDisplaySize().x / 2.f - current_team_text_.getGlobalBounds().size.x / 2.f;
	current_team_text_.setPosition({ x_center, 5.f });
}

void CheckersHUD::HideErrorText() noexcept
{
	UpdateCurrentErrorText("");
}

void CheckersHUD::UpdateCurrentErrorText(const std::string& error_message) noexcept
{	
	error_text_.setString(error_message);
}

void CheckersHUD::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.setView(hud_camera_);
	target.draw(current_team_text_, states);
	target.draw(error_text_, states);
}
