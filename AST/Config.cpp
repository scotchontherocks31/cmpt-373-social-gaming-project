#include "Config.h"
#include <algorithm>

using Json = nlohmann::json;

bool isJsonValueValid(const std::string &kind, const Json &json) {
  if (kind == "integer" && json.is_number_integer() ||
      kind == "string" && json.is_string() ||
      kind == "boolean" && json.is_boolean()) {
    return true;
  }
  if (json.is_object() && json.contains("type")) {
    // TODO: Check the schema for question-answer" and "multiple-choice"
    if (kind == "question-answer" && json.at("type") == "question-answer") {
      return true;
    }
    if (kind == "multiple-choice" && json.at("type") == "multiple-choice") {
      return true;
    }
  }
  return false;
}

coro::Task<Json> getSetupValueFromOwner(Json value, AST::Communicator &com) {
  if (value.is_object() && value.contains("kind") && value.contains("prompt")) {
    while (true) {
      com.sendToOwner(value["prompt"].get<std::string>());
      auto messages = com.receiveFromOwner();
      while (messages.empty()) {
        co_await coro::coroutine::suspend_always();
        messages = com.receiveFromOwner();
      }
      auto kind = value["kind"].get<std::string>();
      auto &message = messages[0].message;
      Json value;
      if (Json::accept(message)) {
        value = Json::parse(message);
      } else {
        value = Json(message);
      }
      if (isJsonValueValid(kind, value)) {
        co_return value;
      } else {
        com.sendToOwner("Invalid value type entered! Please try again.");
      }
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
  auto task = populateSetup(com);
  Json localSetup;
  while (not task.isDone()) {
    localSetup = co_await task;
  }
  co_return createEnvironment(std::move(localSetup), std::move(players));
}

coro::Task<Json> Configurator::populateSetup(Communicator &com) {
  auto localSetup = this->setup[0]; // Copy setup
  for (auto &[key, value] : localSetup.items()) {
    auto task = getSetupValueFromOwner(std::move(value), com);
    while (not task.isDone()) {
      value = co_await task;
    }
  }
  co_return localSetup;
}

PopulatedEnvironment
Configurator::createEnvironment(Json setup, std::vector<Player> players) {
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
  return {std::move(envPtr), PlayerList{std::move(playerBindings)}};
}

} // namespace AST