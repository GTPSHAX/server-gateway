#include <iostream>

#include <BaseApp.h>
#include "server/ENetServer.h"

#include "utils/ConsoleInterface.h"

#include "server/handler/NetMessageGenericText.h"

int main(int, char**){
  // Intro cenutttt
  print_info("Developed by: @oxygenbro");
  ConsoleInterface::show_loading("Initialize enet...", [&]{
    enet_initialize();
  });
  print_info("Enet initialized.");
  ConsoleInterface::show_loading("Loading...", [&] {
    // Generate random seed for srand by time
    std::srand(static_cast<unsigned int>(std::time(NULL)));
  });
  int temp_val = 0;
  ConsoleInterface::show_loading("Initialize NetMessageGenericText handler...", [&] {
    temp_val = NetMessageGenericTextHandler::init();
  });
  print_info("Loaded {} NetMessageGenericText handler.", temp_val);

  ENetAddress address;
  std::string ip = "127.0.0.1";
  enet_address_set_host(&address, ip.c_str());
  address.port = 17090;
  ENetServer app(address);
  std::cin.get();

  return 0;
}
