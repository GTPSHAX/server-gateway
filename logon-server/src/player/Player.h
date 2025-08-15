#pragma once

#include <BaseApp.h>

#include <string>
#include <mutex>

#include <nlohmann/json.hpp>

#include "RoleManager.h"

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