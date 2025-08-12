#include "PacketVariant.h"
#include <utils/Utils.h>

PacketVariant::PacketVariant (int delay , int NetID) {
  len = 61;
  int MessageType = 0x4 , PacketType = 0x1 , CharState = 0x8;
  memset ( packet_data , 0 , 61 );
  memcpy ( packet_data , &MessageType , 4 );
  memcpy ( packet_data + 4 , &PacketType , 4 );
  memcpy ( packet_data + 8 , &NetID , 4 );
  memcpy ( packet_data + 16 , &CharState , 4 );
  memcpy ( packet_data + 24 , &delay , 4 );
};
PacketVariant::~PacketVariant() {
  delete[ ] packet_data;
}
PacketVariant* PacketVariant::Insert ( std::string a ) {
  BYTE* data = new BYTE[ len + 2 + a.length ( ) + 4 ];
  memcpy ( data , packet_data , len );
  delete[ ] packet_data;
  packet_data = data;
  data[ len ] = index;
  data[ len + 1 ] = 0x2;
  int str_len = ( int ) a.length ( );
  memcpy ( data + len + 2 , &str_len , 4 );
  memcpy ( data + len + 6 , a.data ( ) , str_len );
  len = len + 2 + ( int ) a.length ( ) + 4;
  index++;
  packet_data[ 60 ] = index;
  return this;
}
PacketVariant* PacketVariant::Insert ( int a ) {
  BYTE* data = new BYTE[ len + 2 + 4 ];
  memcpy ( data , packet_data , len );
  delete[ ] packet_data;
  packet_data = data;
  data[ len ] = index;
  data[ len + 1 ] = 0x9;
  memcpy ( data + len + 2 , &a , 4 );
  len = len + 2 + 4;
  index++;
  packet_data[ 60 ] = index;
  return this;
}
PacketVariant* PacketVariant::Insert ( unsigned int a ) {
  BYTE* data = new BYTE[ len + 2 + 4 ];
  memcpy ( data , packet_data , len );
  delete[ ] packet_data;
  packet_data = data;
  data[ len ] = index;
  data[ len + 1 ] = 0x5;
  memcpy ( data + len + 2 , &a , 4 );
  len = len + 2 + 4;
  index++;
  packet_data[ 60 ] = index;
  return this;
}
PacketVariant* PacketVariant::Insert ( float a ) {
  BYTE* data = new BYTE[ len + 2 + 4 ];
  memcpy ( data , packet_data , len );
  delete[ ] packet_data;
  packet_data = data;
  data[ len ] = index;
  data[ len + 1 ] = 0x1;
  memcpy ( data + len + 2 , &a , 4 );
  len = len + 2 + 4;
  index++;
  packet_data[ 60 ] = index;
  return this;
}
PacketVariant* PacketVariant::Insert ( float a , float b ) {
  BYTE* data = new BYTE[ len + 2 + 8 ];
  memcpy ( data , packet_data , len );
  delete[ ] packet_data;
  packet_data = data;
  data[ len ] = index;
  data[ len + 1 ] = 0x3;
  memcpy ( data + len + 2 , &a , 4 );
  memcpy ( data + len + 6 , &b , 4 );
  len = len + 2 + 8;
  index++;
  packet_data[ 60 ] = index;
  return this;
}
PacketVariant* PacketVariant::Insert ( float a , float b , float c ) {
  BYTE* data = new BYTE[ len + 2 + 12 ];
  memcpy ( data , packet_data , len );
  delete[ ] packet_data;
  packet_data = data;
  data[ len ] = index;
  data[ len + 1 ] = 0x4;
  memcpy ( data + len + 2 , &a , 4 );
  memcpy ( data + len + 6 , &b , 4 );
  memcpy ( data + len + 10 , &c , 4 );
  len = len + 2 + 12;
  index++;
  packet_data[ 60 ] = index;
  return this;
}
void PacketVariant::CreatePacket ( ENetPeer* peer ) {
  if (!Utils::PeerValidation(peer)) return;

  ENetPacket* packet = enet_packet_create ( packet_data , len , 1 );
  enet_peer_send ( peer , 0 , packet );
}