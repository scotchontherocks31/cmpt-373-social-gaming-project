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

//std::pair<std::pair<AST::AST,AST::Environment>, bool> 
void GameManager::createGame(std::string name, std::string json) {
  // if (games.count(name)) {
  //   return {games.at(name), false};
  // }
  //auto config = AST::Configurator{json};
  auto config = AST::Configurator{std::string{"{\"configuration\":{\"name\":\"Rock,Paper,Scissors\",\"playercount\":{\"min\":2,\"max\":4},\"audience\":false,\"setup\":{\"Rounds\":10}},\"constants\":{\"weapons\":[{\"name\":\"Rock\",\"beats\":\"Scissors\"},{\"name\":\"Paper\",\"beats\":\"Rock\"},{\"name\":\"Scissors\",\"beats\":\"Paper\"}]},\"variables\":{\"winners\":[]},\"per-player\":{\"wins\":0},\"per-audience\":{},\"rules\":[{\"rule\":\"parallelfor\",\"list\":\"players\",\"element\":\"player\",\"rules\":[{\"rule\":\"global-message\",\"value\":\"heyguys\"}]}]}"}};
  auto parser = AST::JSONToASTParser(std::string{"{\"configuration\":{\"name\":\"Rock,Paper,Scissors\",\"playercount\":{\"min\":2,\"max\":4},\"audience\":false,\"setup\":{\"Rounds\":10}},\"constants\":{\"weapons\":[{\"name\":\"Rock\",\"beats\":\"Scissors\"},{\"name\":\"Paper\",\"beats\":\"Rock\"},{\"name\":\"Scissors\",\"beats\":\"Paper\"}]},\"variables\":{\"winners\":[]},\"per-player\":{\"wins\":0},\"per-audience\":{},\"rules\":[{\"rule\":\"parallelfor\",\"list\":\"players\",\"element\":\"player\",\"rules\":[{\"rule\":\"global-message\",\"value\":\"heyguys\"}]}]}"});

  std::pair<AST::AST,AST::Configurator> gamePair(parser.parse(), config); 
  
  
  //auto [it, inserted] = 
  games.insert({std::move(name), std::move(gamePair)});
 
  return; // {it->second, inserted};
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
