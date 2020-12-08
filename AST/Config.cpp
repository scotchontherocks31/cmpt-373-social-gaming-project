#include "Config.h"
#include <algorithm>

using Json = nlohmann::json;

inline bool isValidQuestionAnswer(const std::string &kind, const Json &json) {
  // TODO: Check schema for "question-answer"
  return kind == "question-answer" and json.at("type") == "question-answer";
}

inline bool isValidMutipleChoice(const std::string &kind, const Json &json) {
  // TODO: Check schema for "multiple-choice"
  return kind == "multiple-choice" and json.at("type") == "multiple-choice";
}

bool isJsonValueValid(const std::string &kind, const Json &json) {
  if (kind == "integer" and json.is_number_integer() or
      kind == "string" and json.is_string() or
      kind == "boolean" and json.is_boolean()) {
    return true;
  }
  if (json.is_object() and json.contains("type")) {
    return isValidQuestionAnswer(kind, json) or
           isValidMutipleChoice(kind, json);
  }
  return false;
}

inline bool isValidPrompt(const Json &value) {
  return value.is_object() and value.contains("kind") and
         value.at("kind").is_string() and value.contains("prompt") and
         value.at("prompt").is_string();
}

coro::Task<Json> getSetupValueFromOwner(Json value,
                                        AST::Communicator &communicator) {
  if (isValidPrompt(value)) {
    while (true) {
      communicator.sendToOwner(value["prompt"].get<std::string>());
      auto messages = communicator.receiveFromOwner();
      while (messages.empty()) {
        co_await coro::coroutine::suspend_always();
        messages = communicator.receiveFromOwner();
      }
      auto kind = value["kind"].get<std::string>();
      auto &message = messages.front().message;
      auto value = Json::accept(message) ? Json::parse(message) : Json(message);
      if (isJsonValueValid(kind, value)) {
        co_return value;
      } else {
        communicator.sendToOwner(
            "Invalid value type entered! Please try again.");
      }
    }
  }
  co_return value;
  // TODO: Handle overwrite default setup value
}

namespace AST {

bool Configurator::isSetupValid() {
  return std::none_of(setup.begin(), setup.end(),
                      [](auto &element) { return element.contains("kind"); });
}

coro::Task<PopulatedEnvironment>
Configurator::populateEnvironment(std::vector<Player> players,
                                  Communicator &communicator) {
  auto task = populateSetup(communicator);
  Json localSetup;
  while (not task.isDone()) {
    localSetup = co_await task;
  }
  co_return createEnvironment(std::move(localSetup), std::move(players));
}

coro::Task<Json> Configurator::populateSetup(Communicator &communicator) {
  auto localSetup = this->setup[0]; // Copy setup
  for (auto &[key, value] : localSetup.items()) {
    auto task = getSetupValueFromOwner(std::move(value), communicator);
    while (not task.isDone()) {
      value = co_await task;
    }
  }
  co_return localSetup;
}

void allocateSetup(Environment &env, Json setup) {
  env.allocate("configuration", Symbol{DSLValue{std::move(setup)}});
}

std::vector<DSLPlayer> allocatePlayers(Environment &env,
                                       const std::vector<Player> &players,
                                       const Json &perPlayer) {
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

  env.allocate("players", Symbol{DSLValue{playersDSL}});
  DSLValue &dsl = *env.find("players");
  auto &dslList = dsl.get<List>()->get();
  std::vector<DSLPlayer> playerBindings;
  for (auto &playerDsl : dslList) {
    DSLValue &idDsl = *playerDsl["id"];
    DSLValue &nameDsl = *playerDsl["name"];
    int id = *idDsl.get<int>();
    std::string name = *nameDsl.get<std::string>();
    playerBindings.push_back(DSLPlayer{id, std::move(name), &playerDsl});
  }
  return playerBindings;
}

void allocateConstants(Environment &env, const Json &constants) {
  for (auto &[key, value] : constants.items()) {
    env.allocate(key, Symbol{DSLValue{value}, true});
  }
}

void allocateVariables(Environment &env, const Json &variables) {
  for (auto &[key, value] : variables.items()) {
    env.allocate(key, Symbol{DSLValue{value}});
  }
}

PopulatedEnvironment
Configurator::createEnvironment(Json setup, std::vector<Player> players) {
  auto envPtr = std::make_unique<Environment>();
  auto &env = *envPtr;

  allocateSetup(env, std::move(setup));
  auto playerBindings = allocatePlayers(env, players, this->perPlayer);
  allocateConstants(env, this->constants);
  allocateVariables(env, this->variables);

  return {std::move(envPtr), PlayerList{std::move(playerBindings)}};
}

} // namespace AST