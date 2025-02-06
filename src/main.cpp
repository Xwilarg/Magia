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

    int lastX, lastY;
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
                SDL_GetMouseState(&lastX, &lastY);
                engine.Paint(lastX, lastY);
                break;

            case SDL_MOUSEBUTTONUP:
                isMousePressed = false;
                break;

            case SDL_MOUSEMOTION:
                if (isMousePressed)
                {
                    int newX, newY;
                    SDL_GetMouseState(&newX, &newY);

                    // Intepolate between last pos and current one
                    int dist = static_cast<int>(std::sqrt(std::pow(newX - lastX, 2) + std::pow(newY - lastY, 2)));
                    for (int d = 0; d <= dist; d++)
                    {
                        int cx = static_cast<int>(std::lerp(lastX, newX, d / static_cast<float>(dist)));
                        int cy = static_cast<int>(std::lerp(lastY, newY, d / static_cast<float>(dist)));
                        engine.Paint(cx, cy);
                    }

                    lastX = newX;
                    lastY = newY;
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

        SDL_Delay(10);
    }

    return 0;
}