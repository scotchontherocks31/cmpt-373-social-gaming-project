#include "GameManager.h"
#include "GameServer.h"
#include "Parser.h"
#include "json.hpp"
#include <sstream>

using json = nlohmann::json;

GameManager::GameManager(GameServer &server, RoomManager &roomManager)
    : server{server}, roomManager{roomManager} {}

// If game is already existed for the room, replace with new one.
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
  auto object = json::parse(std::move(json));
  auto parser = AST::JSONToASTParser(std::move(object));
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

std::string
GameManager::processCommand(const User &user,
                            const std::vector<std::string> &tokens) {
  // TODO: rework this to use visitor pattern
  std::ostringstream output;
  if (tokens.size() < 2) {
    return "Invalid command.\n";
  }
  if (tokens[1] == "create") {
    if (tokens.size() < 4) {
      output << "Error. Create command requires 2 arguments.\n";
    } else {
      createGame(tokens[2], tokens[3]);
    }
  }
  if (tokens[1] == "start") {
    if (tokens.size() < 3) {
      output << "Error. Start command requires 1 argument.\n";
    } else {
      auto &handler = getGameInstance(user);
      handler.loadGame(games.at(tokens[2]));
      handler.runGame();
      output << "Starting game \"" << tokens[2] << "\"\n";
    }
  }
  if (tokens[1] == "clean") {
    output << "Cleaning empty game instances.\n";
    cleanEmptyGameHandlers();
  }
  return output.str();
}

void GameManager::cleanEmptyGameHandlers() {
  std::erase_if(instances,
                [](const auto &pair) { return pair.second.isGameUnused(); });
}
