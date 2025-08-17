#pragma once

#include <BaseApp.h>

#include <string>
#include <functional>
#include <unordered_map>

#include <enet/enet.h>

#include <utils/VariantList.h>
#include <utils/ConsoleInterface.h>
#include <utils/Utils.h>
#include <SDK/Proton/TextScanner.h>

#include <player/Player.h>
#include <server/DataManager.h>

#include "FunctionDef.h"

class NetMessageGenericTextHandler {
  private:
  static std::unordered_map<std::string, FnBody> handle;

  public:
  static int init();
  static void execute(ENetPeer* peer, TextScanner* pkt);

  private:
  static bool ltoken(ENetPeer* peer, TextScanner* pkt);
  static bool player_login(ENetPeer* peer, TextScanner* pkt);
  static bool join_server(ENetPeer* peer, TextScanner* pkt);
};