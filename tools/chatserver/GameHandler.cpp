#include "GameHandler.h"
#include "GameServer.h"
#include <chrono>
#include <thread>

GameHandler::GameHandler(Room &room, GameServer &server)
    : room{room}, server{server} {
  for (auto &pair : room.getParticipants()) {
    int counter = players.size();
    players.push_back({counter, pair.second.get().name});
    playerIdMapping.insert({counter, pair.first});
    reversePlayerIdMapping.insert({pair.first, counter});
  }
}

void GameHandler::sendToPlayer(const Player &player, std::string message) {
  auto userId = playerIdMapping.at(player.id);
  auto &user = room.getParticipant(userId);
  server.sendMessageToUser(user, std::move(message));
}

void GameHandler::sendToAllPlayers(std::string message) {
  server.sendMessageToRoom(room, std::move(message));
}

PlayerMessage GameHandler::receiveFromPlayer(const Player &player) {
  auto beginIt = inboundMessageQueue.begin();
  auto endIt = inboundMessageQueue.end();
  auto it = beginIt;
  while (true) {
    it = std::find_if(beginIt, endIt, [&player](PlayerMessage &message) {
      return player.id == message.player.id;
    });
    if (it == inboundMessageQueue.end()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      beginIt = inboundMessageQueue.begin();
      endIt = inboundMessageQueue.end();
    } else {
      break;
    }
  }

  auto message = std::move(*it);
  inboundMessageQueue.erase(it);
  return message;
}

std::vector<PlayerMessage> GameHandler::receiveFromAllPlayers() {
  std::vector<PlayerMessage> messages;
  for (auto &&player : players) {
    messages.push_back(receiveFromPlayer(player));
  }
  return messages;
}

void GameHandler::queueMessage(const DecoratedMessage &message) {
  auto playerId = reversePlayerIdMapping.at(message.user.getId());
  auto &player = players.at(playerId);
  inboundMessageQueue.push_back({player, message.text});
}