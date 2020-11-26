#pragma once

#include "ASTVisitor.h"
#include "Config.h"
#include "Parser.h"
#include "Room.h"
#include <list>
#include <map>
#include <memory>
#include <string>

class GameServer;
class GameManager;

/*
TODO: Consider making message context aware so that a random message
from player doesn't get mixed with an actual response.
*/

struct Player {
  const int id;
  std::string name;
};

class GameInstance : public AST::Communicator {
public:
  GameInstance(Room &room, GameServer &server);
  void startGame(AST::AST &ast, AST::Configurator &config, const User &user);
  void resumeGame();
  bool isRunning() { return !gameTask.isDone(); }

  /// Only allow new message to be queued when the game requests it.
  /// Returns false if fails to queue message.
  bool queueMessage(const User &user, std::string message);

  /// Send ouput message to a player
  void sendToPlayer(int playerId, std::string message);

  void sendToOwner(std::string message) override;

  void sendGlobalMessage(std::string message) override;

  /// Get messages from a player.
  /// Returns empty deque if no message is available.
  std::deque<AST::PlayerMessage> receiveFromPlayer(int playerId);

  std::deque<AST::PlayerMessage> receiveFromOwner() override;

  /// Get info about players in the room
  const std::vector<Player> &getPlayers() const { return players; }

  bool isGameUnused() const { return room->getMembers().empty(); }

private:
  coro::Task<> loadGame(AST::AST &ast, AST::Configurator &config);
  coro::Task<AST::PopulatedEnvironment>
  populateEnvironment(AST::Configurator &config);
  bool waitingForUserInput() const;
  Room *room;
  GameServer *server;
  int ownerId = 0;
  std::map<int, bool> playerMessageRequest;
  std::vector<Player> players;
  std::map<int, userid> playerIdMapping;
  std::map<userid, int> reversePlayerIdMapping;
  std::list<AST::PlayerMessage> inboundMessageQueue;
  coro::Task<> gameTask;
  std::unique_ptr<AST::ASTVisitor> interpreter;
};
