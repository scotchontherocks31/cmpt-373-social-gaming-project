#include "Room.h"
#include <iostream>

Room::Room(roomid id, std::string roomName)
    : name(std::move(roomName)), id(id) {}

void Room::listParticipants() {
  for (std::pair<userid, std::reference_wrapper<User>> p : participants) {
    std::cout << p.second.get().getId() << std::endl;
  }
}
