#pragma once

#include "ABrush.hpp"

namespace Magia
{
	class PaintBrush : public ABrush
	{
	public:
		PaintBrush() noexcept;

		const std::array<int, 4>& GetColor() const noexcept override;
		std::string GetName() const noexcept override;

		void SetColor(int r, int g, int b, int a) noexcept;

	private:
		std::array<int, 4> _color;
	};
}