#pragma once

#include "GameHandler.h"

struct DecoratedMessage;

class GameManager {
public:
  GameManager(GameServer &server);
  GameHandler &createGame(Room &room);
  void dispatch(const DecoratedMessage &message);

private:
  GameServer &server;
  std::map<roomid, GameHandler> games;
  void cleanEmptyGames();
};
