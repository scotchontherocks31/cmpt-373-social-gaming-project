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

  /*bool strToCommandMapIsEnded(const std::string &command);
  Command strToCommandMapGetCommand(const std::string &command);
  //std::function commandToFunctionMapGetCommand

  bool strToGameCommandMapIsEnded(const std::string &command);
  Command strToGameCommandMapGetCommand(const std::string &command);*/
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

class Mapz {
  public:
    virtual auto  initializeMap(std::vector<auto>; std::vector<auto>) = 0;
};

class standartMap : public Mapz {
    auto  initializeMap(std::vector<auto> keys; std::vector<auto> values) override {
      std::map<keys.value_type, values.value_type> theMap;
      if (keys.size() == values.size()) {
        for (int i = 0; i < keys.size(); i++) {
          theMap[keys[i]] = values[i];
        }
      }
      return theMap
    }
};
