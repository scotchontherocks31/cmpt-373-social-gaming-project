#pragma once

#include <string>

struct MessageResult {
  std::string result;
  bool shouldShutdown;
};

struct Room {
	int id;
	std::string roomName;
	std::vector<User*> participants;
	// etc...
};

struct User {
	Connection connection;
	std::string name;
	Room* room;
	// etc...
};
