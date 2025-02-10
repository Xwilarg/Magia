#include <cmath>
#include <ranges>
#include <iterator>

#include "DrawingEngine.hpp"
#include "config.hpp"

namespace Magia
{
	DrawingEngine::DrawingEngine(SDL_Renderer* renderer)
		: _renderer(renderer), _color(), _penSize(7), _penForce(30), _drawMode(DrawMode::MULTIPLICATIVE), _drawDistance(3), _interpMode(InterpolationMode::CENTRIPETAL_CATMULL_ROM), _dev(), _rng(_dev()), _dist(1, 100), _brushPixels(), _layers(), _selectedLayer(), _pixelScreen()
	{
		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, WINDOW_HEIGHT);
		_brushPixels.Clear(TRANSPARENT_PIXEL);

		AddNewLayer();

		SetColor(0, 0, 0, 255);
	}

	void DrawingEngine::UpdateScreen() noexcept
	{
		SDL_Rect canvas = {};
		SDL_FRect fCanvas;
		canvas.x = 0;
		canvas.y = 0;
		canvas.h = WINDOW_HEIGHT;
		canvas.w = CANVAS_WIDTH;
		SDL_RectToFRect(&canvas, &fCanvas);

		_pixelScreen.Clear(WHITE_PIXEL);
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			_pixelScreen.Set(i, MixColor(_brushPixels.Get(i), _pixelScreen.Get(i)));
		}
		for (const auto& layer : _layers | std::views::filter([](auto l) { return l->GetActive(); }))
		{
			for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
			{
				_pixelScreen.Set(i, MixColor(layer->Get(i), _pixelScreen.Get(i)));
			}
		}
		SDL_UpdateTexture(_framebuffer, &canvas, _pixelScreen.Get(), CANVAS_WIDTH * sizeof(uint32_t)); // TODO: optimization

		SDL_RenderTexture(_renderer, _framebuffer, &fCanvas, &fCanvas);
	}

	uint32_t* DrawingEngine::GetFinalFramebuffer() noexcept
	{
		return _pixelScreen.Get();
	}

	void DrawingEngine::AddNewLayer() noexcept
	{
		auto newLayer = std::make_shared<DrawLayer>();
		newLayer->Clear(TRANSPARENT_PIXEL);
		_layers.push_back(newLayer);
		_selectedLayer = _layers.size() - 1;
	}

	void DrawingEngine::RemoveLayer(int index) noexcept
	{
		_layers.erase(_layers.begin() + index);
		if (_selectedLayer > _layers.size() - 1)
		{
			_selectedLayer = _layers.size() - 1;
		}
	}

	bool DrawingEngine::IsPointInsideDrawingCanvas(int x, int y) noexcept
	{
		return y >= 0 && x >= 0 && y < WINDOW_HEIGHT && x < CANVAS_WIDTH && _dist(_rng) < _penForce;
	}

	int DrawingEngine::MixSingleValue(int c1V, int c2V, float alpha1, float alpha2, float alpha) const noexcept
	{
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


		float alpha1 = brushA / 255.0f;
		float alpha2 = canvasA / 255.0f;
		auto alpha = alpha1 + alpha2 * (1 - alpha1);

		r = MixSingleValue(brushR, canvasR, alpha1, alpha2, alpha);
		g = MixSingleValue(brushG, canvasG, alpha1, alpha2, alpha);
		b = MixSingleValue(brushB, canvasB, alpha1, alpha2, alpha);

		a = alpha * 255;

		return (r << 24) + (g << 16) + (b << 8) + a;
	}

	void DrawingEngine::ApplyPixels() noexcept
	{
		auto& layer = _layers[_selectedLayer];
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			layer->Set(i, MixColor(_brushPixels.Get(i), layer->Get(i)));
		}
		_brushPixels.Clear(TRANSPARENT_PIXEL);
	}

	/// <summary>
	/// Draw group of pixel at mouse pos
	/// </summary>
	void DrawingEngine::Paint(int x, int y) noexcept
	{
		if (!_layers[_selectedLayer]->GetActive()) return;

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

					if (IsPointInsideDrawingCanvas(xPos, yPos)) _brushPixels.Draw(xPos, yPos, _color[0], _color[1], _color[2], _color[3]);
					if (IsPointInsideDrawingCanvas(xSym, yPos)) _brushPixels.Draw(xSym, yPos, _color[0], _color[1], _color[2], _color[3]);
					if (IsPointInsideDrawingCanvas(xPos, ySym)) _brushPixels.Draw(xPos, ySym, _color[0], _color[1], _color[2], _color[3]);
					if (IsPointInsideDrawingCanvas(xSym, ySym)) _brushPixels.Draw(xSym, ySym, _color[0], _color[1], _color[2], _color[3]);
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

	int DrawingEngine::GetDrawDistance() const noexcept
	{
		return _drawDistance;
	}

	void DrawingEngine::SetDrawDistance(int distance) noexcept
	{
		_drawDistance = distance;
	}

	InterpolationMode DrawingEngine::GetInterpolationMode() const noexcept
	{
		return _interpMode;
	}

	void DrawingEngine::SetInterpolationMode(InterpolationMode mode) noexcept
	{
		_interpMode = mode;
	}

	std::vector<std::shared_ptr<DrawLayer>>& DrawingEngine::GetLayers() noexcept
	{
		return _layers;
	}

	void DrawingEngine::SetSelectedLayerIndex(int target) noexcept
	{
		_selectedLayer = target;
	}

	int DrawingEngine::GetSelectedLayerIndex() const noexcept
	{
		return _selectedLayer;
	}
}