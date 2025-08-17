#include "NetMessageGenericText.h"

#include <utils/CacheManager.h>

std::unordered_map<std::string, FnBody> NetMessageGenericTextHandler::handle = {};

int NetMessageGenericTextHandler::init() {
  handle["ltoken"] = { PlayerRole::NONE, PlayerAttribute::NONE, &ltoken};
  handle["tankIDName"] = { PlayerRole::NONE, PlayerAttribute::NONE, &player_login};
  handle["join_server"] = { PlayerRole::NONE, PlayerAttribute::NONE, &join_server};

  return handle.size();
}

void NetMessageGenericTextHandler::execute(ENetPeer* peer, TextScanner* pkt) {
  if (!Utils::PeerValidation(peer))
    return;

  std::string key = pkt->GetParmStringFromLine(0,0);
  if (key == "protocol") key = pkt->GetParmStringFromLine(1,0);
  else if (pkt->GetLineCount() == 1) key = pkt->GetParmStringFromLine(0,1);
  else if (pkt->GetParmString("dialog_name", 1) != "") key = pkt->GetParmString("dialog_name", 1);

  // Securityyyyh anjay
  if (key != "ltoken" && key != "tankIDName" && !pClient->tData.contains("ltoken"))
  {
    print_debug("Failed to access handler for key: {}", key);
    return;
  }

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
        VariantList::OnConsoleMessage(peer, e.what());
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

    return;
  }
  print_warning("Unhandled key {}", key);

  return;
}

