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
		: _renderer(renderer), _canvasSize(CANVAS_WIDTH, WINDOW_HEIGHT), _canUseMouse(true), _drawMode(DrawMode::MULTIPLICATIVE), _renderingBrush("internal_brush", 1, 100, 1), _exportBackground(WHITE_PIXEL), _dirtyRects(), _dev(), _rng(_dev()), _dist(1, 100), _brushPixels(_canvasSize.X, _canvasSize.Y), _layers(), _selectedLayer(), _layersBefore(_canvasSize.X, _canvasSize.Y), _layersAfter(_canvasSize.X, _canvasSize.Y), _currentBrush(0), _brushes(), _actionHistory(), _actionIndex(0), _offsetX(0), _offsetY(0), _offsetMoveSpeed(5)
	{
		_brushes.emplace_back(std::make_shared<PaintBrush>("Pencil", 10, 30, 5));
		_brushes.emplace_back(std::make_shared<PaintBrush>("Ink Pen", 5, 100, 1));
		_brushes.emplace_back(std::make_shared<EraserBrush>());

		_framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, WINDOW_HEIGHT);
		_brushPixels.Clear(TRANSPARENT_PIXEL);

		AddNewLayer();
		RedrawLayerCache();
	}

	// https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
	void DrawingEngine::DrawCursor(int xMouse, int yMouse, SDL_Rect canvas, std::vector<uint32_t>& buf) noexcept
	{
		int radius = GetCurrentBrush()->GetPenSize() / 2.0;
		int x = radius - 1;
		int y = 0;
		int dx = 1;
		int dy = 1;
		int err = dx - (radius << 1);

		auto addToBuffer = [&](int x, int y)
		{
			if (x >= canvas.x && y >= canvas.y && x < canvas.x + canvas.w && y < canvas.y + canvas.h)
			{
				auto p = (x - canvas.x) + ((y - canvas.y) * canvas.w);
				buf[p] = BLACK_PIXEL;
			}
		};

		while (x >= y)
		{
			addToBuffer(xMouse + x, yMouse + y);
			addToBuffer(xMouse + y, yMouse + x);
			addToBuffer(xMouse - y, yMouse + x);
			addToBuffer(xMouse - x, yMouse + y);
			addToBuffer(xMouse - x, yMouse - y);
			addToBuffer(xMouse - y, yMouse - x);
			addToBuffer(xMouse + y, yMouse - x);
			addToBuffer(xMouse + x, yMouse - y);

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
			for (int i = 0; i < _canvasSize.X * _canvasSize.Y; i++)
			{
				_layersBefore.Set(i, _renderingBrush.MixColor(_drawMode, layer->Get(i), _layersBefore.Get(i)));
			}
		}
		for (const auto& layer : _layers | std::views::drop(_selectedLayer + 1) | std::views::filter([](auto l) { return l->GetActive(); }))
		{
			for (int i = 0; i < _canvasSize.X * _canvasSize.Y; i++)
			{
				_layersAfter.Set(i, _renderingBrush.MixColor(_drawMode, layer->Get(i), _layersAfter.Get(i)));
			}
		}

		DirtyScreen();
	}

	void DrawingEngine::UpdateScreen(int mouseX, int mouseY) noexcept
	{
		int radius = GetCurrentBrush()->GetPenSize() / 2.0; // TODO: copied code
		SetAreaDirty(mouseX - radius, mouseY - radius, radius * 2, radius * 2);
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
					auto globalI = (canvas.x + x + _offsetX) + ((canvas.y + y + _offsetY) * CANVAS_WIDTH);
					if (globalI >= 0 && globalI < CANVAS_WIDTH * WINDOW_HEIGHT)
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
						auto newX = canvas.x + x + _offsetX;
						auto newY = canvas.y + y + _offsetY;

						if (newX < 0 || newY < 0 || newX >= CANVAS_WIDTH || newY >= WINDOW_HEIGHT)
						{ // TODO: Doesn't work if current layer is disabled
							buf[x + (y * canvas.w)] = BLACK_PIXEL;
						}
						else
						{
							auto globalI = newX + (newY * CANVAS_WIDTH);
							auto step2 = brush->MixColor(_drawMode, _brushPixels.Get(globalI), midLayer->Get(globalI));
							buf[x + (y * canvas.w)] = _renderingBrush.MixColor(_drawMode, step2, buf[x + (y * canvas.w)]);
						}
					}
				}
			}
			for (int y = 0; y < canvas.h; y++)
			{
				for (int x = 0; x < canvas.w; x++)
				{
					auto globalI = (canvas.x + x + _offsetX) + ((canvas.y + y + _offsetY) * CANVAS_WIDTH);
					if (globalI >= 0 && globalI < CANVAS_WIDTH * WINDOW_HEIGHT)
						buf[x + (y * canvas.w)] = _renderingBrush.MixColor(_drawMode, _layersAfter.Get(globalI), buf[x + (y * canvas.w)]);
				}
			}

			DrawCursor(mouseX, mouseY, canvas, buf);
			SDL_UpdateTexture(_framebuffer, &canvas, &buf.front(), canvas.w * sizeof(uint32_t));

			_dirtyRects.clear();
		}
		SetAreaDirty(mouseX - radius, mouseY - radius, radius * 2, radius * 2);

		SDL_FRect rect{};
		rect.x = 0;
		rect.y = 0;
		rect.w = CANVAS_WIDTH;
		rect.h = WINDOW_HEIGHT;
		SDL_RenderTexture(_renderer, _framebuffer, &rect, &rect);
	}

	std::shared_ptr<ABrush> DrawingEngine::GetCurrentBrush() noexcept
	{
		return _brushes[_currentBrush];
	}

	std::vector<uint32_t> DrawingEngine::GetFinalFramebuffer() noexcept
	{
		std::vector<uint32_t> screen;
		for (int y = 0; y < _canvasSize.Y; y++)
		{
			for (int x = 0; x < _canvasSize.X; x++)
			{
				uint32_t color = _exportBackground;
				auto globalI = x + (y * CANVAS_WIDTH);

				color = _renderingBrush.MixColor(_drawMode, _layersBefore.Get(globalI), color);
				auto& midLayer = _layers[_selectedLayer];
				if (midLayer->GetActive()) color = _renderingBrush.MixColor(_drawMode, midLayer->Get(globalI), color);
				color = _renderingBrush.MixColor(_drawMode, _layersAfter.Get(globalI), color);

				screen.push_back(color);
			}
		}
		return screen;
	}

	void DrawingEngine::AddNewLayer() noexcept
	{
		auto newLayer = std::make_shared<DrawLayer>(_canvasSize.X, _canvasSize.Y);
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

		/* // TODO: Following code crash
		// We aren't at the last element of our history
		// Since removing a layer is destructive, we just discard all history after current point
		if (_actionIndex < _actionHistory.size())
		{
			_actionHistory.erase(std::begin(_actionHistory) + _actionIndex, std::end(_actionHistory));
		}

		// Remove layer deleted from history
		_actionHistory.erase(std::remove_if(std::begin(_actionHistory), std::end(_actionHistory),
			[&](std::unique_ptr<Action>& a) { return a->LayerID == index; }));

		// Since we removed a layer, layers ID changed so we need to update everything
		for (auto&& a : _actionHistory)
		{
			if (a->LayerID > index)
			{
				a->LayerID--;
			}
		}*/
		// For now we just clear the whole history when we delete a layer
		_actionHistory.clear();
		_actionIndex = 0;

		RedrawLayerCache();
	}

	void DrawingEngine::ApplyPixels() noexcept
	{
		auto brush = GetCurrentBrush();
		auto& layer = _layers[_selectedLayer];

		// Apply brush to canvas & create history
		auto action = std::make_unique<Action>();
		action->DataBefore = layer->Get();
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			layer->Set(i, brush->MixColor(_drawMode, _brushPixels.Get(i), layer->Get(i)));
		}
		action->DataAfter = layer->Get();
		action->LayerID = _selectedLayer;

		// If we aren't at the end of the history, discard what there is after us
		if (_actionIndex < _actionHistory.size())
		{
			_actionHistory.erase(std::begin(_actionHistory) + _actionIndex, std::end(_actionHistory));
		}

		// Add action to history
		_actionHistory.push_back(std::move(action));
		_actionIndex++;

		// Clear brush storing current stroke
		_brushPixels.Clear(TRANSPARENT_PIXEL);
	}

	void DrawingEngine::MoveCanvas(int x, int y) noexcept
	{
		_offsetX += x * _offsetMoveSpeed;
		_offsetY += y * _offsetMoveSpeed;

		DirtyScreen();
	}

	void DrawingEngine::DirtyScreen() noexcept
	{
		SDL_Rect rect{};
		rect.x = 0;
		rect.y = 0;
		rect.h = WINDOW_HEIGHT;
		rect.w = CANVAS_WIDTH;
		_dirtyRects.push_back(std::move(rect));
	}

	/// <summary>
	/// Draw group of pixel at mouse pos
	/// </summary>
	void DrawingEngine::Paint(int x, int y) noexcept
	{
		if (!_layers[_selectedLayer]->GetActive()) return;

		auto brush = GetCurrentBrush();
		auto& color = brush->GetColor();

		auto radius = brush->GetPenSize() / 2.0;
		auto sqrRad = radius * radius;

		// https://stackoverflow.com/a/60555404
		for (int px = x - radius; px <= x + radius; px++)
		{
			for (int py = y - radius; py <= y + radius; py++)
			{
				int dx = x - px, dy = y - py;
				if (dx * dx + dy * dy <= sqrRad && _dist(_rng) < GetCurrentBrush()->GetPenForce())
				{
					_brushPixels.TryDraw(px + _offsetX, py + _offsetY, color[0], color[1], color[2], color[3]);
				}
			}
		}

		SetAreaDirty(x - radius, y - radius, sqrRad, sqrRad);
	}

	void DrawingEngine::SetAreaDirty(int x, int y, int w, int h) noexcept
	{
		SDL_Rect rect1{};
		rect1.x = x;
		rect1.y = y;
		rect1.w = w;
		rect1.h = h;
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

	int DrawingEngine::GetOffsetMoveSpeed() const noexcept
	{
		return _offsetMoveSpeed;
	}

	void DrawingEngine::SetOffsetMoveSpeed(int speed) noexcept
	{
		_offsetMoveSpeed = speed;
	}

	const Vector2<int>& DrawingEngine::GetCanvasSize() const noexcept
	{
		return _canvasSize;
	}

	void DrawingEngine::Undo() noexcept
	{
		auto&& data = _actionHistory[_actionIndex - 1];
		for (int i = 0; i < WINDOW_HEIGHT * CANVAS_WIDTH; i++)
		{
			_layers[data->LayerID]->Set(i, data->DataBefore[i]);
		}
		_actionIndex--;
		RedrawLayerCache();
	}

	void DrawingEngine::Redo() noexcept
	{
		auto&& data = _actionHistory[_actionIndex];
		for (int i = 0; i < WINDOW_HEIGHT * CANVAS_WIDTH; i++)
		{
			_layers[data->LayerID]->Set(i, data->DataAfter[i]);
		}
		_actionIndex++;
		RedrawLayerCache();
	}

	int DrawingEngine::GetCurrentHistoryIndex() const noexcept
	{
		return _actionIndex;
	}

	int DrawingEngine::GetHistoryCount() const noexcept
	{
		return _actionHistory.size();
	}
}