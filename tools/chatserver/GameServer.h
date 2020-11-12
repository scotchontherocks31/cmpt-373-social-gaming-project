#pragma once

#include "GameManager.h"
#include "Room.h"
#include "RoomManager.h"
#include <deque>
#include <map>
#include <string>
#include <functional>

using networking::Connection;
using networking::Message;
using networking::Server;
using functionType = (User&, std::vector<std::string>&);

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
    UNKNOWN,
    CRT, //Stands for CREATE, but the CREATE creates conflicts with something, so...
    START,
    CLEAN
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
  std::deque<Message> inboundMessages;
  std::deque<Message> outboundMessages;
  std::map<std::string, GameServer::Command> strToCommandMap;
  std::map<GameServer::Command, std::function<functionType>> commandToFunctionMap;
  bool running = false;
  std::map<GameServer::Command, std::function<functionType>>  initializeFunctionMap();
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  void processMessages();
  std::string processCommand(User &user, std::string rawCommand);
  std::string processGameCommand(const User &user,
                                 std::vector<std::string> &tokens);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();

};

class GameCommands {
  private:
    //std::map<std::string, GameServer::Command> strToCommandMap;
    std::map<std::string, GameServer::Command>  initializeCommandMap();
  public:
    std::map<std::string, GameServer::Command> getMap();
    void add(std::string, GameServer::Command);
};