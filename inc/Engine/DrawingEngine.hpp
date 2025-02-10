#pragma once

#include <random>
#include "SDL3/SDL.h"
#include "DrawLayer.hpp"
#include "InterpolationMode.hpp"
#include "ABrush.hpp"

namespace Magia
{
	class DrawingEngine
	{
	public:
		DrawingEngine(SDL_Renderer* renderer);
		void UpdateScreen() noexcept;
		void Paint(int x, int y) noexcept;
		void ApplyPixels() noexcept;

		bool GetCanUseMouse() const noexcept;
		void SetCanUseMouse(bool value) noexcept;
		std::vector<std::shared_ptr<DrawLayer>>& GetLayers() noexcept;
		void AddNewLayer() noexcept;
		void RemoveLayer(int index) noexcept;
		int GetSelectedLayerIndex() const noexcept;
		void SetSelectedLayerIndex(int target) noexcept;
		uint32_t* GetFinalFramebuffer() noexcept;
		std::shared_ptr<ABrush> GetCurrentBrush() noexcept;
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

		std::vector<std::shared_ptr<ABrush>> _brushes;
		int _currentBrush;
		bool _canUseMouse; // Mouse might get into conflict with some tablets

		std::random_device _dev;
		std::mt19937 _rng;
		std::uniform_int_distribution<std::mt19937::result_type> _dist;

		const uint32_t WHITE_PIXEL = 4294967295;
		const uint32_t TRANSPARENT_PIXEL = 4294967040;
	};
}