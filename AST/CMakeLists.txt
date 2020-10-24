add_executable(AST 
  ASTNode.cpp 
  ASTVisitor.cpp
  Parser.cpp 
  ParserTest.cpp
)

set_target_properties(AST
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)
target_include_directories(AST INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(AST 
    task json
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGCC=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DGCC=1")

