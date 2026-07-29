#pragma once

#include <unordered_map>
#include <string>

#include <SFML/Graphics/Texture.hpp>

namespace core
{

class BitmapStore
{
public:
	sf::Texture* GetTexture(std::string key) noexcept;
	sf::Texture* GetTextureOrPlaceholder(std::string key, std::string placeholder_key) noexcept;

	static BitmapStore& Get()
	{
		static BitmapStore BitmapStore;
		return BitmapStore;
	}

private:
	std::unordered_map<std::string, sf::Texture> bitmaps_{};
};

} // namespace core