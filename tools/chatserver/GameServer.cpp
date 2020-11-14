#include "GameServer.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <unistd.h>
using networking::Connection;
using networking::Message;
using networking::Server;
using functionType = std::string(User &user, std::vector<std::string> &tokens);

/// Tokenize raw command string.
///
/// Use double quotes to include whitespace in a token:
/// this is "a big string" -> ["this", "is", "a big string"]
///
/// Everything after " -- " is considered a token:
/// json -- {"id": 1, "color": "red"} -> ["json", "{"id": 1, "color": "red"}"]
std::vector<std::string> tokenizeCommand(std::string command) {
  constexpr auto WHOLE_STRING_SEPARATOR = " -- ";
  std::vector<std::string> tokens;

  std::string wholeStr;
  auto separatorPos = command.find(WHOLE_STRING_SEPARATOR);
  if (separatorPos != std::string::npos) {
    auto beginWholeStr = separatorPos + strlen(WHOLE_STRING_SEPARATOR);
    wholeStr = command.substr(beginWholeStr);
    boost::algorithm::trim(wholeStr);
    command = command.substr(0, separatorPos);
  }
  boost::algorithm::trim(command);

  bool isQuote = false;
  auto pred = [&](char elem) -> bool {
    if (elem == ' ' && !isQuote) {
      return true;
    }
    if (elem == '\"') {
      isQuote = !isQuote;
      return true;
    }
    return false;
  };
  boost::algorithm::split(tokens, command, pred,
                          boost::algorithm::token_compress_on);
  if (tokens.back().empty()) {
    tokens.pop_back();
  }
  if (!wholeStr.empty()) {
    tokens.push_back(std::move(wholeStr));
  }
  return tokens;
}

std::map<std::string, GameServer::Command> GameServer::initializeCommandMap() {
  std::vector<std::string> keys{"quit",  "shutdown", "create", "join",
                                "leave", "list",     "info",   "game"};
  std::vector<GameServer::Command> values{
      GameServer::Command::QUIT,   GameServer::Command::SHUTDOWN,
      GameServer::Command::CREATE, GameServer::Command::JOIN,
      GameServer::Command::LEAVE,  GameServer::Command::LIST,
      GameServer::Command::INFO,   GameServer::Command::GAME};
  englishCommandMap theMap;
  theMap.initializeMap(keys, values);
  return theMap.getMap();
}

GameServer::Command GameServer::matchCommand(const std::string &command) {
  if (strToGameCommandMap.contains(command)) {
    return GameServer::Command::UNKNOWN;
  }
  return GameServer::strToCommandMap[command];
}

GameServer::GameServer(unsigned short port, std::string httpMessage)
    : server(
          port, std::move(httpMessage),
          [this](Connection c) { this->onConnect(c); },
          [this](Connection c) { this->onDisconnect(c); }),
      roomManager(), gameManager(*this, roomManager),
      strToCommandMap(initializeCommandMap()),
      strToGameCommandMap(initializeGameCommandMap()),
      commandToFunctionMap(initializeFunctionMap()),
      commandToGameFunctionMap(initializeGameFunctionMap()) {}

void GameServer::onConnect(Connection c) {
  std::cout << "New connection found: " << c.id << "\n";
  users[c.id] = User{c, std::to_string(c.id)};
  roomManager.putUserToRoom(users[c.id], RoomManager::GLOBAL_ROOM_NAME);
}

void GameServer::onDisconnect(Connection c) {
  std::cout << "Connection lost: " << c.id << "\n";
  roomManager.removeUserFromRoom(getUser(c.id));
  users.erase(c.id);
}

void GameServer::startRunningLoop() {
  running = true;
  while (running) {

    bool errorWhileUpdating = false;
    try {
      server.update();
    } catch (std::exception &e) {
      std::cerr << "Exception from Server update:\n"
                << " " << e.what() << "\n\n";
      errorWhileUpdating = true;
    }

    auto incoming = server.receive();
    if (!incoming.empty()) {
      std::move(incoming.begin(), incoming.end(),
                std::back_inserter(inboundMessages));
      processMessages();
      flush();
    }
    if (errorWhileUpdating) {
      break;
    }

    sleep(1);
  }
}

void GameServer::processMessages() {
  while (!inboundMessages.empty()) {
    auto &message = inboundMessages.front();
    bool isBroadcast = true;
    std::string output;
    auto &user = getUser(message.connection);

    // Check if message is a command (e.g. /create)
    if (message.text[0] == '/') {
      isBroadcast = false;
      output = processCommand(user, message.text.substr(1));
    } else {
      // If not a command then just output a message
      output = user.name + "> " + message.text + "\n";
      gameManager.dispatch(user, std::move(message.text));
    }

    if (isBroadcast) {
      auto &room = roomManager.getRoomFromUser(user);
      sendMessageToRoom(room, std::move(output));
    } else {
      sendMessageToUser(user, std::move(output));
    }
    inboundMessages.pop_front();
  }
}

