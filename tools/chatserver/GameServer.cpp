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
          [this](Connection c) { this->onDisconnect(c); }) {}

void GameServer::onConnect(Connection c) {
  std::cout << "New connection found: " << c.id << "\n";
  auto user = User{c, std::to_string(c.id)};
  users[c.id] = user;
  roomManager.putUserToRoom(user, 0);
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
    auto [log, shouldQuit] = processMessages(server, incoming);
    auto outgoing = buildOutgoing(log);
    server.send(outgoing);

    if (shouldQuit || errorWhileUpdating) {
      break;
    }

    sleep(1);
  }
}

MessageResult GameServer::processMessages(Server &server,
                                          const std::deque<Message> &incoming) {
  std::vector<DecoratedMessage> outMessages;
  bool quit = false;
  for (auto &message : incoming) {
    std::ostringstream log;
    /*
    message.connection is for identifying which client we are dealing with
    from the connection we can know which room that user is in, what their role,
    etc. We should have a method that abstract Connection away and just give us
    a User.
    */
    auto user = getUser(message.connection);

    // Check if message is a command (e.g. /create)
    if (message.text[0] == '/') {
      // // Parse the the command and get the tokens (e.g. /create Room1 RPS ->
      // ["create", "Room1", "RPS"])) std::vector tokens =
      // getCommand(message.text);

      // if (tokens[0] == "quit") {
      // 	// Disconnect from server
      // 	server.disconnect(user.connection);
      // }
      // if (tokens[0] == "shutdown") {
      // 	// Shut down the server
      // 	std::cout << "Shutting down.\n";
      // 	quit = true;
      // }
      // if (tokens[0] == "create") {
      // 	// Create an empty room
      // 	createRoom(user, tokens, log);
      // }
      // if (tokens[0] == "join") {
      // 	joinRoom(user, tokens, log);
      // }
      // if (tokens[0] == "leave") {
      // 	leaveRoom(user, tokens, log);
      // }
      // if (tokens[0] == "start") {
      // 	// Start the game
      // 	startGame(user, tokens, log);
      // }
      // if (tokens[0] == "end") {
      // 	// End the game early
      // 	endGame(user, tokens, log)
      // }
      // if (tokens[0] == "info") {
      // 	// Print info about the room and game
      // 	getInfo(user, tokens, log)
      // }
    } else {
      // If not a command then just output a message
      log << user.connection.id << "> " << message.text << "\n";
    }
    outMessages.push_back(DecoratedMessage{user, log.str()});
  }
  return MessageResult{outMessages, quit};
}

std::deque<Message>
GameServer::buildOutgoing(const std::vector<DecoratedMessage> &messages) {
  std::deque<Message> outgoing;
  for (auto &message : messages) {
    auto room = roomManager.getRoomFromUser(message.user);
    for (auto client : room.getParticipants()) {
      outgoing.push_back({client->connection, message.text});
    }
  }
  return outgoing;
}