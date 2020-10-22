#pragma once

#include "GameHandler.h"

struct DecoratedMessage;

class GameManager {
public:
  GameManager(GameServer &server, RoomManager &roomManager);
  GameHandler &createGame(Room &room);
  GameHandler &getGame(const Room &room) { return games.at(room.getId()); };
  void dispatch(const DecoratedMessage &message);

private:
  GameServer &server;
  RoomManager &roomManager;
  std::map<roomid, GameHandler> games;
  void cleanEmptyGames();
};
