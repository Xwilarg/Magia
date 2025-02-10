#include "InterpolationManager.hpp"

namespace Magia
{
	InterpolationManager::InterpolationManager(DrawingEngine& engine) noexcept
		: _engine(engine), _lastPoints()
	{ }

	void InterpolationManager::AddPoint(int x, int y) noexcept
	{
		if (_lastPoints.size() == 0)
		{
			_engine.Paint(x, y);
		}
	}

	void InterpolationManager::Clear() noexcept
	{
		
	}
}