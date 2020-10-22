#include "GameHandler.h"
#include "GameServer.h"
#include "json.hpp"
#include "Parser.h"
#include <algorithm>

using json = nlohmann::json;

GameHandler::GameHandler(Room &room, GameServer &server)
    : room{&room}, server{&server} {
  int counter = 0;
  for (auto &[id, user] : room.getMembers()) {
    players.push_back({counter, user->name});
    playerIdMapping.insert({counter, id});
    playerMessageRequest.insert({counter, false});
    reversePlayerIdMapping.insert({id, counter});
    ++counter;
  }
}

void GameHandler::configure(const std::string &json) {
  if (ast.empty()) {
    auto object = json::parse(json);
    auto parser = AST::JSONToASTParser(std::move(object));
    ast = parser.parse();
  } else {
    // TODO: handle game reconfigure
  }
}

void GameHandler::sendToPlayer(const Player &player, std::string message) {
  auto userId = playerIdMapping.at(player.id);
  auto &user = room->getMember(userId);
  server->sendMessageToUser(user, std::move(message));
}

void GameHandler::sendToAllPlayers(std::string message) {
  server->sendMessageToRoom(*room, std::move(message));
}

std::deque<PlayerMessage> GameHandler::receiveFromPlayer(const Player &player) {
  std::deque<PlayerMessage> messages;
  auto &&[x, y] =
      std::ranges::partition(inboundMessageQueue, [&player](auto &message) {
        return player.id != message.player->id;
      });
  std::ranges::move(x, inboundMessageQueue.end(), std::back_inserter(messages));
  inboundMessageQueue.erase(x, inboundMessageQueue.end());
  playerMessageRequest.at(player.id) = messages.empty();
  return messages;
}

bool GameHandler::queueMessage(const DecoratedMessage &message) {
  auto playerId = reversePlayerIdMapping.at(message.user.getId());
  auto &player = players.at(playerId);
  if (!playerMessageRequest.at(playerId)) {
    return false;
  }
  inboundMessageQueue.push_back({&player, message.text});
  return true;
}