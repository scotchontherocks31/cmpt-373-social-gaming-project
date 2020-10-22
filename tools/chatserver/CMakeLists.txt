
add_executable(chatserver
  chatserver.cpp GameServer.cpp Room.cpp RoomManager.cpp GameManager.cpp GameHandler.cpp
)

set_target_properties(chatserver
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
                      PREFIX ""
)

find_package(Threads REQUIRED)

target_link_libraries(chatserver
  networking AST json magic_enum
  ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS chatserver
  RUNTIME DESTINATION bin
)

