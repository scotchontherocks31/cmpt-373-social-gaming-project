#include "RoomManager.h"
#include <boost/algorithm/string.hpp>
#include <cctype>
#include <iostream>

/// Returns normalized string (e.g. "This is a string" -> "this-is-a-string")
std::string getNormalizedString(const std::string &str) {
  std::string result = str;
  std::transform(str.begin(), str.end(), result.begin(), [](char c) {
    c = std::tolower(c);
    return c == ' ' ? '-' : c;
  });
  return result;
}

roomid getIdFromName(const std::string &name) {
  return std::hash<std::string>()(getNormalizedString(name));
}

RoomManager::RoomManager() : globalRoomHash(getIdFromName(GLOBAL_ROOM_NAME)) {
  rooms.insert({globalRoomHash, Room{globalRoomHash, GLOBAL_ROOM_NAME}});
}

bool RoomManager::createRoom(const std::string &name) {
  auto roomName = boost::algorithm::trim_copy(name);
  roomName = roomName == "" ? "Room " + std::to_string(roomCounter) : roomName;
  auto roomId = getIdFromName(roomName);
  if (rooms.count(roomId)) {
    return false;
  }
  auto room = Room{roomId, roomName};
  rooms.insert({room.getId(), room});
  ++roomCounter;
  return true;
}

void RoomManager::removeRoom(const std::string &name) {
  auto roomName = boost::algorithm::trim_copy(name);
  if (roomName == "")
    return;
  auto roomId = getIdFromName(roomName);
  // Should not remove global room
  if (!rooms.count(roomId) || roomId == globalRoomHash)
    return;
  auto &room = rooms.at(roomId);
  while (!room.getMembers().empty()) {
    auto it = room.getMembers().begin();
    putUserToRoom(*it->second, GLOBAL_ROOM_NAME); // Put user to global room
  }
  rooms.erase(roomId);
}

/// Put user in specified room, switch room if needed.
/// Return true if successfully put user in or user already in the room.
bool RoomManager::putUserToRoom(User &user, const std::string &roomName) {
  if (roomName == "")
    return false;
  auto roomId = getIdFromName(roomName);
  if (!rooms.count(roomId))
    return false;
  auto [it, inserted] = userRoomMapping.insert({user.getId(), roomId});
  if (!inserted) {
    // User is in a room
    if (it->second == roomId) {
      // User already in the target room
      return true;
    } else {
      // User in a different room,
      removeUserFromRoom(user);
      userRoomMapping.insert({user.getId(), roomId});
    }
  }
  auto &room = rooms.at(roomId);
  room.addMember(user);
  return true;
}

/// Remove user from any room.
void RoomManager::removeUserFromRoom(User &user) {
  auto userId = user.getId();
  if (userRoomMapping.count(userId)) {
    auto roomId = userRoomMapping.at(userId);
    rooms.at(roomId).removeMember(userId);
    userRoomMapping.erase(userId);
  }
}

/// Get room that user is currently in.
/// Throw std::out_of_range if user is not in any room.
Room &RoomManager::getRoomFromUser(const User &user) {
  return rooms.at(userRoomMapping.at(user.getId()));
}

void RoomManager::configureRoom(User &user) {
  // To implement: Read and parse JSON for room setting
}

std::string RoomManager::listRoomsInfo() {
  std::string info = "--------------\n";

  for (auto &r : rooms) {
    info += r.second.getName() + " room members:\n";
    for (std::pair<userid, User *> u : r.second.getMembers()) {
      info += std::to_string(u.first) + "\n";
    }
    info += "--------------\n";
  }
  return info;
}