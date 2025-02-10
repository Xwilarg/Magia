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

    float lastX, lastY;
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
            case SDL_EVENT_QUIT:
                isActive = false;
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                isMousePressed = true;
                float x, y;
                SDL_GetMouseState(&x, &y);
                interManager.AddPoint(x, y);
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                engine.ApplyPixels();
                isMousePressed = false;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                if (isMousePressed)
                {
                    float newX, newY;
                    SDL_GetMouseState(&newX, &newY);

                    if (std::sqrt(std::pow(lastX - newX, 2) + std::pow(lastY - newY, 2)) >= engine.GetDrawDistance())
                    {
                        // Intepolate between last pos and current one
                        int dist = static_cast<int>(std::sqrt(std::pow(newX - lastX, 2) + std::pow(newY - lastY, 2)));
                        for (int d = 0; d <= dist; d += engine.GetDrawDistance())
                        {
                            int cx = static_cast<int>(std::lerp(lastX, newX, d / static_cast<float>(dist)));
                            int cy = static_cast<int>(std::lerp(lastY, newY, d / static_cast<float>(dist)));
                            engine.Paint(cx, cy);
                        }

                        lastX = newX;
                        lastY = newY;
                    }

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