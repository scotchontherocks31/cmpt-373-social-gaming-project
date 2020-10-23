#pragma once

#include "GameManager.h"
#include "Room.h"
#include "RoomManager.h"
#include <deque>
#include <map>
#include <string>

using networking::Connection;
using networking::Message;
using networking::Server;

struct DecoratedMessage {
  const User &user;
  std::string text;
  bool isBroadcast;
};

class GameManager;

class GameServer {
public:
  enum Command {
    QUIT,
    SHUTDOWN,
    CREATE,
    JOIN,
    LEAVE,
    LIST,
    INFO,
    GAME,
    UNKNOWN
  };

  GameServer(unsigned short port, const std::string &httpMessage);
  void sendMessageToUser(const User &user, const std::string &message);
  void sendMessageToRoom(const Room &room, const std::string &message);
  User &getUser(userid id) { return users.at(id); }
  void startRunningLoop();

private:
  Server server;
  RoomManager roomManager;
  GameManager gameManager;
  std::map<userid, User> users;
  std::deque<Message> inboundMessages;
  std::deque<Message> outboundMessages;
  bool running = false;
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  void processMessages();
  std::string processCommand(User &user, const std::string &rawCommand);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();
};
