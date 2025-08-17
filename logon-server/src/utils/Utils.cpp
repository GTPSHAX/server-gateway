#include "Utils.h"

#include <SDK/Builders/WorldOffersBuilder.h>
#include "ColorConverter.h"
#include "FileSystem2.h"
#include <regex>


bool Utils::PeerValidation(ENetPeer* peer) {
  return !(!peer || peer == nullptr || !peer->data || peer->data == NULL || peer->state != ENET_PEER_STATE_CONNECTED);
}
void Utils::disconnect_peer(ENetPeer* peer) {
  enet_peer_disconnect_later(peer, 0);
  if (peer->data != NULL) {
    delete peer->data;
    peer->data = NULL;
  }
}
std::vector<std::string> Utils::split(const std::string& delimiter, const std::string& str) {
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos) {
		result.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}

	// Tambahkan substring terakhir
	result.push_back(str.substr(start));

	return result;
}
std::string Utils::generate_world_offers(Player* player) {
  uint32_t default_color = ColorConverter::toBGRA(214,171,94,255);
  std::string merchant = player->tData["ltoken"]["merchant_name"].get<std::string>();
  std::string base_path = "../../../database/";
  std::string additional_msg = "";
  RoleManager pRole = player->get_roles();
  PlayerCredentials pCredentials = player->get_credentials();
  WorldOffersMenu ctx;
  bool dashboard_showed = false;
  bool hide_servers = false;
  int mCoin = 0;

  // Fetch merchant data
  if (!std::filesystem::exists(base_path + "merchants/" + merchant + ".json")) 
    throw std::runtime_error(fmt::format("This merchant ({}) are not affiliated with us!", merchant));

  nlohmann::json mData = FileSystem2::readJson(base_path + "merchants/" + merchant + ".json");

  if (pCredentials.tankIDName == mData["tankIDName"].get<std::string>() && pCredentials.tankIDPass == mData["tankIDPass"].get<std::string>()) {
    dashboard_showed = true;
    mCoin = mData["coin"].get<int>();

    // Set role
    std::string tRole = mData["role"].get<std::string>();
    if (tRole == "admin")
      pRole.add_role(PlayerRole::ADMIN);
    else if (tRole == "merchant")
      pRole.add_role(PlayerRole::MERCHANT);
    player->set_roles(pRole);

    // Kasih notif buat beli koin
    if (mCoin < 1) {
      additional_msg = "`9Warning`w: Your total coins are now `o0`w, don't forget to top up in `5Dashboard -> My profile -> Top up coins`w.";
    }
    hide_servers = mData["options"]["hide_servers"].get<bool>();
  }

  // testing only
  nlohmann::json tData = (player->tData.contains("OnRequestWorldSelectMenu") 
                          ? player->tData["OnRequestWorldSelectMenu"] 
                          : nlohmann::json());

  int tPage = 0;
  int req_page = (tData.contains("page") ? tData["page"].get<int>() : 0);
  int max_servers_page = 10;
  std::map<int, std::vector<nlohmann::json>> pagination = {};

  // Fetch servers is exists
  if (std::filesystem::exists(base_path + "servers/" + mData["servers_key"].get<std::string>() + ".json"))  {
    nlohmann::json sData = FileSystem2::readJson(base_path + "servers/" + mData["servers_key"].get<std::string>() + ".json");

    if (sData.contains("servers") && sData["servers"].is_array()) {
      auto& servers = sData["servers"];
      
      int i = 0, disabled = 0;
      for (auto& server : servers) {
        // Check if expired 
        if (server["options"]["disable"].get<bool>())
          continue;
          
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
            disabled++;
          }
        }
      
        if (!mData["options"]["hide_servers"].get<bool>() && !server["options"]["hide_server"].get<bool>()) {
          if (i >= max_servers_page) {
            max_servers_page *= 2;
            tPage++;
          }
          
          pagination[tPage].emplace_back(server);
        }

        i++;
      }

      if (disabled) {
        additional_msg += fmt::format("`w[`4{} servers have been disabled due to insufficient coins!`w]", disabled);
      }
    }

    FileSystem2::writeJson(base_path + "servers/" + mData["servers_key"].get<std::string>() + ".json", sData);
    FileSystem2::writeJson(base_path + "merchants/" + merchant + ".json", mData);
  }

  // Build world offers
  ctx.SetupSimpleMenu()->AddHeading("Enter the `#server name `0in the column above `4^`0 (");
  if (additional_msg != "")
    ctx.AddHeading(additional_msg);

  if (pRole.has_role(PlayerRole::ADMIN) || pRole.has_role(PlayerRole::MERCHANT)) {
    ctx.AddHeading("Dashboard<ROW2>");

    if (pRole.has_role(PlayerRole::ADMIN))
      ctx.AddButton("Control Panel", "control_panel", 0.5, default_color);
    ctx.AddButton("My Profile", "my_profile", 0.5, default_color)->AddButton("Add new server", "add_new_server", 0.5, default_color);
  }

  if (pagination.size()) {
    ctx.AddHeading("Available servers" + std::string(dashboard_showed ? "<CR>" : "<ROW2>"));
    for (const auto& a : pagination[req_page]) {
      std::string display_name = a.value("display_name", "NONE");
      std::string name = a.value("name", "NONE");
      std::string host = a.value("host", "127.0.0.1");
      int port = a.value("port", 17091);
      nlohmann::json opts = a.value("options", nlohmann::json());
      bool block_3rd_app = opts.value("block_3rd_app", false);
      nlohmann::json color = opts.value("color", nlohmann::json());
      uint32_t buttonColor = ColorConverter::toBGRA(color.value("blue", 0), color.value("green", 0), color.value("red", 0), color.value("alpha", 0));

      ctx.AddButton(display_name, fmt::format("name={}&host={}&port={}&block_3rd_app={}", name, host, port, block_3rd_app), 0.6, buttonColor);
    }

    // ctx.AddHeading("`oPage `1" + std::to_string(req_page + 1) + " ``of `1" + std::to_string(pagination.size()) + "<CR>");
    if (req_page < pagination.size() - 1)
      ctx.AddButton("Next page", "page_" + std::to_string(req_page + 1), 0.6, default_color);
    if (req_page > 0)
      ctx.AddButton("Previous page", "page_" + std::to_string(req_page - 1), 0.6, default_color);
    // ctx.AddButton("Exit to home page", "exit", 0.5, default_color);
  }

  return ctx.Build();
}
bool Utils::isValidMACAddress ( const std::string& mac ) {
  // Pola regex untuk alamat MAC
  const std::regex macRegex ( "^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$" );

  // Memeriksa apakah alamat MAC sesuai dengan pola regex
  return std::regex_match ( mac , macRegex );
}
bool Utils::isValidGUID ( const std::string& guid ) {
  // Pola regex untuk GUID yang valid
  const std::regex pattern ( R"([a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12})" );

  // Memeriksa apakah GUID sesuai dengan pola
  if ( !std::regex_match ( guid , pattern ) ) {
    return false;
  }

  // Memeriksa apakah GUID mengandung pola invalid berulang (4 karakter identik berturut-turut)
  const std::regex invalid_pattern ( R"(([a-fA-F0-9])\1{4})" );
  if ( std::regex_search ( guid , invalid_pattern ) ) {
    return false;
  }

  return true;
}
std::string Utils::param_get_value(const std::string& key, const std::string& data) { 
  std::string pattern = key + "="; 
  size_t start = data.find(pattern); 

  if (start == std::string::npos) 
    return ""; 

  start += pattern.length(); 

  // Special handling for key "password" 
  if (key == "password") { 
    return data.substr(start); // everything after "password=" 
  } 

  size_t end = data.find('&', start); 
  return (end == std::string::npos)  
    ? data.substr(start) 
    : data.substr(start, end - start); 
}
std::string Utils::base64_decode(std::string const& encoded_string) {
  std::string decoded_string;
  std::vector<int> T(256, -1);
  for (int i = 0; i < 64; i++) {
    T[base64_chars[i]] = i;
  }

  int val = 0, valb = -8;
  for (unsigned char c : encoded_string) {
    if (T[c] == -1) break;
    val = (val << 6) + T[c];
    valb += 6;
    if (valb >= 0) {
      decoded_string.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }

  return decoded_string;
}
std::string Utils::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;
}
bool Utils::is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}
void Utils::SendPacket(ENetPeer* peer, const int& num, char* data, const int& len) {
  if (!PeerValidation(peer))
    return;
  
  const auto packet = enet_packet_create ( nullptr , len + 5 , ENET_PACKET_FLAG_RELIABLE );
  memcpy ( packet->data , &num , 4 );
  if ( data != nullptr ) {
    memcpy ( packet->data + 2 , data , len );
  }
  char zero = 0;
  memcpy ( packet->data + 2 + len , &zero , 1 );
  enet_peer_send ( peer , 0 , packet ); return;

  if ( len >= 5 ) {
    packet->data[ 2 ] = 0xAA;
    packet->data[ 3 ] = 0xBB;
    packet->data[ 4 ] = 0xCC;
    packet->data[ 5 ] = 0xDD;
    packet->data[ 6 ] = 0xEE;
  }
}