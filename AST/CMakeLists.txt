project(AST)
cmake_minimum_required(VERSION 3.12)

add_executable(AST
    ParserTest.cpp
)

FetchContent_Declare(
  json_download
  URL  https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
  DOWNLOAD_NO_EXTRACT TRUE
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)
FetchContent_MakeAvailable(json_download)

set_target_properties(AST
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
                      CMAKE_CXX_STANDARD_REQUIRED ON
                      CMAKE_BUILD_TYPE debug
                      CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20"
)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(AST nlohmann_json::nlohmann_json)