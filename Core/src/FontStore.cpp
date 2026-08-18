#include "Core/FontStore.h"

#include "Core/Debugging/Logging.h"

sf::Font* core::FontStore::GetFont(std::string key) noexcept
{
	const auto it = fonts_map_.find(key);
	if (it == fonts_map_.end())
	{
		const auto success = fonts_map_[key].openFromFile(key);
		if (!success)
		{
			debugging::LogError("font not found for key `{}`", key);
			return nullptr;
		}
		return &fonts_map_.at(key);
	}
	return &it->second;
}