#pragma once

#include "GameManager.h"
#include "Room.h"
#include "RoomManager.h"
#include <deque>
#include <functional>
#include <map>
#include <string>

using networking::Connection;
using networking::Message;
using networking::Server;
using functionType = void(User &user, std::vector<std::string> &tokens);

struct DecoratedMessage {
  const User &user;
  std::string text;
  bool isBroadcast;
};

class GameManager;
class BaseStringToServerCommandMap;
class BaseStringToGameCommandMap;

class GameServer {
public:
  enum class ServerCommand {
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
  enum class GameCommand { CREATE, START, CLEAN, UNKNOWN };

  GameServer(unsigned short port, std::string httpMessage,
             BaseStringToServerCommandMap &serverMap,
             BaseStringToGameCommandMap &gameMap);
  void sendMessageToUser(const User &user, std::string message);
  void sendMessageToRoom(const Room &room, std::string message);
  User &getUser(userid id) { return users.at(id); }
  void startRunningLoop();
  std::map<ServerCommand, std::function<functionType>> initializeFunctionMap();
  std::map<GameCommand, std::function<functionType>>
  initializeGameFunctionMap();

  ServerCommand matchCommand(const std::string &command);
  GameCommand matchGameCommand(const std::string &command);

private:
  Server server;
  RoomManager roomManager;
  GameManager gameManager;
  std::map<userid, User> users;
  std::deque<Message> inboundMessages;
  std::deque<Message> outboundMessages;
  std::map<std::string, ServerCommand> strToCommandMap;
  std::map<std::string, GameCommand> strToGameCommandMap;
  std::map<ServerCommand, std::function<functionType>> commandToFunctionMap;
  std::map<GameCommand, std::function<functionType>> commandToGameFunctionMap;
  bool running = false;
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  void processMessages();
  void processCommand(User &user, std::string rawCommand);
  void processGameCommand(User &user, std::vector<std::string> &tokens);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();
};

class BaseStringToServerCommandMap {
public:
  virtual std::map<std::string, GameServer::ServerCommand> &getMap() = 0;
};

class StringToCommandMap : public BaseStringToServerCommandMap {
private:
  std::map<std::string, GameServer::ServerCommand> theMap;

public:
  StringToCommandMap() {
    theMap = {{"quit", GameServer::ServerCommand::QUIT},
              {"shutdown", GameServer::ServerCommand::SHUTDOWN},
              {"create", GameServer::ServerCommand::CREATE},
              {"join", GameServer::ServerCommand::JOIN},
              {"leave", GameServer::ServerCommand::LEAVE},
              {"list", GameServer::ServerCommand::LIST},
              {"info", GameServer::ServerCommand::INFO},
              {"game", GameServer::ServerCommand::GAME}};
  }
  std::map<std::string, GameServer::ServerCommand> &getMap() override {
    return theMap;
  }
};

class BaseStringToGameCommandMap {
public:
  virtual std::map<std::string, GameServer::GameCommand> &getMap() = 0;
};

class StringToGameCommandMap : public BaseStringToGameCommandMap {
private:
  std::map<std::string, GameServer::GameCommand> theMap;

public:
  StringToGameCommandMap() {
    theMap = {{"create", GameServer::GameCommand::CREATE},
              {"start", GameServer::GameCommand::START},
              {"clean", GameServer::GameCommand::CLEAN}};
  }
  std::map<std::string, GameServer::GameCommand> &getMap() override {
    return theMap;
  }
};
