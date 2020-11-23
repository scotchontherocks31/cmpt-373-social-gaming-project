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

void GameInstance::sendToPlayer(int playerId, std::string message) {
  auto userId = playerIdMapping.at(playerId);
  auto &user = room->getMember(userId);
  server->sendMessageToUser(user, std::move(message));
}

void GameInstance::sendToOwner(std::string message) {
  sendToPlayer(ownerId, std::move(message));
}

void GameInstance::sendGlobalMessage(std::string message) {
  server->sendMessageToRoom(*room, std::move(message));
}

std::deque<AST::PlayerMessage> GameInstance::receiveFromPlayer(int playerId) {
  std::deque<AST::PlayerMessage> messages;
  auto it = std::partition(
      inboundMessageQueue.begin(), inboundMessageQueue.end(),
      [playerId](auto &message) { return playerId != message.playerId; });
  std::ranges::move(it, inboundMessageQueue.end(),
                    std::back_inserter(messages));
  inboundMessageQueue.erase(it, inboundMessageQueue.end());
  playerMessageRequest.at(playerId) = messages.empty();
  return messages;
}

std::deque<AST::PlayerMessage> GameInstance::receiveFromOwner() {
  return receiveFromPlayer(ownerId);
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
  inboundMessageQueue.push_back({playerId, std::move(message)});
  return true;
}

coro::Task<> GameInstance::loadGame(AST::AST &ast, AST::Configurator &config) {
  auto &players = this->getPlayers();
  auto toAstPlayer = [](auto &player) {
    return AST::Player{player.id, player.name};
  };
  auto playersTran = players | std::views::transform(toAstPlayer);
  auto env = co_await config.populateEnvironment(
      {playersTran.begin(), playersTran.end()}, *this);
  this->interpreter = std::make_unique<AST::Interpreter>(std::move(env), *this);
  co_await ast.accept(*(this->interpreter));
}

void GameInstance::resumeGame() {
  if (!gameTask.isDone()) {
    gameTask.resume();
  }
}

void GameInstance::startGame(AST::AST &ast, AST::Configurator &config,
                             const User &owner) {
  ownerId = reversePlayerIdMapping.at(owner.getId());
  gameTask = loadGame(ast, config);
  resumeGame();
}
