#include "DrawingEngine.hpp"

#include "config.hpp"

namespace Magia
{
	DrawingEngine::DrawingEngine(SDL_Renderer* renderer)
		: _renderer(renderer)
	{
		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
		_pixels = new uint32_t[WINDOW_WIDTH * WINDOW_HEIGHT];
		memset(_pixels, 255, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
	}

	DrawingEngine::~DrawingEngine()
	{
		delete _pixels;
	}

	void DrawingEngine::UpdateScreen() noexcept
	{
		SDL_UpdateTexture(_framebuffer, NULL, _pixels, WINDOW_WIDTH * sizeof(uint32_t));

		SDL_RenderClear(_renderer);
		SDL_RenderCopy(_renderer, _framebuffer, NULL, NULL);
		SDL_RenderPresent(_renderer);
	}

	void DrawingEngine::Paint(int x, int y) noexcept
	{
		_pixels[y * WINDOW_WIDTH + x] = 0;
		_pixels[y * WINDOW_WIDTH + x + 1] = 0;
		_pixels[y * WINDOW_WIDTH + x + 2] = 0;
	}
}