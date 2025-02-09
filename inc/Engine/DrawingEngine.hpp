#pragma once

#include <array>
#include <random>
#include "SDL3/SDL.h"
#include "DrawMode.hpp"
#include "DrawLayer.hpp"

namespace Magia
{
	class DrawingEngine
	{
	public:
		DrawingEngine(SDL_Renderer* renderer);
		void UpdateScreen() noexcept;
		void Paint(int x, int y) noexcept;
		void ApplyPixels() noexcept;

		const std::array<int, 4>& GetColor() const noexcept;
		void SetColor(int r, int g, int b, int a) noexcept;
		int GetPenSize() const noexcept;
		void SetPenSize(int size) noexcept;
		int GetPenForce() const noexcept;
		void SetPenForce(int force) noexcept;
		DrawMode GetDrawMode() const noexcept;
		void SetDrawMode(DrawMode mode) noexcept;
		int GetDrawDistance() const noexcept;
		void SetDrawDistance(int distance) noexcept;
		std::vector<std::shared_ptr<DrawLayer>>& GetLayers() noexcept;
		void AddNewLayer() noexcept;
		int GetSelectedLayerIndex() const noexcept;
		void SetSelectedLayerIndex(int target) noexcept;
	private:
		bool IsPointInsideDrawingCanvas(int x, int y) noexcept;
		uint32_t MixColor(uint32_t brush, uint32_t canvas) const noexcept;
		int MixSingleValue(int c1V, int c2V, float alpha1, float alpha2, float alpha) const noexcept;

		SDL_Renderer* _renderer;
		SDL_Texture* _framebuffer;

		DrawLayer _brushPixels;
		std::vector<std::shared_ptr<DrawLayer>> _layers;
		int _selectedLayer;
		DrawLayer _pixelScreen;

		std::array<int, 4> _color;
		int _penSize;
		int _penForce; // 100 mean all points are colored, 50 mean half of them are
		DrawMode _drawMode;
		int _drawDistance;

		std::random_device _dev;
		std::mt19937 _rng;
		std::uniform_int_distribution<std::mt19937::result_type> _dist;

		const uint32_t WHITE_PIXEL = 4294967295;
		const uint32_t TRANSPARENT_PIXEL = 4294967040;
	};
}