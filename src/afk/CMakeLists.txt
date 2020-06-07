cmake_minimum_required(VERSION 3.16)

target_sources(${PROJECT_NAME} PRIVATE
    Afk.cpp
)

add_subdirectory(component)
add_subdirectory(event)
add_subdirectory(io)
add_subdirectory(physics)
add_subdirectory(renderer)
add_subdirectory(script)
add_subdirectory(debug)
add_subdirectory(asset)
add_subdirectory(terrain)
add_subdirectory(ui)
add_subdirectory(ai)