bool NetMessageGenericTextHandler::join_server(ENetPeer* peer, TextScanner* pkt) {
  RoleManager roles = pClient->get_roles();
  std::string param = pkt->GetParmString("param", 1);
  if (param == "")
    return 1;

  std::string base_path = "../../../database/";
  std::string name = Utils::param_get_value("name", param);
  std::string host = Utils::param_get_value("host", param);
  std::string port = Utils::param_get_value("port", param);
  std::string block_3rd_app = Utils::param_get_value("block_3rd_app", param);
  std::string session = pClient->tData["ltoken"]["_session"].get<std::string>();
  std::string merchant = pClient->tData["ltoken"]["merchant_name"].get<std::string>();
  bool detected = pClient->tData["using_3rd_app"]["status"].get<bool>();
  bool founded = false;
  int mCoin = 0;

  if (detected && !roles.is_have_parent_role(PlayerRole::MERCHANT))
    throw std::runtime_error("The system has detected suspicious behavior from your account. This server does not allow abnormal player activity.");

  // Fetch merchant data
  if (!std::filesystem::exists(base_path + "merchants/" + merchant + ".json")) 
    throw std::runtime_error(fmt::format("This merchant ({}) are not affiliated with us!", merchant));

  nlohmann::json mData = FileSystem2::readJson(base_path + "merchants/" + merchant + ".json");
  mCoin = mData["coin"].get<int>();

  if (std::filesystem::exists(base_path + "servers/" + mData["servers_key"].get<std::string>() + ".json"))  {
    nlohmann::json sData = FileSystem2::readJson(base_path + "servers/" + mData["servers_key"].get<std::string>() + ".json");

    if (sData.contains("servers") && sData["servers"].is_array()) {
      auto& servers = sData["servers"];
      
      int i = -1;
      for (auto& server : servers) {
        i++;

        // Check if expired 
        if (server["options"]["disable"].get<bool>())
          continue;
        if (server["name"].get<std::string>() != name)
          continue;

        if (roles.is_have_parent_role(PlayerRole::MERCHANT)) {
          std::string display_name_ = pkt->GetParmString("options_display_name", 1);
          if (display_name_ != "") server["display_name"] = display_name_;
          std::string name_ = pkt->GetParmString("options_name", 1);
          if (name_ != "") server["name"] = name_;
          std::string host_ = pkt->GetParmString("options_host", 1);
          if (host_ != "") server["host"] = host_;
          uint32_t port_ = pkt->GetParmUInt("options_port", 1);
          if (port_ != 0) server["port"] = port_;
          std::vector<std::string> color = Utils::split(",", pkt->GetParmString("options_color", 1));
          if (color.size() > 3) {
            server["options"]["color"]["red"] = std::atoi(color.at(0).c_str());
            server["options"]["color"]["green"] = std::atoi(color.at(1).c_str());
            server["options"]["color"]["blue"] = std::atoi(color.at(2).c_str());
            server["options"]["color"]["alpha"] = std::atoi(color.at(3).c_str());
          }
          bool options_hide_server = pkt->GetParmInt("options_hide_server", 1);
          server["options"]["hide_server"] = options_hide_server;
          bool block_3rd_app_ = pkt->GetParmInt("options_block_3rd_app", 1);
          server["options"]["block_3rd_app"] = block_3rd_app_;
          bool options_disable = pkt->GetParmInt("options_disable", 1);
          server["options"]["disable"] = options_disable;

          std::string buttonClicked = pkt->GetParmString("buttonClicked", 1);
          if (buttonClicked == "apply") {
            pkt->Replace("buttonClicked", "amboyyyy");
            try {
              std::string ctx = Utils::generate_world_offers(pClient);
              VariantList::OnRequestWorldSelectMenu(peer, ctx);
            }
            catch (const std::runtime_error& e) {
              VariantList::OnConsoleMessage(peer, fmt::format("`4Error`w: {}", e.what()));
              Utils::disconnect_peer(peer);
            }
            break;
          }
          if (buttonClicked == "options_delete") {
            servers.erase(i);
            break;
          }
        }
          
        TimePoint expired_at = std::chrono::steady_clock::time_point(std::chrono::seconds(server["expired_at"].get<long long>()));
        if (expired_at < current_time()) {
          // Perpanjang durasi
          if (mCoin > 0) {
            mCoin--;
            expired_at = current_time() + std::chrono::days(30);
            server["expired_at"] = std::chrono::duration_cast<std::chrono::seconds>(expired_at.time_since_epoch()).count();
            mData["coin"] = mCoin;
          }
          else {
            server["options"]["disable"] = true;
            break;
          }
        }
        founded = true;
        break;
      }
    }

    FileSystem2::writeJson(base_path + "servers/" + mData["servers_key"].get<std::string>() + ".json", sData);
    FileSystem2::writeJson(base_path + "merchants/" + merchant + ".json", mData);
  }

  if (founded) {
    VariantList::OnSendToServer(peer, std::atoi(port.c_str()), host, LoginMode::REDIRECT_LOGIN, session, pClient->get_credentials().tankIDName);
    param = Utils::split("&last_access=", param)[0];
    param += "&last_access=" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(current_time().time_since_epoch()).count());
    FileSystem2::writeFile(base_path + "sessions/" + session, param);
    Utils::disconnect_peer(peer);

    return 0;
  }

  VariantList::SetHasGrowID(peer, 0, "", "");
  VariantList::OnConsoleMessage(peer, fmt::format("`oOops! It looks like the `9{} ``service has been shut down. Please contact the merchant for more information.", name));
  return 1;
}
bool NetMessageGenericTextHandler::player_login(ENetPeer* peer, TextScanner* pkt) {
  std::string base_path = "../../../database/";
  PlayerCredentials data = pClient->get_credentials();
  data.tankIDName = pkt->GetParmString("tankIDName", 1);
  data.tankIDPass = pkt->GetParmString("tankIDPass", 1);
  pClient->set_credentials(data);
  pClient->tData["ltoken"]["_session"] = pkt->GetParmString("UUIDToken", 1);
  std::string session = pkt->GetParmString("UUIDToken", 1);
  pClient->tData["ltoken"]["merchant_name"] = pkt->GetParmString("doorID", 1);
  bool using_3rd_app = false;

  std::string mac = pkt->GetParmString("mac", 1);
  std::string gid = pkt->GetParmString("gid", 1);
  ePlatformType platformID = static_cast<ePlatformType>(pkt->GetParmUInt("platformID", 1));

  if (!Utils::isValidMACAddress(mac) || ((platformID != ePlatformType::PLATFORM_ID_IOS && platformID != ePlatformType::PLATFORM_ID_OSX && platformID != ePlatformType::PLATFORM_ID_WINDOWS && mac != "02:00:00:00:00:00") || (platformID == ePlatformType::PLATFORM_ID_WINDOWS && !pkt->Contain("fz", 1) && mac != "02:00:00:00:00:00"))) {
    pClient->tData["using_3rd_app"]["reason"] = "Invalid MAC";
    using_3rd_app = true;
  }
  if (pkt->GetParmStringFromLine(1, 0) == "tankIDPass" && pkt->GetParmStringFromLine(1, 1) == "requestedName" && pkt->GetParmStringFromLine(3, 0) != "requestedName") {
    pClient->tData["using_3rd_app"]["reason"] = "Invalid requestedName";
    using_3rd_app = true;
  }
  if (platformID == ePlatformType::PLATFORM_ID_WINDOWS && pkt->Contain("token", 1)) {
    pClient->tData["using_3rd_app"]["reason"] = "Invalid token";
    using_3rd_app = true;
  }
  if ((!Utils::isValidGUID(gid) && platformID == ePlatformType::PLATFORM_ID_ANDROID) || (platformID != ePlatformType::PLATFORM_ID_ANDROID) ) {
    pClient->tData["using_3rd_app"]["reason"] = "Invalid GID";
    using_3rd_app = true;
  }
  pClient->tData["using_3rd_app"]["status"] = using_3rd_app;

  if (using_3rd_app) {
    print_warning("Player with GrowID {} detected using 3rd app: {}", data.tankIDName, pClient->tData["using_3rd_app"]["reason"].get<std::string>());
    VariantList::OnConsoleMessage(peer, "`9Warning: Our system has detected that you're using a `4third-party application`9. Some servers may deny your access. If you believe this is a mistake, please contact the `omerchant owner`9.");
  }

  VariantList::SetHasGrowID(peer, 0, data.tankIDName, data.tankIDPass);

  // Use session
  if (std::filesystem::exists(base_path + "sessions/" + session)) {
    std::string param = FileSystem2::readFile(base_path + "sessions/" + session);
    std::string pkt_str = "action|dialog_return\ndialog_name|join_server\nparam|" + param + "|\n";
    TextScanner pckt(pkt_str.c_str());

    try {
      if (join_server(peer, &pckt))
        Utils::disconnect_peer(peer);
    }
    catch (const std::runtime_error& e) {
      VariantList::OnConsoleMessage(peer, e.what());
    }
    return 0;
  }

  try {
    std::string ctx = Utils::generate_world_offers(pClient);
    VariantList::OnRequestWorldSelectMenu(peer, ctx);
  }
  catch (const std::runtime_error& e) {
    VariantList::OnConsoleMessage(peer, fmt::format("`4Error`w: {}", e.what()));
    Utils::disconnect_peer(peer);
  }

  return 0;
}
bool NetMessageGenericTextHandler::ltoken(ENetPeer* peer, TextScanner* pkt) {
  std::string ltoken_ = Utils::base64_decode(pkt->GetParmString("ltoken", 1));
  pClient->tData["ltoken"]["_session"] = Utils::param_get_value("_session", ltoken_);
  pClient->tData["ltoken"]["merchant_name"] = Utils::param_get_value("merchant_name", ltoken_);

  std::string session = pClient->tData["ltoken"]["_session"].get<std::string>();
  std::string merchant = pClient->tData["ltoken"]["merchant_name"].get<std::string>();

  PlayerCredentials data = pClient->get_credentials();
  data.tankIDName = Utils::param_get_value("growId", ltoken_);
  data.tankIDPass = Utils::param_get_value("password", ltoken_);
  pClient->set_credentials(data);
  CacheManager::setWithTTL(data.IPv4, 1, 60000 * 5);

  if (merchant == "")
    pClient->tData["ltoken"]["merchant_name"] = "GTPS Gateway", merchant = pClient->tData["ltoken"]["merchant_name"].get<std::string>();
  VariantList::OnConsoleMessage(peer, "`oConnected on `w" + merchant + "``.");

  const auto& sConfig = DataManager::get_server_config();
  VariantList::SetHasGrowID(peer, 0, data.tankIDName, data.tankIDPass);
  VariantList::OnSendToServer(peer, sConfig.server_port, sConfig.server_ip, LoginMode::CLIENT_LOGIN, session, data.tankIDName, merchant, session);
  Utils::disconnect_peer(peer);
  return 0;
}