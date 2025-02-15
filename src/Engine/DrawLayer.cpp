#include <cstdlib>
#include <cstring>
#include <algorithm>

#include "DrawLayer.hpp"
#include "config.hpp"

namespace Magia
{
	DrawLayer::DrawLayer() noexcept
		: _isActive(true), _pixels(CANVAS_WIDTH * WINDOW_HEIGHT), _drawRect(std::nullopt)
	{
		_name = new char[50];
		strcpy(_name, "Default");
	}

	DrawLayer::~DrawLayer() noexcept
	{
		delete[] _name;
	}

	void DrawLayer::Clear(uint32_t color)
	{
		std::fill(_pixels.begin(), _pixels.end(), color);
		_drawRect = std::nullopt;
	}

	void DrawLayer::TryDraw(int x, int y, int r, int g, int b, int a) noexcept
	{
		if (y >= 0 && x >= 0 && y < WINDOW_HEIGHT && x < CANVAS_WIDTH)
		{
			Draw(x, y, r, g, b, a);
		}
	}

	void DrawLayer::Draw(int x, int y, int r, int g, int b, int a) noexcept
	{
		Set(y * CANVAS_WIDTH + x, (r << 24) + (g << 16) + (b << 8) + a);
	}

	uint32_t* DrawLayer::Get() noexcept
	{
		return &_pixels.front();
	}

	uint32_t DrawLayer::Get(int i) const noexcept
	{
		return _pixels[i];
	}

	void DrawLayer::Set(int i, uint32_t value) noexcept
	{
		_pixels[i] = value;
	}

	char* DrawLayer::GetName() noexcept
	{
		return _name;
	}

	void DrawLayer::SetActive(bool value) noexcept
	{
		_isActive = value;
	}

	bool DrawLayer::GetActive() const noexcept
	{
		return _isActive;
	}

	bool DrawLayer::HasRect() const noexcept
	{
		return _drawRect != std::nullopt;
	}

	SDL_Rect DrawLayer::GetRect() const noexcept
	{
		return *_drawRect;
	}
}