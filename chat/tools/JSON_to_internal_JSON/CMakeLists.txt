cmake_minimum_required(VERSION 3.2)
project(JSON_to_internal_JSON)

set_target_properties(JSON_to_internal_JSON PROPERTIES CXX_STANDART 17)
set(CXX_STANDART 17)

add_library(json INTERFACE [IMPORTED [GLOBAL]])
add_executable(main main.cpp JSON_to_internal_JSON.cpp)
set_target_properties(main PROPERTIES CXX_STANDARD 17)
target_link_libraries(json PRIVATE nlohmann_json::nlohmann_json)
