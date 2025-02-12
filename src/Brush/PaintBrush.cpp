#include "PaintBrush.hpp"

namespace Magia
{
	PaintBrush::PaintBrush(std::string name, int penSize, int penForce, int drawingDistance) noexcept
		: ABrush(std::move(name), penSize, penForce, drawingDistance), _color()
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
}