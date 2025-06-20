set(IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_impl_sdl3.cpp
    imgui_impl_sdlgpu3.cpp
    imgui_impl_sdlrenderer3.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC {CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imgui PRIVATE SDL3::SDL3)