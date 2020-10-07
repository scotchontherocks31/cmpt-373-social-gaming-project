#pragma once

#include "RoomManager.h"
#include "GameManager.h"
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

class GameManager;

class GameServer {
public:
  GameServer(unsigned short port, std::string httpMessage);
  void sendMessageToUser(const User &user, std::string message);
  void sendMessageToRoom(const Room &room, std::string message);
  User &getUser(userid id) { return users.at(id); }
  RoomManager &getRoomManager() { return roomManager; }
  void startRunningLoop();

private:
  Server server;
  RoomManager roomManager;
  GameManager gameManager;
  std::map<userid, User> users;
  std::deque<Message> outboundMessages;
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  MessageResult processMessages(Server &server,
                                const std::deque<Message> &incoming);
  void buildOutgoing(const std::vector<DecoratedMessage> &log);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();
  std::vector<std::string> getCommand(const std::string &message);
};
