#include <string>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "DebugRenderer.hpp"
#include "PngExporter.hpp"
#include "McfExporter.hpp"
#include "PaintBrush.hpp"
#include "config.hpp"

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
        ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        ImGui::SeparatorText("Brushes");
        auto currBrush = _engine.GetCurrentBrushIndex();
        int index = 0;
        for (auto&& name : _engine.GetBrushNames())
        {
            if (index > 0) ImGui::SameLine();
            if (index == currBrush) ImGui::BeginDisabled();
            if (ImGui::Button(name.c_str()))
            {
                _engine.SetCurrentBrushIndex(index);
            }
            if (index == currBrush) ImGui::EndDisabled();
            index++;
        }

        ImGui::SeparatorText("Brush Settings");
        auto brush = _engine.GetCurrentBrush();

        auto colorPen = dynamic_cast<PaintBrush*>(brush.get());

        if (colorPen != nullptr)
        {
            auto& engineC = colorPen->GetColor();
            float colors[4] = { engineC[0] / 255.0f, engineC[1] / 255.0f, engineC[2] / 255.0f, engineC[3] / 255.0f };
            ImGui::ColorPicker4("Color", colors, 0, 0);
            colorPen->SetColor(static_cast<int>(colors[0] * 255.0f), static_cast<int>(colors[1] * 255.0f), static_cast<int>(colors[2] * 255.0f), static_cast<int>(colors[3] * 255.0f));
        }

        int penSize = brush->GetPenSize();
        ImGui::SliderInt("Pen size", &penSize, 1, 1000);
        brush->SetPenSize(penSize);

        int penForce = brush->GetPenForce();
        ImGui::SliderInt("Pen force", &penForce, 1, 50);
        brush->SetPenForce(penForce);

        int drawDistance = brush->GetDrawDistance();
        ImGui::SliderInt("Draw distance", &drawDistance, 1, 50);
        brush->SetDrawDistance(drawDistance);

        int interpMode = brush->GetInterpolationMode();
        const char* interpModes[] = { "None", "Centripetal Catmull-Rom"};
        ImGui::Combo("Interpolation mode", &interpMode, interpModes, IM_ARRAYSIZE(interpModes));
        brush->SetInterpolationMode(static_cast<InterpolationMode>(interpMode));

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
            for (int row = _engine.GetLayers().size() - 1; row >= 0; row--)
            {
                auto layer = _engine.GetLayers()[row];

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

                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        ImGui::SeparatorText("Export");
        if (ImGui::Button("Save project"))
        {
            McfExporter expoter;
            expoter.Export("project.mcf", CANVAS_WIDTH, WINDOW_HEIGHT, _engine.GetLayers());
        }
        bool isBgTransparent = _engine.GetExportBackgroundColor() == TRANSPARENT_PIXEL;
        ImGui::Checkbox("Make export background transparent", &isBgTransparent);
        _engine.SetExportBackgroundColor(isBgTransparent ? TRANSPARENT_PIXEL : WHITE_PIXEL);
        if (ImGui::Button("Export to PNG"))
        {
            PngExporter expoter;
            expoter.Export("export.png", CANVAS_WIDTH, WINDOW_HEIGHT, _engine.GetFinalFramebuffer());
        }

        ImGui::SeparatorText("Config");

        bool useMouse = _engine.GetCanUseMouse();
        ImGui::Checkbox("Allow mouse", &useMouse);
        _engine.SetCanUseMouse(useMouse);

        int drawMode = _engine.GetDrawMode();
        const char* drawModes[] = { "Multiplicative" };
        ImGui::Combo("Draw mode", &drawMode, drawModes, IM_ARRAYSIZE(drawModes));
        _engine.SetDrawMode(static_cast<DrawMode>(drawMode));

        ImGui::SeparatorText("Debug");
        ImGui::Text("Framerate");
        ImGui::SameLine();
        ImGui::Text(std::to_string(ImGui::GetIO().Framerate).c_str());

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