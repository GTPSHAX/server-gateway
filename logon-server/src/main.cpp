#include <iostream>

#include <BaseApp.h>
#include "server/ENetServer.h"

int main(int, char**){
  std::cout << "Hello, from logon-server!\n";
  enet_initialize();

  ENetAddress address;
  std::string ip = "127.0.0.1";
  enet_address_set_host(&address, ip.c_str());
  address.port = 17090;
  ENetServer app(address);
  std::cin.get();

  return 0;
}
