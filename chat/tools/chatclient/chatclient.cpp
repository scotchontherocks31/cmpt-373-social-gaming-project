/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <unistd.h>

#include "ChatWindow.h"
#include "Client.h"


int
main(int argc, char* argv[]) {
  if (argc < 3) {
    return 1;
  }

  networking::Client client{argv[1], argv[2]}; // 4000 , webhtml passed

  bool done = false;
  
  auto onTextEntry = [&done, &client] (std::string text) {
    if ("exit" == text || "quit" == text) {
      done = true;
    } else {
      client.send(text);
    }
  };

  ChatWindow chatWindow(onTextEntry);

  while (!done && !client.isDisconnected()) {
    try {
      client.update();
    } catch (std::exception& e) {
      chatWindow.displayText("Exception from Client update:");
      chatWindow.displayText(e.what());
      
      done = true;
    }

    auto response = client.receive();
    if (!response.empty()) {
      
      chatWindow.displayText(response);
    }
    chatWindow.update();
  }

  return 0;
}

