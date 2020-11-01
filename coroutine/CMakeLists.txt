add_library(task INTERFACE)
target_include_directories(task INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

