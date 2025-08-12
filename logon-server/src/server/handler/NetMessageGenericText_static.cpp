#include "NetMessageGenericText.h"

#include <utils/VariantList.h>
#include <utils/ConsoleInterface.h>
#include <utils/Utils.h>

#include <player/Player.h>

std::unordered_map<std::string, FnBody> NetMessageGenericTextHandler::handle = {};

int NetMessageGenericTextHandler::init() {
  handle["ltoken"] = { PlayerRole::NONE, PlayerAttribute::NONE, &ltoken};

  return handle.size();
}

void NetMessageGenericTextHandler::execute(ENetPeer* peer, TextScanner* pkt) {
  if (!Utils::PeerValidation(peer))
    return;

  std::string key = pkt->GetParmStringFromLine(0,0);
  if (key == "protocol") key = pkt->GetParmStringFromLine(1,0);

  const auto& it = handle.find(key);
  if (it != handle.end()) {
    RoleManager roles = pClient->get_roles();
    PlayerCredentials credentials = pClient->get_credentials();

    bool has_required_role = roles.has_role(it->second.required_role);
    bool has_required_attribute = roles.has_attribute(it->second.required_attribute);

    if (has_required_role && has_required_attribute) {
      bool response = true;
      try {
        response = it->second.Fn(peer, pkt);
      }
      catch (const std::runtime_error& e) {
        print_error("Error when player with {}:{}({}) executing {}", credentials.IPv4, peer->address.port, credentials.tankIDName, key);
        response = true;
      }

      if (response) {
        // ....
      }
      else {
        // Bisa dipake buat debug
      }
    }
  }
        

  return;
}

bool NetMessageGenericTextHandler::ltoken(ENetPeer* peer, TextScanner* pkt) {
  VariantList::OnConsoleMessage(peer, pkt->GetParmStringFromLine(1,0));
  std::string ltoken_ = Utils::base64_decode(pkt->GetParmString("ltoken", 1));
  std::cout << ltoken_ << std::endl;
  print_debug("ltoken: {}", pkt->GetParmString("ltoken", 1));

  return 0;
}