#include "GameManager.h"
#include "GameServer.h"

GameManager::GameManager(GameServer &server) : server{server} {}

// If game is already existed for the room, replace with new one.
GameHandler &GameManager::createGame(Room &room) {
  auto roomId = room.getId();
  if (games.count(roomId)){
    games.erase(roomId);
  }
  games.insert({roomId, GameHandler{room, server}});
  return games.at(roomId);
}

void GameManager::dispatch(const DecoratedMessage &message) {
  auto &room = server.getRoomManager().getRoomFromUser(message.user);
  auto roomId = room.getId();
  if (!games.count(roomId))
    throw "Game does not exist. Cannot dispatch message to game.";
  auto &game = games.at(roomId);
  game.queueMessage(message);
}


// TODO: Call this method somewhere to clean empty games if needed.
void GameManager::cleanEmptyGames() {
  auto iter = games.begin();
  while (iter != games.end()) {
    if (iter->second.room.getParticipants().empty()) {
      iter = games.erase(iter);
    } else {
      ++iter;
    }
  }
}
