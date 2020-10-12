#include "GameHandler.h"
#include "GameServer.h"

GameHandler::GameHandler(Room &room, GameServer &server)
    : room{&room}, server{&server} {
  for (auto &[id, user] : room.getMembers()) {
    int counter = players.size();
    players.push_back({counter, user->name});
    playerIdMapping.insert({counter, id});
    reversePlayerIdMapping.insert({id, counter});
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
  auto it = inboundMessageQueue.begin();
  auto endIt = inboundMessageQueue.end();
  auto checkPlayerId = [&player](PlayerMessage &message) {
    return player.id == message.player->id;
  };
  do {
    it = std::find_if(it, endIt, checkPlayerId);
    if (it != endIt) {
      messages.push_back(std::move(*it));
      it = inboundMessageQueue.erase(it);
    }
  } while (it != endIt);
  return messages;
}

void GameHandler::queueMessage(const DecoratedMessage &message) {
  auto playerId = reversePlayerIdMapping.at(message.user.getId());
  auto &player = players.at(playerId);
  inboundMessageQueue.push_back({&player, message.text});
}