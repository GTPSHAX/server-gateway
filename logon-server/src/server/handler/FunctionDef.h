#pragma once

#include <functional>
#include <enet/enet.h>

#include <player/RoleDef.h>
#include <SDK/Proton/TextScanner.h>

struct FnBody {
  PlayerRole required_role = PlayerRole::NONE;
  PlayerAttribute required_attribute = PlayerAttribute::NONE;
  std::function<bool(ENetPeer* peer, TextScanner* pkt)> Fn = NULL;
};