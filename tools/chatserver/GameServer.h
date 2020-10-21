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
  std::vector<userid> receiversId;
};

struct MessageResult {
  std::vector<DecoratedMessage> messages;
  bool shouldShutdown;
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
    WHISPER,
    UNKNOWN
  };

  GameServer(unsigned short port, std::string httpMessage);
  void sendMessageToUser(const User &user, std::string message);
  void sendMessageToRoom(const Room &room, std::string message);
  User &getUser(userid id) { return users.at(id); }
  void startRunningLoop();

private:
  Server server;
  RoomManager roomManager;
  GameManager gameManager;
  std::map<userid, User> users;
  std::deque<Message> outboundMessages;
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  bool processMessages(const std::deque<Message> &incoming);
  void broadcast(const DecoratedMessage &message);
  void narrowcast(const DecoratedMessage &message);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();
};
