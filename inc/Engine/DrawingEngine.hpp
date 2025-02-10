#pragma once

#include <random>
#include <deque>
#include "SDL3/SDL.h"
#include "DrawLayer.hpp"
#include "InterpolationMode.hpp"
#include "DrawMode.hpp"
#include "PaintBrush.hpp"

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

		DrawMode GetDrawMode() const noexcept;
		void SetDrawMode(DrawMode mode) noexcept;
		uint32_t* GetFinalFramebuffer() noexcept;
		
		std::shared_ptr<ABrush> GetCurrentBrush() noexcept;
		int GetCurrentBrushIndex() const noexcept;
		void SetCurrentBrushIndex(int index) noexcept;
		std::deque<std::string> GetBrushNames() const noexcept;
	private:
		bool IsPointInsideDrawingCanvas(int x, int y) noexcept;

		SDL_Renderer* _renderer;
		SDL_Texture* _framebuffer;

		DrawLayer _brushPixels;
		std::vector<std::shared_ptr<DrawLayer>> _layers;
		int _selectedLayer;
		DrawLayer _pixelScreen;

		PaintBrush _renderingBrush; // TODO: We should probably not create a whole brush just to use its "MixColor" method
		DrawMode _drawMode;
		std::vector<std::shared_ptr<ABrush>> _brushes;
		int _currentBrush;
		bool _canUseMouse; // Mouse might get into conflict with some tablets

		std::random_device _dev;
		std::mt19937 _rng;
		std::uniform_int_distribution<std::mt19937::result_type> _dist;
	};
}