#pragma once

#include "Room.h"
#include <list>
#include <map>
#include <string>

class GameServer;
class GameManager;
struct DecoratedMessage;

/*
TODO: Consider making message context aware so that a random message
from player doesn't get mixed with an actual response.
*/

struct Player {
  int id;
  std::string name;
};

struct PlayerMessage {
  const Player &player;
  std::string message;
};

class GameHandler {
public:
  GameHandler(Room &room, GameServer &server);
  void queueMessage(const DecoratedMessage &message);

  /// Send ouput message to a player
  void sendToPlayer(const Player &player, std::string message);

  /// Send output message to all players in the room
  void sendToAllPlayers(std::string message);

  /// Block until receive message from a specific player
  PlayerMessage receiveFromPlayer(const Player &player);

  /// Block until receive message from all players in the room
  std::vector<PlayerMessage> receiveFromAllPlayers();

  /// Get info about players in the room
  const std::vector<Player> &getPlayers() const { return players; }

  bool isGameUnused() const {
    return room.getParticipants().empty();
  }

  /// Invoked when number of players changed
  // void onPlayersChanged();

private:
  Room &room;
  GameServer &server;
  std::vector<Player> players;
  std::map<int, userid> playerIdMapping;
  std::map<userid, int> reversePlayerIdMapping;
  std::list<PlayerMessage> inboundMessageQueue;
  // std::list<InputRequest> inputRequestQueue;
};
