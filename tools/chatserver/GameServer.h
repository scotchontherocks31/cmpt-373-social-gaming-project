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
using functionType = std::string(User&, std::vector<std::string>&);

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
  enum GameCommand {
    CREATE_GAME, 
    START,
    CLEAN,
    UNKNOWN_GAME
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
  std::map<std::string, GameServer::Command> strToGameCommandMap;
  std::map<GameServer::Command, std::function<functionType>> commandToFunctionMap;
  std::map<GameServer::Command, std::function<functionType>> commandToGameFunctionMap;
  bool running = false;
  void onConnect(Connection c);
  void onDisconnect(Connection c);
  void processMessages();
  std::string processCommand(User &user, std::string rawCommand);
  std::string processGameCommand(const User &user,
                                 std::vector<std::string> &tokens);
  User &getUser(Connection connection) { return users.at(connection.id); }
  void flush();
};

class strToCommandMap {
  protected:
    std::map<std::string, GameServer::Command> theMap;
  public:
    virtual void initializeMap(std::vector<std::string> keys, std::vector<GameServer::Command> values) = 0;
    virtual std::map<std::string, GameServer::Command> getMap() = 0;

};

class englishCommandMap : public strToCommandMap {
  public:
    std::map<std::string, GameServer::Command> getMap() override {
      return theMap;
    }
    void initializeMap(std::vector<std::string> keys, std::vector<GameServer::Command> values) override {
      if (keys.size() == values.size()) {
        for (int i = 0; i < keys.size(); i++) {
          theMap[keys[i]] = values[i];
        }
      }
    }
};

class commandToFunctionMap {
  protected:
    std::map<GameServer::Command, std::function<functionType>> theMap;
  public:
    virtual void initializeMap(std::vector<GameServer::Command> keys, std::vector<std::function<functionType>> values) = 0;
    virtual std::map<GameServer::Command, std::function<functionType>> getMap() = 0;

};

class gameServerFunctions : public commandToFunctionMap {
  public:
    std::map<GameServer::Command, std::function<functionType>> getMap() {
      return theMap;
    }
    void initializeMap(std::vector<GameServer::Command> keys, std::vector<std::function<functionType>> values) override {
      if (keys.size() == values.size()) {
        for (int i = 0; i < keys.size(); i++) {
          theMap[keys[i]] = values[i];
        }
      }
    }
};
