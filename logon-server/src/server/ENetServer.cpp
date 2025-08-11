#include "ENetServer.h"

#include <utils/ConsoleInterface.h>

ENetServer::ENetServer(const ENetAddress& address) {
  m_address = address;

  std::string host;
  try {
    host = get_host_ip(&m_address);
  }
  catch (const std::runtime_error& e) {
    print_error("{}", e.what());
    host = (char*)"null";
  }

  print_debug("New ENetServer created {}:{}", host, m_address.port);
}
ENetServer::~ENetServer() {
  #if IS_DEBUG
  std::string host;
  try {
    host = get_host_ip(&m_address);
  }
  catch (const std::runtime_error& e) {
    print_error("{}", e.what());
    host = (char*)"null";
  }
  print_warning("ENetServer destroyed {}:{}", host, m_address.port);
  #endif

  delete m_server;
  m_server = nullptr;
  m_address.host = 0;
  m_address.port = 0;
}