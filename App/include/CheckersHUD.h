#pragma once

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Enums/Sides.h"

class CheckersHUD : public sf::Transformable, public sf::Drawable
{
public:
	CheckersHUD();

	void UpdateCurrentTeamText(Sides current_team) noexcept;

	void HideErrorText() noexcept;
	void UpdateCurrentErrorText(const std::string& error_message) noexcept;

	// Inherited via sf::Drawable
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Text error_text_;
	sf::Text current_team_text_;

	sf::View hud_camera_;
};