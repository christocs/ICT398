cmake_minimum_required(VERSION 3.16)

add_library(glad)

target_sources(glad PRIVATE
    src/glad.cpp
)

#set_target_properties(glad PROPERTIES ENABLE_EXPORTS ON)
#set_target_properties(glad PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
#target_compile_definitions(glad PUBLIC GLAD_GLAPI_EXPORT PRIVATE GLAD_GLAPI_EXPORT_BUILD)
target_include_directories(glad PUBLIC include)
