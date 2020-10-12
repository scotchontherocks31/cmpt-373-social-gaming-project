#include "GameServer.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <unistd.h>

using networking::Connection;
using networking::Message;
using networking::Server;

GameServer::GameServer(unsigned short port, std::string httpMessage)
    : server(
          port, httpMessage, [this](Connection c) { this->onConnect(c); },
          [this](Connection c) { this->onDisconnect(c); }),
      gameManager(*this, roomManager) {}

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
    auto [output, shouldQuit] = processMessages(server, incoming);
    buildOutgoing(output);
    flush();

    if (shouldQuit || errorWhileUpdating) {
      break;
    }

    sleep(1);
  }
}

// take owner command from client and return command split into tokens
std::vector<std::string> GameServer::getCommand(const std::string &message) {
  std::vector<std::string> tokens;

  // remove \ from start of string
  std::string new_message = message.substr(1, message.size() - 1);

  // string split source:
  // http://www.martinbroadhurst.com/how-to-split-a-string-in-c.html
  std::stringstream ss(new_message);
  std::string token;
  while (std::getline(ss, token, ' ')) {
    tokens.push_back(token);
  }
  return tokens;
}

MessageResult GameServer::processMessages(Server &server,
                                          const std::deque<Message> &incoming) {
  std::vector<DecoratedMessage> outMessages;
  bool quit = false;

  for (auto &message : incoming) {
    std::ostringstream output;
    auto &user = getUser(message.connection);

    // Check if message is a command (e.g. /create)
    if (message.text[0] == '/') {
      // // Parse the the command and get the tokens (e.g. /create Room1 RPS ->
      // ["create", "Room1", "RPS"])) std::vector tokens =
      auto tokens = getCommand(message.text);

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
        if (tokens.size() == 2) {
          roomManager.createRoom(tokens[1]);
        } else {
          roomManager.createRoom("");
        }
      }

      if (tokens[0] == "join") {
        // Create an empty room
        if (tokens.size() == 2) {
          roomManager.putUserToRoom(user, tokens[1]);
        }
      }

      if (tokens[0] == "leave") {
        roomManager.removeUserFromRoom(user);
      }

      if (tokens[0] == "list") {
        roomManager.listRooms();
      }
      // if (tokens[0] == "start") {
      // 	// Start the game
      // 	startGame(user, tokens, output);
      // }
      // if (tokens[0] == "end") {
      // 	// End the game early
      // 	endGame(user, tokens, output)
      // }
      // if (tokens[0] == "info") {
      // 	// Print info about the room and game
      // 	getInfo(user, tokens, output)
      // }
    } else {
      // If not a command then just output a message
      output << user.name << "> " << message.text << "\n";
    }
    outMessages.push_back(DecoratedMessage{user, output.str()});
  }
  return MessageResult{outMessages, quit};
}

void GameServer::buildOutgoing(const std::vector<DecoratedMessage> &messages) {
  for (auto &message : messages) {
    auto room = roomManager.getRoomFromUser(message.user);
    for (auto &&[_, user] : room.getMembers()) {
      outboundMessages.push_back({user->connection, message.text});
    }
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