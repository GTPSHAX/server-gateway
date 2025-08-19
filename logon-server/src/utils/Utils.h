#pragma once

#include <BaseApp.h>

#include <string>

#include <enet/enet.h>

#include <player/Player.h>
#include <SDK/Builders/DialogBuilder.h>

inline const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

namespace Utils {
  bool PeerValidation(ENetPeer* peer);
  void SendPacket(ENetPeer* peer, const int& num, char* data, const int& len);
  bool is_base64(unsigned char c);
  std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
  std::string base64_decode(std::string const& encoded_string);
  std::string param_get_value(const std::string& key, const std::string& data);
  std::string generate_world_offers(Player* player);
  void disconnect_peer(ENetPeer* peer);
  std::vector<std::string> split(const std::string& delimiter, const std::string& str);
  // Fungsi untuk memeriksa apakah GUID valid
  bool isValidGUID ( const std::string& guid );
  // Fungsi untuk memeriksa apakah alamat MAC valid
  bool isValidMACAddress ( const std::string& mac );
  std::string format_number(long long int number, bool add_comma = true, int max_digits = 0);
  void merchant_profile(ENetPeer* peer, nlohmann::json& tData);
  // Validasi text untuk path safety
  bool isPathSafeText(const std::string& text);
  // Bersihkan text dari karakter tidak diizinkan
  std::string sanitizePathText(const std::string& text);
  // Deteksi karakter berbahaya untuk path
  bool containsPathTraversal(const std::string& text);
  GameDialog DialogJoinMerchant(const std::string& name, const std::string& tankIDName, const std::string& tankIDPass, const std::string& message);
};
