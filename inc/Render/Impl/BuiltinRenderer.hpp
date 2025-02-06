#pragma once

#include "SDL2/SDL.h"
#include "ARenderer.hpp"

namespace Magia
{
	class BuiltinRenderer: public ARenderer
	{
	public:
		BuiltinRenderer();
		~BuiltinRenderer() noexcept;
		void PrepareRender() noexcept override;
		void Render() noexcept override;
		void ProcessEvents(SDL_Event& events) noexcept override;

		SDL_Window* GetWindow();
		SDL_Renderer* GetRenderer();
	private:
		SDL_Window* _window;
		SDL_Renderer* _renderer;
	};
}