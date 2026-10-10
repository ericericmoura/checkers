#pragma once

#include <string_view>

#include <SFML/Graphics/Color.hpp>

namespace checkers::window_constants
{

constexpr sf::Color background_color_ = {0, 0, 0};
constexpr std::string_view title_ = "C++ Checkers";

} // namespace checkers::window_constants