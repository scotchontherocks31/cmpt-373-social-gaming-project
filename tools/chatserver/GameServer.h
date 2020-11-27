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
class BaseStringToCommandMap;

// Struct to create a bundle of maps before passing into GameServer class
struct CommandMappings {
  BaseStringToCommandMap serverCommandMap;
  BaseStringToCommandMap gameCommandMap;
};

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
             CommandMappings &maps);
  void sendMessageToUser(const User &user, std::string message);
  void sendMessageToRoom(const Room &room, std::string message);
  User &getUser(userid id) { return users.at(id); }
  void startRunningLoop();
  std::map<std::string, Command> initializeCommandMap();
  std::map<std::string, Command> initializeGameCommandMap();
  std::map<Command, std::function<functionType>> initializeFunctionMap();
  std::map<Command, std::function<functionType>> initializeGameFunctionMap();

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
  std::map<Command, std::function<functionType>> commandToFunctionMap;
  std::map<Command, std::function<functionType>> commandToGameFunctionMap;
  bool running = false;
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  void processMessages();
  std::string processCommand(User &user, std::string rawCommand);
  std::string processGameCommand(User &user, std::vector<std::string> &tokens);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();
};

class BaseStringToCommandMap {
public:
  virtual std::map<std::string, GameServer::Command> getMap() = 0;
  virtual GameServer::Command getValue(std::string command) = 0;
  virtual bool have(std::string command) = 0;
};

class StrToCommandMap : public BaseStringToCommandMap {
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
  std::map<std::string, GameServer::Command> getMap() override {
    return theMap;
  }
  GameServer::Command getValue(std::string command) override {
    return theMap[command];
  }
  bool have(std::string command) override { return theMap.contains(command); }
};

class StrToGameCommandMap : public BaseStringToCommandMap {
private:
  std::map<std::string, GameServer::Command> theMap;

public:
  StrToGameCommandMap() {
    theMap = {{"create", GameServer::Command::CREATE_GAME},
              {"start", GameServer::Command::START_GAME},
              {"clean", GameServer::Command::CLEAN_GAME}};
  }
  std::map<std::string, GameServer::Command> getMap() override {
    return theMap;
  }
  GameServer::Command getValue(std::string command) override {
    return theMap[command];
  }
  bool have(std::string command) override { return theMap.contains(command); }
};