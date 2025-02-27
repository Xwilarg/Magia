#pragma once

#include <vector>
#include "SDL3/SDL.h"
#include "DrawingEngine.hpp"

namespace Magia
{
	class ShortcutManager
	{
	public:
		ShortcutManager() noexcept;

		void HandleEvent(SDL_Keycode key, bool isDown) noexcept;
		void ActivateShortcut(DrawingEngine& engine) noexcept;

	private:
		std::vector<SDL_Keycode> _keyPressed;
		bool _isCtrlPressed;
	};
}