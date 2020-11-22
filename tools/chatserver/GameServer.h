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
using functionType = std::string(User &user, std::vector<std::string> &tokens);

struct DecoratedMessage {
  const User &user;
  std::string text;
  bool isBroadcast;
};

class GameManager;

class AnyStrToCommandM;
class StrToCommandMap;
class StrToGameCommandMap;
class AnyCommandToFunctionM;
class CommandToFunctionMap;
class CommandToGameFunctionMap;

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
    CREATE_GAME,
    START_GAME,
    CLEAN_GAME,
    UNKNOWN_GAME
  };

  GameServer(unsigned short port, std::string httpMessage,
             AnyStrToCommandM &strToComm, AnyStrToCommandM &strToGameComm);
  void sendMessageToUser(const User &user, std::string message);
  void sendMessageToRoom(const Room &room, std::string message);
  User &getUser(userid id) { return users.at(id); }
  void startRunningLoop();
  std::map<std::string, GameServer::Command> initializeCommandMap();
  std::map<std::string, GameServer::Command> initializeGameCommandMap();
  std::map<GameServer::Command, std::function<functionType>>
  initializeFunctionMap();
  std::map<GameServer::Command, std::function<functionType>>
  initializeGameFunctionMap();

  Command matchCommand(const std::string &command);
  Command matchGameCommand(const std::string &command);

private:
  Server server;
  RoomManager roomManager;
  GameManager gameManager;
  std::map<userid, User> users;
  std::deque<Message> inboundMessages;
  std::deque<Message> outboundMessages;
  std::map<std::string, GameServer::Command> strToCommandMap;
  std::map<std::string, GameServer::Command> strToGameCommandMap;
  std::map<GameServer::Command, std::function<functionType>>
      commandToFunctionMap;
  std::map<GameServer::Command, std::function<functionType>>
      commandToGameFunctionMap;
  bool running = false;
  std::map<GameServer::Command, std::function<functionType>>
  initializeFunctionMap();
  std::map<GameServer::Command, std::function<functionType>>
  initializeGameFunctionMap();
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  void processMessages();
  std::string processCommand(User &user, std::string rawCommand);
  std::string processGameCommand(User &user, std::vector<std::string> &tokens);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();
};

class AnyStrToCommandM {
public:
  virtual std::map<std::string, GameServer::Command> getMap() const = 0;
};

class StrToCommandMap : public AnyStrToCommandM {
private:
  std::map<std::string, GameServer::Command> theMap;

public:
  StrToCommandMap() {
    theMap = {{"quit", GameServer::Command::QUIT},
              {"shutdown", GameServer::Command::SHUTDOWN},
              {"create", GameServer::Command::CREATE},
              {"join", GameServer::Command::JOIN},
              {"leave", GameServer::Command::LEAVE},
              {"list", GameServer::Command::LIST},
              {"info", GameServer::Command::INFO},
              {"game", GameServer::Command::GAME}};
  }
  std::map<std::string, GameServer::Command> getMap() const override {
    return theMap;
  }
};

class StrToGameCommandMap : public AnyStrToCommandM {
private:
  std::map<std::string, GameServer::Command> theMap;

public:
  StrToGameCommandMap() {
    theMap = {{"create", GameServer::Command::CREATE_GAME},
              {"start", GameServer::Command::START_GAME},
              {"clean", GameServer::Command::CLEAN_GAME}};
  }
  std::map<std::string, GameServer::Command> getMap() const override {
    return theMap;
  }
};

/*class AnyCommandToFunctionM {
public:
  virtual std::map<GameServer::Command, std::function<functionType>>
  getMap() const = 0;
};

class CommandToFunctionMap : public AnyCommandToFunctionM {
private:
  std::map<GameServer::Command, std::function<functionType>> theMap;

public:
  CommandToFunctionMap();
  std::map<GameServer::Command, std::function<functionType>>
  getMap() const override {
    return theMap;
  }
};

class CommandToGameFunctionMap : public AnyCommandToFunctionM {
private:
  std::map<GameServer::Command, std::function<functionType>> theMap;

public:
  CommandToGameFunctionMap();
  std::map<GameServer::Command, std::function<functionType>>
  getMap() const override {
    return theMap;
  }
};*/
