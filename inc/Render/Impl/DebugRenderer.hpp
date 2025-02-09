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
	private:
		SDL_Renderer* _renderer;
		DrawingEngine& _engine;
	};
}