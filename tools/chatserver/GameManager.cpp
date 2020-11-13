#include "GameManager.h"
#include "GameServer.h"
#include "Parser.h"
#include "json.hpp"
#include <sstream>

using json = nlohmann::json;

GameManager::GameManager(GameServer &server, RoomManager &roomManager)
    : server{server}, roomManager{roomManager} {}

// If game already exists in the room, replace with new one.
GameInstance &GameManager::getGameInstance(const User &user) {
  auto &room = roomManager.getRoomFromUser(user);
  auto roomId = room.getId();
  auto [it, _] = instances.insert({roomId, GameInstance{room, server, user}});
  return it->second;
}

bool GameManager::createGame(std::string name, std::string json) {

  auto config = AST::Configurator{std::move(json)};
  auto parser = AST::JSONToASTParser{std::move(json)};
  std::pair<AST::AST, AST::Configurator> gamePair(parser.parse(), config);
  auto [it, inserted] = games.insert({std::move(name), std::move(gamePair)});

  return inserted;
}

void GameManager::dispatch(const User &user, std::string message) {
  auto &room = roomManager.getRoomFromUser(user);
  auto roomId = room.getId();
  if (!instances.count(roomId)) {
    return; // Game does not exist. Exit.
  }
  auto &game = instances.at(roomId);
  if (game.queueMessage(user, std::move(message))) {
    game.runGame();
  }
}

void GameManager::cleanEmptyGameInstances() {
  std::erase_if(instances,
                [](const auto &pair) { return pair.second.isGameUnused(); });
}
