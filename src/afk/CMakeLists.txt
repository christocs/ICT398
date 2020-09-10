target_sources(${PROJECT_NAME} PRIVATE
    Afk.cpp
)

add_subdirectory(config)
add_subdirectory(debug)
add_subdirectory(ecs)
add_subdirectory(event)
add_subdirectory(io)
add_subdirectory(physics)
add_subdirectory(prefab)
add_subdirectory(render)
add_subdirectory(scene)
add_subdirectory(ui)
