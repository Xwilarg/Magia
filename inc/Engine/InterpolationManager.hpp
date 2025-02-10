#pragma once

#include "DrawingEngine.hpp"
#include "Vector2.hpp"

namespace Magia
{
	class InterpolationManager
	{
	public:
		InterpolationManager(DrawingEngine& engine) noexcept;
		void AddPoint(int x, int y) noexcept;
		void Clear() noexcept;

	private:
		DrawingEngine& _engine;
		std::vector<Vector2<int>> _lastPoints;
	};
}