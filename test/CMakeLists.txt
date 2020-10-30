enable_testing()

add_executable(runAllTests
    ASTTests.cpp
)

set_target_properties(runAllTests 
    PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
)


target_link_libraries(runAllTests gtest gmock gtest_main AST  )

add_test(NAME AllTests COMMAND runAllTests)
