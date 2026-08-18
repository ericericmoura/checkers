#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include "Engine.h"

namespace core
{

class Layer : public sf::Drawable
{
public:
	Layer(unsigned int id) noexcept;

	template <typename TLayer, typename ... Args>
	void TransitionTo(Args&&... args)
	{		
		Engine::Get().QueueLayerTransition(id_, std::make_unique<TLayer>(id_, std::forward<Args>(args)...));
	};

	void DestroySelf() const
	{
		Engine::Get().QueueLayerForDeletion(id_);
	};

	virtual void HandleEvent(sf::Event event) {};
	virtual void Update(float delta) {};

	// Inherited via sf::Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;

private:
	unsigned int id_ = 0;
};

} // namespace core
