#include <cmath>

#include "DrawingEngine.hpp"
#include "config.hpp"

namespace Magia
{
	DrawingEngine::DrawingEngine(SDL_Renderer* renderer)
		: _renderer(renderer), _color(), _penSize(5), _penForce(30), _drawMode(DrawMode::MULTIPLICATIVE), _dev(), _rng(_dev()), _dist(1, 100)
	{
		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, WINDOW_HEIGHT);
		_pixels = new uint32_t[CANVAS_WIDTH * WINDOW_HEIGHT];
		SetToValue(_pixels, TRANSPARENT_PIXEL, CANVAS_WIDTH * WINDOW_HEIGHT);
		_currPixels = new uint32_t[CANVAS_WIDTH * WINDOW_HEIGHT];
		SetToValue(_currPixels, TRANSPARENT_PIXEL, CANVAS_WIDTH * WINDOW_HEIGHT);
		_pixelsScreen = new uint32_t[CANVAS_WIDTH * WINDOW_HEIGHT];

		SetColor(0, 0, 0, 255);
	}

	DrawingEngine::~DrawingEngine()
	{
		delete[] _pixels;
		delete[] _currPixels;
		delete[] _pixelsScreen;
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

		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			_pixelsScreen[i] = MixColor(_currPixels[i], _pixels[i]);
		}
		SDL_UpdateTexture(_framebuffer, &canvas, _pixelsScreen, CANVAS_WIDTH * sizeof(uint32_t)); // TODO: optimization

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

	int DrawingEngine::MixSingleValue(int c1V, int c1A, int c2V, int c2A) const noexcept
	{
		float alpha1 = c1A / 255.0f;
		float alpha2 = c2A / 255.0f;

		auto alpha = alpha1 + alpha2 * (1 - alpha1);

		return ((c1V * alpha1) + (c2V * alpha2 * (1 - alpha1))) / (alpha1 + alpha2 * (1 - alpha1));
	}

	uint32_t DrawingEngine::MixColor(uint32_t brush, uint32_t canvas) const noexcept
	{
		int brushR = (brush >> 24) & 0xFF;
		int brushG = (brush >> 16) & 0xFF;
		int brushB = (brush >> 8) & 0xFF;
		int brushA = brush & 0xFF;
		int canvasR = (canvas >> 24) & 0xFF;
		int canvasG = (canvas >> 16) & 0xFF;
		int canvasB = (canvas >> 8) & 0xFF;
		int canvasA = canvas & 0xFF;

		uint32_t r, g, b, a;
		/*if (_drawMode == DrawMode::ADDITIVE)
		{
			r = (((c1 >> 24) & 0xFF) + ((c2 >> 24) & 0xFF)) / 2;
			g = (((c1 >> 16) & 0xFF) + ((c2 >> 16) & 0xFF)) / 2;
			b = (((c1 >> 8) & 0xFF) + ((c2 >> 8) & 0xFF)) / 2;
			a = ((c1 & 0xFF) + (c2 & 0xFF)) / 2;
		}
		else*/
		{
			/*r = (((brush >> 24) & 0xFF) * ((canvas >> 24) & 0xFF)) / 255;
			g = (((brush >> 16) & 0xFF) * ((canvas >> 16) & 0xFF)) / 255;
			b = (((brush >> 8) & 0xFF) * ((canvas >> 8) & 0xFF)) / 255;
			a = brush & 0xFF;*/

			r = MixSingleValue(brushR, brushA, canvasR, canvasA);
			g = MixSingleValue(brushG, brushA, canvasG, canvasA);
			b = MixSingleValue(brushB, brushA, canvasB, canvasA);
			//a = MixSingleValue(brushA, brushA, canvasA, canvasA);
			/*r = ((brushR)+(canvasR)) / 2;
			g = ((brushG) + (canvasG)) / 2;
			b = ((brushB) + (canvasB)) / 2;*/
		}
		a = 255;

		return (r << 24) + (g << 16) + (b << 8) + a;
	}

	void DrawingEngine::ApplyPixels() noexcept
	{
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			_pixels[i] = MixColor(_currPixels[i], _pixels[i]);
		}
		SetToValue(_currPixels, TRANSPARENT_PIXEL, CANVAS_WIDTH * WINDOW_HEIGHT);
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

	DrawMode DrawingEngine::GetDrawMode() const noexcept
	{
		return _drawMode;
	}

	void DrawingEngine::SetDrawMode(DrawMode mode) noexcept
	{
		_drawMode = mode;
	}
}