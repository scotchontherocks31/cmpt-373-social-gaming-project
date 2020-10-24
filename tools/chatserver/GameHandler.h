#pragma once

#include "ASTVisitor.h"
#include "Room.h"
#include <list>
#include <map>
#include <string>

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
  Player *player;
  std::string message;
};

class GameHandler {
public:
  GameHandler(Room &room, GameServer &server);
  void loadGame(AST::AST &ast);
  void runGame();
  bool isRunning() { return !gameTask.isDone(); }

  /// Only allow new message to be queued when the game requests it.
  /// Returns false if fails to queue message.
  bool queueMessage(const User &user, const std::string &message);

  /// Send ouput message to a player
  void sendToPlayer(const Player &player, std::string message);

  /// Send output message to all players in the room
  void sendToAllPlayers(std::string message);

  /// Get messages from a player.
  /// Returns empty deque if no message is available.
  std::deque<PlayerMessage> receiveFromPlayer(const Player &player);

  /// Get info about players in the room
  const std::vector<Player> &getPlayers() const { return players; }

  bool isGameUnused() const { return room->getMembers().empty(); }

private:
  Room *room;
  GameServer *server;
  AST::Communication bridge;
  AST::Interpreter interpreter;
  std::map<int, bool> playerMessageRequest;
  std::vector<Player> players;
  std::map<int, userid> playerIdMapping;
  std::map<userid, int> reversePlayerIdMapping;
  std::list<PlayerMessage> inboundMessageQueue;
  coro::Task<void> gameTask;
};
