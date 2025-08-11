#include "NetMessageGenericText.h"

std::unordered_map<std::string, std::function<bool(ENetPeer* peer, const std::string& pkt)>> NetMessageGenericTextHandler::handle = {};

int NetMessageGenericTextHandler::init() {
  handle["test"] = &test;

  return handle.size();
}

bool NetMessageGenericTextHandler::test(ENetPeer* peer, const std::string& pkt) {
  return 0;
}