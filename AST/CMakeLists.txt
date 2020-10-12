project(AST)

cmake_minimum_required(VERSION 3.18)

add_library(AST ASTNode.cpp)
set_target_properties(AST
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(AST nlohmann_json::nlohmann_json)