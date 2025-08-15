#pragma once

#include <BaseApp.h>

struct ServerConfig {
  std::string server_ip = "127.0.0.1";
  int server_port = 17091;
  std::string default_name = "GTPS Gateway";
};

class DataManager {
  private:
    static ServerConfig server_config;

  public:
    static void load_all() {
      load_server_config();
    }
    static void save_all() {
      save_server_config();
    }

    static void set_server_config(const ServerConfig& data) {
      server_config = server_config;
    }
    static const ServerConfig& get_server_config() {
      return server_config;
    }
    static void load_server_config(const std::string path = "../config.json");
    static void save_server_config(const std::string path = "../config.json");
};
