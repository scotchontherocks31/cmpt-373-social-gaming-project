if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/json/json.hpp)
  file(DOWNLOAD 
    https://raw.githubusercontent.com/nlohmann/json/v3.9.1/single_include/nlohmann/json.hpp 
    ${CMAKE_CURRENT_BINARY_DIR}/json/json.hpp
  )
endif()

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/magic_enum/magic_enum.hpp)
  file(DOWNLOAD 
    https://raw.githubusercontent.com/Neargye/magic_enum/v0.7.0/include/magic_enum.hpp 
    ${CMAKE_CURRENT_BINARY_DIR}/magic_enum/magic_enum.hpp
  )
endif()

add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/json)

add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/magic_enum)
