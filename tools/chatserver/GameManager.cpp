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
  auto [it, _] = instances.insert({roomId, GameInstance{room, server}});
  return it->second;
}

std::pair<AST::AST *, bool> GameManager::createGame(std::string name,
                                                    std::string json) {
  if (games.count(name)) {
    return {&games.at(name), false};
  }
  //auto config = Configurator{json}
  auto parser = AST::JSONToASTParser(std::move(json));
  // pass it along ast like this :{parser.parse(), config.parse()}
  // config.parse() will return environment instance
  auto [it, inserted] = games.insert({std::move(name), parser.parse()}); 
  return {&it->second, inserted};
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
