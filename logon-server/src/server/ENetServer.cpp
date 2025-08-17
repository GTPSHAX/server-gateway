#include "ENetServer.h"

#include "handler/NetMessageGameMessage.h"

#include <utils/CacheManager.h>

ENetServer::ENetServer(const ENetAddress& address): m_address(address) {
  std::string host = get_host_ip(&m_address);

  m_server = enet_host_create(&m_address, m_max_peer, 0, m_max_incoming_bandwidth, m_max_outgoing_bandwidth);
  if (m_server == nullptr) {
    throw std::runtime_error("Failed to creating enet server.");
  }
  m_server->checksum = enet_crc32;
  m_server->usingNewPacketForServer = 1;
  enet_host_compress_with_range_coder(m_server);

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
std::thread* ENetServer::service() {
  m_service_thread = std::thread([&] {
    ENetEvent event;
    std::string sIP = get_host_ip(&m_address);
    print_success("ENetServer started with {}:{}", sIP, m_address.port);

    while (true) {
      if (!m_server || m_server == nullptr) {
        throw std::runtime_error("Server is null pointer.");
      }
      if (m_paused) {
        continue;
      }
      
      if (enet_host_service(m_server, &event, 100) > 0) {
        ENetPeer* peer = event.peer;
        std::string pIP = get_host_ip(&peer->address);

        switch (event.type) {
          case ENET_EVENT_TYPE_CONNECT: {
            if (peer->data != NULL) {
              Utils::disconnect_peer(peer);
              break;
            }
            print_debug("[{}:{}] Peer with {}:{} connected to server.", sIP, m_address.port, pIP, peer->address.port);

            peer->data = new Player();
            PlayerCredentials data = pClient->get_credentials();
            data.IPv4 = pIP;
            pClient->set_credentials(data);
            
            enet_peer_timeout(peer, 5000, 3000, 10000);

            VariantList::OnConsoleMessage(peer, "`oValidating request...");
            enet_host_flush(m_server);
            
            Curl curl;
            curl.setUrl("http://localhost:8080/check-ip/" + pIP);
            curl.setTimeout((CacheManager::exists(pIP) ? 2 : 5));
            curl.setSSLVerification(false);

            if (curl.perform()) {
              try {
                nlohmann::json response = nlohmann::json::parse(curl.getResponseData());
                bool dc = false;
                if ((response.contains("status") && response["status"] == true) || (response.contains("is_vps") && response["is_vpn"] == true) || (response.contains("is_vps") && response["is_vps"] == true) || (response.contains("is_proxy") && response["is_proxy"] == true)) {
                  VariantList::OnConsoleMessage(peer, "`o`4Oops``: It appears you're using a `4prohibited third-party application`` or logging in with a `4prohibited address``. If this is a false alert, please contact the `0merchant owner`` or try login again.");
                  dc = true;
                }
                if (response.contains("presentence") && response["presentence"] >= 85) {
                  VariantList::OnConsoleMessage(peer, "`o`6Warning``: It appears you're using a `4prohibited third-party application`` or logging in with a `4prohibited address``.");
                }

                if (dc) {
                  Utils::disconnect_peer(peer);
                  break;
                }
              }
              catch (...) {}
            }

            Utils::SendPacket(peer, 1, nullptr, 0);
            break;
          }
          case ENET_EVENT_TYPE_RECEIVE: {
            std::string pkt_txt = get_packet_text(event.packet);
            TextScanner ctx(pkt_txt.c_str());
            int packet_type = get_packet_type(event.packet);
            print_debug("[{}:{}] Packet {} receive from Peer {}:{} >> {}", sIP, m_address.port, packet_type, pIP, peer->address.port, pkt_txt);

            switch(packet_type) {
              case NET_MESSAGE_GENERIC_TEXT: {
                NetMessageGenericTextHandler::execute(peer, &ctx);
                break;
              }
              case NET_MESSAGE_GAME_MESSAGE: {
                NetMessageGameMessageHandler::execute(peer, &ctx);
                break;
              }
              default: {
                print_warning("Unhandled net packet type: {} sended by peer {}:{}", packet_type, pIP, peer->address.host);
              }
            }

            enet_packet_destroy(event.packet);
            break;
          }
          case ENET_EVENT_TYPE_DISCONNECT: {
            print_debug("[{}:{}] Player with {}:{} disconnected from server.", 
                        sIP, m_address.port, pIP, peer->address.port);
            
            // Cleanup peer data
            if (peer->data) {
              delete peer->data;
              peer->data = NULL;
            }
            break;
          }
          default: {
            print_warning("Unknown event type rechived from peer {}:{}", pIP, peer->address.port);
            break;
          }
        }
      }
      CacheManager::cleanupExpired();
    }
  });

  return &m_service_thread;
}
