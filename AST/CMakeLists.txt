add_library(AST ASTNode.cpp ASTVisitor.cpp Parser.cpp DSLValue.cpp Environment.cpp Config.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sample.json
     DESTINATION ${CMAKE_BINARY_DIR})

set_target_properties(AST
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)
target_include_directories(AST INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(AST 
    task json
)
