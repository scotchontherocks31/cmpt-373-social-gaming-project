#include "Room.h"
#include "RoomManager.h"
#include <iostream>

#define DEFAULT_CAPACITY 8
#define MAX_CAPACITY 1000

Room::Room(roomid id, std::string roomName)
    : name(std::move(roomName)), id(id) {
      
        if(roomName != RoomManager::GLOBAL_ROOM_NAME){
            capacity = DEFAULT_CAPACITY;
        }
        else{
            capacity = MAX_CAPACITY;
        } 
    }

void Room::listParticipants() {
  for (std::pair<userid, std::reference_wrapper<User>> p : participants) {
    std::cout << p.second.get().getId() << std::endl;
  }
}
