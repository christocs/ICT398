# Set cmake minimum version.
cmake_minimum_required(VERSION 3.16)

# Set the project details.
set(PROJECT_NAME afk)
project(${PROJECT_NAME}
    VERSION 0.1.0
    LANGUAGES CXX
)

# Treat warnings as errors.
option(WarningsAsErrors "WarningsAsErrors" OFF)
# Clang sanitizer settings.
set(SANITIZER_OS "Darwin,Linux")
set(SANITIZER_FLAGS "-fsanitize=address,undefined,leak")
set(LIBCPP_OS "Linux")
# Disable in-source builds.
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

# Set build type to debug by default.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add Linux flag.
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# Set build output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG    ${PROJECT_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG    ${PROJECT_BINARY_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${PROJECT_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE  ${PROJECT_BINARY_DIR}/out/lib)

# Define the executable.
add_executable(${PROJECT_NAME})

# Set C++ standard and output directory.
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Add source files.
add_subdirectory(src)
# Add third party libraries.
add_subdirectory(lib)

# Remove the default warning level from MSVC.
if (MSVC)
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# Treat warnings as errors if enabled.
if (WarningsAsErrors)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
    )
endif()

# Set compile flags.
target_compile_options(${PROJECT_NAME} PRIVATE
    # Clang
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Weverything -fcolor-diagnostics
        # Disable unhelpful warnings.
        -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded
        -Wno-deprecated-declarations -Wno-weak-vtables
        -Wno-exit-time-destructors -Wno-global-constructors
        -Wno-c++2a-compat>
    # Visual Studio
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /MANIFEST:NO>
    # Enable the clang sanitizer.
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:${SANITIZER_OS}>>:${SANITIZER_FLAGS}>
    # Set default visibility to hidden.
    $<$<AND:$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>,$<PLATFORM_ID:Windows>>:
        -fvisibility=hidden -fvisibility-inlines-hidden>
)

# Link against the clang sanitizer.
target_link_options(${PROJECT_NAME} PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:${SANITIZER_OS}>>:${SANITIZER_FLAGS}>
)

# Find dependencies.
find_package(OpenGL COMPONENTS OpenGL REQUIRED)

# Include and link against dependencies.
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad
    EnTT::EnTT
    assimp
    reactphysics3d
    cpplocate
    lua
    LuaBridge
    glm
    stb
    imgui
    frozen::frozen
    ctti
    fastnoise-simd
    ozz_animation
    Recast
    Detour
)

# Symlink resources to the binary location.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/shader $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/cfg $<TARGET_FILE_DIR:${PROJECT_NAME}>/cfg)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/asset $<TARGET_FILE_DIR:${PROJECT_NAME}>/asset)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/script $<TARGET_FILE_DIR:${PROJECT_NAME}>/script)

# Setup git header.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/cmake-git-version-tracking)
set(PRE_CONFIGURE_FILE cmake/Git.hpp.in)
set(POST_CONFIGURE_FILE ${CMAKE_BINARY_DIR}/cmake/Git.hpp)
include(git_watcher)

# Setup version header.
configure_file(cmake/Version.hpp.in ${CMAKE_BINARY_DIR}/cmake/Version.hpp @ONLY)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
add_dependencies(${PROJECT_NAME} check_git)
