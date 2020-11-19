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

coro::Task<Environment>
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

Environment Configurator::createEnvironment(std::vector<Player> players) {
  auto env = Environment{nullptr};

  env.setBinding("configuration", DSLValue{setup});

  // add the current members into the game
  std::vector<DSLValue> playersDSL;

  for (auto &player : players) {
    Json playerJson;
    playerJson["id"] = player.getId();
    playerJson["name"] = player.getName();
    for (auto &[key, value] : perPlayer.items()) {
      playerJson[key] = value;
    }
    // add the DSLValue player to the DSL List
    playersDSL.push_back(DSLValue{playerJson});
  }

  env.setBinding("players", playersDSL);

  // add constants
  for (auto &[key, value] : constants.items()) {
    env.setBinding(key, value);
  }

  // add variables
  for (auto &[key, value] : variables.items()) {
    env.setBinding(key, value);
  }
  return env;
}

} // namespace AST