#pragma once

#include "Server.h"

typedef uintptr_t userid;

struct User {
  networking::Connection connection;
  std::string name;
  // etc...
  bool operator==(const User &other) const {
    return connection == other.connection;
  }
  userid getId() const { return connection.id; }
};
