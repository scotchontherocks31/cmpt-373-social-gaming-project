#include "GameManager.h"

GameManager::GameManager(GameServer &server) : server{server} {}

GameHandler &GameManager::createGame(const Room &room) {
  auto roomId = room.getId();
  games.insert_or_assign(room.getId(), GameHandler{room, server});
  return games.at(roomId);
}

void GameManager::dispatch(const DecoratedMessage &message) {
  auto &room = server.getRoomManager().getRoomFromUser(message.user);
  auto roomId = room.getId();
  if (!games.count(roomId))
    throw "Game does not exist. Cannot dispatch message to game.";
  auto &game = games.at(roomId);
  // TODO: Dispatch the message to the game.
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
