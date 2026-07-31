#pragma once

#include <fmt/base.h>
#include <fmt/color.h>

namespace core
{
namespace debugging
{

template <typename... Args>
void LogInfo(fmt::format_string<Args...> format, Args&&... args)
{
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
        "\nINFO: {}", fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
void LogWarn(fmt::format_string<Args...> format, Args&&... args)
{
    fmt::print(fg(fmt::color::orange) | fmt::emphasis::italic,
        "\nWARN: {}", fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
void LogError(fmt::format_string<Args...> format, Args&&... args)
{
    fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
        "\nERROR: {}", fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
void LogDebugImpl(fmt::format_string<Args...> format, Args&&... args)
{
    fmt::print(fg(fmt::color::ghost_white) | fmt::emphasis::italic,
        "\nDEBUG: {}", fmt::format(format, std::forward<Args>(args)...));
}

} // namespace debugging
} // namespace core

#if PRODUCTION_BUILD
#define LOG_DEBUG(...) ((void)0)
#else
#define LOG_DEBUG(...) ::core::debugging::LogDebugImpl(__VA_ARGS__)
#endif