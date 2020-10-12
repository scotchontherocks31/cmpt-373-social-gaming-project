cmake_minimum_required(VERSION 3.2)
project(JSON_to_internal_JSON)

set_target_properties(JSON_to_internal_JSON PROPERTIES CXX_STANDART 20)
set(CXX_STANDART 20)

add_executable(main main.cpp JSON_to_internal_JSON.cpp)
set_target_properties(main PROPERTIES CXX_STANDARD 20)
target_link_libraries(main PUBLIC nlohmann_json::nlohmann_json)
