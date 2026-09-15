#pragma once

#include <expected>
#include <string>
#include <optional>

class CheckersInputManager
{
public:
	void SelectIndex(size_t index) noexcept;
	void TargetIndex(size_t index) noexcept;

	bool IsIndexSelected() const noexcept;

	std::expected<std::string, std::string> GetCommand() noexcept;

private:
	std::optional<size_t> selected_index_ = {};
	std::optional<size_t> target_index_   = {};
};