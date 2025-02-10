#include "PaintBrush.hpp"

namespace Magia
{
	PaintBrush::PaintBrush() noexcept
		: ABrush(7, 30, 3), _color(), _drawMode(DrawMode::MULTIPLICATIVE)
	{
		SetColor(0, 0, 0, 255);
	}

	const std::array<int, 4>& PaintBrush::GetColor() const noexcept
	{
		return _color;
	}

	void PaintBrush::SetColor(int r, int g, int b, int a) noexcept
	{
		_color[0] = r;
		_color[1] = g;
		_color[2] = b;
		_color[3] = a;
	}

	DrawMode PaintBrush::GetDrawMode() const noexcept
	{
		return _drawMode;
	}

	void PaintBrush::SetDrawMode(DrawMode mode) noexcept
	{
		_drawMode = mode;
	}
}