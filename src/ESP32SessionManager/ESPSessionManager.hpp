#pragma once

#include <WiFi.h>
#include <unordered_map>
#include <lwip/etharp.h>
#include <esp_wifi.h>
#include <Arduino_JSON.h>

struct session_info_t
{
    uint32_t _ip = 0;
    eth_addr _mac{}; // TODO: in C++ 20 use std::array
    wifi_interface_t _conn;
    String _userID;

    JSONVar toJSON();
    operator bool() const;
    session_info_t(const uint32_t &ip, const eth_addr &mac, const wifi_interface_t &conn, const String &id);
    session_info_t();
    ~session_info_t();
};

namespace cst
{
    class ESPSessionManager // ? singleton?
    {
    private:
        std::unordered_map<uint32_t, session_info_t> sta_sessions, ap_sessions;

        void on_sta_connect(const uint32_t &ip);                 // register new station IP & MAC address once device connects to AP
        void on_sta_disconnect(const uint8_t (&mac)[6]);         // remove session once a AP station disconnects from Wi-Fi
        const session_info_t &handle_sta_ip(const uint32_t &ip); // TODO: use std::optional
        const session_info_t &handle_ap_ip(const uint32_t &ip);  // TODO: use std::optional
        eth_addr *get_sta_mac(const uint32_t &ip);
        inline bool is_sta(const IPAddress &localIP);
        inline bool is_ap(const IPAddress &localIP);

    public:
        const session_info_t &get_session_info(const IPAddress &localIP, const uint32_t &remoteAddr);
        const session_info_t &new_session(const IPAddress &localIP, const uint32_t &remoteAddr, const String &id);
        void remove_session(const IPAddress &localIP, const uint32_t &remoteAddr);
        JSONVar to_json();

        ESPSessionManager(); // register Wi-Fi event handlers for connection and disconnection

        const session_info_t emptySession; // TODO: remove when std::optional is available
    };

    extern ESPSessionManager session_manager;
} // namespace cst
