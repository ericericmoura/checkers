#pragma once

#include <string_view>

#define FONTS_PATH    RESOURCES_PATH "fonts/"
#define TEXTURES_PATH RESOURCES_PATH "textures/"

namespace resources_constants
{

// FONTS
constexpr std::string_view pixel_font_key_ = FONTS_PATH "pixel_game_by_suhadidesign.otf";

// TEXTURES
constexpr std::string_view checkers_board_texture_key_ = TEXTURES_PATH "board_alt.png";
constexpr std::string_view window_icon_texture_key_    = TEXTURES_PATH "icon.png";

} // namespace resources_constants