include("${CMAKE_CURRENT_LIST_DIR}/cmake/global-flags.cmake")
project(Cheat)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE IMGUI_SOURCES
        thirdparty/imgui/imgui.cpp
        thirdparty/imgui/imgui_draw.cpp
        thirdparty/imgui/imgui_widgets.cpp
        thirdparty/imgui/imgui_demo.cpp
        thirdparty/imgui/imgui_tables.cpp

        thirdparty/imgui/backends/imgui_impl_win32.cpp
        thirdparty/imgui/backends/imgui_impl_dx11.cpp
)

add_executable(Cheat ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(Cheat PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui")
target_link_libraries(Cheat PUBLIC ntdll.dll dwmapi.dll dxgi.dll d3d11.dll)
