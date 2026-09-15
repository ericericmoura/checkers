#include "Rendering/BoardIndicatorsRenderer.h"

#include <bit>

#include "Utils/CheckersUtils.h"

void BoardIndicatorsRenderer::UpdateMoves(sf::Vector2f board_position, checkers_types::bitboard movements)
{
	movement_indicator_graphics_ = UpdateArray(board_position, movements, sf::Color::Green);
}

void BoardIndicatorsRenderer::UpdateCaptures(sf::Vector2f board_position, checkers_types::bitboard captures)
{
	capture_indicator_graphics_ = UpdateArray(board_position, captures, sf::Color::Red);
}

void BoardIndicatorsRenderer::ClearMoves() noexcept
{
	movement_indicator_graphics_.clear();
}

void BoardIndicatorsRenderer::ClearCaptures() noexcept
{
	capture_indicator_graphics_.clear();
}

void BoardIndicatorsRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(movement_indicator_graphics_, states);
	target.draw(capture_indicator_graphics_, states);
}

sf::VertexArray BoardIndicatorsRenderer::UpdateArray(sf::Vector2f board_position, checkers_types::bitboard bb, sf::Color color)
{
	sf::VertexArray result;
	result.setPrimitiveType(sf::PrimitiveType::Triangles);
	while (bb != 0)
	{
		const auto index = std::countr_zero(bb);
		bb &= ~(0x1ull << index);

		const auto position = utils::checkers::BitboardIndexToPosition(index);

		const auto offset = checkers_constants::cell_size_ / 4;
		const auto size = checkers_constants::cell_size_ / 2;

		const auto top = position.y + offset + board_position.y;
		const auto left = position.x + offset + board_position.x;
		const auto bottom = position.y + size + offset;
		const auto right = position.x + size + offset;

		result.append(sf::Vertex(sf::Vector2f(left, top), color));
		result.append(sf::Vertex(sf::Vector2f(right, top), color));
		result.append(sf::Vertex(sf::Vector2f(left, bottom), color));

		result.append(sf::Vertex(sf::Vector2f(right, top), color));
		result.append(sf::Vertex(sf::Vector2f(right, bottom), color));
		result.append(sf::Vertex(sf::Vector2f(left, bottom), color));
	}
	return result;
}
