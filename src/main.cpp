#include <iostream>
#include <vector>

#include "BuiltinRenderer.hpp"
#include "DebugRenderer.hpp"
#include "DrawingEngine.hpp"
#include "InterpolationManager.hpp"

#undef main

int main()
{
    std::vector<std::unique_ptr<Magia::ARenderer>> renderers;
    auto baseRenderer = std::make_unique<Magia::BuiltinRenderer>();
    Magia::DrawingEngine engine(baseRenderer->GetRenderer());
    auto debugRenderer = std::make_unique<Magia::DebugRenderer>(baseRenderer->GetWindow(), baseRenderer->GetRenderer(), engine);

    renderers.push_back(std::move(debugRenderer));
    renderers.push_back(std::move(baseRenderer));

    Magia::InterpolationManager interManager(engine);

    bool isActive = true;
    bool isMousePressed = false;

    while (isActive)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            for (const auto& r : renderers)
            {
                r->ProcessEvents(event);
            }
            float x, y;
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                isActive = false;
                break;

            case SDL_EVENT_PEN_DOWN:
                std::cout << "Pen down received" << std::endl;
                isMousePressed = true;
                SDL_GetMouseState(&x, &y);
                interManager.AddPoint(x, y);
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                std::cout << "Mouse down received" << std::endl;
                isMousePressed = true;
                SDL_GetMouseState(&x, &y);
                interManager.AddPoint(x, y);
                break;

            case SDL_EVENT_PEN_UP:
                std::cout << "Pen up received" << std::endl;
                engine.ApplyPixels();
                interManager.Clear();
                isMousePressed = false;
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                std::cout << "Pen down received" << std::endl;
                engine.ApplyPixels();
                interManager.Clear();
                isMousePressed = false;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                if (isMousePressed)
                {
                    SDL_GetMouseState(&x, &y);
                    interManager.AddPoint(x, y);
                }
                break;

            default:
                break;
            }
        }

        for (const auto& r : renderers)
        {
            r->PrepareRender();
        }
        engine.UpdateScreen();
        for (const auto& r : renderers)
        {
            r->Render();
        }
    }

    return 0;
}