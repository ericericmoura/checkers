#pragma once

#include <unordered_map>
#include <string>

#include <SFML/Graphics/Font.hpp>

namespace core
{

class FontStore
{
public:
	sf::Font* GetFont(std::string key) noexcept;

	static FontStore& Get()
	{
		static FontStore font_store;
		return font_store;
	}

private:
	std::unordered_map<std::string, sf::Font> fonts_map_{};
};

} // namespace core