#pragma once

#include "Server.h"
#include <functional>
#include <map>
#include <string>
#include <unordered_set>

typedef uintptr_t userid;

struct User {
  networking::Connection connection;
  std::string name;
  // etc...
  bool operator==(const User &other) const {
    return connection == other.connection;
  }
  userid getId() const { return connection.id; }
};

class Room {
public:
  std::string roomName;
  Room(int id, std::string roomName);
  int getId() { return id; }
  const std::map<userid, std::reference_wrapper<User>> &
  getParticipants() const {
    return participants;
  }

private:
  int id;
  std::map<userid, std::reference_wrapper<User>> participants;
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
  std::map<userid, int> userRoomMapping;
};