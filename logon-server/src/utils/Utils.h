#pragma once

#include <BaseApp.h>

namespace Utils {
  bool PeerValidation(ENetPeer* peer);
  void SendPacket(ENetPeer* peer, const int& num, char* data, const int& len);
  bool is_base64(unsigned char c);
  std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
  std::string base64_decode(std::string const& encoded_string);
};
