#pragma once

#include <string_view>

#include <SFML/Graphics/Color.hpp>

namespace window_constants
{

constexpr sf::Color background_color_ = {0, 0, 0};
constexpr std::string_view title_ = "C++ Checkers";

} // namespace window_constants