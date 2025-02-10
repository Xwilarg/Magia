#include <cmath>
#include <ranges>
#include <iterator>

#include "DrawingEngine.hpp"
#include "PaintBrush.hpp"
#include "config.hpp"

namespace Magia
{
	DrawingEngine::DrawingEngine(SDL_Renderer* renderer)
		: _renderer(renderer), _canUseMouse(true), _dev(), _rng(_dev()), _dist(1, 100), _brushPixels(), _layers(), _selectedLayer(), _pixelScreen(), _currentBrush(0), _brushes()
	{
		_brushes.emplace_back(std::make_shared<PaintBrush>());

		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, WINDOW_HEIGHT);
		_brushPixels.Clear(TRANSPARENT_PIXEL);

		AddNewLayer();
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
		for (const auto& layer : _layers | std::views::filter([](auto l) { return l->GetActive(); }))
		{
			for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
			{
				_pixelScreen.Set(i, MixColor(layer->Get(i), _pixelScreen.Get(i)));
			}
		}
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			_pixelScreen.Set(i, MixColor(_brushPixels.Get(i), _pixelScreen.Get(i)));
		}
		SDL_UpdateTexture(_framebuffer, &canvas, _pixelScreen.Get(), CANVAS_WIDTH * sizeof(uint32_t)); // TODO: optimization

		SDL_RenderTexture(_renderer, _framebuffer, &fCanvas, &fCanvas);
	}

	std::shared_ptr<ABrush> DrawingEngine::GetCurrentBrush() noexcept
	{
		return _brushes[_currentBrush];
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
		return y >= 0 && x >= 0 && y < WINDOW_HEIGHT && x < CANVAS_WIDTH && _dist(_rng) < GetCurrentBrush()->GetPenForce();
	}

	void DrawingEngine::ApplyPixels() noexcept
	{
		auto brush = GetCurrentBrush();
		auto& layer = _layers[_selectedLayer];
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			layer->Set(i, brush->MixColor(_brushPixels.Get(i), layer->Get(i)));
		}
		_brushPixels.Clear(TRANSPARENT_PIXEL);
	}

	/// <summary>
	/// Draw group of pixel at mouse pos
	/// </summary>
	void DrawingEngine::Paint(int x, int y) noexcept
	{
		if (!_layers[_selectedLayer]->GetActive()) return;

		auto brush = GetCurrentBrush();
		auto& color = brush->GetColor();
		for (int yPos = y; yPos <= y + static_cast<int>(brush->GetPenSize() / 2.0f); yPos++)
		{
			for (int xPos = x; xPos <= x + static_cast<int>(brush->GetPenSize() / 2.0f); xPos++)
			{
				if ((xPos - x) * (xPos - x) + (yPos - y) * (yPos - y) < brush->GetPenSize()) // We are within the 'circle' shape of the brush
				{
					// We only take a quadrant and use symmetry to get the rest
					// This avoid the "is inside circle" check that is quite expensive
					int xSym = x - (xPos - x);
					int ySym = y - (yPos - y);

					if (IsPointInsideDrawingCanvas(xPos, yPos)) _brushPixels.Draw(xPos, yPos, color[0], color[1], color[2], color[3]);
					if (IsPointInsideDrawingCanvas(xSym, yPos)) _brushPixels.Draw(xSym, yPos, color[0], color[1], color[2], color[3]);
					if (IsPointInsideDrawingCanvas(xPos, ySym)) _brushPixels.Draw(xPos, ySym, color[0], color[1], color[2], color[3]);
					if (IsPointInsideDrawingCanvas(xSym, ySym)) _brushPixels.Draw(xSym, ySym, color[0], color[1], color[2], color[3]);
				}
			}
		}
	}

	bool DrawingEngine::GetCanUseMouse() const noexcept
	{
		return _canUseMouse;
	}

	void DrawingEngine::SetCanUseMouse(bool value) noexcept
	{
		_canUseMouse = value;
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