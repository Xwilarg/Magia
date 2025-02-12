#pragma once

#include <array>
#include <string>
#include "InterpolationMode.hpp"
#include "DrawMode.hpp"

namespace Magia
{
	class ABrush
	{
	public:
		/// <summary>
		/// Get the color that will be used for rendering
		/// </summary>
		virtual const std::array<int, 4>& GetColor() const noexcept = 0;
		/// <summary>
		/// Mix a color from the brush (yourself) and canvas (where we drawing to)
		/// </summary>
		virtual uint32_t MixColor(DrawMode mode, uint32_t brush, uint32_t canvas) const noexcept;
		/// <summary>
		/// Return the name of this brush
		/// </summary>
		const std::string& GetName() const noexcept;

		int GetPenSize() const noexcept;
		void SetPenSize(int size) noexcept;
		int GetPenForce() const noexcept;
		void SetPenForce(int force) noexcept;
		int GetDrawDistance() const noexcept;
		void SetDrawDistance(int distance) noexcept;
		InterpolationMode GetInterpolationMode() const noexcept;
		void SetInterpolationMode(InterpolationMode mode) noexcept;
	protected:
		ABrush(std::string&& name, int penSize, int penForce, int drawDistance, InterpolationMode interpMode = InterpolationMode::CENTRIPETAL_CATMULL_ROM) noexcept;
	private:
		int MixSingleValue(int c1V, int c2V, float alpha1, float alpha2, float alpha) const noexcept;
		int _penSize; // Define the radius of the amount of pixels we are drawing
		int _penForce; // 100 mean all points are colored, 50 mean half of them are
		int _drawDistance; // Min distance between 2 pen stamps
		InterpolationMode _interpMode; // Interpolation methods between 2 dots
		std::string _name;
	};
}