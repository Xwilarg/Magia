#include "Stamp.hpp"

namespace Magia
{
	Stamp::Stamp(int baseSize) noexcept
		: _brush()
	{
		Recompute(baseSize);
	}

	void Stamp::Recompute(int size) noexcept
	{
		_brush.resize(size * size);
		std::fill(_brush.begin(), _brush.end(), false);

		auto radius = size / 2.0;
		auto sqrRad = radius * radius;

		// https://stackoverflow.com/a/60555404
		for (int px = 0; px <= radius + radius; px++)
		{
			for (int py = 0; py <= radius + radius; py++)
			{
				int dx = radius - px, dy = radius - py;
				if (dx * dx + dy * dy <= sqrRad && py * size + px < size * size)
				{
					_brush[py * size + px] = true;
				}
			}
		}
	}

	bool Stamp::Get(int pos) const noexcept
	{
		return _brush[pos];
	}
}