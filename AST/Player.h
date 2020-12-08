#ifndef AST_PLAYER_H
#define AST_PLAYER_H

#include "DSLValue.h"
#include <algorithm>
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
    std::transform(
        players.begin(), players.end(),
        std::inserter(playerMapping, playerMapping.begin()), [](auto &player) {
          return std::pair<int, DSLPlayer>{player.id, std::move(player)};
        });
  }
  std::optional<std::reference_wrapper<DSLPlayer>> at(int playerId) {
    auto it = playerMapping.find(playerId);
    return it != playerMapping.end()
               ? it->second
               : std::optional<std::reference_wrapper<DSLPlayer>>{};
  }
  bool empty() const { return playerMapping.empty(); }
  auto size() const { return playerMapping.size(); }
};

} // namespace AST

#endif