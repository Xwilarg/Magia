#pragma once

#include "SDL2/SDL.h"
#include "ARenderer.hpp"
#include "DrawingEngine.hpp"

namespace Magia
{
	class DebugRenderer: public ARenderer
	{
	public:
		DebugRenderer(SDL_Window* window, SDL_Renderer* renderer);
		~DebugRenderer();
		void PrepareRender() noexcept override;
		void Render() noexcept override;
		void ProcessEvents(SDL_Event& events) noexcept override;
	private:
		SDL_Renderer* _renderer;
	};
}