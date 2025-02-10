#include "ABrush.hpp"

namespace Magia
{
	ABrush::ABrush(int penSize, int penForce, int drawDistance, InterpolationMode interpMode) noexcept
		: _penSize(penSize), _penForce(penForce), _drawDistance(drawDistance), _interpMode(interpMode)
	{ }

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