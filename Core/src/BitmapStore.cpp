#include "Core/BitmapStore.h"

#include <string>
#include <utility>

#include <SFML/Graphics/Texture.hpp>

#include "Core/Debugging/Logging.h"

sf::Texture* core::BitmapStore::GetTexture(std::string key) noexcept
{
	const auto it = bitmaps_.find(key);
	if (it == bitmaps_.end())
	{
		sf::Texture texture{};
		const auto success = texture.loadFromFile(key);
		if (!success)
		{
			debugging::LogError("texture not found for key `{}`", key);
			return nullptr;
		}
		bitmaps_[key] = std::move(texture);
		return &bitmaps_.at(key);
	}
	return &it->second;
}

sf::Texture* core::BitmapStore::GetTextureOrPlaceholder(std::string key, std::string placeholder_key) noexcept
{
	auto texture = GetTexture(key);
	if (texture == nullptr)
	{
		return GetTexture(placeholder_key);
	}
	return texture;
}