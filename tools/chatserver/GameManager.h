#pragma once

#include "ASTNode.h"
#include "GameInstance.h"

struct DecoratedMessage;

class GameManager {
public:
  GameManager(GameServer &server, RoomManager &roomManager);
  GameInstance &getGameInstance(const User &user);
  //std::pair<std::pair<AST::AST,AST::Environment>, bool> 
  void createGame(std::string name, std::string json);

  std::pair<AST::AST,AST::Environment> &getGame(const std::string &name) { return games.at(name); };
  void dispatch(const User &user, std::string message);
  void cleanEmptyGameInstances();

private:
  GameServer &server;
  RoomManager &roomManager;
  std::map<roomid, GameInstance> instances;
  std::map<std::string,std::pair<AST::AST,AST::Environment>> games;
};
