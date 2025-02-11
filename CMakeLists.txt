cmake_minimum_required(VERSION 3.10)
project(Magia)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "/W4 /EHsc")

find_package(PNG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

include_directories(./inc/ ./inc/Render ./inc/Render/Impl ./inc/Engine ./inc/Export ./inc/Brush ./inc/Event)

add_executable(Magia
"src/main.cpp"
"src/Render/Impl/BuiltinRenderer.cpp" "src/Render/Impl/DebugRenderer.cpp"
"src/Engine/DrawingEngine.cpp" "src/Engine/DrawLayer.cpp" "src/Engine/InterpolationManager.cpp"
"src/Export/PngExporter.cpp" "src/Export/McfExporter.cpp"
"src/Brush/ABrush.cpp" "src/Brush/PaintBrush.cpp" "src/Brush/EraserBrush.cpp"
"src/Event/ShortcutManager.cpp")

target_link_libraries(Magia PRIVATE SDL3::SDL3 imgui::imgui PNG::PNG)