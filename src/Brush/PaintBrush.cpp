#include "PaintBrush.hpp"

namespace Magia
{
	PaintBrush::PaintBrush() noexcept
		: ABrush(7, 30, 3), _color(), _drawMode(DrawMode::MULTIPLICATIVE)
	{
		SetColor(0, 0, 0, 255);
	}

	int PaintBrush::MixSingleValue(int c1V, int c2V, float alpha1, float alpha2, float alpha) const noexcept
	{
		return ((c1V * alpha1) + (c2V * alpha2 * (1 - alpha1))) / (alpha1 + alpha2 * (1 - alpha1));
	}

	uint32_t PaintBrush::MixColor(uint32_t brush, uint32_t canvas) const noexcept
	{
		// We only handle multiplicative for now

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


	const std::string& PaintBrush::GetName() const noexcept
	{
		return "Pen";
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