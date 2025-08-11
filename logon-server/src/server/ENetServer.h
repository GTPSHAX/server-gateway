#pragma once

#include <BaseApp.h>

/**
 * ENetServer
 * Class for server management
 */
class ENetServer {
  private:
    ENetHost* m_server = nullptr;
    ENetAddress m_address;

  public:
    /**
     * Param:
     * - @address - Set server address
     */
    ENetServer(const ENetAddress& address);
    ~ENetServer();

  public:
    /**
     * Used for getting host in 
     * - @return - char host[16]
     */
    std::string get_host_ip(ENetAddress* address);
};