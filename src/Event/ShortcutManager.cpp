#include "ShortcutManager.hpp"

namespace Magia
{
	ShortcutManager::ShortcutManager() noexcept
		: _keyPressed()
	{ }


	void ShortcutManager::HandleEvent(SDL_Keycode key, bool isDown) noexcept
	{
		if (key == SDLK_LCTRL)
		{
			_isCtrlPressed = isDown;
		}
		else if (isDown)
		{
			if (std::find(_keyPressed.begin(), _keyPressed.end(), key) == _keyPressed.end())
			{
				_keyPressed.push_back(key);
			}
		}
		/*else
		{
			if (std::find(_keyPressed.begin(), _keyPressed.end(), key) != _keyPressed.end())
			{
				_keyPressed.erase(std::remove(_keyPressed.begin(), _keyPressed.end(), key), _keyPressed.end());
			}
		}*/
	}

	void ShortcutManager::ActivateShortcut(DrawingEngine& engine) noexcept
	{
		for (auto& key : _keyPressed)
		{
			if (key == SDLK_A)
			{
				const auto& brush = engine.GetCurrentBrush();
				auto force = brush->GetPenSize();
				if (force > 2) brush->SetPenSize(force - 1);
			}
			else if (key == SDLK_S)
			{
				const auto& brush = engine.GetCurrentBrush();
				auto force = brush->GetPenSize();
				if (force < 1000) brush->SetPenSize(force + 1);
			}
			else if (_isCtrlPressed && key == SDLK_Z)
			{
				if (engine.GetCurrentHistoryIndex() > 0) engine.Undo();
			}
			else if (_isCtrlPressed && key == SDLK_Y)
			{
				if (engine.GetCurrentHistoryIndex() < engine.GetHistoryCount()) engine.Redo();
			}
		}
		_keyPressed.clear();
	}
}