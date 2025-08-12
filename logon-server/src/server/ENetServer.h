#pragma once

#include <BaseApp.h>

enum { 
  NET_MESSAGE_UNKNOWN = 0, 
  NET_MESSAGE_SERVER_HELLO, 
  NET_MESSAGE_GENERIC_TEXT, 
  NET_MESSAGE_GAME_MESSAGE, 
  NET_MESSAGE_GAME_PACKET, 
  NET_MESSAGE_ERROR, 
  NET_MESSAGE_TRACK, 
  NET_MESSAGE_CLIENT_LOG_REQUEST, 
  NET_MESSAGE_CLIENT_LOG_RESPONSE
};

/**
 * ENetServer
 * Class for managing ENet-based server functionality
 * 
 * This class provides a wrapper around the ENet library to handle
 * server operations including initialization, service handling,
 * and network communication management.
 * 
 * Example usage:
 * @code
 * if (enet_initialize() != 0) {
 *     std::cerr << "Failed to initialize ENet!" << std::endl;
 *     return EXIT_FAILURE;
 * }
 * 
 * // Create server address
 * ENetAddress address;
 * enet_address_set_host(&address, "127.0.0.1");
 * address.port = 12345;
 * 
 * // Initialize server
 * ENetServer server(address);
 * server.set_max_peer(100);
 * server.set_max_incoming_bandwidth(0); // Unlimited
 * server.set_max_outgoing_bandwidth(0); // Unlimited
 * 
 * // Start server service loop
 * server.service();
 * 
 * enet_deinitialize();
 * @endcode
 */
class ENetServer {
private:
  ENetHost* m_server = nullptr;
  ENetAddress m_address;
  bool m_paused = false;
  size_t m_max_peer = 1024;
  enet_uint32 m_max_incoming_bandwidth = 0;   /** <- 0 for unlimited bandwidth */
  enet_uint32 m_max_outgoing_bandwidth = 0;   /** <- 0 for unlimited bandwidth */
  std::thread m_service_thread;

public:
  /**
   * Constructor for ENetServer
   * 
   * @param address - The network address to bind the host to server
   * 
   * Example:
   * @code
   * ENetAddress address;
   * enet_address_set_host(&address, "127.0.0.1");
   * address.port = 8080;
   * ENetServer server(address);
   * @endcode
   */
  ENetServer(const ENetAddress& address);

  /**
   * Destructor for ENetServer
   * Cleans up and shuts down the ENet server resources
   */
  ~ENetServer();

  /**
   * Start server service
   * 
   * Example:
   * @code
   * server.service()->join(); // Start listening for connections
   * @endcode
   */
  std::thread* service();

  /**
   * Set pause state for the server
   * 
   * @param status true for pause, false to resume
   * 
   * Example:
   * @code
   * server.set_pause(true);  // Pause
   * server.set_pause(false); // Resume
   * @endcode
   */
  void set_pause(const bool& status) { m_paused = status; }

  /**
   * Check if server is paused
   * 
   * Example:
   * @code
   * if (server.is_paused()) {
   *     std::cout << "Server paused" << std::endl;
   * }
   * @endcode
   */
  const bool& is_paused() const { return m_paused; }

  /**
   * Set maximum number of peers
   * 
   * @param amount the maximum number of peers that should be allocated for the host.
   * 
   * Example:
   * @code
   * server.set_max_peer(500);
   * @endcode
   */
  void set_max_peer(size_t amount) { m_max_peer = amount; }

  /**
   * Get maximum number of peers
   * 
   * Example:
   * @code
   * size_t maxPeers = server.get_max_peer();
   * std::cout << "Max peers: " << maxPeers << std::endl;
   * @endcode
   */
  const size_t& get_max_peer() const { return m_max_peer; }

  /**
   * Set maximum incoming bandwidth
   * 
   * @param amount downstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
   * 
   * Example:
   * @code
   * server.set_max_incoming_bandwidth(102400); // 100 KB/s
   * server.set_max_incoming_bandwidth(0);      // Unlimited
   * @endcode
   */
  void set_max_incoming_bandwidth(enet_uint32 amount) { m_max_incoming_bandwidth = amount; }

  /**
   * Get maximum incoming bandwidth
   * 
   * Example:
   * @code
   * enet_uint32 bw = server.get_max_incoming_bandwidth();
   * if (bw == 0) std::cout << "Unlimited incoming bandwidth" << std::endl;
   * @endcode
   */
  const enet_uint32& get_max_incoming_bandwidth() const { return m_max_incoming_bandwidth; }

  /**
   * Set maximum outgoing bandwidth
   * 
   * @param amount upstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
   * 
   * Example:
   * @code
   * server.set_max_outgoing_bandwidth(51200); // 50 KB/s
   * server.set_max_outgoing_bandwidth(0);     // Unlimited
   * @endcode
   */
  void set_max_outgoing_bandwidth(enet_uint32 amount) { m_max_outgoing_bandwidth = amount; }

  /**
   * Get maximum outgoing bandwidth
   * 
   * Example:
   * @code
   * enet_uint32 bw = server.get_max_outgoing_bandwidth();
   * if (bw == 0) std::cout << "Unlimited outgoing bandwidth" << std::endl;
   * @endcode
   */
  const enet_uint32& get_max_outgoing_bandwidth() const { return m_max_outgoing_bandwidth; }

public:
  /**
   * Get host IP as string
   * 
   * Example:
   * @code
   * ENetAddress clientAddr;
   * std::string ip = server.get_host_ip(&clientAddr);
   * std::cout << "Client IP: " << ip << std::endl;
   * @endcode
   */
  static std::string get_host_ip(ENetAddress* address);
  static char* get_packet_text(ENetPacket* pkt);
  static int get_packet_type(ENetPacket* pkt);
};
