#pragma once

#include "ASTNode.h"
#include "GameHandler.h"

struct DecoratedMessage;

class GameManager {
public:
  GameManager(GameServer &server, RoomManager &roomManager);
  GameInstance &getGameInstance(const User &user);
  std::pair<AST::AST *, bool> createGame(std::string name, std::string json);
  AST::AST &getGame(const std::string &name) { return games.at(name); };
  std::string processCommand(const User &user,
                             const std::vector<std::string> &tokens);
  void dispatch(const User &user, std::string message);

private:
  GameServer &server;
  RoomManager &roomManager;
  std::map<roomid, GameInstance> instances;
  std::map<std::string, AST::AST> games;
  void cleanEmptyGameHandlers();
};
