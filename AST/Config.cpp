#include "Config.h"
#include <algorithm>

using Json = nlohmann::json;

coro::Task<Json> getSetupValueFromOwner(const std::string &key, Json &value,
                                        AST::Communicator &com) {
  if (value.is_object() && value.contains("kind") && value.contains("prompt")) {
    com.sendToOwner(value["prompt"].get<std::string>());
    auto messages = com.receiveFromOwner();
    while (messages.empty()) {
      co_await coro::coroutine::suspend_always();
      messages = com.receiveFromOwner();
    }
    auto kind = value["kind"].get<std::string>();
    auto &data = messages[0].message;
    // TODO: Better type handling, prevent user from entering the wrong type
    // One way to do it: Use Json::parse() -> check type -> give error and ask
    // for a different value.
    if (kind == "integer") {
      int userValue = std::stoi(data);
      co_return Json{userValue};
    } else if (kind == "string") {
      co_return Json{data};
    } else if (kind == "boolean") {
      bool userValue = data.starts_with("true");
      co_return Json{userValue};
    } else {
      co_return nlohmann::json::parse(data);
    }
  }
  co_return std::move(value);
  // TODO: Handle overwrite default setup value
}

namespace AST {

bool Configurator::isSetupValid() {
  return std::none_of(setup.begin(), setup.end(),
                      [](auto &element) { return element.contains("kind"); });
}

coro::Task<PopulatedEnvironment>
Configurator::populateEnvironment(std::vector<Player> players,
                                  Communicator &com) {
  co_await populateSetup(com);
  co_return createEnvironment(std::move(players));
}

coro::Task<> Configurator::populateSetup(Communicator &com) {
  for (auto &[key, value] : setup.items()) {
    value = co_await getSetupValueFromOwner(key, value, com);
  }
}

PopulatedEnvironment
Configurator::createEnvironment(std::vector<Player> players) {
  auto envPtr = std::make_unique<Environment>();

  envPtr->allocate("configuration", Symbol{DSLValue{setup}});

  // add the current members into the game
  std::vector<DSLValue> playersDSL;
  for (auto &player : players) {
    Json playerJson;
    for (auto &[key, value] : perPlayer.items()) {
      playerJson[key] = value;
    }
    playerJson["id"] = player.id;
    playerJson["name"] = player.name;
    playersDSL.push_back(DSLValue{playerJson});
  }

  envPtr->allocate("players", Symbol{DSLValue{playersDSL}});
  DSLValue &dsl = *envPtr->find("players");
  std::vector<DSLPlayer> playerBindings;
  for (int i = 0; i < dsl.size(); i++) {
    // Relying on DSLValue conversion to keep the ordering of players.
    // It's terrible but it's the only way to get both id and DSLValue pointer
    // at the same time.
    playerBindings.push_back(DSLPlayer{std::move(players[i]), &dsl[i]->get()});
  }

  // add constants
  for (auto &[key, value] : constants.items()) {
    envPtr->allocate(key, Symbol{DSLValue{value}, true});
  }

  // add variables
  for (auto &[key, value] : variables.items()) {
    envPtr->allocate(key, Symbol{DSLValue{value}});
  }
  return {std::move(envPtr), PlayerList{playerBindings}};
}

} // namespace AST