#pragma once

#include <vector>

namespace Magia
{
	class Stamp
	{
	public:
		Stamp(int baseSize) noexcept;
		void Recompute(int size) noexcept;
		bool Get(int pos) const noexcept;

	private:
		std::vector<bool> _brush;
	};
}