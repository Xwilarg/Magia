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
		: _renderer(renderer), _canUseMouse(true), _drawMode(DrawMode::MULTIPLICATIVE), _renderingBrush("internal_brush", 1, 100, 1), _exportBackground(WHITE_PIXEL), _dirtyRects(), _dev(), _rng(_dev()), _dist(1, 100), _screenBuffer(CANVAS_WIDTH * WINDOW_HEIGHT), _brushPixels(), _layers(), _selectedLayer(), _layersBefore(), _layersAfter(), _currentBrush(0), _brushes()
	{
		_brushes.emplace_back(std::make_shared<PaintBrush>("Pencil", 10, 30, 5));
		_brushes.emplace_back(std::make_shared<PaintBrush>("Ink Pen", 5, 100, 1));
		_brushes.emplace_back(std::make_shared<EraserBrush>());

		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, WINDOW_HEIGHT);
		_brushPixels.Clear(TRANSPARENT_PIXEL);
		std::fill(_screenBuffer.begin(), _screenBuffer.end(), TRANSPARENT_PIXEL);

		AddNewLayer();
		RedrawLayerCache();
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
			/*_finalScreen.TryDraw(xMouse + x, yMouse + y, 0, 0, 0, 255);
			_finalScreen.TryDraw(xMouse + y, yMouse + x, 0, 0, 0, 255);
			_finalScreen.TryDraw(xMouse - y, yMouse + x, 0, 0, 0, 255);
			_finalScreen.TryDraw(xMouse - x, yMouse + y, 0, 0, 0, 255);
			_finalScreen.TryDraw(xMouse - x, yMouse - y, 0, 0, 0, 255);
			_finalScreen.TryDraw(xMouse - y, yMouse - x, 0, 0, 0, 255);
			_finalScreen.TryDraw(xMouse + y, yMouse - x, 0, 0, 0, 255);
			_finalScreen.TryDraw(xMouse + x, yMouse - y, 0, 0, 0, 255);*/

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

	void DrawingEngine::RedrawLayerCache() noexcept
	{
		// We precompute the sum of layers that are before and after selected layers
		_layersBefore.Clear(TRANSPARENT_PIXEL);
		_layersAfter.Clear(TRANSPARENT_PIXEL);

		for (const auto& layer : _layers | std::views::take(_selectedLayer) | std::views::filter([](auto l) { return l->GetActive(); }))
		{
			for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
			{
				_layersBefore.Set(i, _renderingBrush.MixColor(_drawMode, layer->Get(i), _layersBefore.Get(i)));
			}
		}
		for (const auto& layer : _layers | std::views::drop(_selectedLayer + 1) | std::views::filter([](auto l) { return l->GetActive(); }))
		{
			for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
			{
				_layersAfter.Set(i, _renderingBrush.MixColor(_drawMode, layer->Get(i), _layersAfter.Get(i)));
			}
		}

		SDL_Rect rect{};
		rect.x = 0;
		rect.y = 0;
		rect.h = WINDOW_HEIGHT;
		rect.w = CANVAS_WIDTH;
		_dirtyRects.push_back(std::move(rect));
	}

	void DrawingEngine::UpdateScreen(int mouseX, int mouseY) noexcept
	{
		for (auto&& canvas : _dirtyRects)
		{
			auto brush = GetCurrentBrush();
			auto size = canvas.w * canvas.h;
			std::vector<uint32_t> buf(size);
			std::fill(buf.begin(), buf.end(), WHITE_PIXEL);

			for (int y = 0; y < canvas.h; y++)
			{
				for (int x = 0; x < canvas.w; x++)
				{
					auto globalI = (canvas.x + x) + ((canvas.y + y) * CANVAS_WIDTH);
					buf[x + (y * canvas.w)] = _renderingBrush.MixColor(_drawMode, _layersBefore.Get(globalI), buf[x + (y * canvas.w)]);
				}
			}
			auto& midLayer = _layers[_selectedLayer];
			if (midLayer->GetActive())
			{
				for (int y = 0; y < canvas.h; y++)
				{
					for (int x = 0; x < canvas.w; x++)
					{
						auto globalI = (canvas.x + x) + ((canvas.y + y) * CANVAS_WIDTH);

						auto step2 = brush->MixColor(_drawMode, _brushPixels.Get(globalI), midLayer->Get(globalI));
						buf[x + (y * canvas.w)] = _renderingBrush.MixColor(_drawMode, step2, buf[x + (y * canvas.w)]);
					}
				}
			}
			for (int y = 0; y < canvas.h; y++)
			{
				for (int x = 0; x < canvas.w; x++)
				{
					auto globalI = (canvas.x + x) + ((canvas.y + y) * CANVAS_WIDTH);
					buf[x + (y * canvas.w)] = _renderingBrush.MixColor(_drawMode, _layersAfter.Get(globalI), buf[x + (y * canvas.w)]);
				}
			}

			for (int y = 0; y < canvas.h; y++)
			{
				for (int x = 0; x < canvas.w; x++)
				{
					auto globalI = (canvas.x + x) + ((canvas.y + y) * CANVAS_WIDTH);
					_screenBuffer[globalI] = buf[x + (y * canvas.w)];
				}
			}
			SDL_UpdateTexture(_framebuffer, &canvas, &buf.front(), canvas.w * sizeof(uint32_t));

			_dirtyRects.clear();
		}

		SDL_FRect rect{};
		rect.x = 0;
		rect.y = 0;
		rect.w = CANVAS_WIDTH;
		rect.h = WINDOW_HEIGHT;
		SDL_RenderTexture(_renderer, _framebuffer, &rect, &rect);

//		DrawCursor(mouseX, mouseY);

	}

	std::shared_ptr<ABrush> DrawingEngine::GetCurrentBrush() noexcept
	{
		return _brushes[_currentBrush];
	}

	uint32_t* DrawingEngine::GetFinalFramebuffer() noexcept
	{
		return &_screenBuffer.front();
	}

	void DrawingEngine::AddNewLayer() noexcept
	{
		auto newLayer = std::make_shared<DrawLayer>();
		newLayer->Clear(TRANSPARENT_PIXEL);
		_layers.push_back(newLayer);
		_selectedLayer = _layers.size() - 1;
		RedrawLayerCache();
	}

	void DrawingEngine::RemoveLayer(int index) noexcept
	{
		_layers.erase(_layers.begin() + index);
		if (_selectedLayer > _layers.size() - 1)
		{
			_selectedLayer = _layers.size() - 1;
		}
		RedrawLayerCache();
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

		int radius = static_cast<int>(std::ceil(brush->GetPenSize() / 2.0)); // TOOD
		if (radius == 1) radius = 2;
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

		SDL_Rect rect1{};
		rect1.x = x - radius;
		rect1.y = y - radius;
		rect1.w = sqrRad;
		rect1.h = sqrRad;
		SDL_Rect rect2{};
		rect2.x = 0;
		rect2.y = 0;
		rect2.w = CANVAS_WIDTH;
		rect2.h = WINDOW_HEIGHT;
		SDL_Rect _finalRect;
		SDL_GetRectIntersection(&rect1, &rect2, &_finalRect);
		if (_finalRect.w > 0 && _finalRect.h > 0)
		{
			_dirtyRects.push_back(std::move(_finalRect));
		}
	}

	DrawMode DrawingEngine::GetDrawMode() const noexcept
	{
		return _drawMode;
	}

	void DrawingEngine::SetDrawMode(DrawMode mode) noexcept
	{
		auto isDirty = mode != _drawMode;
		_drawMode = mode;
		if (isDirty) RedrawLayerCache();
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
		auto isDirty = target != _selectedLayer;
		_selectedLayer = target;
		if (isDirty) RedrawLayerCache();
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

	uint32_t DrawingEngine::GetExportBackgroundColor() const noexcept
	{
		return _exportBackground;
	}

	void DrawingEngine::SetExportBackgroundColor(uint32_t color) noexcept
	{
		_exportBackground = color;
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