#pragma once

#include "User.h"
#include <functional>
#include <map>
#include <string>
#include <unordered_set>

using roomid = size_t;

class Room {
public:
  Room(std::string roomName);
  const std::string &getName() const { return name; }
  const std::map<userid, std::reference_wrapper<User>> &
  getParticipants() const {
    return participants;
  }
  User &getParticipant(userid userId) const {
    return participants.at(userId).get();
  };
  void listParticipants();

private:
  std::string name;
  roomid id;
  std::map<userid, std::reference_wrapper<User>> participants;
  Room(roomid id, std::string roomName);
  friend class RoomManager;
};

class RoomManager {
public:
  RoomManager();
  bool createRoom(const std::string &name = "");
  void removeRoom(const std::string &name);
  bool putUserToRoom(User &user, const std::string &roomName);
  void removeUserFromRoom(User &user);
  Room &getRoomFromUser(const User &user);
  void listRooms();
  inline static const std::string GLOBAL_ROOM_NAME = "Global";

private:
  const roomid GLOBAL_ROOM_HASH;
  int roomCounter = 1;
  std::map<roomid, Room> rooms;
  std::map<userid, roomid> userRoomMapping;
};