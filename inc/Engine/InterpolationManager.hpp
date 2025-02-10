#pragma once

#include "DrawingEngine.hpp"
#include "Vector2.hpp"

namespace Magia
{
	class InterpolationManager
	{
	public:
		InterpolationManager(DrawingEngine& engine) noexcept;
		void AddPoint(float x, float y) noexcept;
		void Clear() noexcept;

	private:
		float GetT(float t, float alpha, const Vector2<float>& p0, const Vector2<float>& p1);
		Vector2<float> CatmullRom(const Vector2<float>& p0, const Vector2<float>& p1, const Vector2<float>& p2, float t, float alpha = .5f);
		DrawingEngine& _engine;
		std::vector<Vector2<float>> _lastPoints;
	};
}