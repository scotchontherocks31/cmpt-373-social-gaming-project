#ifndef AST_RUNTIME_H
#define AST_RUNTIME_H
#include "Environment.h"
#include "json.hpp"

using Json = nlohmann::json;

namespace AST {

class Runtime {
public:
    Environment createEnvironment() {
        return createEnvironmentHelper();
    }
private:
    virtual Environment createEnvironmentHelper() = 0;
};

class JsonRuntime : public Runtime {
public:
    JsonRuntime(const Json &json) : json{json} {}
private:
    virtual Environment createEnvironmentHelper() final;
    const Json &json;
};

Environment JsonRuntime::createEnvironmentHelper() {
    auto env = Environment{};
    size_t numPlayers = json["configuration"]["player count"]["max"];
    auto players = DSLValue{};
    for (auto i = 0; i < numPlayers; ++i) {
        players["player" + std::to_string(i)] = DSLValue{};
    }
    env.setBinding("Players", players);
    return env;
}

}


#endif
