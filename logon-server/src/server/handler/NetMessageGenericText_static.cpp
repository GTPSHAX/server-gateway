#include "NetMessageGenericText.h"

std::unordered_map<std::string, FnBody> NetMessageGenericTextHandler::handle = {};

int NetMessageGenericTextHandler::init() {
  handle["ltoken"] = { PlayerRole::NONE, PlayerAttribute::NONE, &ltoken};
  handle["tankIDName"] = { PlayerRole::NONE, PlayerAttribute::NONE, &player_login};

  return handle.size();
}

void NetMessageGenericTextHandler::execute(ENetPeer* peer, TextScanner* pkt) {
  if (!Utils::PeerValidation(peer))
    return;

  std::string key = pkt->GetParmStringFromLine(0,0);
  if (key == "protocol") key = pkt->GetParmStringFromLine(1,0);

  // Securityyyyh anjay
  if (key != "ltoken" && key != "tankIDName" && !pClient->tData.contains("ltoken"))
    return;

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

bool NetMessageGenericTextHandler::player_login(ENetPeer* peer, TextScanner* pkt) {
  PlayerCredentials data = pClient->get_credentials();
  data.tankIDName = pkt->GetParmString("tankIDName", 1);
  data.tankIDPass = pkt->GetParmString("tankIDPass", 1);
  pClient->set_credentials(data);
  pClient->tData["ltoken"]["merchant_name"] = pkt->GetParmString("doorID", 1);

  try {
    std::string ctx = Utils::generate_world_offers(pClient);
    VariantList::OnRequestWorldSelectMenu(peer, ctx);
  }
  catch (const std::runtime_error& e) {
    VariantList::OnConsoleMessage(peer, fmt::format("`4Error`w: {}", e.what()));
    enet_peer_disconnect_later(peer, 0);
  }

  // std::string growId = pkt->GetParmString("tankIDName", 1);
  // std::string pass = pkt->GetParmString("tankIDPass", 1);
  // std::string session = pkt->GetParmString("UUIDToken", 1);
  // 
  // VariantList::OnConsoleMessage(peer, "`oTest redirect ke real server...");

  // const auto& sConfig = DataManager::get_server_config();
  // VariantList::SetHasGrowID(peer, 0, growId, pass);
  // VariantList::OnSendToServer(peer, 17092, "68.183.189.85", LoginMode::REDIRECT_LOGIN, session, growId, merchant, session);
  // enet_peer_disconnect_later(peer, 0);
  return 0;
}
bool NetMessageGenericTextHandler::ltoken(ENetPeer* peer, TextScanner* pkt) {
  std::string ltoken_ = Utils::base64_decode(pkt->GetParmString("ltoken", 1));
  pClient->tData["ltoken"]["_session"] = Utils::param_get_value("_session", ltoken_);
  pClient->tData["ltoken"]["merchant_name"] = Utils::param_get_value("merchant_name", ltoken_);

  std::string session = pClient->tData["ltoken"]["_session"].get<std::string>();
  std::string merchant = pClient->tData["ltoken"]["merchant_name"].get<std::string>();

  allowed_ip_temp.emplace_back(peer->address.host);

  PlayerCredentials data = pClient->get_credentials();
  data.tankIDName = Utils::param_get_value("growId", ltoken_);
  data.tankIDPass = Utils::param_get_value("password", ltoken_);
  pClient->set_credentials(data);

  if (merchant == "")
    pClient->tData["ltoken"]["merchant_name"] = "GTPS Gateway", merchant = pClient->tData["ltoken"]["merchant_name"].get<std::string>();
  VariantList::OnConsoleMessage(peer, "`oConnected on `w" + merchant + "``.");

  const auto& sConfig = DataManager::get_server_config();
  VariantList::SetHasGrowID(peer, 0, data.tankIDName, data.tankIDPass);
  VariantList::OnSendToServer(peer, sConfig.server_port, sConfig.server_ip, LoginMode::CLIENT_LOGIN, session, data.tankIDName, merchant, session);
  enet_peer_disconnect_later(peer, 0);
  return 0;
}