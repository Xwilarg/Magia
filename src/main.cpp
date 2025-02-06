#include <iostream>
#include <vector>

#include "BuiltinRenderer.hpp"
#include "DebugRenderer.hpp"
#include "DrawingEngine.hpp"

#undef main

int main()
{
    std::vector<std::unique_ptr<Magia::ARenderer>> renderers;
    auto baseRenderer = std::make_unique<Magia::BuiltinRenderer>();

    Magia::DrawingEngine engine(baseRenderer->GetRenderer());
    renderers.push_back(std::make_unique<Magia::DebugRenderer>(baseRenderer->GetWindow(), baseRenderer->GetRenderer(), engine));
    renderers.push_back(std::move(baseRenderer));

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
            switch (event.type)
            {
            case SDL_QUIT:
                isActive = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
                isMousePressed = true;
                break;

            case SDL_MOUSEBUTTONUP:
                isMousePressed = false;
                break;

            default:
                break;
            }
        }

        if (isMousePressed)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
            engine.Paint(x, y);
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

        SDL_Delay(10);
    }

    return 0;
}