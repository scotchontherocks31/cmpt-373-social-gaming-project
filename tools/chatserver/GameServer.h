#pragma once

#include "Room.h"
#include <deque>
#include <map>
#include <string>

using networking::Connection;
using networking::Message;
using networking::Server;

struct DecoratedMessage {
  const User &user;
  std::string text;
};

struct MessageResult {
  std::vector<DecoratedMessage> messages;
  bool shouldShutdown;
};

class GameServer {
public:
  GameServer(unsigned short port, std::string httpMessage);
  User &getUser(uintptr_t id) { return users.at(id); }
  void startRunningLoop();

private:
  Server server;
  RoomManager roomManager;
  std::map<uintptr_t, User> users;
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  MessageResult processMessages(Server &server,
                                const std::deque<Message> &incoming);
  std::deque<Message> buildOutgoing(const std::vector<DecoratedMessage> &log);
  User &getUser(Connection connection) { return users.at(connection.id); }
};
