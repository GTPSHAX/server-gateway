#pragma once

#include <BaseApp.h>

#include <string>
#include <unordered_map>

#include <utils/ConsoleInterface.h>
#include <utils/FileSystem2.h>

#include "RoleDef.h"

/**
 * @class RoleManager
 * @brief Mengelola role dan atribut player
 *
 * Class ini bertanggung jawab untuk mengelola role dan atribut yang dimiliki oleh player,
 * termasuk operasi untuk menambah, menghapus, dan memeriksa role/atribut.
 * Juga menyediakan fungsi untuk mengelola prefix role.
 */
class RoleManager {
public:
    PlayerRole m_role;
    PlayerAttribute m_attribute;

    /**
     * @brief Constructor default
     * Menginisialisasi role dan attribute ke NONE
     */
    RoleManager();

    /**
     * @brief Destructor default
     */
    ~RoleManager() = default;

    /**
     * @brief Memeriksa apakah player memiliki role parent atau lebih tinggi
     * @param parent Role parent yang akan diperiksa
     * @return true jika memiliki role parent atau lebih tinggi
     * @note Fungsi ini sepertinya memiliki logika yang salah, perlu diperbaiki
     */
    bool is_have_parent_role(PlayerRole parent);

    /**
     * @brief Menambahkan role ke player
     * @param role Role yang akan ditambahkan
     */
    void add_role(PlayerRole role) {
        m_role = static_cast<PlayerRole>(static_cast<int>(m_role) | static_cast<int>(role));
    }

    /**
     * @brief Menghapus role dari player
     * @param role Role yang akan dihapus
     */
    void remove_role(PlayerRole role) {
        m_role = static_cast<PlayerRole>(static_cast<int>(m_role) & ~static_cast<int>(role));
    }

    /**
     * @brief Menambahkan atribut ke player
     * @param attribute Atribut yang akan ditambahkan
     */
    void add_attribute(PlayerAttribute attribute) {
        m_attribute = static_cast<PlayerAttribute>(static_cast<int>(m_attribute) | static_cast<int>(attribute));
    }

    /**
     * @brief Menghapus atribut dari player
     * @param attribute Atribut yang akan dihapus
     */
    void remove_attribute(PlayerAttribute attribute) {
        m_attribute = static_cast<PlayerAttribute>(static_cast<int>(m_attribute) & ~static_cast<int>(attribute));
    }

    /**
     * @brief Menetapkan raw bit flags untuk role player.
     * @param flags Integer yang merepresentasikan bit flags role.
     * @note Digunakan untuk memuat data dari database.
     */
    void set_role_flags(int flags);

    /**
     * @brief Mendapatkan raw bit flags dari role player.
     * @return Integer yang merepresentasikan bit flags role.
     * @note Digunakan untuk menyimpan data ke database.
     */
    int get_role_flags() const;

    /**
     * @brief Menetapkan raw bit flags untuk atribut player.
     * @param flags Integer yang merepresentasikan bit flags atribut.
     * @note Digunakan untuk memuat data dari database.
     */
    void set_attribute_flags(int flags);

    /**
     * @brief Mendapatkan raw bit flags dari atribut player.
     * @return Integer yang merepresentasikan bit flags atribut.
     * @note Digunakan untuk menyimpan data ke database.
     */
    int get_attribute_flags() const;

    /**
     * @brief Memeriksa apakah player memiliki role tertentu
     * @param role Role yang akan diperiksa
     * @return true jika player memiliki role tersebut
     */
    bool has_role(PlayerRole role) {
        if (PlayerRole::NONE == role)
            return true;

        return (static_cast<int>(m_role) & static_cast<int>(role)) != 0;
    }

    /**
     * @brief Memeriksa apakah player memiliki atribut tertentu
     * @param attribute Atribut yang akan diperiksa
     * @return true jika player memiliki atribut tersebut
     */
    bool has_attribute(PlayerAttribute attribute) {
        if (attribute == PlayerAttribute::NONE)
            return true;

        return (static_cast<int>(m_attribute) & static_cast<int>(attribute)) != 0;
    }

    /**
     * @brief Mendapatkan role tertinggi yang dimiliki player
     * @return PlayerRole tertinggi, atau NONE jika tidak memiliki role
     *
     * Fungsi ini akan mencari bit tertinggi yang diset dalam m_role
     * dan mengembalikan role dengan prioritas tertinggi.
     * Urutan prioritas (dari tertinggi ke terendah):
     * OWN > OWNCO > STAFFH > STAFF > STAFFT > DEVU > DEVS > DEV >
     * MODU > MODS > MOD > MVPM > MVPP > MVP > VIPM > VIPP > VIP
     */
    PlayerRole get_highest_role();

    /**
     * @brief Mendapatkan nama role atau attrubute dalam bentuk string
     * @param Role atau Attribute yang akan dikonversi ke string
     * @return String nama role atau attribute
     */
    static std::string get_name(PlayerRole role);
    static std::string get_name(PlayerAttribute attribute);

    /**
     * @brief Memeriksa apakah role1 lebih tinggi dari role2
     * @param role1 Role pertama
     * @param role2 Role kedua
     * @return true jika role1 lebih tinggi dari role2
     */
    static bool is_role_higher(PlayerRole role1, PlayerRole role2);

    // Static members untuk prefix management
    static std::unordered_map<std::string, RolePrefix> prefixs;

    /**
     * @brief Memuat konfigurasi prefix dari file JSON
     * @param path Path ke file konfigurasi (default: "config/role_prefix.json")
     * @return false jika berhasil, true jika gagal
     */
    static bool load_prefix(const std::string path = "config/role_prefix.json");

    /**
     * @brief Menyimpan konfigurasi prefix ke file JSON
     * @param path Path ke file konfigurasi (default: "config/role_prefix.json")
     * @return false jika berhasil, true jika gagal
     */
    static bool save_prefix(const std::string path = "config/role_prefix.json");
};