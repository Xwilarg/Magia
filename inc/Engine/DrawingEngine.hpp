#pragma once

#include <random>
#include <deque>
#include "SDL3/SDL.h"
#include "DrawLayer.hpp"
#include "InterpolationMode.hpp"
#include "DrawMode.hpp"
#include "PaintBrush.hpp"
#include "Action.hpp"
#include "Vector2.hpp"

namespace Magia
{
	class DrawingEngine
	{
	public:
		DrawingEngine(SDL_Renderer* renderer, int width, int height);
		void UpdateScreen(int mouseX, int mouseY) noexcept;
		void Paint(int x, int y) noexcept;
		/// <summary>
		/// Called when mouse is raised, apply current stroke to canvas
		/// </summary>
		void ApplyPixels() noexcept;

		void MoveCanvas(int x, int y) noexcept;

		void DirtyScreen() noexcept;

		// Layers
		std::vector<std::shared_ptr<DrawLayer>>& GetLayers() noexcept;
		void AddNewLayer() noexcept;
		void RemoveLayer(int index) noexcept;
		int GetSelectedLayerIndex() const noexcept;
		void SetSelectedLayerIndex(int target) noexcept;

		// History
		void Undo() noexcept;
		void Redo() noexcept;
		int GetCurrentHistoryIndex() const noexcept;
		int GetHistoryCount() const noexcept;

		// Others getter/setters
		bool GetCanUseMouse() const noexcept;
		void SetCanUseMouse(bool value) noexcept;
		DrawMode GetDrawMode() const noexcept;
		void SetDrawMode(DrawMode mode) noexcept;
		std::vector<uint32_t> GetFinalFramebuffer() noexcept;
		std::shared_ptr<ABrush> GetCurrentBrush() noexcept;
		int GetCurrentBrushIndex() const noexcept;
		void SetCurrentBrushIndex(int index) noexcept;
		uint32_t GetExportBackgroundColor() const noexcept;
		void SetExportBackgroundColor(uint32_t color) noexcept;
		std::deque<std::string> GetBrushNames() const noexcept;
		int GetOffsetMoveSpeed() const noexcept;
		void SetOffsetMoveSpeed(int speed) noexcept;
		const Vector2<int>& GetCanvasSize() const noexcept;

		/// <summary>
		/// Recompute layers that aren't drawn on
		/// Need to be called everytimes something change on non-current layer (layer being disabled, draw mode changing, layer switch...)
		/// </summary>
		void RedrawLayerCache() noexcept;
	private:
		void DrawCursor(int xMouse, int yMouse, SDL_Rect canvas, std::vector<uint32_t>& buf) noexcept;
		void SetAreaDirty(int x, int y, int w, int h) noexcept;

		SDL_Renderer* _renderer;
		SDL_Texture* _framebuffer;

		Vector2<int> _canvasSize;

		std::vector<std::shared_ptr<DrawLayer>> _layers;
		int _selectedLayer;
		DrawLayer _brushPixels; // Current brush the user is drawing on
		DrawLayer _layersBefore;
		DrawLayer _layersAfter;

		PaintBrush _renderingBrush; // TODO: We should probably not create a whole brush just to use its "MixColor" method
		DrawMode _drawMode;
		std::vector<std::shared_ptr<ABrush>> _brushes;
		int _currentBrush;
		bool _canUseMouse; // Mouse might get into conflict with some tablets
		uint32_t _exportBackground;

		std::vector<SDL_Rect> _dirtyRects;

		std::random_device _dev;
		std::mt19937 _rng;
		std::uniform_int_distribution<std::mt19937::result_type> _dist;

		std::vector<std::unique_ptr<Action>> _actionHistory;
		int _actionIndex;

		int _offsetX, _offsetY;
		int _offsetMoveSpeed;
	};
}