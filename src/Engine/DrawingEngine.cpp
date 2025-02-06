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
		SDL_FRect fCanvas;
		canvas.x = 0;
		canvas.y = 0;
		canvas.h = WINDOW_HEIGHT;
		canvas.w = CANVAS_WIDTH;
		SDL_RectToFRect(&canvas, &fCanvas);
		SDL_UpdateTexture(_framebuffer, &canvas, _pixels, CANVAS_WIDTH * sizeof(uint32_t)); // TODO: optimization

		SDL_RenderClear(_renderer);
		SDL_RenderTexture(_renderer, _framebuffer, &fCanvas, &fCanvas);
		SDL_RenderPresent(_renderer);
	}

	bool DrawingEngine::IsPointInsideDrawingCanvas(int x, int y) noexcept
	{
		return y >= 0 && x >= 0 && y < WINDOW_HEIGHT && x < CANVAS_WIDTH && _dist(_rng) < _penForce;
	}

	void DrawingEngine::DrawPixel(int x, int y) noexcept
	{
		_pixels[y * CANVAS_WIDTH + x] = (_color[0] << 24) + (_color[1] << 16) + (_color[2] << 8) + _color[3];
	}

	/// <summary>
	/// Draw group of pixel at mouse pos
	/// </summary>
	void DrawingEngine::Paint(int x, int y) noexcept
	{
		for (int yPos = y; yPos <= y + static_cast<int>(_penSize / 2.0f); yPos++)
		{
			for (int xPos = x; xPos <= x + static_cast<int>(_penSize / 2.0f); xPos++)
			{
				if ((xPos - x) * (xPos - x) + (yPos - y) * (yPos - y) < _penSize) // We are within the 'circle' shape of the brush
				{
					// We only take a quadrant and use symmetry to get the rest
					// This avoid the "is inside circle" check that is quite expensive
					int xSym = x - (xPos - x);
					int ySym = y - (yPos - y);

					if (IsPointInsideDrawingCanvas(xPos, yPos)) DrawPixel(xPos, yPos);
					if (IsPointInsideDrawingCanvas(xSym, yPos)) DrawPixel(xSym, yPos);
					if (IsPointInsideDrawingCanvas(xPos, ySym)) DrawPixel(xPos, ySym);
					if (IsPointInsideDrawingCanvas(xSym, ySym)) DrawPixel(xSym, ySym);
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