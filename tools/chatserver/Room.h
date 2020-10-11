#pragma once

#include "User.h"
#include <functional>
#include <map>
#include <string>
#include <unordered_set>

using roomid = size_t;

class RoomManager;

class Room {
public:
  roomid getId() const { return id; }
  const std::string &getName() const { return name; }
  const std::map<userid, std::reference_wrapper<User>> &
  getParticipants() const {
    return participants;
  }
  User &getParticipant(userid userId) const {
    return participants.at(userId).get();
  };
  void listParticipants();
  int getCurrentSize(){ return participants.size(); }
  int getCapacity(){ return capacity; }
  void setCapacity(int newCapacity);
  void setName(std::string newName);

private:
  userid hostid;
  int capacity;
  std::string name;
  roomid id;
  std::map<userid, std::reference_wrapper<User>> participants;
  Room(roomid id, std::string roomName);
  friend class RoomManager;
};
