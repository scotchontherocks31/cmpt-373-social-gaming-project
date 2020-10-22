FetchContent_Declare(
  json_download
  URL  https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
  DOWNLOAD_NO_EXTRACT TRUE
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)
FetchContent_MakeAvailable(json_download)
