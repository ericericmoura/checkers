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
#include "BitboardPositionMapping.h"

namespace checkers
{

GameLayer::GameLayer(unsigned int id) noexcept
	: Layer(id)
{
	auto camera_size = rendering::BoardRenderer::GetBoardSize();

	auto display_size = core::Engine::Get().GetCurrentDisplaySize();

	auto camera_ratio = static_cast<float>(camera_size.x) / static_cast<float>(camera_size.y);
	auto factor = camera_ratio * (static_cast<float>(display_size.y) / display_size.x);

	camera_.setSize(sf::Vector2f(camera_size));
	camera_.setCenter(sf::Vector2f(camera_size / 2u));
	camera_.setViewport({ {(1.f - factor), 0.f}, {factor, 1.f} });
	camera_.zoom(1.1f);

	pieces_renderer_.Update(
		rendering::BoardRenderer::GetBoardSize(),
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
			hud_.HideErrorText();

			const auto world_pos   = core::Engine::Get().GetWindow().mapPixelToCoords(mouse_event->position, camera_);
			const auto board_index = position_mapping::WorldPositionToBitboardIndex(world_pos);

			if (!checkers_input_manager_.IsIndexSelected())
			{
				SelectBoardIndex(board_index);
				return;
			}

			MoveSelectedIndexTo(board_index);
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
	target.draw(board_renderer_, states);
	target.draw(pieces_renderer_, states);
	target.draw(board_indicators_renderer_, states);
	target.draw(hud_, states);
}

void GameLayer::DeselectBoardIndex()
{
	checkers_input_manager_.DeselectIndex();
	board_indicators_renderer_.ClearMoves();
}

void GameLayer::SelectBoardIndex(size_t index)
{	
	const auto result = checkers_input_manager_.SelectIndex(index, checkers_engine_.IsIndexOccupied(index));
	if (!result)
	{
		hud_.UpdateCurrentErrorText(result.error());
		return;
	}
	const auto moves = checkers_engine_.GetMoves(index);
	if (moves == 0)
	{
		hud_.UpdateCurrentErrorText("Invalid selection: no available moves for that piece.");
		checkers_input_manager_.DeselectIndex();
		return;
	}
	board_indicators_renderer_.UpdateMoves(
		board_renderer_.getPosition(),
		moves.value()
	);
}

void GameLayer::MoveSelectedIndexTo(size_t index)
{
	const auto selected_index = checkers_input_manager_.GetSelectedIndex();
	const auto selected_side  = checkers_engine_.GetSideByIndex(selected_index.value());

	bool is_selecting_same_piece_twice = selected_index.has_value() && selected_index.value() == index;
	if (is_selecting_same_piece_twice)
	{
		return;
	}

	bool is_selecting_other_piece = checkers_engine_.IsIndexOccupied(index) &&
		checkers_engine_.GetSideByIndex(index) == selected_side;
	if (is_selecting_other_piece)
	{
		DeselectBoardIndex();
		SelectBoardIndex(index);
		return;
	}

	const auto movement_result = checkers_input_manager_.MoveSelectedIndexTo(checkers_engine_, index);
	DeselectBoardIndex();

	if (!movement_result)
	{
		hud_.UpdateCurrentErrorText(movement_result.error());
		core::debugging::LogError("Failed to move piece: {}", movement_result.error());
		return;
	}

	// UPDATE CAPTURES
	board_indicators_renderer_.ClearCaptures();
	const auto captures = checkers_engine_.GetCaptures();
	if (captures.bits_ > 0)
	{
		board_indicators_renderer_.UpdateCaptures(
			board_renderer_.getPosition(),
			captures
		);
	}

	UpdateGraphics();
}

void GameLayer::UpdateGraphics()
{
	pieces_renderer_.Update(
		rendering::BoardRenderer::GetBoardSize(),
		board_renderer_.getPosition(),
		checkers_engine_.GetBoard(Sides::kWhite, Pieces::kPawn),
		checkers_engine_.GetBoard(Sides::kWhite, Pieces::kQueen),
		checkers_engine_.GetBoard(Sides::kBlack, Pieces::kPawn),
		checkers_engine_.GetBoard(Sides::kBlack, Pieces::kQueen)
	);

	hud_.UpdateCurrentTeamText(checkers_engine_.GetCurrentTeam());
	hud_.HideErrorText();
}

} // namespace checkers