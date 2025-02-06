#pragma once

#include <array>
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

		const std::array<int, 4>& GetColor() const noexcept;
		void SetColor(int r, int g, int b, int a) noexcept;
	private:
		SDL_Renderer* _renderer;
		SDL_Texture* _framebuffer;
		uint32_t* _pixels;

		std::array<int, 4> _color;
	};
}