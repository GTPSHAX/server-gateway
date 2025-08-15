#include "VariantList.h"

#include <packet/PacketVariant.h>

void VariantList::OnConsoleMessage(ENetPeer* peer, const std::string& msg, int delay) {
  PacketVariant pkt(delay);
  pkt.Insert("OnConsoleMessage")->Insert(msg)->CreatePacket(peer);
}
void VariantList::SetHasGrowID(ENetPeer* peer, const bool& is_guest, const std::string& tankIDName, const std::string& tankIDPass) {
  PacketVariant pkt;
  pkt.Insert("SetHasGrowID")->Insert((is_guest ? 0 : 1))->Insert(tankIDName)->Insert(tankIDPass)->CreatePacket(peer);
}
void VariantList::OnSendToServer(ENetPeer* peer, const int& port, const std::string& host, LoginMode mode, const std::string& session, const std::string& display_name, const std::string doorID, const std::string auth_data) {
  PacketVariant pkt;
  pkt.Insert("OnSendToServer")->Insert(port)->Insert(session)->Insert(peer->connectID)->Insert(host + "|" + doorID + "|" + auth_data)->Insert(static_cast<int>(mode))->Insert(display_name)->CreatePacket(peer);
}