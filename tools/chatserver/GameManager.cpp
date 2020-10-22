#include "GameManager.h"
#include "GameServer.h"

GameManager::GameManager(GameServer &server, RoomManager &roomManager)
    : server{server}, roomManager{roomManager} {}

// If game is already existed for the room, replace with new one.
GameHandler &GameManager::createGame(Room &room) {
  auto roomId = room.getId();
  games.insert_or_assign(roomId, GameHandler{room, server});
  return games.at(roomId);
}

void GameManager::dispatch(const DecoratedMessage &message) {
  auto &room = roomManager.getRoomFromUser(message.user);
  auto roomId = room.getId();
  if (!games.count(roomId)) {
    return; // Game does not exist. Exit.
  }
  auto &game = games.at(roomId);
  game.queueMessage(message);
}

// TODO: Call this method somewhere to clean empty games if needed.
void GameManager::cleanEmptyGames() {
  std::erase_if(games,
                [](const auto &pair) { return pair.second.isGameUnused(); });
}
