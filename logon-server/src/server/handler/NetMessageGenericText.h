#pragma once

#include <BaseApp.h>

class NetMessageGenericTextHandler {
  private:
  static std::unordered_map<std::string, std::function<bool(ENetPeer* peer, const std::string& pkt)>> handle;

  public:
  static int init();

  private:
  static bool test(ENetPeer* peer, const std::string& pkt);
};