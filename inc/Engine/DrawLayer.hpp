#pragma once

#include <cstdint>
#include <vector>
#include <optional>

#include "SDL3//SDL_rect.h"

namespace Magia
{
	class DrawLayer
	{
	public:
		DrawLayer() noexcept;
		~DrawLayer() noexcept;
		/// <summary>
		/// Paint all pixels with the color given in parameter
		/// </summary>
		void Clear(uint32_t color);
		/// <summary>
		/// Call Draw but only if the pixel is in canvas bounds
		/// </summary>
		void TryDraw(int x, int y, int r, int g, int b, int a) noexcept;
		/// <summary>
		/// Draw a pixel at the specified position
		/// </summary>
		void Draw(int x, int y, int r, int g, int b, int a) noexcept;
		const std::vector<uint32_t>& Get() noexcept;
		uint32_t Get(int i) const noexcept;
		void Set(int i, uint32_t value) noexcept;
		char* GetName() noexcept;
		void SetActive(bool value) noexcept;
		bool GetActive() const noexcept;

	private:
		/// <summary>
		/// Recalculate _drawRect
		/// </summary>
		std::vector<uint32_t> _pixels;
		char* _name; // TODO: Turn into a std::string later (need to fight imgui first)
		bool _isActive;
	};
}