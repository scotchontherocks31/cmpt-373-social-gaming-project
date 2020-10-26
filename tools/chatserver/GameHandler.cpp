#include "GameHandler.h"
#include "GameServer.h"
#include <algorithm>

GameHandler::GameHandler(Room &room, GameServer &server)
    : room{&room}, server{&server}, interpreter{AST::Environment{nullptr},
                                                *this} {
  int counter = 0;
  for (auto &[id, user] : room.getMembers()) {
    players.push_back({counter, user->name});
    playerIdMapping.insert({counter, id});
    playerMessageRequest.insert({counter, false});
    reversePlayerIdMapping.insert({id, counter});
    ++counter;
  }
}

void GameHandler::sendToPlayer(const Player &player, std::string message) {
  auto userId = playerIdMapping.at(player.id);
  auto &user = room->getMember(userId);
  server->sendMessageToUser(user, std::move(message));
}

void GameHandler::sendGlobalMessage(std::string message) {
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

bool GameHandler::queueMessage(const User &user, const std::string &message) {
  if (gameTask.isDone()) {
    return false;
  }
  auto playerId = reversePlayerIdMapping.at(user.getId());
  auto &player = players.at(playerId);
  if (!playerMessageRequest.at(playerId)) {
    return false;
  }
  inboundMessageQueue.push_back({&player, message});
  return true;
}

void GameHandler::loadGame(AST::AST &ast) {
  gameTask = ast.accept(interpreter);
}

void GameHandler::runGame() {
  if (!gameTask.isDone()) {
    gameTask.resume();
  }
}