std::map<GameServer::Command, std::function<functionType>>
GameServer::initializeFunctionMap() {
  std::function<functionType> quitFunc =
      [this](User &user,
             std::vector<std::string>
                 &tokens) { // some functions don't need the inputs, however all
                            // of them take it for convinience of the user
        std::ostringstream output;
        this->server.disconnect(user.connection);
        output << "Server disconnected\n";
        return output.str();
      };
  std::function<functionType> shutdownFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        running = false;
        output << "Shutting down\n";
        return output.str();
      };
  std::function<functionType> createFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        auto [roomPtr, created] =
            roomManager.createRoom(tokens.size() >= 2 ? tokens[1] : "");
        if (created) {
          output << "Creating room \"" << roomPtr->getName() << "\"...\n";
        } else {
          output << "Room already existed.\n";
        }
        return output.str();
      };
  std::function<functionType> joinFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        if (tokens.size() >= 2) {
          if (roomManager.putUserToRoom(user, tokens[1])) {
            output << "Joining room \""
                   << roomManager.getRoomFromUser(user).getName() << "\"...\n";
          } else {
            output << "Failed to join room.";
          }
        } else
          output << "Token size is less than 2!";
        return output.str();
      };
  std::function<functionType> leaveFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        roomManager.putUserToRoom(user, RoomManager::GLOBAL_ROOM_NAME);
        output << "Leaving room \""
               << roomManager.getRoomFromUser(user).getName() << "\"...\n";
        return output.str();
      };
  std::function<functionType> listFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        output << roomManager.listRoomsInfo();
        return output.str();
      };
  std::function<functionType> infoFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        auto &room = roomManager.getRoomFromUser(user);
        output << "Your name is: " << user.name << "\n"
               << "You are in room: " << room.getName() << " ("
               << room.getCurrentSize() << "/" << room.getCapacity() << ")\n";
        return output.str();
      };
  std::function<functionType> gameFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        output << processGameCommand(user, tokens);
        return output.str();
      };

  std::vector<GameServer::Command> keys{
      GameServer::Command::QUIT,   GameServer::Command::SHUTDOWN,
      GameServer::Command::CREATE, GameServer::Command::JOIN,
      GameServer::Command::LEAVE,  GameServer::Command::LIST,
      GameServer::Command::INFO,   GameServer::Command::GAME,
      GameServer::Command::LEAVE};
  std::vector<std::function<functionType>> values{
      quitFunc, shutdownFunc, createFunc, joinFunc, leaveFunc,
      listFunc, infoFunc,     gameFunc,   leaveFunc};
  gameServerFunctions theMap;
  theMap.initializeMap(keys, values);
  return theMap.getMap();
}

std::string GameServer::processCommand(User &user, std::string rawCommand) {
  // tokenize command
  auto tokens = tokenizeCommand(std::move(rawCommand));
  auto command = matchCommand(tokens[0]);

  std::function<functionType> func = commandToFunctionMap[command];
  std::string output = func(user, tokens);

  return output;
}

std::map<std::string, GameServer::Command>
GameServer::initializeGameCommandMap() {
  std::vector<std::string> keys{"create", "start", "clean"};
  std::vector<GameServer::Command> values{GameServer::Command::CREATE_GAME,
                                          GameServer::Command::START_GAME,
                                          GameServer::Command::CLEAN_GAME};
  englishCommandMap strToGameCommandMap;

  englishCommandMap theMap;
  theMap.initializeMap(keys, values);
  return theMap.getMap();
}

std::map<GameServer::Command, std::function<functionType>>
GameServer::initializeGameFunctionMap() {
  std::function<functionType> createFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        if (tokens.size() < 4) {
          output << "Error. Create command requires 2 arguments.\n";
        } else {
          output << "Creating game \"" << tokens[2] << "\"\n";
          gameManager.createGame(std::move(tokens[2]), std::move(tokens[3]));
        }
        return output.str();
      };
  std::function<functionType> startFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        if (tokens.size() < 3) {
          output << "Error. Start command requires 1 argument.\n";
        } else {
          auto &instance = gameManager.getGameInstance(user);
          output << "Starting game \"" << tokens[2] << "\"\n";
          instance.loadGame(gameManager.getGame(tokens[2]));
          instance.runGame();
        }
        return output.str();
      };
  std::function<functionType> cleanFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        output << "Cleaning empty game instances.\n";
        gameManager.cleanEmptyGameInstances();
        return output.str();
      };

  std::vector<GameServer::Command> keys{GameServer::Command::CREATE_GAME,
                                        GameServer::Command::START_GAME,
                                        GameServer::Command::CLEAN_GAME};
  std::vector<std::function<functionType>> values{createFunc, startFunc,
                                                  cleanFunc};
  gameServerFunctions theMap;
  theMap.initializeMap(keys, values);
  return theMap.getMap();
}

GameServer::Command GameServer::matchGameCommand(const std::string &command) {
  if (strToGameCommandMap.contains(command)) {
    return GameServer::Command::UNKNOWN_GAME;
  }
  return GameServer::strToGameCommandMap[command];
}

std::string GameServer::processGameCommand(User &user,
                                           std::vector<std::string> &tokens) {
  // TODO: rework this to use visitor pattern
  if (tokens.size() < 2) {
    return "Invalid command.\n";
  }
  auto command = matchGameCommand(tokens[1]);

  if (command == GameServer::Command::UNKNOWN_GAME) {
    return "Bad input\n";
  }

  std::function<functionType> func = commandToGameFunctionMap[command];
  std::string output = func(user, tokens);

  return output;
}

void GameServer::flush() {
  if (!outboundMessages.empty()) {
    server.send(outboundMessages);
    outboundMessages.clear();
  }
}

void GameServer::sendMessageToUser(const User &user, std::string message) {
  outboundMessages.push_back({user.connection, std::move(message)});
}

void GameServer::sendMessageToRoom(const Room &room, std::string message) {
  for (auto &&[_, user] : room.getMembers()) {
    outboundMessages.push_back({user->connection, message});
  }
}