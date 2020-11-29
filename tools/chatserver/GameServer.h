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
struct CommandMappings;

class GameServer {
public:
  GameServer(unsigned short port, std::string httpMessage,
             CommandMappings &maps);
  void sendMessageToUser(const User &user, std::string message);
  void sendMessageToRoom(const Room &room, std::string message);
  User &getUser(userid id) { return users.at(id); }
  void startRunningLoop();
  std::map<std::string, Command> initializeCommandMap();
  std::map<std::string, Command> initializeGameCommandMap();
  std::map<BaseStringToCommandMap::Command, std::function<functionType>>
  initializeFunctionMap();
  std::map<BaseStringToGameCommandMap::Command, std::function<functionType>>
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
  BaseStringToCommandMap strToCommandMap;
  BaseStringToGameCommandMap strToGameCommandMap;
  std::map<BaseStringToCommandMap::Command, std::function<functionType>>
      commandToFunctionMap;
  std::map<BaseStringToGameCommandMap::Command, std::function<functionType>>
      commandToGameFunctionMap;
  bool running = false;
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  void processMessages();
  std::string processCommand(User &user, std::string rawCommand);
  std::string processGameCommand(User &user, std::vector<std::string> &tokens);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();
};

// Interface to translate strings to commands for a server
class BaseStringToCommandMap {
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
  BaseStringToCommandMap() {}
  virtual std::map<std::string, Command> &getMap() = 0;
  virtual Command getValue(std::string command) = 0;
  virtual bool contains(std::string command) = 0;
};

class StringToCommandMap : public BaseStringToCommandMap {
private:
  std::map<std::string, Command> theMap;

public:
  StringToCommandMap() {
    theMap = {{"quit", Command::QUIT},     {"shutdown", Command::SHUTDOWN},
              {"create", Command::CREATE}, {"join", Command::JOIN},
              {"leave", Command::LEAVE},   {"list", Command::LIST},
              {"info", Command::INFO},     {"game", Command::GAME}};
  }
  std::map<std::string, Command> &getMap() override { return theMap; }
  Command getValue(std::string command) override { return theMap.at(command); }
  bool contains(std::string command) override {
    return theMap.contains(command);
  }
};

// Interface to translate strings to commands for a game
class BaseStringToGameCommandMap {
public:
  enum Command { CREATE, START, CLEAN, UNKNOWN };
  BaseStringToGameCommandMap() {}
  virtual std::map<std::string, Command> &getMap() = 0;
  virtual Command getValue(std::string command) = 0;
  virtual bool contains(std::string command) = 0;
};

class StringToGameCommandMap : public BaseStringToGameCommandMap {
private:
  std::map<std::string, Command> theMap;

public:
  StringToGameCommandMap() {
    theMap = {{"create", Command::CREATE},
              {"start", Command::START},
              {"clean", Command::CLEAN}};
  }
  std::map<std::string, Command> &getMap() override { return theMap; }
  Command getValue(std::string command) override { return theMap.at(command); }
  bool contains(std::string command) override {
    return theMap.contains(command);
  }
};

// Class to create a bundle of maps before passing into GameServer class
class CommandMappings {
public:
  BaseStringToCommandMap *ptrServerCommandMap;
  BaseStringToGameCommandMap *ptrGameCommandMap;
};
