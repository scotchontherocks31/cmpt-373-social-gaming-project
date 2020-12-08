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

GameServer::ServerCommand GameServer::matchCommand(const std::string &command) {
  if (!strToCommandMap.contains(command)) {
    return GameServer::ServerCommand::UNKNOWN;
  }
  return strToCommandMap[command];
}

GameServer::GameServer(unsigned short port, std::string httpMessage,
                       BaseStringToServerCommandMap &serverMap,
                       BaseStringToGameCommandMap &gameMap)
    // recieves maps to translate strings
    // (possibly from any language)
    : server(
          port, std::move(httpMessage),
          [this](Connection c) { this->onConnect(c); },
          [this](Connection c) { this->onDisconnect(c); }),
      roomManager(), gameManager(*this, roomManager),
      strToCommandMap(std::move(serverMap.getMap())),
      strToGameCommandMap(std::move(gameMap.getMap())),
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
    auto &user = getUser(message.connection);

    // Check if message is a command (e.g. /create)
    if (message.text[0] == '/') {
      processCommand(user, message.text.substr(1));
    } else {
      // If not a command then just output a message
      std::ostringstream output;
      output << user.name + "> " + message.text + "\n";
      gameManager.dispatch(user, std::move(message.text));
      auto &room = roomManager.getRoomFromUser(user);
      sendMessageToRoom(room, output.str());
    }
    inboundMessages.pop_front();
  }
}

std::map<GameServer::ServerCommand, std::function<functionType>>
GameServer::initializeFunctionMap() {
  // some functions don't need the inputs, however all
  // of them take it for convinience of the user
  std::function<functionType> quitFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        this->server.disconnect(user.connection);
        sendMessageToUser(user, "Server disconnected\n");
      };
  std::function<functionType> shutdownFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        running = false;
        sendMessageToUser(user, "Shutting down\n");
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
        sendMessageToUser(user, output.str());
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
        sendMessageToUser(user, output.str());
      };
  std::function<functionType> leaveFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        roomManager.putUserToRoom(user, RoomManager::GLOBAL_ROOM_NAME);
        output << "Leaving room \""
               << roomManager.getRoomFromUser(user).getName() << "\"...\n";
        sendMessageToUser(user, output.str());
      };
  std::function<functionType> listFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        output << roomManager.listRoomsInfo();
        sendMessageToUser(user, output.str());
      };
  std::function<functionType> infoFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        std::ostringstream output;
        auto &room = roomManager.getRoomFromUser(user);
        output << "Your name is: " << user.name << "\n"
               << "You are in room: " << room.getName() << " ("
               << room.getCurrentSize() << "/" << room.getCapacity() << ")\n";
        sendMessageToUser(user, output.str());
      };
  std::function<functionType> gameFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        processGameCommand(user, tokens);
      };
  std::function<functionType> unknownFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        sendMessageToUser(user, "Unknown command.\n");
      };

  std::map<GameServer::ServerCommand, std::function<functionType>> theMap = {
      {GameServer::ServerCommand::QUIT, std::move(quitFunc)},
      {GameServer::ServerCommand::SHUTDOWN, std::move(shutdownFunc)},
      {GameServer::ServerCommand::CREATE, std::move(createFunc)},
      {GameServer::ServerCommand::JOIN, std::move(joinFunc)},
      {GameServer::ServerCommand::LEAVE, std::move(leaveFunc)},
      {GameServer::ServerCommand::LIST, std::move(listFunc)},
      {GameServer::ServerCommand::INFO, std::move(infoFunc)},
      {GameServer::ServerCommand::GAME, std::move(gameFunc)},
      {GameServer::ServerCommand::LEAVE, std::move(leaveFunc)},
      {GameServer::ServerCommand::UNKNOWN, std::move(unknownFunc)}};
  return theMap;
}

void GameServer::processCommand(User &user, std::string rawCommand) {
  // tokenize command
  auto tokens = tokenizeCommand(std::move(rawCommand));
  auto command = matchCommand(tokens[0]);

  auto &func = commandToFunctionMap[command];
  func(user, tokens);
}

std::map<GameServer::GameCommand, std::function<functionType>>
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
        sendMessageToUser(user, output.str());
      };
  std::function<functionType> startFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        if (tokens.size() < 3) {
          sendMessageToUser(user,
                            "Error. Start command requires 1 argument.\n");
        } else {
          std::ostringstream output;
          auto &instance = gameManager.getGameInstance(user);
          output << "Starting game \"" << tokens[2] << "\"\n";
          auto &[ast, config] = gameManager.getGame(tokens[2]);
          instance.startGame(ast, config, user);
          sendMessageToUser(user, output.str());
        }
      };
  std::function<functionType> cleanFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        sendMessageToUser(user, "Cleaning empty game instances.\n");
        gameManager.cleanEmptyGameInstances();
      };
  std::function<functionType> unknownFunc =
      [this](User &user, std::vector<std::string> &tokens) {
        sendMessageToUser(user, "Unkown game command.");
      };

  std::map<GameServer::GameCommand, std::function<functionType>> theMap = {
      {GameServer::GameCommand::CREATE, std::move(createFunc)},
      {GameServer::GameCommand::START, std::move(startFunc)},
      {GameServer::GameCommand::CLEAN, std::move(cleanFunc)},
      {GameServer::GameCommand::UNKNOWN, std::move(unknownFunc)}};
  return theMap;
}

GameServer::GameCommand
GameServer::matchGameCommand(const std::string &command) {
  if (!strToGameCommandMap.contains(command)) {
    return GameServer::GameCommand::UNKNOWN;
  }
  return strToGameCommandMap[command];
}

void GameServer::processGameCommand(User &user,
                                    std::vector<std::string> &tokens) {
  // TODO: rework this to use visitor pattern
  if (tokens.size() < 2) {
    sendMessageToUser(user, "Invalid command.\n");
  }
  auto command = matchGameCommand(tokens[1]);

  auto &func = commandToGameFunctionMap[command];
  func(user, tokens);
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
