#pragma once

#include <WiFi.h>
#include <unordered_map>
#include <lwip/etharp.h>
#include <esp_wifi.h>
#include <Arduino_JSON.h>

enum class CONN_TYPE_t
{
    CONN_AP,
    CONN_STA
};

struct SessionInfo_t
{
    uint32_t _ip = 0;
    eth_addr _mac{};
    CONN_TYPE_t _conn;
    String _userID;

    JSONVar toJSON();
    operator bool() const;
    SessionInfo_t(const uint32_t &ip, const eth_addr &mac, const CONN_TYPE_t &conn, const String &id);
    SessionInfo_t();
    ~SessionInfo_t();
};

namespace cst
{
    class ESPSessionManager
    {
    private:
        std::unordered_map<uint32_t, SessionInfo_t> sta_sessions, ap_sessions;

        void onStaConnect(const uint32_t &ip);         // register new station IP & MAC address once device connects to AP
        void onStaDisconnect(const uint8_t (&mac)[6]); // remove session once a AP station disconnects from Wi-Fi
        const SessionInfo_t &handle_sta_ip(const uint32_t &ip);
        const SessionInfo_t &handle_ap_ip(const uint32_t &ip);
        eth_addr *get_sta_mac(const uint32_t &ip);
        inline bool isSTA(const IPAddress &localIP);
        inline bool isAP(const IPAddress &localIP);

    public:
        const SessionInfo_t &getSessionInfo(const IPAddress &localIP, const uint32_t &remoteAddr);
        const SessionInfo_t &newSession(const IPAddress &localIP, const uint32_t &remoteAddr, const String &id);
        void removeSession(const IPAddress &localIP, const uint32_t &remoteAddr);
        JSONVar toJSON();

        ESPSessionManager(); // register Wi-Fi event handlers for connection and disconnection

        const SessionInfo_t emptySession;
    };

    extern ESPSessionManager session_manager;
} // namespace cst
