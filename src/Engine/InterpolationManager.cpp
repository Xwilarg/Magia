#include "InterpolationManager.hpp"

namespace Magia
{
	InterpolationManager::InterpolationManager(DrawingEngine& engine) noexcept
		: _engine(engine), _lastPoints()
	{ }

	void InterpolationManager::AddPoint(float x, float y) noexcept
	{
		if (_lastPoints.size() == 0)
		{
			_engine.Paint(x, y);
			_lastPoints.push_back(Vector2<float>(x, y));
		}
		else
		{
			auto last = _lastPoints.back();
			auto brush = _engine.GetCurrentBrush();
			if (std::sqrt(std::pow(last.X - x, 2) + std::pow(last.Y - y, 2)) >= brush->GetDrawDistance())
			{
				int dist = static_cast<int>(std::sqrt(std::pow(x - last.X, 2) + std::pow(y - last.Y, 2)));
				if (brush->GetInterpolationMode() == InterpolationMode::NONE || _lastPoints.size() < 2)
				{
					// Intepolate between last pos and current one
					for (int d = 0; d <= dist; d += brush->GetDrawDistance())
					{
						int cx = static_cast<int>(std::lerp(last.X, x, d / static_cast<float>(dist)));
						int cy = static_cast<int>(std::lerp(last.Y, y, d / static_cast<float>(dist)));
						_engine.Paint(cx, cy);
					}
					_lastPoints.push_back(Vector2<float>(x, y));
				}
				else
				{
					// https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline
					// Thanks Luxio
					auto lastB = _lastPoints.end()[-2];
					auto last = _lastPoints.back();
					auto curr = Vector2<float>(x, y);
					for (int d = 0; d <= dist; d += brush->GetDrawDistance())
					{
						auto p = CatmullRom(lastB, last, curr, static_cast<float>(d) / dist);
						_engine.Paint(p.X, p.Y);
					}
					_lastPoints.push_back(Vector2<float>(x, y));
				}
			}
		}
	}

	Vector2<float> InterpolationManager::CatmullRom(const Vector2<float>& p0, const Vector2<float>& p1, const Vector2<float>& p2, float t, float alpha)
	{
		float t0 = 0.0f;
		float t1 = GetT(t0, alpha, p0, p1);
		float t2 = GetT(t1, alpha, p1, p2);
		t = std::lerp(t1, t2, t);
		Vector2<float> A1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
		Vector2<float> A2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
		Vector2<float> B1 = (t2 - t) / (t2 - t0) * A1 + (t - t0) / (t2 - t0) * A2;
		return B1;
	}


	float InterpolationManager::GetT(float t, float alpha, const Vector2<float>& p0, const Vector2<float>& p1)
	{
		auto d = p1 - p0;
		float a = d | d;
		float b = std::pow(a, alpha * .5f);
		return (b + t);
	}

	void InterpolationManager::Clear() noexcept
	{
		_lastPoints.clear();
	}
}