#pragma once

#include "Server.h"
#include <map>
#include <string>
#include <unordered_set>

using networking::Connection;

struct User {
  Connection connection;
  std::string name;
  // etc...
  bool operator==(const User &other) const {
    return connection == other.connection;
  }
};

class Room {
public:
  std::string roomName;
  Room(int id, std::string roomName);
  int getId() { return id; }
  const std::unordered_set<User *> &getParticipants() const {
    return participants;
  }

private:
  int id;
  std::unordered_set<User *> participants;
  friend class RoomManager;
};

class RoomManager {
public:
  RoomManager();
  Room &createRoom(std::string name = "");
  void removeRoom(int id);
  bool putUserToRoom(User &user, int roomNumber);
  void removeUserFromRoom(User &user);
  Room &getRoomFromUser(const User &user);

private:
  int roomCounter = 0;
  std::map<int, Room> rooms;
  std::map<uintptr_t, int> userRoomMapping;
};