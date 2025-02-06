#pragma once

#include <array>
#include <random>
#include "SDL2/SDL.h"

namespace Magia
{
	class DrawingEngine
	{
	public:
		DrawingEngine(SDL_Renderer* renderer);
		~DrawingEngine() noexcept;
		void UpdateScreen() noexcept;
		void Paint(int x, int y) noexcept;

		const std::array<int, 4>& GetColor() const noexcept;
		void SetColor(int r, int g, int b, int a) noexcept;
		int GetPenSize() const noexcept;
		void SetPenSize(int size) noexcept;
		int GetPenForce() const noexcept;
		void SetPenForce(int force) noexcept;
	private:
		bool IsPointInsideDrawingCanvas(int x, int y) noexcept;
		void DrawPixel(int x, int y) noexcept;
		SDL_Renderer* _renderer;
		SDL_Texture* _framebuffer;
		uint32_t* _pixels;

		std::array<int, 4> _color;
		int _penSize;
		int _penForce; // 100 mean all points are colored, 50 mean half of them are

		std::random_device _dev;
		std::mt19937 _rng;
		std::uniform_int_distribution<std::mt19937::result_type> _dist;
	};
}