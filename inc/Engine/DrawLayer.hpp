#pragma once

#include <cstdint>

namespace Magia
{
	class DrawLayer
	{
	public:
		DrawLayer() noexcept;
		void Clear(uint32_t color);
		uint32_t* Get() noexcept;
		const uint32_t& Get(int i) const noexcept;
		void Set(int i, uint32_t value) noexcept;

	private:
		uint32_t* _pixels;
		uint32_t _defaultColor;
	};
}