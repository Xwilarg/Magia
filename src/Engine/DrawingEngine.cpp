#include "DrawingEngine.hpp"

#include "config.hpp"

namespace Magia
{
	DrawingEngine::DrawingEngine(SDL_Renderer* renderer)
		: _renderer(renderer), _color()
	{
		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
		_pixels = new uint32_t[WINDOW_WIDTH * WINDOW_HEIGHT];
		memset(_pixels, 255, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));

		SetColor(0, 0, 0, 255);
	}

	DrawingEngine::~DrawingEngine()
	{
		delete _pixels;
	}

	void DrawingEngine::UpdateScreen() noexcept
	{
		SDL_UpdateTexture(_framebuffer, NULL, _pixels, WINDOW_WIDTH * sizeof(uint32_t)); // TODO: optimization

		SDL_RenderClear(_renderer);
		SDL_RenderCopy(_renderer, _framebuffer, NULL, NULL);
		SDL_RenderPresent(_renderer);
	}

	void DrawingEngine::Paint(int x, int y) noexcept
	{
		_pixels[y * WINDOW_WIDTH + x] = (_color[0] << 24) + (_color[1] << 16) + (_color[2] << 8) + _color[3];
	}

	const std::array<int, 4>& DrawingEngine::GetColor() const noexcept
	{
		return _color;
	}

	void DrawingEngine::SetColor(int r, int g, int b, int a) noexcept
	{
		_color[0] = r;
		_color[1] = g;
		_color[2] = b;
		_color[3] = a;
	}
}