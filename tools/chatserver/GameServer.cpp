#include "GameServer.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <iterator>
#include <sstream>
#include <unistd.h>
using networking::Connection;
using networking::Message;
using networking::Server;

// take owner command from client and return command split into tokens
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

  // Split string like so
  // `this is "a big string"` -> ["this", "is", "a big string"]
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

GameServer::GameServer(unsigned short port, std::string httpMessage)
    : server(
          port, httpMessage, [this](Connection c) { this->onConnect(c); },
          [this](Connection c) { this->onDisconnect(c); }),
      roomManager(), gameManager(*this, roomManager) {}

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
  while (true) {

    bool errorWhileUpdating = false;
    try {
      server.update();
    } catch (std::exception &e) {
      std::cerr << "Exception from Server update:\n"
                << " " << e.what() << "\n\n";
      errorWhileUpdating = true;
    }

    auto incoming = server.receive();
    bool shouldQuit = processMessages(server, incoming);
    flush();
    if (shouldQuit || errorWhileUpdating) {
      break;
    }

    sleep(1);
  }
}

bool GameServer::processMessages(Server &server,
                                 const std::deque<Message> &incoming) {
  std::vector<DecoratedMessage> outMessages;
  bool quit = false;
  bool isBroadcast = true;
  std::vector<userid> receiversId;

  for (auto &message : incoming) {
    receiversId.clear();
    std::ostringstream output;
    auto &user = getUser(message.connection);

    // Check if message is a command (e.g. /create)
    if (message.text[0] == '/') {
      // tokenizes command
      isBroadcast = false;
      receiversId.push_back(user.getId());
      auto tokens = tokenizeCommand(message.text.substr(1));

      if (tokens[0] == "quit") {
        // Disconnect from server
        server.disconnect(user.connection);
      }

      if (tokens[0] == "shutdown") {
        // Shut down the server
        std::cout << "Shutting down.\n";
        quit = true;
      }

      if (tokens[0] == "create") {
        // Create an empty room
        if (tokens.size() >= 2) {
          roomManager.createRoom(tokens[1]);
          output << "creating room " << tokens[1] << "...\n";
        } else {
          output << "please specify a name\n";
        }
      }

      if (tokens[0] == "join") {
        // Create an empty room
        if (tokens.size() >= 2) {
          roomManager.putUserToRoom(user, tokens[1]);
          output << "joining room "
                 << roomManager.getRoomFromUser(user).getName() << "...\n";
        }
      }

      if (tokens[0] == "leave") {
        output << "leaving room " << roomManager.getRoomFromUser(user).getName()
               << "...\n";
        roomManager.putUserToRoom(user, RoomManager::GLOBAL_ROOM_NAME);
      }

      if (tokens[0] == "list") {
        output << roomManager.listRoomsInfo();
      }

      if (tokens[0] == "info") {
        output << "Your id is: " << user.name << "\n"
               << "You are in room: "
               << roomManager.getRoomFromUser(user).getName() << "\n"
               << roomManager.getRoomFromUser(user).getCurrentSize() << "/"
               << roomManager.getRoomFromUser(user).getCapacity()
               << "\n"; // Global room Capacity not working
      }

      if (tokens[0] == "whisper") {
        bool userFound = false;
        if (tokens.size() >= 2) {
          // TODO: must change from string to uintptr_t (userid)
          // roomManager.getRoomFromUser(user).getParticipant()
          // receiversId.push_back(receiver);
        }
        if (userFound) {
          // Add receiversId.push_back()
        }
      }

      /* TODO: must send JSON room configuration from the user
      if (tokens[0] == "configure") {
        roomManager.configureRoom(user);
      }
      */

    } else {
      // If not a command then just output a message
      output << user.name << "> " << message.text << "\n";
    }
    outMessages.push_back(
        DecoratedMessage{user, output.str(), isBroadcast, receiversId});
  }

  for (auto &message : outMessages) {
    if (message.isBroadcast) {
      broadcast(message);
    } else {
      narrowcast(message);
    }
  }
  return quit;
}

// message to everyone in room
void GameServer::broadcast(const DecoratedMessage &message) {
  auto room = roomManager.getRoomFromUser(message.user);
  for (auto &&[_, user] : room.getMembers()) {
    outboundMessages.push_back({user->connection, message.text});
  }
}

// message to specific players
void GameServer::narrowcast(const DecoratedMessage &message) {
  for (userid userId : message.receiversId) {
    outboundMessages.push_back({getUser(userId).connection, message.text});
  }
}

void GameServer::flush() {
  if (outboundMessages.size()) {
    server.send(outboundMessages);
    outboundMessages.clear();
  }
}

void GameServer::sendMessageToUser(const User &user, std::string message) {
  outboundMessages.push_back({user.connection, std::move(message)});
}

void GameServer::sendMessageToRoom(const Room &room, std::string message) {
  for (auto &&[_, user] : room.getMembers()) {
    outboundMessages.push_back({user->connection, std::move(message)});
  }
}
