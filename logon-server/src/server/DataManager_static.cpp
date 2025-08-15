#include "DataManager.h"

#include <utils/FileSystem2.h>
#include <utils/ConsoleInterface.h>

ServerConfig DataManager::server_config = {};
void DataManager::load_server_config(const std::string path) {
  try {
    nlohmann::json data = FileSystem2::readJson(path);
    server_config.server_ip = data["server_ip"].get<std::string>();
    server_config.server_port = data["server_port"].get<int>();
    server_config.default_name = data["default_name"].get<std::string>();

    return;
  }
  catch (const nlohmann::json::exception& e) {
    print_error("{}", e.what());
  }
  catch (const std::runtime_error& e) {
    print_error("{}", e.what());
  }
  catch (const std::exception& e) {
    print_error("{}", e.what());
  }
  save_server_config();
}
void DataManager::save_server_config(const std::string path) {
  try {
    nlohmann::json data;
    data["server_ip"] = server_config.server_ip;
    data["server_port"] = server_config.server_port;
    data["default_name"] = server_config.default_name;

    FileSystem2::writeJson(path, data);
  }
  catch (const nlohmann::json::exception& e) {
    print_error("{}", e.what());
  }
  catch (const std::runtime_error& e) {
    print_error("{}", e.what());
  }
  catch (const std::exception& e) {
    print_error("{}", e.what());
  }
}