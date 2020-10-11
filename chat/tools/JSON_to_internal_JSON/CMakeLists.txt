cmake_minimum_required(VERSION 3.2)
project(JSON_to_internal_JSON)

set_target_properties(JSON_to_internal_JSON PROPERTIES CXX_STANDART 17)
set(CXX_STANDART 17)

find_package(nlohmann_json 3.2.0 REQUIRED)

#add_library(json ...)
add_executable(main main.cpp JSON_to_internal_JSON.cpp)
set_target_properties(main PROPERTIES CXX_STANDARD 17)
target_link_libraries(main PUBLIC nlohmann_json::nlohmann_json)
