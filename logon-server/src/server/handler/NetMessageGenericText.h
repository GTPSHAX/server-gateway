#pragma once

#include <BaseApp.h>

#include <SDK/Proton/TextScanner.h>

#include <player/RoleDef.h>

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
};