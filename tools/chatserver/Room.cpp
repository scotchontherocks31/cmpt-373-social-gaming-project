#include "Room.h"

Room::Room(int id, std::string roomName) : roomName(roomName), id(id) {}

RoomManager::RoomManager() {
  rooms.insert({roomCounter, Room{roomCounter, "Global"}});
  ++roomCounter;
}

Room& RoomManager::createRoom(std::string name){
  if (name == "")
    name =  "Room" + roomCounter;
  rooms.insert({roomCounter, Room{roomCounter, name}});
  return rooms.at(roomCounter++);
}

void RoomManager::removeRoom(int id){
  // Should not remove global room
  if (!rooms.count(id) || id == 0) return;
  auto& room = rooms.at(id);
  while (!room.participants.empty()) {
    auto it = room.participants.begin();
    putUserToRoom(**it, 0); // Put user to global room
  }
  rooms.erase(id);
}

/// Put user in specified room, switch room if needed.
/// Return true if successfully put user in or user already in the room.
bool RoomManager::putUserToRoom(User &user, int roomNumber) {
  if (!rooms.count(roomNumber)) return false;
  auto &room = rooms.at(roomNumber);

  // Check if user already in target room
  if (room.participants.count(&user)) {
    return true;
  }

  // Check if user is in any other room
  if (userRoomMapping.count(user.connection.id)) {
    removeUserFromRoom(user);
  }

  auto [_, result] = room.participants.insert(&user);
  if (result) {
    userRoomMapping[user.connection.id] = roomNumber;
  }
  return result;

}

/// Remove user from any room.
void RoomManager::removeUserFromRoom(User &user) {
  auto userId = user.connection.id;
  if (userRoomMapping.count(userId)) {
    auto roomNumber = userRoomMapping.at(userId);
    rooms.at(roomNumber).participants.erase(&user);
    userRoomMapping.erase(userId);
  }
}

/// Get room that user is currently in.
/// Throw std::out_of_range if user is not in any room.
Room &RoomManager::getRoomFromUser(const User &user) {
  return rooms.at(userRoomMapping.at(user.connection.id));
}