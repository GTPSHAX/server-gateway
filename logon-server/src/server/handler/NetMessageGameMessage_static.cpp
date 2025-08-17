#include "NetMessageGameMessage.h"

#include <SDK/Builders/DialogBuilder.h>

std::unordered_map<std::string, FnBody> NetMessageGameMessageHandler::handle = {};

int NetMessageGameMessageHandler::init() {
  handle["exit"] = { PlayerRole::NONE, PlayerAttribute::NONE, &exit};
  handle["quit_to_exit"] = { PlayerRole::NONE, PlayerAttribute::NONE, &quit_to_exit};
  handle["join_request"] = { PlayerRole::NONE, PlayerAttribute::NONE, &join_request};
  handle["world_button"] = { PlayerRole::NONE, PlayerAttribute::NONE, &join_request};

  return handle.size();
}

void NetMessageGameMessageHandler::execute(ENetPeer* peer, TextScanner* pkt) {
  if (!Utils::PeerValidation(peer))
    return;

  std::string key = pkt->GetParmStringFromLine(0,0);
  if (pkt->GetLineCount() == 1 || "action" == key) key = pkt->GetParmStringFromLine(0,1);

  // Securityyyyh anjay
  if (!pClient->tData.contains("ltoken"))
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

bool NetMessageGameMessageHandler::join_request(ENetPeer* peer, TextScanner* pkt) {
  std::string buttonClicked = pkt->GetParmString("name", 1);

  // Cancel entering world
  VariantList::OnFailedToEnterWorld(peer);

  // if ("exit" == buttonClicked)
  //   Utils::disconnect_peer(peer);
  if (buttonClicked._Starts_with("page_")) {
    std::vector<std::string> data = Utils::split("_", buttonClicked);
    pClient->tData["OnRequestWorldSelectMenu"]["page"] = data.size() > 1 ? std::atoi(data[1].c_str()) : 0;
    quit_to_exit(peer, pkt);
  }
  else if (buttonClicked._Starts_with("name=")) {
    RoleManager roles = pClient->get_roles();
    std::string name = Utils::param_get_value("name", buttonClicked);
    std::string host = Utils::param_get_value("host", buttonClicked);
    std::string port = Utils::param_get_value("port", buttonClicked);
    std::string block_3rd_app = Utils::param_get_value("block_3rd_app", buttonClicked);
    bool detected = pClient->tData["using_3rd_app"]["status"].get<bool>();

    GameDialog ctx;
    ctx.SetDefaultColor('o');
    if (block_3rd_app == "true" && detected) {
      ctx.AddLabel(eDialogElementSizes::BIG, "`4Access Denied", eDialogElementDirections::CENTER)->AddSmallText("The system has detected suspicious behavior from your account. This server does not allow abnormal player activity.");

      if (roles.is_have_parent_role(PlayerRole::MERCHANT))
        ctx.AddSmallText("`9(Merchant account still can access the server)");

      ctx.AddSpacer(eDialogElementSizes::SMALL);
    }
    else {
      ctx.EmbedData("param", buttonClicked)->AddLabel(eDialogElementSizes::BIG, "`wConfirm action", eDialogElementDirections::CENTER)->AddSmallText(fmt::format("Connecting you to the {} server! The game might freeze for a moment while downloading data, so hang tight. If you want to switch servers, just log out from the Home screen and log back in.", name))->AddSpacer(eDialogElementSizes::SMALL);
    }

    if (roles.is_have_parent_role(PlayerRole::MERCHANT))
     ctx.AddSpacer(eDialogElementSizes::SMALL)->AddLabel(eDialogElementSizes::LARGE, "`wServer options:", eDialogElementDirections::LEFT)->AddTextInput("options_display_name", "New display name: ", "", 10)->AddSmallText("Show display name in server button (Server's list)")->AddTextInput("options_name", "Name: ", name, 10)->AddTextInput("options_host", "Host: ", host, 16)->AddTextInput("options_port", "Port: ", port, 5)->AddTextInput("options_color", "Button color (r,g,b,a) [example: 255,10,255,255]: ", "", 16)->AddSmallText("Hide this server from Server's list")->AddCheckbox("options_hide_server", "`9Hide server", false)->AddSmallText("Don't use any 3rd-app on this server >:")->AddCheckbox("options_block_3rd_app", "`4Block 3rd App", block_3rd_app == "true")->AddSmallText("The server will be temporarily disabled, and coin usage will be paused until it resumes.")->AddCheckbox("options_disable", "`4Disable", false)->AddSpacer(eDialogElementSizes::SMALL)->AddButton("options_delete", "`4Delete this server")->AddButton("apply", "Apply")->AddSpacer(eDialogElementSizes::SMALL);

    ctx.EndDialog("join_server", "Nevermind", "Continue");

    VariantList::OnDialogRequest(peer, ctx.Build());
  }
    
  return 0;
}
bool NetMessageGameMessageHandler::exit(ENetPeer* peer, TextScanner* pkt) {
  Utils::disconnect_peer(peer);
  return 0;
}
bool NetMessageGameMessageHandler::quit_to_exit(ENetPeer* peer, TextScanner* pkt) {
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

