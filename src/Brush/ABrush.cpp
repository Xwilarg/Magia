#include "ABrush.hpp"

namespace Magia
{
	ABrush::ABrush(int penSize, int penForce, int drawDistance, InterpolationMode interpMode) noexcept
		: _penSize(penSize), _penForce(penForce), _drawDistance(drawDistance), _interpMode(interpMode)
	{ }

	int ABrush::MixSingleValue(int c1V, int c2V, float alpha1, float alpha2, float alpha) const noexcept
	{
		return ((c1V * alpha1) + (c2V * alpha2 * (1 - alpha1))) / (alpha1 + alpha2 * (1 - alpha1));
	}

	uint32_t ABrush::MixColor(DrawMode mode, uint32_t brush, uint32_t canvas) const noexcept
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

	int ABrush::GetPenSize() const noexcept
	{
		return _penSize;
	}

	void ABrush::SetPenSize(int size) noexcept
	{
		_penSize = size;
	}

	int ABrush::GetPenForce() const noexcept
	{
		return _penForce;
	}

	void ABrush::SetPenForce(int force) noexcept
	{
		_penForce = force;
	}

	int ABrush::GetDrawDistance() const noexcept
	{
		return _drawDistance;
	}

	void ABrush::SetDrawDistance(int distance) noexcept
	{
		_drawDistance = distance;
	}

	InterpolationMode ABrush::GetInterpolationMode() const noexcept
	{
		return _interpMode;
	}

	void ABrush::SetInterpolationMode(InterpolationMode mode) noexcept
	{
		_interpMode = mode;
	}
}