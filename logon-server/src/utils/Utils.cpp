#include "Utils.h"

#include <SDK/Builders/WorldOffersBuilder.h>
#include <utils/ColorConverter.h>

bool Utils::PeerValidation(ENetPeer* peer) {
  return !(!peer || peer == nullptr || !peer->data || peer->data == NULL || peer->state != ENET_PEER_STATE_CONNECTED);
}
std::string Utils::generate_world_offers(Player* player) {
  RoleManager pRole = player->get_roles();
  WorldOffersMenu ctx;
  uint32_t default_color = ColorConverter::toBGRA(214,171,94,255);

  // testing only
  nlohmann::json tData = (player->tData.contains("OnRequestWorldSelectMenu") 
                          ? player->tData["OnRequestWorldSelectMenu"] 
                          : nlohmann::json());

  int tPage = 0;
  int req_page = (tData.contains("page") ? tData["page"].get<int>() : 0);
  int max_servers_page = 10;
  std::map<int, std::vector<std::string>> pagination = {};
  std::vector<std::string> servers = { "Gemtopia", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT", "MafiaPS", "CreativePS", "RGT" };

  // Susun pagination
  for (int i = 0; i < servers.size(); i++) {
    if (i >= max_servers_page) {
      max_servers_page *= 2;
      tPage++;
    }
    
    pagination[tPage].push_back(servers[i]);
  }
  
  // Build world offers
  ctx.SetupSimpleMenu()->AddHeading("Enter the `#server name `0in the column above `4^`0 (");
  if (pRole.has_role(PlayerRole::ADMIN) || pRole.has_role(PlayerRole::MERCHANT)) {
    ctx.AddHeading("Dashboard<ROW2>");

    if (pRole.has_role(PlayerRole::ADMIN))
      ctx.AddButton("Control Panel", "control_panel", 0.5, default_color);
    ctx.AddButton("My Profile", "my_profile", 0.5, default_color)->AddButton("Add new server", "add_new_server", 0.5, default_color);
  }

  ctx.AddHeading("Available servers<CR>");
  for (const auto& a : pagination[req_page])
    ctx.AddButton(a, "enter_" + a, 0.5, default_color);

  ctx.AddHeading("`oPage `1" + std::to_string(req_page + 1) + " ``of `1" + std::to_string(pagination.size()) + "<CR>");
  if (req_page < pagination.size())
    ctx.AddButton("Next page", "page_" + std::to_string(req_page + 1), 0.5, default_color);
  if (req_page > 0)
    ctx.AddButton("Previous page", "page_" + std::to_string(req_page - 1), 0.5, default_color);

  return ctx.Build();
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