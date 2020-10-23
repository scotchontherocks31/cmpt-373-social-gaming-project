#pragma once

#include "ASTNode.h"
#include "GameHandler.h"

struct DecoratedMessage;

class GameManager {
public:
  GameManager(GameServer &server, RoomManager &roomManager);
  GameHandler &getGameHandler(const User &user);
  std::pair<AST::AST *, bool> createGame(const std::string &name,
                                         const std::string &json);
  AST::AST &getGame(const std::string &name) { return games.at(name); };
  std::string processCommand(const User &user, const std::vector<std::string> &tokens);
  void dispatch(const User &user, const std::string &message);

private:
  GameServer &server;
  RoomManager &roomManager;
  std::map<roomid, GameHandler> handlers;
  std::map<std::string, AST::AST> games;
  void cleanEmptyGameHandlers();
};
