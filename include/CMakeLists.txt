if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/json/json.hpp)
  file(DOWNLOAD 
    https://raw.githubusercontent.com/nlohmann/json/v3.9.1/single_include/nlohmann/json.hpp 
    ${CMAKE_CURRENT_BINARY_DIR}/json/json.hpp
  )
endif()

add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/json)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(googletest)