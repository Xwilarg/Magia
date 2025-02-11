#include <cmath>
#include <ranges>
#include <iterator>

#include "DrawingEngine.hpp"
#include "PaintBrush.hpp"
#include "EraserBrush.hpp"
#include "config.hpp"

namespace Magia
{
	DrawingEngine::DrawingEngine(SDL_Renderer* renderer)
		: _renderer(renderer), _canUseMouse(true), _drawMode(DrawMode::MULTIPLICATIVE), _renderingBrush(), _dev(), _rng(_dev()), _dist(1, 100), _brushPixels(), _layers(), _selectedLayer(), _pixelScreen(), _currentBrush(0), _brushes()
	{
		_brushes.emplace_back(std::make_shared<PaintBrush>());
		_brushes.emplace_back(std::make_shared<EraserBrush>());

		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, WINDOW_HEIGHT);
		_brushPixels.Clear(TRANSPARENT_PIXEL);

		AddNewLayer();
	}

	// https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
	void DrawingEngine::DrawCursor(int xMouse, int yMouse) noexcept
	{
		int radius = GetCurrentBrush()->GetPenSize() / 2.0;
		int x = radius - 1;
		int y = 0;
		int dx = 1;
		int dy = 1;
		int err = dx - (radius << 1);

		while (x >= y)
		{
			_pixelScreen.TryDraw(xMouse + x, yMouse + y, 0, 0, 0, 255);
			_pixelScreen.TryDraw(xMouse + y, yMouse + x, 0, 0, 0, 255);
			_pixelScreen.TryDraw(xMouse - y, yMouse + x, 0, 0, 0, 255);
			_pixelScreen.TryDraw(xMouse - x, yMouse + y, 0, 0, 0, 255);
			_pixelScreen.TryDraw(xMouse - x, yMouse - y, 0, 0, 0, 255);
			_pixelScreen.TryDraw(xMouse - y, yMouse - x, 0, 0, 0, 255);
			_pixelScreen.TryDraw(xMouse + y, yMouse - x, 0, 0, 0, 255);
			_pixelScreen.TryDraw(xMouse + x, yMouse - y, 0, 0, 0, 255);

			if (err <= 0)
			{
				y++;
				err += dy;
				dy += 2;
			}

			if (err > 0)
			{
				x--;
				dx += 2;
				err += dx - (radius << 1);
			}
		}
	}

	void DrawingEngine::UpdateScreen(int mouseX, int mouseY) noexcept
	{
		SDL_Rect canvas = {};
		SDL_FRect fCanvas;
		canvas.x = 0;
		canvas.y = 0;
		canvas.h = WINDOW_HEIGHT;
		canvas.w = CANVAS_WIDTH;
		SDL_RectToFRect(&canvas, &fCanvas);

		auto brush = GetCurrentBrush();

		_pixelScreen.Clear(WHITE_PIXEL);
		_intermPixels.Clear(TRANSPARENT_PIXEL);
		for (const auto& layer : _layers | std::views::take(_selectedLayer) | std::views::filter([](auto l) { return l->GetActive(); }))
		{
			for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
			{
				_pixelScreen.Set(i, _renderingBrush.MixColor(_drawMode, layer->Get(i), _pixelScreen.Get(i)));
			}
		}
		auto& midLayer = _layers[_selectedLayer];
		if (midLayer->GetActive())
		{
			for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
			{
				_intermPixels.Set(i, _renderingBrush.MixColor(_drawMode, midLayer->Get(i), _intermPixels.Get(i)));
				_intermPixels.Set(i, brush->MixColor(_drawMode, _brushPixels.Get(i), _intermPixels.Get(i)));
				_pixelScreen.Set(i, _renderingBrush.MixColor(_drawMode, _intermPixels.Get(i), _pixelScreen.Get(i)));
			}
		}
		for (const auto& layer : _layers | std::views::drop(_selectedLayer + 1) | std::views::filter([](auto l) { return l->GetActive(); }))
		{
			for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
			{
				_pixelScreen.Set(i, _renderingBrush.MixColor(_drawMode, layer->Get(i), _pixelScreen.Get(i)));
			}
		}

		DrawCursor(mouseX, mouseY);

		SDL_UpdateTexture(_framebuffer, &canvas, _pixelScreen.Get(), CANVAS_WIDTH * sizeof(uint32_t)); // TODO: optimization

		SDL_RenderTexture(_renderer, _framebuffer, &fCanvas, &fCanvas);
	}

	std::shared_ptr<ABrush> DrawingEngine::GetCurrentBrush() noexcept
	{
		return _brushes[_currentBrush];
	}

	uint32_t* DrawingEngine::GetFinalFramebuffer() noexcept
	{
		_intermPixels.Clear(TRANSPARENT_PIXEL);
		for (const auto& layer : _layers | std::views::filter([](auto l) { return l->GetActive(); }))
		{
			for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
			{
				_intermPixels.Set(i, _renderingBrush.MixColor(_drawMode, layer->Get(i), _intermPixels.Get(i)));
			}
		}
		return _intermPixels.Get();
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

	void DrawingEngine::ApplyPixels() noexcept
	{
		auto brush = GetCurrentBrush();
		auto& layer = _layers[_selectedLayer];
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			layer->Set(i, brush->MixColor(_drawMode, _brushPixels.Get(i), layer->Get(i)));
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

		int radius = brush->GetPenSize() / 2.0;
		int sqrRad = radius * radius;

		// https://stackoverflow.com/a/60555404
		for (int px = x - radius; px <= x + radius; px++)
		{
			for (int py = y - radius; py <= y + radius; py++)
			{
				int dx = x - px, dy = y - py;
				if (dx * dx + dy * dy <= sqrRad && _dist(_rng) < GetCurrentBrush()->GetPenForce())
				{
					_brushPixels.TryDraw(px, py, color[0], color[1], color[2], color[3]);
				}
			}
		}
	}

	DrawMode DrawingEngine::GetDrawMode() const noexcept
	{
		return _drawMode;
	}

	void DrawingEngine::SetDrawMode(DrawMode mode) noexcept
	{
		_drawMode = mode;
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

	int DrawingEngine::GetCurrentBrushIndex() const noexcept
	{
		return _currentBrush;
	}

	void DrawingEngine::SetCurrentBrushIndex(int index) noexcept
	{
		_currentBrush = index;
	}

	std::deque<std::string> DrawingEngine::GetBrushNames() const noexcept
	{
		std::deque<std::string> names;
		for (auto&& name : _brushes | std::views::transform([](auto b) { return b->GetName(); }))
		{
			names.push_back(name);
		}
		return names;
	}
}