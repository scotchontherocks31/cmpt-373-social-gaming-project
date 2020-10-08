#pragma once

#include "Room.h"
#include <map>
#include <string>
#include <list>

class GameServer;
class GameManager;

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

  /// Send ouput message to a player
  void sendToPlayer(const Player &player, std::string message);

  /// Send output message to all players in the room
  void sendToAllPlayers(std::string message);

  /// Block until recieve message from a specific player
  PlayerMessage recieveFromPlayer(const Player &player);

  /// Block until recieve message from all players in the room
  std::vector<PlayerMessage> recieveFromAllPlayers();

  /// Get info about players in the room
  const std::vector<Player> &getPlayers() const { return players; }

  /// Invoked when number of players changed
  // void onPlayersChanged();

private:
  Room &room;
  GameServer &server;
  int playerCounter = 0;
  std::vector<Player> players;
  std::map<int, userid> playerIdMapping;
  std::map<userid, int> reversePlayerIdMapping;
  std::list<PlayerMessage> inboundMessageQueue;
  GameHandler(Room &room, GameServer &server);
  friend class GameManager;
};
