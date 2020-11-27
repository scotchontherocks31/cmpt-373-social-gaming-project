#ifndef AST_CONFIG_H
#define AST_CONFIG_H

#include "Parser.h"
#include "Player.h"
#include "task.h"

using Json = nlohmann::json;

namespace AST {

class Configurator {
public:
  // clang-format off
  Configurator(ConfigParser configParser)
      : parser{std::move(configParser)}, 
        name{parser.parseName()},
        playerCount{parser.parsePlayerCount()},
        hasAudience{parser.parseHasAudience()}, 
        setup{parser.parseSetup()},
        perPlayer{parser.parsePerPlayer()},
        perAudience{parser.parsePerAudience()},
        variables{parser.parseVariables()}, 
        constants{parser.parseConstants()} {}
  // clang-format on
  Configurator(std::string json)
      : Configurator(ConfigParser{std::move(json)}) {}
  coro::Task<PopulatedEnvironment>
  populateEnvironment(std::vector<Player> players, Communicator &com);
  std::string getName() { return name; }
  std::pair<int, int> getPlayerCount() { return playerCount; }
  bool getHasAudience() { return hasAudience; }

private:
  bool isSetupValid();
  coro::Task<Json> populateSetup(Communicator &com);
  PopulatedEnvironment createEnvironment(Json setup,
                                         std::vector<Player> players);
  ConfigParser parser;
  std::string name;
  std::pair<int, int> playerCount;
  bool hasAudience;
  Json setup;
  Json perPlayer;
  Json perAudience;
  Json variables;
  Json constants;
};

} // namespace AST

#endif