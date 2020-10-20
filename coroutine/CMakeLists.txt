set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/task.h
)
add_library(task INTERFACE)
target_sources(task INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_include_directories(task INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

