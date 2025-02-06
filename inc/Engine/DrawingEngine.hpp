#pragma once

#include <array>
#include <random>
#include "SDL3/SDL.h"

namespace Magia
{
	class DrawingEngine
	{
	public:
		DrawingEngine(SDL_Renderer* renderer);
		~DrawingEngine() noexcept;
		void UpdateScreen() noexcept;
		void Paint(int x, int y) noexcept;
		void ApplyPixels() noexcept;

		const std::array<int, 4>& GetColor() const noexcept;
		void SetColor(int r, int g, int b, int a) noexcept;
		int GetPenSize() const noexcept;
		void SetPenSize(int size) noexcept;
		int GetPenForce() const noexcept;
		void SetPenForce(int force) noexcept;
	private:
		bool IsPointInsideDrawingCanvas(int x, int y) noexcept;
		void DrawPixel(int x, int y) noexcept;
		uint32_t MixColor(uint32_t c1, uint32_t c2) const noexcept;
		SDL_Renderer* _renderer;
		SDL_Texture* _framebuffer;
		uint32_t* _pixels;
		uint32_t* _currPixels;

		uint32_t* _pixelsScreen;

		std::array<int, 4> _color;
		int _penSize;
		int _penForce; // 100 mean all points are colored, 50 mean half of them are

		std::random_device _dev;
		std::mt19937 _rng;
		std::uniform_int_distribution<std::mt19937::result_type> _dist;
	};
}