cmake_minimum_required(VERSION 3.16)

function(target_ignore_header_warnings target scope)
    get_target_property(lib_include_dirs ${target} INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${target} SYSTEM ${scope} ${lib_include_dirs})
endfunction()

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
# ozz-animation
option(ozz_build_tools "Build tools" OFF)
option(ozz_build_fbx "Build Fbx pipeline (Requires Fbx SDK)" ON)
option(ozz_build_gltf "Build glTF importer (Requires c++11)" ON)
option(ozz_build_samples "Build samples" OFF)
option(ozz_build_howtos "Build howtos" OFF)
option(ozz_build_tests "Build unit tests" OFF)
option(ozz_build_postfix "Use per config postfix name" OFF)
#  recastnavigation
option(RECASTNAVIGATION_DEMO "Build demo" OFF)
option(RECASTNAVIGATION_TESTS "Build tests" OFF)
option(RECASTNAVIGATION_EXAMPLES "Build examples" OFF)
option(RECASTNAVIGATION_STATIC "Build static libraries" ON)

# Fixes an entt warning on UNIX systems.
if (UNIX)
    option(USE_LIBCPP OFF)
endif()

# Build libraries.
string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Disable compiler warnings.
add_compile_options(
    # Clang
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-everything>
    # GCC
    $<$<CXX_COMPILER_ID:GNU>:-w>
    # Visual Studio
    $<$<CXX_COMPILER_ID:MSVC>:/W0>
)

# Include our dependencies.
add_subdirectory(entt)
add_subdirectory(assimp)
add_subdirectory(cpplocate)
add_subdirectory(reactphysics3d)
add_subdirectory(lua)
add_subdirectory(lua-bridge)
add_subdirectory(glm)
add_subdirectory(stb)
add_subdirectory(glfw)
add_subdirectory(glad)
add_subdirectory(imgui)
add_subdirectory(fastnoise-simd)
add_subdirectory(frozen)
add_subdirectory(ctti)
add_subdirectory(ozz_animation)
add_subdirectory(recastnavigation)

# DLLs were a mistake.
set_target_properties(reactphysics3d PROPERTIES ENABLE_EXPORTS ON)
set_target_properties(reactphysics3d PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Stop the *helpful* practise of postfixing 'd' onto debug libraries.
set_target_properties(assimp PROPERTIES DEBUG_POSTFIX "")
set_target_properties(cpplocate PROPERTIES DEBUG_POSTFIX "")
set_target_properties(reactphysics3d PROPERTIES DEBUG_POSTFIX "")
set_target_properties(lua PROPERTIES DEBUG_POSTFIX "")
set_target_properties(glfw PROPERTIES DEBUG_POSTFIX "")
set_target_properties(glad PROPERTIES DEBUG_POSTFIX "")
set_target_properties(imgui PROPERTIES DEBUG_POSTFIX "")
set_target_properties(ozz_animation PROPERTIES DEBUG_POSTFIX "")

# Ignore header warnings in our libraries.
target_ignore_header_warnings(EnTT INTERFACE)
target_ignore_header_warnings(assimp INTERFACE)
target_ignore_header_warnings(reactphysics3d INTERFACE)
target_ignore_header_warnings(cpplocate INTERFACE)
target_ignore_header_warnings(lua INTERFACE)
target_ignore_header_warnings(LuaBridge INTERFACE)
target_ignore_header_warnings(stb INTERFACE)
target_ignore_header_warnings(glm INTERFACE)
target_ignore_header_warnings(glfw INTERFACE)
target_ignore_header_warnings(glad INTERFACE)
target_ignore_header_warnings(imgui INTERFACE)
target_ignore_header_warnings(frozen INTERFACE)
target_ignore_header_warnings(ctti INTERFACE)
target_ignore_header_warnings(ctti INTERFACE)
target_ignore_header_warnings(ozz_animation INTERFACE)
target_ignore_header_warnings(Recast INTERFACE)
target_ignore_header_warnings(Detour INTERFACE)
target_ignore_header_warnings(DetourCrowd INTERFACE)
