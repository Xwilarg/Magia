#include "ShortcutManager.hpp"

namespace Magia
{
	ShortcutManager::ShortcutManager() noexcept
		: _keyPressed()
	{ }


	void ShortcutManager::HandleEvent(SDL_Keycode key, bool isDown) noexcept
	{
		if (isDown)
		{
			if (std::find(_keyPressed.begin(), _keyPressed.end(), key) == _keyPressed.end())
			{
				_keyPressed.push_back(key);
			}
		}
		else
		{
			if (std::find(_keyPressed.begin(), _keyPressed.end(), key) != _keyPressed.end())
			{
				_keyPressed.erase(std::remove(_keyPressed.begin(), _keyPressed.end(), key), _keyPressed.end());
			}
		}
	}

	void ShortcutManager::ActivateShortcut(DrawingEngine& engine) noexcept
	{
		for (auto& key : _keyPressed)
		{
			if (key == SDLK_A)
			{
				const auto& brush = engine.GetCurrentBrush();
				auto force = brush->GetPenSize();
				if (force > 1) brush->SetPenSize(force - 1);
			}
			else if (key == SDLK_Q)
			{
				const auto& brush = engine.GetCurrentBrush();
				auto force = brush->GetPenSize();
				if (force < 50) brush->SetPenSize(force + 1);
			}
		}
	}
}