add_executable(AST 
  ASTNode.cpp 
  ASTVisitor.cpp
  Parser.cpp 
  ParserTest.cpp
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sample.json
     DESTINATION ${CMAKE_BINARY_DIR})

set_target_properties(AST
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)

target_link_libraries(AST 
    PUBLIC
    json
    task)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGCC=1")
set(CMAKE_CXX_FLAGS_DEBUG 
    "${CMAKE_CXX_FLAGS_DEBUG} -DGCC=1"
)

