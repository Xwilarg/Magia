#pragma once

#include <cstdint>

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
		uint32_t* Get() noexcept;
		uint32_t Get(int i) const noexcept;
		void Set(int i, uint32_t value) noexcept;
		char* GetName() noexcept;
		void SetActive(bool value) noexcept;
		bool GetActive() const noexcept;

	private:
		uint32_t* _pixels;
		char* _name; // TODO: Turn into a std::string later (need to fight imgui first)
		bool _isActive;
	};
}