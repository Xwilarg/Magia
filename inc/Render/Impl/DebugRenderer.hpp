#pragma once

#include <chrono>
#include "SDL3/SDL.h"
#include "ARenderer.hpp"
#include "DrawingEngine.hpp"

namespace Magia
{
	class DebugRenderer: public ARenderer
	{
	public:
		DebugRenderer(SDL_Window* window, SDL_Renderer* renderer, DrawingEngine& engine);
		~DebugRenderer() noexcept;
		void PrepareRender() noexcept override;
		void Render() noexcept override;
		void ProcessEvents(SDL_Event& events) noexcept override;
		void SaveToPng(const char* const* filelist, int filter);
		void SaveToMcf(const char* const* filelist, int filter);
		void OpenFromMcf(const char* const* filelist, int filter);
		void OpenFromPng(const char* const* filelist, int filter);
		bool GetPendingImport() const noexcept;
	private:
		SDL_Window* _window;
		SDL_Renderer* _renderer;
		DrawingEngine& _engine;
		bool _isPendingImport;

		float _lowestFps;
	};

	void forwardToSaveToPng(void* userdata, const char* const* filelist, int filter);
	void forwardToSaveToMcf(void* userdata, const char* const* filelist, int filter);
	void forwardToSaveToMcf(void* userdata, const char* const* filelist, int filter);
	void forwardToOpenFromMcf(void* userdata, const char* const* filelist, int filter);
}