#pragma once

#include <cstdint>

namespace Magia
{
	class DrawLayer
	{
	public:
		DrawLayer() noexcept;
		~DrawLayer() noexcept;
		void Clear(uint32_t color);
		void Draw(int x, int y, int r, int g, int b, int a);
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