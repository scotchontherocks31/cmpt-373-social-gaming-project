#pragma once

#include "Room.h"

class RoomManager {
public:
  RoomManager();
  bool createRoom(const std::string &name = "");
  void removeRoom(const std::string &name);
  bool putUserToRoom(User &user, const std::string &roomName);
  void removeUserFromRoom(User &user);
  Room &getRoomFromUser(const User &user);
  void listRooms();
  void configureRoom(User &user);
  inline static const std::string GLOBAL_ROOM_NAME = "Global";

private:
  const roomid GLOBAL_ROOM_HASH;
  int roomCounter = 1;
  std::map<roomid, Room> rooms;
  std::map<userid, roomid> userRoomMapping;
};