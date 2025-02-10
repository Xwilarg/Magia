#include "EraserBrush.hpp"
#include "config.hpp"

namespace Magia
{
	EraserBrush::EraserBrush() noexcept
		: ABrush(200, 50, 3), _defaultColor(255, 255, 255, 255)
	{ }

	uint32_t EraserBrush::MixColor(DrawMode drawMode, uint32_t brush, uint32_t canvas) const noexcept
	{
		int brushA = brush & 0xFF;
		if (brushA == 255) return TRANSPARENT_PIXEL;

		return canvas;
	}

	std::string EraserBrush::GetName() const noexcept
	{
		return "Eraser";
	}

	const std::array<int, 4>& EraserBrush::GetColor() const noexcept
	{
		return _defaultColor;
	}
}