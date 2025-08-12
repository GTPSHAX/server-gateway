#include "VariantList.h"

#include <packet/PacketVariant.h>

void VariantList::OnConsoleMessage(ENetPeer* peer, const std::string& msg, int delay) {
  PacketVariant pkt(delay);
  pkt.Insert("OnConsoleMessage")->Insert(msg)->CreatePacket(peer);
}