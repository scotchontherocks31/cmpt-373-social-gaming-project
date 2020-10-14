set(header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
)

add_library(JSON_to_internal_JSON JSON_to_internal_JSON.h)
set_target_properties(JSON_to_internal_JSON PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(JSON_to_internal_JSON INTERFACE nlohmann_json::nlohmann_json)
