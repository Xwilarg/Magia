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
		void SetColor(int r, int g, int b, int a) noexcept;
		DrawMode GetDrawMode() const noexcept;
		void SetDrawMode(DrawMode mode) noexcept;

	private:
		std::array<int, 4> _color;
		DrawMode _drawMode;
	};
}