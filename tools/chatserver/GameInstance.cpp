#include "GameInstance.h"
#include "GameServer.h"
#include <algorithm>

GameInstance::GameInstance(Room &room, GameServer &server)
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

void GameInstance::sendToPlayer(const Player &player, std::string message) {
  auto userId = playerIdMapping.at(player.id);
  auto &user = room->getMember(userId);
  server->sendMessageToUser(user, std::move(message));
}

void GameInstance::sendGlobalMessage(std::string message) {
  server->sendMessageToRoom(*room, std::move(message));
}

std::deque<PlayerMessage>
GameInstance::receiveFromPlayer(const Player &player) {
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

bool GameInstance::queueMessage(const User &user, std::string message) {
  if (gameTask.isDone()) {
    return false;
  }
  auto playerId = reversePlayerIdMapping.at(user.getId());
  auto &player = players.at(playerId);
  if (!playerMessageRequest.at(playerId)) {
    return false;
  }
  inboundMessageQueue.push_back({&player, std::move(message)});
  return true;
}

void GameInstance::loadGame(AST::AST &ast, AST::Environment env) {
  (this->interpreter).reset();
  this->interpreter = std::make_unique<AST::Interpreter>(std::move(env), *this);
  gameTask = ast.accept(*(this->interpreter));
}

void GameInstance::runGame() {
  if (!gameTask.isDone()) {
    gameTask.resume();
  }
}
