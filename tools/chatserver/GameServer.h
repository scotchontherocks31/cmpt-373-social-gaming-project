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
using commandToFunctionMapType = std::map<GameServer::Command, std::function<functionType>>;
using stringToCommandMapType = std::map<std::string, GameServer::Command>;

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
    CREATE_GAME,
    START_GAME,
    CLEAN_GAME,
    UNKNOWN_GAME
  };

  GameServer(unsigned short port, std::string httpMessage);
  void sendMessageToUser(const User &user, std::string message);
  void sendMessageToRoom(const Room &room, std::string message);
  User &getUser(userid id) { return users.at(id); }
  void startRunningLoop();
  stringToCommandMapType initializeCommandMap();
  stringToCommandMapType initializeGameCommandMap();
  commandToFunctionMapType
  initializeFunctionMap();
  commandToFunctionMapType
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
  stringToCommandMapType strToCommandMap;
  stringToCommandMapType strToGameCommandMap;
  commandToFunctionMapType
      commandToFunctionMap;
  commandToFunctionMapType
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

class StrToCommandM {
protected:
  stringToCommandMapType theMap;

public:
  virtual void initializeMap(std::vector<std::string> keys,
                             std::vector<GameServer::Command> values) = 0;
  virtual stringToCommandMapType getMap() = 0;
};

class EnglishCommandMap : public StrToCommandM {
public:
  stringToCommandMapType getMap() override {
    return theMap;
  }
  /*void initializeMap(std::vector<std::string> keys,
                     std::vector<GameServer::Command> values) override {
    if (keys.size() == values.size()) {
      for (int i = 0; i < keys.size(); i++) {
        theMap[keys[i]] = values[i];
      }
    }
  }*/
  EnglishCommandMap(std::vector<std::string> keys,
                    std::vector<GameServer::Command> values) {
    if (keys.size() == values.size()) {
      for (int i = 0; i < keys.size(); i++) {
        theMap[keys[i]] = values[i];
      }
    }
  }
};

class CommandToFunctionM {
protected:
  commandToFunctionMapType theMap;

public:
  /*virtual void
  initializeMap(std::vector<GameServer::Command> keys,
                std::vector<std::function<functionType>> values) = 0;*/
  CommandToFunctionM(std::vector<GameServer::Command> keys,
                     std::vector<std::function<functionType>> values);
  virtual commandToFunctionMapType
  getMap() = 0;
};

class GameServerFunctions : public CommandToFunctionM {
public:
  commandToFunctionMapType getMap() {
    return theMap;
  }
  /*void initializeMap(std::vector<GameServer::Command> keys,
                     std::vector<std::function<functionType>> values) override {
    if (keys.size() == values.size()) {
      for (int i = 0; i < keys.size(); i++) {
        theMap[keys[i]] = values[i];
      }
    }
  }*/
  GameServerFunctions(std::vector<GameServer::Command> keys,
                      std::vector<std::function<functionType>> values) {
    if (keys.size() == values.size()) {
      for (int i = 0; i < keys.size(); i++) {
        theMap[keys[i]] = values[i];
      }
    }
    else {

    }
  }
};
