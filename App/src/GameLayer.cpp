#include "GameLayer.h"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>

#include "Core/Debugging/Logging.h"
#include "Core/Layer.h"
#include "Core/Engine.h"
#include "GameHudLayer.h"
#include "MainMenuLayer.h"
#include "Rendering/BoardRenderer.h"

GameLayer::GameLayer(unsigned int id) noexcept
	: Layer(id)
{
	auto camera_size = BoardRenderer::GetBoardSize();

	auto display_size = core::Engine::Get().GetCurrentDisplaySize();

	auto camera_ratio = static_cast<float>(camera_size.x) / static_cast<float>(camera_size.y);
	auto factor = camera_ratio * (static_cast<float>(display_size.y) / display_size.x);

	camera_.setSize(sf::Vector2f(camera_size));
	camera_.setCenter(sf::Vector2f(camera_size / 2u));
	camera_.setViewport({ {(1.f - factor) / 2.f, 0.f}, {factor, 1.f} });
	camera_.zoom(1.1f);
}

void GameLayer::HandleEvent(sf::Event event)
{
	if (auto key_event = event.getIf<sf::Event::KeyPressed>())
	{
		if (key_event->scancode == sf::Keyboard::Scancode::F1)
		{
			core::debugging::LogInfo("Transitioning to main menu...");
			TransitionTo<MainMenuLayer>();
		}
		else if (key_event->scancode == sf::Keyboard::Scancode::Escape && core::Engine::Get().GetLayer<GameHudLayer>(pause_menu_screen_id_) == nullptr)
		{
			core::debugging::LogInfo("Creating pause screen...");
			pause_menu_screen_id_ = core::Engine::Get().PushLayer<GameHudLayer>();
		}
	}
}

void GameLayer::Update(float delta)
{
	// TODO: Update the pieces graphics only when the board state changes, not every frame
	pieces_renderer_.Update(
		BoardRenderer::GetBoardSize(),
		board_renderer_.getPosition(),
		checkers_engine_.GetBoard(Sides::kWhite, Pieces::kPawn),
		checkers_engine_.GetBoard(Sides::kWhite, Pieces::kQueen),
		checkers_engine_.GetBoard(Sides::kBlack, Pieces::kPawn),
		checkers_engine_.GetBoard(Sides::kBlack, Pieces::kQueen)
	);
}

void GameLayer::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.setView(camera_);
	target.draw(board_renderer_);
	target.draw(pieces_renderer_);
}
