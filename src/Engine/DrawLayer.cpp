#include "DrawLayer.hpp"
#include "config.hpp"

namespace Magia
{
	DrawLayer::DrawLayer() noexcept
	{
		_pixels = new uint32_t[CANVAS_WIDTH * WINDOW_HEIGHT];
	}

	void DrawLayer::Clear(uint32_t color)
	{
		for (int i = 0; i < CANVAS_WIDTH * WINDOW_HEIGHT; i++)
		{
			_pixels[i] = color;
		}
	}

	uint32_t* DrawLayer::Get() noexcept
	{
		return _pixels;
	}

	const uint32_t& DrawLayer::Get(int i) const noexcept
	{
		return _pixels[i];
	}

	void DrawLayer::Set(int i, uint32_t value) noexcept
	{
		_pixels[i] = value;
	}
}