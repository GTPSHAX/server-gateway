#pragma once

#include <BaseApp.h>

#include <string>
#include <mutex>

#include <nlohmann/json.hpp>

#include "RoleManager.h"

enum ePlatformType {
	PLATFORM_ID_UNKNOWN = -1 ,
	PLATFORM_ID_WINDOWS , // 0
	PLATFORM_ID_IOS , // 1
	PLATFORM_ID_OSX , // 2
	PLATFORM_ID_LINUX , // 3
	PLATFORM_ID_ANDROID , // 4
	PLATFORM_ID_WINDOWS_MOBILE , // 5
	PLATFORM_ID_WEBOS , // 6
	PLATFORM_ID_BBX , // 7
	PLATFORM_ID_FLASH , // 8
	PLATFORM_ID_HTML5 , // 9
	PLATFORM_MAXVAL // 10
};

struct PlayerCredentials {
  std::string tankIDName = "";
  std::string tankIDPass = "";
  std::string RID = "";
  std::string IPv4 = "127.0.0.1";
};

class Player {
  private:
    PlayerCredentials credentials;
    RoleManager roles;

  public:
    nlohmann::json tData;

    void set_credentials(const PlayerCredentials& creds) {
      std::lock_guard<std::mutex> lock(mtx);
      credentials = creds;
    }
    PlayerCredentials get_credentials() {
      std::lock_guard<std::mutex> lock(mtx);
      return credentials;
    }

    void set_roles(const RoleManager& data) {
      std::lock_guard<std::mutex> lock(mtx);
      roles = data;
    }
    RoleManager get_roles() {
      std::lock_guard<std::mutex> lock(mtx);
      return roles;
    }

  private:
    mutable std::mutex mtx;
};

#define pInfo(peer) ((Player*)(peer->data))
#define pClient pInfo(peer)
#define cpClient pInfo(currentPeer)