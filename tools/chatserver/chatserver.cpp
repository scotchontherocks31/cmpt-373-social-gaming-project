#include "GameServer.h"

#include <fstream>
#include <iostream>
#include <unistd.h>

std::string getHTTPMessage(const char *htmlLocation) {
  if (access(htmlLocation, R_OK) != -1) {
    std::ifstream infile{htmlLocation};
    return std::string{std::istreambuf_iterator<char>(infile),
                       std::istreambuf_iterator<char>()};

  } else {
    std::cerr << "Unable to open HTML index file:\n" << htmlLocation << "\n";
    std::exit(-1);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
              << "  e.g. " << argv[0] << " 4002 ./webchat.html\n";
    return 1;
  }

  unsigned short port = std::stoi(argv[1]);
  StringToCommandMap strToComm;
  // StringToCommandMap *ptrStrToComm;
  StringToGameCommandMap strToGameComm;
  // StringToGameCommandMap *ptrStrToGameComm;
  struct CommandMappings maps(ptrStrToComm, ptrStrToGameComm);
  GameServer server{port, getHTTPMessage(argv[2]), strToComm, strToGameComm};
  server.startRunningLoop();

  return 0;
}
