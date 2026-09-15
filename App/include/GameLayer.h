#pragma once

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>

#include "Core/Layer.h"
#include "Rendering/PiecesRenderer.h"
#include "Rendering/BoardRenderer.h"
#include "CheckersEngine.h"
#include "CheckersInputManager.h"

class GameLayer : public core::Layer
{
public:
	GameLayer(unsigned int id) noexcept;

	// Inherited via core::Layer
	void HandleEvent(sf::Event event) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	unsigned int pause_menu_screen_id_ = 0;

	sf::View   camera_;

	CheckersInputManager checkers_input_manager_{};

	BoardRenderer  board_renderer_ {};
	PiecesRenderer pieces_renderer_{};

	CheckersEngine checkers_engine_{};
};