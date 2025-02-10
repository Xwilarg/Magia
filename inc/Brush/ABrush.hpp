#pragma once

#include <array>
#include <string>
#include "InterpolationMode.hpp"

namespace Magia
{
	class ABrush
	{
	public:
		virtual const std::array<int, 4>& GetColor() const noexcept = 0;
		virtual uint32_t MixColor(uint32_t brush, uint32_t canvas) const noexcept = 0;
		virtual const std::string& GetName() const noexcept = 0;

		int GetPenSize() const noexcept;
		void SetPenSize(int size) noexcept;
		int GetPenForce() const noexcept;
		void SetPenForce(int force) noexcept;
		int GetDrawDistance() const noexcept;
		void SetDrawDistance(int distance) noexcept;
		InterpolationMode GetInterpolationMode() const noexcept;
		void SetInterpolationMode(InterpolationMode mode) noexcept;
	protected:
		ABrush(int penSize, int penForce, int drawDistance, InterpolationMode interpMode = InterpolationMode::CENTRIPETAL_CATMULL_ROM) noexcept;
	private:
		int _penSize;
		int _penForce; // 100 mean all points are colored, 50 mean half of them are
		int _drawDistance;
		InterpolationMode _interpMode;
	};
}