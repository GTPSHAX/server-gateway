#include "RoleManager.h"
#include <utils/ConsoleInterface.h>
#include <utils/FileSystem2.h>
#include <algorithm>

RoleManager::RoleManager() : m_role(PlayerRole::NONE), m_attribute(PlayerAttribute::NONE) {}

void RoleManager::set_role_flags(int flags) {
    m_role = static_cast<PlayerRole>(flags);
}

int RoleManager::get_role_flags() const {
    return static_cast<int>(m_role);
}

void RoleManager::set_attribute_flags(int flags) {
    m_attribute = static_cast<PlayerAttribute>(flags);
}

int RoleManager::get_attribute_flags() const {
    return static_cast<int>(m_attribute);
}

bool RoleManager::is_have_parent_role(PlayerRole parent) {
    // Perbaikan: Fungsi ini seharusnya memeriksa apakah role yang dimiliki
    // lebih tinggi atau sama dengan role parent
    return has_role(parent) || is_role_higher(get_highest_role(), parent);
}

PlayerRole RoleManager::get_highest_role() {
    static const PlayerRole role_hierarchy[] = {
        PlayerRole::ADMIN,
        PlayerRole::MERCHANT,
        PlayerRole::NONE
    };

    // Cari role tertinggi yang dimiliki
    for (const PlayerRole& role : role_hierarchy) {
        if (has_role(role)) {
            return role;
        }
    }

    return PlayerRole::NONE;
}

std::string RoleManager::get_name(PlayerRole role) {
    switch (role) {
    case PlayerRole::ADMIN: return "Admin";
    case PlayerRole::MERCHANT: return "Merchant";
    case PlayerRole::NONE: return "Player";
    default: return "Unknown";
    }
}
std::string RoleManager::get_name(PlayerAttribute attribute) {
    switch (attribute) {
    case PlayerAttribute::NONE: return "None";
    default: return "Unknown";
    }
}

bool RoleManager::is_role_higher(PlayerRole role1, PlayerRole role2) {
    // Definisi hierarki role (nilai lebih tinggi = prioritas lebih tinggi)
    auto get_role_priority = [](PlayerRole role) -> int {
        switch (role) {
        case PlayerRole::ADMIN: return 2;
        case PlayerRole::MERCHANT: return 1;
        case PlayerRole::NONE: return 0;
        default: return -1;
        }
        };

    return get_role_priority(role1) >= get_role_priority(role2);
}

std::unordered_map<std::string, RolePrefix> RoleManager::prefixs = {};

bool RoleManager::load_prefix(const std::string path) {
    try {
        const nlohmann::json& data = FileSystem2::readJson(path);
        prefixs.clear();

        for (const auto& [key, value] : data.items()) {
            RolePrefix rp;
            rp.name = value.value("name", "");
            rp.chat = value.value("chat", "");
            prefixs[key] = rp;
        }
    }
    catch (const std::exception& e) {
        print_error("Error while loading {}: {}", path, e.what());
        return true;
    }
    return false;
}

bool RoleManager::save_prefix(const std::string path) {
    try {
        nlohmann::json data;

        for (const auto& [key, rp] : prefixs) {
            data[key] = {
                {"name", rp.name},
                {"chat", rp.chat}
            };
        }

        FileSystem2::writeJson(path, data);
    }
    catch (const std::exception& e) {
        print_error("Error while saving {}: {}", path, e.what());
        return true;
    }
    return false;
}