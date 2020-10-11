cmake_minimum_required(VERSION 3.12)
project(WebSocketNetworking)

set(PACKAGE_NAME WebSocketNetworking)
set(PACKAGE_VERSION 0.2)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "wsumner@sfu.ca")
find_package(nlohmann_json 3.2.0 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG 
    "-O0 -g -fsanitize=address,undefined -fno-omit-frame-pointer"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")


add_subdirectory(lib)
add_subdirectory(tools)
add_library(json INTERFACE [IMPORTED [GLOBAL]])
target_link_libraries(json PRIVATE nlohmann_json::nlohmann_json)



#cmake_minimum_required(VERSION 3.2)
#project(JSON_to_internal_JSON)

#set_target_properties(JSON_to_internal_JSON PROPERTIES CXX_STANDART 17)
#set(CXX_STANDART 17)

#add_executable(main main.cpp JSON_to_internal_JSON.cpp)
#set_target_properties(main PROPERTIES CXX_STANDARD 17)
#target_link_libraries(json_lib PRIVATE nlohmann_json::nlohmann_json)
