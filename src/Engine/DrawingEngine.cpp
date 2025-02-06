#include <cmath>

#include "DrawingEngine.hpp"
#include "config.hpp"

namespace Magia
{
	DrawingEngine::DrawingEngine(SDL_Renderer* renderer)
		: _renderer(renderer), _color(), _penSize(5), _penForce(30), _dev(), _rng(_dev()), _dist(1, 100)
	{
		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, WINDOW_HEIGHT);
		_pixels = new uint32_t[CANVAS_WIDTH * WINDOW_HEIGHT];
		memset(_pixels, 255, CANVAS_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));

		SetColor(0, 0, 0, 255);
	}

	DrawingEngine::~DrawingEngine()
	{
		delete _pixels;
	}

	void DrawingEngine::UpdateScreen() noexcept
	{
		SDL_Rect canvas;
		canvas.x = 0;
		canvas.y = 0;
		canvas.h = WINDOW_HEIGHT;
		canvas.w = CANVAS_WIDTH;
		SDL_UpdateTexture(_framebuffer, &canvas, _pixels, CANVAS_WIDTH * sizeof(uint32_t)); // TODO: optimization

		SDL_RenderClear(_renderer);
		SDL_RenderCopy(_renderer, _framebuffer, &canvas, &canvas);
		SDL_RenderPresent(_renderer);
	}

	void DrawingEngine::Paint(int x, int y) noexcept
	{
		// Draw in form of a circle using brush size
		for (int yPos = y - static_cast<int>(_penSize / 2.0f); yPos <= y + static_cast<int>(_penSize / 2.0f); yPos++)
		{
			for (int xPos = x - static_cast<int>(_penSize / 2.0f); xPos <= x + static_cast<int>(_penSize / 2.0f); xPos++)
			{
				// TODO: All of that can prob be optimized too
				if (yPos >= 0 && xPos >= 0 && yPos < WINDOW_HEIGHT && xPos < CANVAS_WIDTH && // We are within the canvas...
					std::pow(xPos - x, 2) + std::pow(yPos - y, 2) < _penSize && // ... and we are within the 'circle' shape of the brush...
					_dist(_rng) < _penForce) // ...and RNG tell us we are within brush force bounds
				{
					_pixels[yPos * CANVAS_WIDTH + xPos] = (_color[0] << 24) + (_color[1] << 16) + (_color[2] << 8) + _color[3];
				}
			}
		}
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

	int DrawingEngine::GetPenSize() const noexcept
	{
		return _penSize;
	}

	void DrawingEngine::SetPenSize(int size) noexcept
	{
		_penSize = size;
	}

	int DrawingEngine::GetPenForce() const noexcept
	{
		return _penForce;
	}

	void DrawingEngine::SetPenForce(int force) noexcept
	{
		_penForce = force;
	}
}