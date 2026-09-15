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
#include "Utils/CheckersUtils.h"

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

	pieces_renderer_.Update(
		BoardRenderer::GetBoardSize(),
		board_renderer_.getPosition(),
		checkers_engine_.GetBoard(Sides::kWhite, Pieces::kPawn),
		checkers_engine_.GetBoard(Sides::kWhite, Pieces::kQueen),
		checkers_engine_.GetBoard(Sides::kBlack, Pieces::kPawn),
		checkers_engine_.GetBoard(Sides::kBlack, Pieces::kQueen)
	);
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
	if (auto mouse_event = event.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouse_event->button == sf::Mouse::Button::Left)
		{
			const auto world_pos = core::Engine::Get().GetWindow().mapPixelToCoords(mouse_event->position, camera_);
			const auto board_index = utils::checkers::PositionToBitboardIndex(world_pos);
			if (!checkers_input_manager_.IsIndexSelected())
			{
				checkers_input_manager_.SelectIndex(board_index);

				const auto moves = checkers_input_manager_.GetMovementsForSelectedIndex(checkers_engine_);
				if (!moves)
				{
					core::debugging::LogError("Failed to get moves for selected index: {}", moves.error());
					return;
				}
				board_indicators_renderer_.UpdateMoves(
					board_renderer_.getPosition(),
					moves.value()
				);				
				return;
			}

			const auto movement_result = checkers_input_manager_.MoveSelectedIndexTo(checkers_engine_, board_index);
			checkers_input_manager_.DeselectIndex();
			board_indicators_renderer_.ClearMoves();

			// UPDATE CAPTURES
			board_indicators_renderer_.ClearCaptures();
			const auto captures = checkers_input_manager_.GetCaptures(checkers_engine_);
			if (captures > 0)
			{
				board_indicators_renderer_.UpdateCaptures(
					board_renderer_.getPosition(),
					captures
				);
			}
			// END UPDATE CAPTURES

			if (!movement_result)
			{
				core::debugging::LogError("Failed to move piece: {}", movement_result.error());
				return;
			}
			pieces_renderer_.Update(
				BoardRenderer::GetBoardSize(),
				board_renderer_.getPosition(),
				checkers_engine_.GetBoard(Sides::kWhite, Pieces::kPawn),
				checkers_engine_.GetBoard(Sides::kWhite, Pieces::kQueen),
				checkers_engine_.GetBoard(Sides::kBlack, Pieces::kPawn),
				checkers_engine_.GetBoard(Sides::kBlack, Pieces::kQueen)
			);
		}
		else if (mouse_event->button == sf::Mouse::Button::Right)
		{
			checkers_input_manager_.DeselectIndex();
			board_indicators_renderer_.ClearMoves();
		}
	}
}

void GameLayer::draw(sf::RenderTarget & target, sf::RenderStates states) const
{	
	target.setView(camera_);
	target.draw(board_renderer_);
	target.draw(pieces_renderer_);
	target.draw(board_indicators_renderer_);
}
