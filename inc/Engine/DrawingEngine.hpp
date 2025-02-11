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
		void UpdateScreen(int mouseX, int mouseY) noexcept;
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
		uint32_t GetExportBackgroundColor() const noexcept;
		void SetExportBackgroundColor(uint32_t color) noexcept;
		std::deque<std::string> GetBrushNames() const noexcept;
	private:
		void DrawCursor(int xMouse, int yMouse) noexcept;

		SDL_Renderer* _renderer;
		SDL_Texture* _framebuffer;

		DrawLayer _brushPixels; // Current brush the user is drawing on
		DrawLayer _intermPixels; // Intermediate layer we use to combine _brushPixels and currently active layer, also used for exports
	
		std::vector<std::shared_ptr<DrawLayer>> _layers;
		int _selectedLayer;
		DrawLayer _pixelScreen;

		PaintBrush _renderingBrush; // TODO: We should probably not create a whole brush just to use its "MixColor" method
		DrawMode _drawMode;
		std::vector<std::shared_ptr<ABrush>> _brushes;
		int _currentBrush;
		bool _canUseMouse; // Mouse might get into conflict with some tablets
		uint32_t _exportBackground;

		std::random_device _dev;
		std::mt19937 _rng;
		std::uniform_int_distribution<std::mt19937::result_type> _dist;
	};
}