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

#include <GlobalVar.h>

struct FnBody {
  PlayerRole required_role = PlayerRole::NONE;
  PlayerAttribute required_attribute = PlayerAttribute::NONE;
  std::function<bool(ENetPeer* peer, TextScanner* pkt)> Fn = NULL;
};

class NetMessageGenericTextHandler {
  private:
  static std::unordered_map<std::string, FnBody> handle;

  public:
  static int init();
  static void execute(ENetPeer* peer, TextScanner* pkt);

  private:
  static bool ltoken(ENetPeer* peer, TextScanner* pkt);
  static bool player_login(ENetPeer* peer, TextScanner* pkt);
};