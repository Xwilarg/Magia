#include "DebugRenderer.hpp"

#include "config.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

namespace Magia
{
	DebugRenderer::DebugRenderer(SDL_Window* window, SDL_Renderer* renderer, DrawingEngine& engine)
        : _renderer(renderer), _engine(engine)
	{
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);
	}

    DebugRenderer::~DebugRenderer() noexcept
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void DebugRenderer::PrepareRender() noexcept
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(CANVAS_WIDTH), 0.f));
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(WINDOW_WIDTH - CANVAS_WIDTH), static_cast<float>(WINDOW_HEIGHT)));
        ImGui::Begin("Pen", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        ImGui::SeparatorText("Pen");
        auto& engineC = _engine.GetColor();
        float colors[4] = { engineC[0] / 255.0f, engineC[1] / 255.0f, engineC[2] / 255.0f, engineC[3] / 255.0f };
        ImGui::ColorPicker4("Color", colors, 0, 0);
        _engine.SetColor(static_cast<int>(colors[0] * 255.0f), static_cast<int>(colors[1] * 255.0f), static_cast<int>(colors[2] * 255.0f), static_cast<int>(colors[3] * 255.0f));

        int penSize = _engine.GetPenSize();
        ImGui::SliderInt("Pen size", &penSize, 1, 1000);
        _engine.SetPenSize(penSize);

        int penForce = _engine.GetPenForce();
        ImGui::SliderInt("Pen force", &penForce, 1, 50);
        _engine.SetPenForce(penForce);

        int drawDistance = _engine.GetDrawDistance();
        ImGui::SliderInt("Draw distance", &drawDistance, 1, 50);
        _engine.SetDrawDistance(drawDistance);

        int drawMode = _engine.GetDrawMode();
        const char* drawModes[] = { "Multiplicative" };
        ImGui::Combo("Draw mode", &drawMode, drawModes, IM_ARRAYSIZE(drawModes));
        _engine.SetDrawMode(static_cast<DrawMode>(drawMode));

        ImGui::SeparatorText("Layers");
        if (ImGui::Button("Add"))
        {
            _engine.AddNewLayer();
        }
        if (ImGui::BeginTable("tableLayers", 4))
        {
            ImGui::TableSetupColumn("col1", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("col2", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("col3", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("col4", ImGuiTableColumnFlags_WidthFixed);

            int currentLayer = _engine.GetSelectedLayerIndex();
            int row = 0;
            for (const auto& layer : _engine.GetLayers())
            {
                ImGui::PushID(row);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                bool isActive = layer->GetActive();
                ImGui::Checkbox("##active", &isActive);
                layer->SetActive(isActive);

                ImGui::TableSetColumnIndex(1);
                if (row == currentLayer) ImGui::BeginDisabled();
                if (ImGui::Button("Go"))
                {
                    _engine.SetSelectedLayerIndex(row);
                }
                if (row == currentLayer) ImGui::EndDisabled();

                ImGui::TableSetColumnIndex(2);
                ImGui::InputText("##name", layer->GetName(), 50);

                ImGui::TableSetColumnIndex(3);
                auto layerCount = _engine.GetLayers().size();
                if (layerCount == 1) ImGui::BeginDisabled();
                if (ImGui::Button("Del"))
                {
                    _engine.RemoveLayer(row);
                }
                if (layerCount == 1) ImGui::EndDisabled();

                row++;
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        ImGui::End();

        ImGui::Render();
    }

    void DebugRenderer::Render() noexcept
    {
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);
    }

    void DebugRenderer::ProcessEvents(SDL_Event& events) noexcept
    {
        ImGui_ImplSDL3_ProcessEvent(&events);
    }
}