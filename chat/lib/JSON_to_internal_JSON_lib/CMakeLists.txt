set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/json.hpp
)
find_package(nlohmann_json 3.2.0 REQUIRED)


add_library(json INTERFACE)
target_sources(json INTERFACE "$<BUILD_INTERFACE:${header_files}>")
target_include_directories(json INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/JSON_to_internal_JSON/>)


add_library(JSON_to_internal_JSON JSON_to_internal_JSON.h)
target_link_libraries(JSON_to_internal_JSON nlohmann_json::nlohmann_json)
