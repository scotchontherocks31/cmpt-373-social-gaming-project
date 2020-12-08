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
  auto populateTask = populateEnvironment(config);
  AST::PopulatedEnvironment env;
  while (not populateTask.isDone()) {
    env = co_await populateTask;
  }
  this->interpreter = std::make_unique<AST::Interpreter>(std::move(env), *this);
  auto interpretTask = ast.accept(*(this->interpreter));
  while (not interpretTask.isDone()) {
    co_await interpretTask;
  }
}

coro::Task<AST::PopulatedEnvironment>
GameInstance::populateEnvironment(AST::Configurator &config) {
  auto &players = this->getPlayers();
  auto toAstPlayer = [](auto &player) {
    return AST::Player{player.id, player.name};
  };
  auto playersTran = players | std::views::transform(toAstPlayer);
  auto populateTask = config.populateEnvironment(
      {playersTran.begin(), playersTran.end()}, *this);
  AST::PopulatedEnvironment env;
  while (not populateTask.isDone()) {
    env = co_await populateTask;
  }
  co_return env;
}

inline bool GameInstance::hasError() const {
  return interpreter ? interpreter->hasError() : false;
}

void GameInstance::resumeGame() {
  while (gameTask.resume() and not waitingForUserInput() and not hasError()) {
  }
  if (hasError()) {
    sendGlobalMessage("Game encountered an error and cannot continue.");
  }
}

void GameInstance::startGame(AST::AST &ast, AST::Configurator &config,
                             const User &owner) {
  ownerId = reversePlayerIdMapping.at(owner.getId());
  gameTask = loadGame(ast, config);
  resumeGame();
}

bool GameInstance::waitingForUserInput() const {
  return std::any_of(playerMessageRequest.cbegin(), playerMessageRequest.cend(),
                     [](auto &elem) { return elem.second; });
}
