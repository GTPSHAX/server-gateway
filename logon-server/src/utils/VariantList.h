#pragma once

#include <BaseApp.h>

class VariantList {
  public:
  // Send message to console chat
  static void OnConsoleMessage(ENetPeer* peer, const std::string& msg, int delay = 0);
};
