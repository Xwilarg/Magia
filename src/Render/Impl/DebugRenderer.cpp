#include "DebugRenderer.hpp"

#include "config.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

namespace Magia
{
	DebugRenderer::DebugRenderer(SDL_Window* window, SDL_Renderer* renderer, DrawingEngine& engine)
        : _renderer(renderer), _engine(engine)
	{
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer2_Init(renderer);
	}

    DebugRenderer::~DebugRenderer() noexcept
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void DebugRenderer::PrepareRender() noexcept
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void DebugRenderer::Render() noexcept
    {
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(CANVAS_WIDTH), 0.f));
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(WINDOW_WIDTH - CANVAS_WIDTH), static_cast<float>(WINDOW_HEIGHT)));
        ImGui::Begin("Pen", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        auto& engineC = _engine.GetColor();
        float colors[4] = { engineC[0] / 255.0f, engineC[1] / 255.0f, engineC[2] / 255.0f, engineC[3] / 255.0f };
        ImGui::ColorPicker4("Color", colors, 0, 0);
        _engine.SetColor(static_cast<int>(colors[0] * 255.0f), static_cast<int>(colors[1] * 255.0f), static_cast<int>(colors[2] * 255.0f), static_cast<int>(colors[3] * 255.0f));

        int penSize = _engine.GetPenSize();
        ImGui::SliderInt("Pen size", &penSize, 1, 100);
        _engine.SetPenSize(penSize);

        int penForce = _engine.GetPenForce();
        ImGui::SliderInt("Pen force", &penForce, 1, 100);
        _engine.SetPenForce(penForce);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), _renderer);
    }

    void DebugRenderer::ProcessEvents(SDL_Event& events) noexcept
    {
        ImGui_ImplSDL2_ProcessEvent(&events);
    }
}