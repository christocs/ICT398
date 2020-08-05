cmake_minimum_required(VERSION 3.16)

project(imgui)

if (WIN32)
    add_library(imgui STATIC)
else()
    add_library(imgui)
endif()

set_target_properties(imgui PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if(APPLE)
   find_library(APP_SERVICES_LIBRARY ApplicationServices)
   find_library(COCOA_LIBRARY Cocoa)
endif()

target_sources(imgui PRIVATE
    imgui/imgui.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/examples/imgui_impl_glfw.cpp
    imgui/examples/imgui_impl_opengl3.cpp
    $<$<PLATFORM_ID:Darwin>:imgui/examples/imgui_impl_osx.mm>
)

target_compile_definitions(imgui PRIVATE IMGUI_USER_CONFIG=\"${CMAKE_CURRENT_SOURCE_DIR}/imgui_config.h\")
target_include_directories(imgui PUBLIC . imgui)
target_link_libraries(imgui PRIVATE glfw glad ${APP_SERVICES_LIBRARY} ${COCOA_LIBRARY})
