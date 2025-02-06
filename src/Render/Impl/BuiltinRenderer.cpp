#include <stdexcept>

#include "BuiltinRenderer.hpp"
#include "config.hpp"

namespace Magia
{
	BuiltinRenderer::BuiltinRenderer()
	{
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            throw std::runtime_error("SDL failed to init: " + std::string(SDL_GetError()));
        }

        SDL_CreateWindowAndRenderer("Magia", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &_window, &_renderer);
        if (!_window || !_renderer)
        {
            throw std::runtime_error("Window/Renderer failed to init: " + std::string(SDL_GetError()));
        }
	}

    BuiltinRenderer::~BuiltinRenderer()
    {
        SDL_Quit();
    }

    void BuiltinRenderer::PrepareRender() noexcept
    {
        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(_renderer);
    }

    void BuiltinRenderer::Render() noexcept
    {
        SDL_RenderPresent(_renderer);
    }

    void BuiltinRenderer::ProcessEvents(SDL_Event& events) noexcept
    { }

    SDL_Window* BuiltinRenderer::GetWindow()
    {
        return _window;
    }

    SDL_Renderer* BuiltinRenderer::GetRenderer()
    {
        return _renderer;
    }
}