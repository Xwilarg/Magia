#include <string>
#include <functional>

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
    void forwardToSaveToPng(void* userdata, const char* const* filelist, int filter)
    {
        static_cast<DebugRenderer*>(userdata)->SaveToPng(filelist, filter);
    }

    void forwardToSaveToMcf(void* userdata, const char* const* filelist, int filter)
    {
        static_cast<DebugRenderer*>(userdata)->SaveToMcf(filelist, filter);
    }

    void forwardToOpenFromMcf(void* userdata, const char* const* filelist, int filter)
    {
        static_cast<DebugRenderer*>(userdata)->OpenFromMcf(filelist, filter);
    }

	DebugRenderer::DebugRenderer(SDL_Window* window, SDL_Renderer* renderer, DrawingEngine& engine)
        : _window(window), _renderer(renderer), _engine(engine), _isPendingImport(false), _lowestFps(std::numeric_limits<float>::max())
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

    void DebugRenderer::SaveToPng(const char* const* filelist, int filter)
    {
        if (!filelist || !*filelist)
        {
            _isPendingImport = false;
            return;
        }

        auto target = std::string(*filelist);
        if (!target.ends_with(".png")) target += ".png";

        PngExporter exporter;
        auto size = _engine.GetCanvasSize();
        auto pixels = _engine.GetFinalFramebuffer();
        exporter.Export(target, size.X, size.Y, &pixels.front());
        _isPendingImport = false;
    }

    void DebugRenderer::SaveToMcf(const char* const* filelist, int filter)
    {
        if (!filelist || !*filelist)
        {
            _isPendingImport = false;
            return;
        }

        auto target = std::string(*filelist);
        if (!target.ends_with(".mcf")) target += ".mcf";

        McfExporter exporter;
        auto size = _engine.GetCanvasSize();
        exporter.Export(target, size.X, size.Y, _engine.GetLayers());
        _isPendingImport = false;
    }

    void DebugRenderer::OpenFromMcf(const char* const* filelist, int filter)
    {
        if (!filelist || !*filelist)
        {
            _isPendingImport = false;
            return;
        }

        McfExporter expoter;
        expoter.Import(std::string(*filelist), _engine.GetLayers());
        _isPendingImport = false;

        _engine.DirtyScreen();
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
        ImGui::SliderInt("Pen size", &penSize, 2, 1000);
        brush->SetPenSize(penSize);

        int penForce = brush->GetPenForce();
        bool isForceEnabled = penForce < 100;
        ImGui::Checkbox("##EnableForce", &isForceEnabled);
        if (!isForceEnabled) ImGui::BeginDisabled();
        ImGui::SameLine(); ImGui::SliderInt("Pen force", &penForce, 1, 50);
        if (!isForceEnabled) ImGui::EndDisabled();
        brush->SetPenForce(isForceEnabled ? (penForce == 100 ? 50 : penForce) : 100);

        int drawDistance = brush->GetDrawDistance();
        ImGui::SliderInt("Draw distance", &drawDistance, 1, 50);
        brush->SetDrawDistance(drawDistance);

        int interpMode = brush->GetInterpolationMode();
        const char* interpModes[] = { "None", "Centripetal Catmull-Rom"};
        ImGui::Combo("Interpolation mode", &interpMode, interpModes, IM_ARRAYSIZE(interpModes));
        brush->SetInterpolationMode(static_cast<InterpolationMode>(interpMode));

        ImGui::SeparatorText("History");
        auto currIndex = _engine.GetCurrentHistoryIndex();
        auto maxIndex = _engine.GetHistoryCount();
        if (currIndex == 0) ImGui::BeginDisabled();
        if (ImGui::Button("Undo"))
        {
            _engine.Undo();
        }
        if (currIndex == 0) ImGui::EndDisabled();
        ImGui::SameLine();
        if (currIndex == maxIndex) ImGui::BeginDisabled();
        if (ImGui::Button("Redo"))
        {
            _engine.Redo();
        }
        if (currIndex == maxIndex) ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::Text((std::to_string(currIndex) + " / " + std::to_string(maxIndex)).c_str());

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
                bool oldActive = isActive;
                ImGui::Checkbox("##active", &isActive);
                layer->SetActive(isActive);
                if (isActive != oldActive) // TODO: Do that in DrawingEngine instead of here (check if value changed)
                    _engine.RedrawLayerCache();

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

        ImGui::SeparatorText("Movements");
        ImGui::Dummy(ImVec2());
        ImGui::SameLine(56);
        if (ImGui::Button("Up", ImVec2(40, 20))) _engine.MoveCanvas(0, 1);
        if (ImGui::Button("Left", ImVec2(40, 20))) _engine.MoveCanvas(1, 0);
        ImGui::SameLine(); if (ImGui::Button("Down", ImVec2(40, 20))) _engine.MoveCanvas(0, -1);
        ImGui::SameLine(); if (ImGui::Button("Right", ImVec2(40, 20))) _engine.MoveCanvas(-1, 0);

        int offsetSpeed = _engine.GetOffsetMoveSpeed();
        ImGui::SliderInt("Move speed", &offsetSpeed, 1, 100);
        _engine.SetOffsetMoveSpeed(offsetSpeed);

        ImGui::SeparatorText("Export");
        if (ImGui::Button("Save project"))
        {
            const SDL_DialogFileFilter filters[] = {
                { "Compressed Magia project file",  "mcf" }
            };
            SDL_ShowSaveFileDialog(forwardToSaveToMcf, this, _window, filters, 1, nullptr);
            _isPendingImport = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Load project"))
        {
            const SDL_DialogFileFilter filters[] = {
                { "Compressed Magia project file",  "mcf" }
            };
            SDL_ShowOpenFileDialog(forwardToOpenFromMcf, this, _window, filters, 1, nullptr, false);
            _isPendingImport = true;
        }
        bool isBgOpaque = _engine.GetExportBackgroundColor() != TRANSPARENT_PIXEL;
        ImGui::Checkbox("Export background", &isBgOpaque);
        _engine.SetExportBackgroundColor(isBgOpaque ? WHITE_PIXEL : TRANSPARENT_PIXEL);
        if (ImGui::Button("Export to PNG"))
        {
            const SDL_DialogFileFilter filters[] = {
                { "PNG image",  "png" }
            };
            SDL_ShowSaveFileDialog(forwardToSaveToPng, this, _window, filters, 1, nullptr);
            _isPendingImport = true;
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
        auto framerate = ImGui::GetIO().Framerate;
        std::stringstream streamFps;
        streamFps << std::fixed << std::setprecision(2) << framerate;
        std::string fps = streamFps.str();
        ImGui::Text(fps.c_str());
        if (framerate < _lowestFps)
        {
            _lowestFps = framerate;
        }
        ImGui::SameLine();
        ImGui::Text("Lowest");
        ImGui::SameLine();
        std::stringstream streamLowestFps;
        streamLowestFps << std::fixed << std::setprecision(2) << _lowestFps;
        std::string lowestFps = streamLowestFps.str();
        ImGui::Text(lowestFps.c_str());
        if (ImGui::Button("Reset"))
        {
            _lowestFps = framerate;
        }
        if (ImGui::Button("Dirty canvas"))
        {
            _engine.DirtyScreen();
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

    bool DebugRenderer::GetPendingImport() const noexcept
    {
        return _isPendingImport;
    }
}