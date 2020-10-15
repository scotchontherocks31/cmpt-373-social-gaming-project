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
  Room(roomid id, std::string roomName);
  roomid getId() const { return id; }
  const std::string &getName() const { return name; }
  User &getMember(userid userId) const { return *members.at(userId); };
  const std::map<userid, User *> &getMembers() const { return members; }
  void addMember(User &user) { members[user.getId()] = &user; }
  void removeMember(userid userId) { members.erase(id); }
  int getCurrentSize() { return members.size(); }
  int getCapacity() { return capacity; }
  // void setCapacity(int newCapacity);
  // void setName(std::string newName);

private:
  // userid hostid;
  int capacity;
  std::string name;
  roomid id;
  std::map<userid, User *> members;
};
