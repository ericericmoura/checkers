#include "Rendering/BoardRenderer.h"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

#include "Core/BitmapStore.h"
#include "Constants/ResourcesConstants.h"

BoardRenderer::BoardRenderer()
	: board_sprite_(*core::BitmapStore::Get().GetTexture(resources_constants::checkers_board_texture_key_.data()))
{}

sf::Vector2u BoardRenderer::GetBoardSize() noexcept
{		
	return core::BitmapStore::Get().GetTexture(resources_constants::checkers_board_texture_key_.data())->getSize();
}

void BoardRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(board_sprite_);
}
