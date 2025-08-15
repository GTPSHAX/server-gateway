#include "BaseApp.h"

#include "server/ENetServer.h"
#include "server/handler/NetMessageGenericText.h"
#include "server/DataManager.h"

#include "utils/ConsoleInterface.h"

int main(int, char**){
  enet_initialize();

  // Intro cenutttt
  print_info("Developed by: @oxygenbro");
  ConsoleInterface::show_loading("Loading...", [&] {
    // Load semua data
    DataManager::load_all();
    // Generate random seed for srand by time
    std::srand(static_cast<unsigned int>(std::time(NULL)));
  });
  int temp_val = 0;
  ConsoleInterface::show_loading("Initialize NetMessageGenericText handler...", [&] {
    temp_val = NetMessageGenericTextHandler::init();
  });
  print_info("Loaded {} NetMessageGenericText handler.", temp_val);

  ENetAddress address;
  std::string ip = "0.0.0.0";
  enet_address_set_host(&address, ip.c_str());
  address.port = 17090;

  try {
    ENetServer app(address);
    app.service()->join();
  }
  catch (const std::runtime_error& e) {
    print_error("{}", e.what());
  }

  enet_deinitialize();
  return 0;
}
