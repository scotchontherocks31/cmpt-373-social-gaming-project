#include "Room.h"
#include <iostream>

Room::Room(roomid id, std::string roomName)
    : name(std::move(roomName)), id(id) {}

void Room::listMembers() {
  for (auto &&p : members) {
    std::cout << p.second->getId() << std::endl;
  }
}
