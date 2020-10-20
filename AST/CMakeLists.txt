add_executable(AST 
  ASTNode.cpp 
  Parser.cpp 
  ParserTest.cpp
)

set_target_properties(AST
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(AST 
    PUBLIC
    nlohmann_json::nlohmann_json
    task)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGCC=1")
set(CMAKE_CXX_FLAGS_DEBUG 
    "${CMAKE_CXX_FLAGS_DEBUG} -DGCC=1"
)

install(TARGETS AST
  RUNTIME DESTINATION bin
)
