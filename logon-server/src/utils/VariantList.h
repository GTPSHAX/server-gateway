#pragma once

#include <BaseApp.h>

#include <string>
#include <cstdint>

#include <enet/enet.h>

enum LoginMode {
  CLIENT_LOGIN = 0 ,
  REDIRECT_LOGIN = 1 ,
  REDIRECT_SUBSERVER = 2 ,
  REDIRECT_SUBSERVER_SILENT = 3 ,
};

class VariantList {
  public:
  // Send message to console chat
  static void OnConsoleMessage(ENetPeer* peer, const std::string& msg, int delay = 0);
  // Set account session
  static void SetHasGrowID(ENetPeer* peer, const bool& is_guest, const std::string& tankIDName, const std::string& tankIDPass);
  // Send peer to other server
  static void OnSendToServer(ENetPeer* peer, const int& port, const std::string& host, LoginMode mode, const std::string& session, const std::string& display_name,  const std::string doorID = "0", const std::string auth_data = "0");
};
