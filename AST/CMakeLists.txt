add_library(AST ASTNode.cpp)
set_target_properties(AST
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
)
