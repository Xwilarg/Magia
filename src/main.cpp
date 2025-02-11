#include <iostream>
#include <vector>

#include "BuiltinRenderer.hpp"
#include "DebugRenderer.hpp"
#include "DrawingEngine.hpp"
#include "InterpolationManager.hpp"
#include "ShortcutManager.hpp"
#include "config.hpp"

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
    Magia::ShortcutManager shortcutManager;

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

            case SDL_EVENT_KEY_DOWN:
                shortcutManager.HandleEvent(event.key.key, true);
                break;

            case SDL_EVENT_KEY_UP:
                shortcutManager.HandleEvent(event.key.key, false);
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (!engine.GetCanUseMouse()) continue;
                [[fallthrough]];
            case SDL_EVENT_PEN_DOWN:
                x = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? event.button.x : event.pbutton.x;
                y = event.type == SDL_EVENT_PEN_DOWN ? event.button.y : event.pbutton.y;

                if (x >= 0.0f && x <= CANVAS_WIDTH && y >= 0.0f && y <= WINDOW_HEIGHT) // We don't draw if we clicked outside the window
                {
                    isMousePressed = true;
                    interManager.AddPoint(x, y);
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (!engine.GetCanUseMouse()) continue;
                [[fallthrough]];
            case SDL_EVENT_PEN_UP:
                engine.ApplyPixels();
                interManager.Clear();
                isMousePressed = false;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                if (!engine.GetCanUseMouse()) continue;
                [[fallthrough]];
            case SDL_EVENT_PEN_MOTION:
                if (isMousePressed)
                {
                    x = event.type == SDL_EVENT_MOUSE_MOTION ? event.motion.x : event.pmotion.x;
                    y = event.type == SDL_EVENT_MOUSE_MOTION ? event.motion.y : event.pmotion.y;
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

        shortcutManager.ActivateShortcut(engine);

        float x, y;
        SDL_GetMouseState(&x, &y);
        engine.UpdateScreen(x, y);
        for (const auto& r : renderers)
        {
            r->Render();
        }
    }

    return 0;
}