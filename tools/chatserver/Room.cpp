#include "Room.h"

Room::Room(roomid id, std::string roomName)
    : name(std::move(roomName)), id(id) {}
