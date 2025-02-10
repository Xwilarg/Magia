#pragma once

#include "ABrush.hpp"
#include "DrawMode.hpp"

namespace Magia
{
	class PaintBrush : public ABrush
	{
	public:
		PaintBrush() noexcept;

		const std::array<int, 4>& GetColor() const noexcept override;
		uint32_t MixColor(uint32_t brush, uint32_t canvas) const noexcept override;
		std::string GetName() const noexcept override;

		void SetColor(int r, int g, int b, int a) noexcept;
		DrawMode GetDrawMode() const noexcept;
		void SetDrawMode(DrawMode mode) noexcept;

	private:
		int MixSingleValue(int c1V, int c2V, float alpha1, float alpha2, float alpha) const noexcept;
		std::array<int, 4> _color;
		DrawMode _drawMode;
	};
}