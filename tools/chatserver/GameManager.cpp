#include "GameManager.h"
#include "GameServer.h"
#include "Parser.h"
#include "json.hpp"

using json = nlohmann::json;

GameManager::GameManager(GameServer &server, RoomManager &roomManager)
    : server{server}, roomManager{roomManager} {}

// If game is already existed for the room, replace with new one.
GameHandler &GameManager::getGameHandler(Room &room) {
  auto roomId = room.getId();
  auto [it, _] = handlers.insert({roomId, GameHandler{room, server}});
  return it->second;
}

std::pair<AST::AST *, bool> GameManager::createGame(const std::string &name,
                                                    const std::string &json) {
  if (games.count(name)) {
    return {&games.at(name), false};
  }
  auto object = json::parse(json);
  auto parser = AST::JSONToASTParser(std::move(object));
  auto [it, inserted] = games.insert({name, parser.parse()});
  return {&it->second, inserted};
}

void GameManager::dispatch(const DecoratedMessage &message) {
  auto &room = roomManager.getRoomFromUser(message.user);
  auto roomId = room.getId();
  if (!handlers.count(roomId)) {
    return; // Game does not exist. Exit.
  }
  auto &game = handlers.at(roomId);
  game.queueMessage(message);
}

// TODO: Call this method somewhere to clean empty handlers if needed.
void GameManager::cleanEmptyGameHandlers() {
  std::erase_if(handlers,
                [](const auto &pair) { return pair.second.isGameUnused(); });
}
