#pragma once

#include "SDL2/SDL.h"

namespace Magia
{
	class DrawingEngine
	{
	public:
		DrawingEngine(SDL_Renderer* renderer);
		~DrawingEngine() noexcept;
		void UpdateScreen() noexcept;
		void Paint(int x, int y) noexcept;
	private:
		SDL_Renderer* _renderer;
		SDL_Texture* _framebuffer;
		uint32_t* _pixels;
	};
}