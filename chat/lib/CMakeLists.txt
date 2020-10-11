add_subdirectory(networking)
add_subdirectory(JSON_to_internal_JSON_lib)

add_library(json json.hpp)
target_link_libraries(json nlohmann_json::nlohmann_json)

add_library(JSON_to_internal_JSON JSON_to_internal_JSON.h)
target_link_libraries(JSON_to_internal_JSON nlohmann_json::nlohmann_json)
