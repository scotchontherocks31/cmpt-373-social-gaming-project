#pragma once

#include "Room.h"

class RoomManager {
public:
  RoomManager();
  std::pair<Room *, bool> createRoom(const std::string &name = "");
  void removeRoom(const std::string &name);
  bool putUserToRoom(User &user, const std::string &roomName);
  void removeUserFromRoom(User &user);
  Room &getRoomFromUser(const User &user);
  static constexpr auto GLOBAL_ROOM_NAME = "Global";
  std::string listRoomsInfo();

private:
  const roomid globalRoomHash;
  std::map<roomid, Room> rooms;
  std::map<userid, roomid> userRoomMapping;
};