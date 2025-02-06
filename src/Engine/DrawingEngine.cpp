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
		_currPixels = new uint32_t[CANVAS_WIDTH * WINDOW_HEIGHT];
		memset(_currPixels, 255, CANVAS_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));

		SetColor(0, 0, 0, 255);
	}

	DrawingEngine::~DrawingEngine()
	{
		delete[] _pixels;
		delete[] _currPixels;
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

		auto finalPxls = new uint32_t[CANVAS_WIDTH * WINDOW_HEIGHT];
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			finalPxls[i] = MixColor(_pixels[i], _currPixels[i]);
		}
		SDL_UpdateTexture(_framebuffer, &canvas, finalPxls, CANVAS_WIDTH * sizeof(uint32_t)); // TODO: optimization
		delete[] finalPxls;

		SDL_RenderTexture(_renderer, _framebuffer, &fCanvas, &fCanvas);
	}

	bool DrawingEngine::IsPointInsideDrawingCanvas(int x, int y) noexcept
	{
		return y >= 0 && x >= 0 && y < WINDOW_HEIGHT && x < CANVAS_WIDTH && _dist(_rng) < _penForce;
	}

	void DrawingEngine::DrawPixel(int x, int y) noexcept
	{
		auto prev = _pixels[y * CANVAS_WIDTH + x];
		_currPixels[y * CANVAS_WIDTH + x] = (_color[0] << 24) + (_color[1] << 16) + (_color[2] << 8) + _color[3];
	}

	uint32_t DrawingEngine::MixColor(uint32_t c1, uint32_t c2) const noexcept
	{
		int r = (((c1 >> 24) & 0xFF) * ((c2 >> 24) & 0xFF)) / 255.0f;
		int g = (((c1 >> 16) & 0xFF) * ((c2 >> 16) & 0xFF)) / 255.0f;
		int b = (((c1 >> 8) & 0xFF) * ((c2 >> 8) & 0xFF)) / 255.0f;
		int a = ((c1 & 0xFF) * (c2 & 0xFF)) / 255.0f;

		return (r << 24) + (g << 16) + (b << 8) + a;
	}

	void DrawingEngine::ApplyPixels() noexcept
	{
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			_pixels[i] = MixColor(_pixels[i], _currPixels[i]);
		}
		memset(_currPixels, 255, CANVAS_WIDTH* WINDOW_HEIGHT * sizeof(uint32_t));
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