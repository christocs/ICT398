cmake_minimum_required(VERSION 3.16)

function(target_ignore_header_warnings target scope)
    get_target_property(lib_include_dirs ${target} INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${target} SYSTEM ${scope} ${lib_include_dirs})
endfunction()

# Build only static libraries.
option(BUILD_SHARED_LIBS OFF)

# Assimp
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
# cpplocate
option(OPTION_BUILD_TESTS OFF)
# glfw
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_USE_HYBRID_HPG ON)
# JSON.
set(JSON_BuildTests OFF CACHE INTERNAL "")

# Fixes an entt warning on UNIX systems.
if (UNIX)
    option(USE_LIBCPP OFF)
endif()

# Remove the default warning level in MSVC.
string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Disable compiler warnings.
add_compile_options(
    # Clang
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-w>
    # Visual Studio
    $<$<CXX_COMPILER_ID:MSVC>:/W0>
)

# Include our dependencies.
add_subdirectory(entt EXCLUDE_FROM_ALL)
add_subdirectory(assimp EXCLUDE_FROM_ALL)
add_subdirectory(cpplocate EXCLUDE_FROM_ALL)
add_subdirectory(reactphysics3d EXCLUDE_FROM_ALL)
add_subdirectory(glm EXCLUDE_FROM_ALL)
add_subdirectory(stb EXCLUDE_FROM_ALL)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
add_subdirectory(glad EXCLUDE_FROM_ALL)
add_subdirectory(imgui EXCLUDE_FROM_ALL)
add_subdirectory(frozen EXCLUDE_FROM_ALL)
add_subdirectory(ctti EXCLUDE_FROM_ALL)
add_subdirectory(json EXCLUDE_FROM_ALL)

# Ignore header warnings in our libraries.
target_ignore_header_warnings(EnTT INTERFACE)
target_ignore_header_warnings(assimp INTERFACE)
target_ignore_header_warnings(reactphysics3d INTERFACE)
target_ignore_header_warnings(cpplocate INTERFACE)
target_ignore_header_warnings(stb INTERFACE)
target_ignore_header_warnings(glm INTERFACE)
target_ignore_header_warnings(glfw INTERFACE)
target_ignore_header_warnings(glad INTERFACE)
target_ignore_header_warnings(imgui INTERFACE)
target_ignore_header_warnings(frozen INTERFACE)
target_ignore_header_warnings(ctti INTERFACE)
target_ignore_header_warnings(nlohmann_json INTERFACE)
