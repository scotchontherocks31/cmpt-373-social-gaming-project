#ifndef AST_PLAYER_H
#define AST_PLAYER_H

#include "DSLValue.h"
#include <string>

namespace AST {

struct Player {
  int id;
  std::string name;
};

struct DSLPlayer : Player {
  DSLValue *handle;
};

class PlayerList {
private:
  std::map<int, DSLPlayer> playerMapping;

public:
  PlayerList() = default;
  PlayerList(std::vector<DSLPlayer> players) {
    for (auto &player : players) {
      playerMapping.insert({player.id, std::move(player)});
    }
  }
  std::optional<DSLPlayer *> at(int playerId) {
    if (playerMapping.count(playerId)) {
      return &playerMapping.at(playerId);
    }
    return {};
  }
  bool empty() const { return playerMapping.empty(); }
  auto size() const { return playerMapping.size(); }
};

} // namespace AST

#endif