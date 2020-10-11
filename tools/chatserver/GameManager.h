#pragma once

#include "GameHandler.h"

struct DecoratedMessage;

class GameManager {
public:
  GameManager(GameServer &server, RoomManager &roomManager);
  GameHandler &createGame(Room &room);
  void dispatch(const DecoratedMessage &message);

private:
  GameServer &server;
  RoomManager &roomManager;
  std::map<roomid, GameHandler> games;
  void cleanEmptyGames();
};
