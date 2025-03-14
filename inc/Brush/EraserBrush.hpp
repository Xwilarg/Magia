#pragma once

#include "ABrush.hpp"

namespace Magia
{
	class EraserBrush : public ABrush
	{
	public:
		EraserBrush() noexcept;

		const std::array<int, 4>& GetColor() const noexcept override;
		uint32_t MixColor(DrawMode drawMode, uint32_t brush, uint32_t canvas) const noexcept override;

	private:
		std::array<int, 4> _defaultColor;
	};
}