set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(include)

set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/json.hpp
)
add_library(json INTERFACE)
target_sources(json INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_include_directories(json INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
