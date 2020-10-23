#include "GameManager.h"
#include "GameServer.h"
#include "Parser.h"
#include "json.hpp"
#include <sstream>

using json = nlohmann::json;

GameManager::GameManager(GameServer &server, RoomManager &roomManager)
    : server{server}, roomManager{roomManager} {}

// If game is already existed for the room, replace with new one.
GameHandler &GameManager::getGameHandler(const User &user) {
  auto &room = roomManager.getRoomFromUser(user);
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

void GameManager::dispatch(const User &user, const std::string &message) {
  auto &room = roomManager.getRoomFromUser(user);
  auto roomId = room.getId();
  if (!handlers.count(roomId)) {
    return; // Game does not exist. Exit.
  }
  auto &game = handlers.at(roomId);
  if (game.queueMessage(user, message)) {
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
      auto &handler = getGameHandler(user);
      handler.loadGame(games.at(tokens[2]));
      handler.runGame();
      output << "Starting game \"" << tokens[2] << "\"\n";
    }
  }
  if (tokens[1] == "clean") {
    output << "Cleaning empty game handlers.\n";
    cleanEmptyGameHandlers();
  }
  return output.str();
}

// TODO: Call this method somewhere to clean empty handlers if needed.
void GameManager::cleanEmptyGameHandlers() {
  std::erase_if(handlers,
                [](const auto &pair) { return pair.second.isGameUnused(); });
}
