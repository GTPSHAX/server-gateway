#include "ENetServer.h"

std::string ENetServer::get_host_ip(ENetAddress* address) {
  // Konversi manual IP address
  uint32_t ip = address->host;
  unsigned char bytes[4];
  bytes[0] = ip & 0xFF;
  bytes[1] = (ip >> 8) & 0xFF;
  bytes[2] = (ip >> 16) & 0xFF;
  bytes[3] = (ip >> 24) & 0xFF;
  
  char host[16];
  snprintf(host, sizeof(host), "%d.%d.%d.%d", 
    bytes[0], bytes[1], bytes[2], bytes[3]);
  
  return std::string(host);
}
char* ENetServer::get_packet_text(ENetPacket* pkt) {
  char zero = 0;
  memcpy ( pkt->data + pkt->dataLength - 1 , &zero , 1 );
  return ( char* ) (pkt->data + 4);
}
int ENetServer::get_packet_type(ENetPacket* pkt) {
  if ( pkt->dataLength > 3u ) {
    return *(pkt->data);
  }
  return 0;
}