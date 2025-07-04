/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "sta_wlan_hal_nl80211.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>

namespace bwl {
namespace nl80211 {

// Convert BWL to NL80211 security string
static std::string nl80211_security_val(WiFiSec sec)
{
    switch (sec) {
    case WiFiSec::None:
        return "NONE";
    case WiFiSec::WEP_64:
        return "WEP-64";
    case WiFiSec::WEP_128:
        return "WEP-128";
    case WiFiSec::WPA_PSK:
        return "WPA-PSK";
    case WiFiSec::WPA2_PSK:
        return "WPA-PSK";
    case WiFiSec::WPA_WPA2_PSK:
        return "WPA-PSK";

    default:
        return "INVALID";
    }
}

static sta_wlan_hal::Event nl80211_to_bwl_event(const std::string &opcode)
{
    if (opcode == "CTRL-EVENT-CONNECTED") {
        return sta_wlan_hal::Event::Connected;
    } else if (opcode == "CTRL-EVENT-DISCONNECTED") {
        return sta_wlan_hal::Event::Disconnected;
    } else if (opcode == "CTRL-EVENT-TERMINATING") {
        return sta_wlan_hal::Event::Terminating;
    } else if (opcode == "CTRL-EVENT-SCAN-RESULTS") {
        return sta_wlan_hal::Event::ScanResults;
    } else if (opcode == "CTRL-EVENT-CHANNEL-SWITCH") {
        return sta_wlan_hal::Event::ChannelSwitch;
    }

    return sta_wlan_hal::Event::Invalid;
}

sta_wlan_hal_nl80211::sta_wlan_hal_nl80211(const std::string &iface_name, hal_event_cb_t callback,
                                           const bwl::hal_conf_t &hal_conf)
    : base_wlan_hal(bwl::HALType::Station, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_nl80211(bwl::HALType::Station, iface_name, callback, BUFFER_SIZE, hal_conf)
{
    m_filtered_events.insert({});
}

sta_wlan_hal_nl80211::~sta_wlan_hal_nl80211() { sta_wlan_hal_nl80211::detach(); }

bool sta_wlan_hal_nl80211::detach() { return true; }

bool sta_wlan_hal_nl80211::start_wps_pbc()
{
    LOG(DEBUG) << "Initiating wps_pbc on interface: " << get_iface_name();

    if (!wpa_ctrl_send_msg("WPS_PBC multi_ap=1")) {
        LOG(ERROR) << "start_wps_pbc - wpa_ctrl_send_msg failed for " << get_iface_name();
        return false;
    }
    return true;
}

bool sta_wlan_hal_nl80211::initiate_scan() { return true; }

bool sta_wlan_hal_nl80211::scan_bss(const sMacAddr &bssid, uint8_t channel,
                                    beerocks::eFreqType freq_type)
{
    if (bssid == beerocks::net::network_utils::ZERO_MAC || channel == 0) {
        LOG(ERROR) << "Invalid parameters";
        return false;
    }
    auto freq = son::wireless_utils::channel_to_freq(channel, freq_type);

    LOG(DEBUG) << "Scan with TYPE=ONLY on interface: " << get_iface_name() << " for bssid " << bssid
               << " channel " << channel << " (frequency " << freq << ").";

    // Start the scan
    if (!wpa_ctrl_send_msg("SCAN TYPE=ONLY freq=" + std::to_string(freq) +
                           " bssid=" + tlvf::mac_to_string(bssid))) {
        LOG(ERROR) << "scan_bss - wpa_ctrl_send_msg failed for " << get_iface_name();
        return false;
    }
    return true;
}

int sta_wlan_hal_nl80211::get_scan_results(const std::string &ssid, std::vector<sScanResult> &list,
                                           bool parse_vsie)
{
    return 0;
}

bool sta_wlan_hal_nl80211::connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                                   bool mem_only_psk, const std::string &bssid,
                                   ChannelFreqPair channel, bool hidden_ssid)
{
    LOG(DEBUG) << "Connect interface " << get_iface_name() << " to SSID = " << ssid
               << ", BSSID = " << bssid << ", Channel = " << int(channel.first)
               << ", freq type = " << channel.second << ", Sec = " << nl80211_security_val(sec)
               << ", mem_only_psk=" << int(mem_only_psk);

    if (ssid.empty() || bssid.empty() || sec == WiFiSec::Invalid) {
        LOG(ERROR) << "Invalid params!";
        return false;
    }

    // First disconnect (or do nothing if not connected)
    if (!disconnect()) {
        LOG(WARNING) << "Failed disconnecting before connecting to the new BSSID";
        return false;
    }

    // Add a new network
    auto network_id = add_network();
    if (network_id < 0) {
        LOG(ERROR) << "Failed (" << network_id
                   << ") adding new network to interface: " << get_iface_name();
        return false;
    }

    auto freq = son::wireless_utils::channel_to_freq(int(channel.first), channel.second);

    // Update network parameters
    if (!set_network_params(network_id, ssid, bssid, sec, mem_only_psk, pass, hidden_ssid, freq)) {
        LOG(ERROR) << "Failed setting network id = " << network_id
                   << " on interface: " << get_iface_name();
        return false;
    }

    // Enable the network
    if (!enable_network(network_id)) {
        LOG(ERROR) << "Failed enabling network id = " << network_id
                   << " on interface: " << get_iface_name();
        return false;
    }

    LOG(DEBUG) << "Network with id " << network_id << " has been added and enabled on interface "
               << get_iface_name();

    // Update active network parameters
    m_active_ssid.assign(ssid);
    m_active_bssid.assign(bssid);
    m_active_pass.assign(pass);
    m_active_channel    = channel.first;
    m_active_network_id = network_id;

    return true;
}

bool sta_wlan_hal_nl80211::disconnect()
{
    LOG(TRACE) << "Disconnect network id " << m_active_network_id
               << " on interface: " << get_iface_name();

    ConnectionStatus connection_status;
    if (!read_status(connection_status)) {
        LOG(ERROR) << "Failed reading status for " << get_iface_name() << "! can't disconnect";
        return false;
    }

    // Return gracefully if network state is not connected
    if (!is_connected(connection_status.wpa_state)) {
        LOG(DEBUG) << "Active network is not connected";
        return true;
    }

    // Return gracefully if no network is connected
    if (m_active_network_id < 0) {
        LOG(DEBUG) << "Active network does not exist";
        return true;
    }

    // Connection status id must be the same as the active network id
    if (m_active_network_id != connection_status.id) {
        LOG(ERROR) << "Network id mismatch: m_active_network_id(" << m_active_network_id << ") != "
                   << "connection_status.id(" << connection_status.id << ")";
        return false;
    }

    const std::string cmd = "REMOVE_NETWORK " + std::to_string(m_active_network_id);
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "WPA command '" << cmd << "' failed";
    }

    // Clear state
    m_active_ssid       = "";
    m_active_bssid      = "";
    m_active_pass       = "";
    m_active_channel    = 0;
    m_active_network_id = -1;

    return true;
}

bool sta_wlan_hal_nl80211::roam(const sMacAddr &bssid, ChannelFreqPair channel)
{
    if (m_active_network_id == -1) {
        LOG(ERROR) << "Incorrect active network " << m_active_network_id;
        return false;
    }

    if (!is_connected()) {
        LOG(ERROR) << get_iface_name() << " Not connected, can't roam";
        return false;
    }

    auto freq = son::wireless_utils::channel_to_freq(int(channel.first), channel.second);
    if (!set_network(m_active_network_id, "freq_list", std::to_string(freq))) {
        LOG(ERROR) << "Failed setting frequency on interface " << get_iface_name();
        return false;
    }

    auto bssid_str = tlvf::mac_to_string(bssid);

    const std::string cmd = "ROAM " + bssid_str;
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "WPA command '" << cmd << "' failed";
        ;
        return false;
    }

    // Update the active channel and bssid
    m_active_bssid.assign(bssid_str);
    m_active_channel = channel.first;

    return true;
}

bool sta_wlan_hal_nl80211::get_4addr_mode() { return true; }

bool sta_wlan_hal_nl80211::set_4addr_mode(bool enable) { return true; }

bool sta_wlan_hal_nl80211::set_3addr_mcast(bool enable) { return true; }

bool sta_wlan_hal_nl80211::unassoc_rssi_measurement(const std::string &mac, int chan,
                                                    beerocks::eWiFiBandwidth bw,
                                                    int vht_center_frequency, int delay,
                                                    int window_size)
{
    return true;
}

bool sta_wlan_hal_nl80211::is_connected()
{
    ConnectionStatus connection_status;
    constexpr int max_retries = 10;
    int retry                 = 0;
    while ((!read_status(connection_status)) && (retry++ < max_retries)) {
        LOG(ERROR) << "Failed to read connection status, retry: " << retry;
        usleep(1000);
    }

    return is_connected(connection_status.wpa_state);
}

int sta_wlan_hal_nl80211::get_channel() { return m_active_channel; }

std::string sta_wlan_hal_nl80211::get_ssid() { return m_active_ssid; }

std::string sta_wlan_hal_nl80211::get_bssid() { return m_active_bssid; }

std::string sta_wlan_hal_nl80211::get_wireless_backhaul_mac()
{
    std::string mac;
    if (!beerocks::net::network_utils::linux_iface_get_mac(m_radio_info.iface_name, mac)) {
        LOG(ERROR) << "Failed to get radio mac from ifname " << m_radio_info.iface_name;
    }
    return mac;
}

bool sta_wlan_hal_nl80211::process_nl80211_event(parsed_obj_map_t &parsed_obj)
{
    auto opcode = parsed_obj["_opcode"];
    LOG(TRACE) << __func__ << " " << get_iface_name() << " - opcode: |" << opcode << "|";

    auto event = nl80211_to_bwl_event(opcode);

    switch (event) {
    // Client Connected
    case Event::Connected: {

        ConnectionStatus connection_status;
        if (!read_status(connection_status)) {
            LOG(ERROR) << "Failed reading connection status for iface: " << get_iface_name();
            return false;
        }

        LOG(DEBUG) << get_iface_name() << " - Connected: bssid = " << connection_status.bssid
                   << ", freq = " << connection_status.freq;

        // Add sACTION_BACKHAUL_CONNECTED_NOTIFICATION which contain the Multi-AP profile, and the
        // Primary VLAN ID. Since this is not implemented on this flavour of BWL, leave it empty.
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_BACKHAUL_CONNECTED_NOTIFICATION));
        auto msg      = reinterpret_cast<sACTION_BACKHAUL_CONNECTED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_BACKHAUL_CONNECTED_NOTIFICATION));

        update_status(connection_status);

        // Forward the event
        event_queue_push(event, msg_buff);

    } break;

    // Client disconnected
    case Event::Disconnected: {

        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION));

        // Fill in the message
        msg->bssid             = tlvf::mac_from_string(parsed_obj["bssid"]);
        msg->disconnect_reason = std::strtoul(parsed_obj["reason"].c_str(), nullptr, 10);

        // Forward the event
        event_queue_push(event, msg_buff);

    } break;

    case Event::Terminating:
    case Event::ScanResults:
    case Event::ChannelSwitch: {
        // Forward the event
        event_queue_push(event);

    } break;

    // Gracefully ignore unhandled events
    // TODO: Should be changed to an error once we handle hostapd all expected events
    default:
        LOG(DEBUG) << "Unhandled event received: " << opcode;
        break;
    }

    return true;
}

bool sta_wlan_hal_nl80211::update_status()
{
    ConnectionStatus connection_status;
    if (!read_status(connection_status)) {
        LOG(ERROR) << "Failed reading connection status for iface: " << get_iface_name();
        return false;
    }
    update_status(connection_status);

    return true;
}

bool sta_wlan_hal_nl80211::reassociate() { return wpa_ctrl_send_msg("REASSOCIATE"); }

int sta_wlan_hal_nl80211::add_network()
{
    char *reply = nullptr;

    std::string cmd = "ADD_NETWORK";
    if (!wpa_ctrl_send_msg(cmd, &reply)) {
        LOG(ERROR) << "WPA command '" << cmd << "' failed";
        ;
        return -1;
    }

    // Return the newly added network ID
    return beerocks::string_utils::stoi(reply);
}

bool sta_wlan_hal_nl80211::set_network(int network_id, const std::string &param,
                                       const std::string &value)
{
    const std::string cmd = "SET_NETWORK " + std::to_string(network_id) + " " + param + " " + value;
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "WPA command '" << cmd << "' failed";
        ;
        return false;
    }
    return true;
}

bool sta_wlan_hal_nl80211::set_network_params(int network_id, const std::string &ssid,
                                              const std::string &bssid, WiFiSec sec,
                                              bool mem_only_psk, const std::string &pass,
                                              bool hidden_ssid, int freq)
{
    // Set SSID
    if (!set_network(network_id, "ssid", "\"" + ssid + "\"")) {
        LOG(ERROR) << "Failed setting ssid on interface " << get_iface_name();
        return false;
    }

    // Set BSSID
    if (!set_network(network_id, "bssid", bssid)) {
        LOG(ERROR) << "Failed setting bssid on interface " << get_iface_name();
        return false;
    }

    // Optional: set frequency
    if ((freq > 0) && (!set_network(network_id, "freq_list", std::to_string(freq)))) {
        LOG(ERROR) << "Failed setting frequency on interface " << get_iface_name();
        return false;
    }

    // Security and Password
    if (!set_network(network_id, "key_mgmt", nl80211_security_val(sec))) {
        LOG(ERROR) << "Failed setting key_mgmt on interface " << get_iface_name();
        return false;
    }

    // Optional: set hidden-ssid
    if (hidden_ssid && (!set_network(network_id, "scan_ssid", "1"))) {
        LOG(ERROR) << "Failed setting scan_ssid on interface " << get_iface_name();
        return false;
    }

    // mem_only_psk flag tells whether to keep PSK/passphrase only in memory.
    // Possible values are:
    // 0 = allow PSK/passphrase to be stored to the configuration file
    // 1 = do not store PSK/passphrase to the configuration file
    // If false, then set the WPA ASCII passphrase. The PSK is computed from the quoted
    // "passphrase" string (PSK could be used instead but passing it without quotes).
    if (mem_only_psk) {
        if (!set_network(network_id, "mem_only_psk", "1")) {
            LOG(ERROR) << "Failed setting mem_only_psk on interface " << get_iface_name();
            return false;
        }
    } else if ((!pass.empty()) && (!set_network(network_id, "psk", "\"" + pass + "\""))) {
        LOG(ERROR) << "Failed setting psk on interface " << get_iface_name();
        return false;
    }

    return true;
}

bool sta_wlan_hal_nl80211::enable_network(int network_id)
{
    const std::string cmd = "ENABLE_NETWORK " + std::to_string(network_id);
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "WPA command '" << cmd << "' failed";
        ;
        return false;
    }

    return true;
}

bool sta_wlan_hal_nl80211::is_connected(const std::string &wpa_state)
{
    return (wpa_state.compare("COMPLETED") == 0);
}

bool sta_wlan_hal_nl80211::read_status(ConnectionStatus &connection_status)
{
    const char *cmd = "STATUS";
    parsed_obj_map_t reply;
    if (!wpa_ctrl_send_msg(cmd, reply)) {
        LOG(ERROR) << "WPA command '" << cmd << "' failed";
        ;
        return false;
    }

    connection_status.bssid           = reply["bssid"];
    connection_status.freq            = std::strtoul(reply["freq"].c_str(), nullptr, 10);
    connection_status.ssid            = reply["ssid"];
    connection_status.id              = std::strtoul(reply["id"].c_str(), nullptr, 10);
    connection_status.mode            = reply["mode"];
    connection_status.pairwise_cipher = reply["pairwise_cipher"];
    connection_status.group_cipher    = reply["group_cipher"];
    connection_status.key_mgmt        = reply["key_mgmt"];
    connection_status.wpa_state       = reply["wpa_state"];
    connection_status.address         = reply["address"];
    connection_status.uuid            = reply["uuid"];

    std::string reply_str;
    for (const auto &entry : reply) {
        if (!reply_str.empty()) {
            reply_str += "\n";
        }
        reply_str += entry.first + "=" + entry.second;
    }
    LOG(TRACE) << "STATUS reply= \n" << reply_str;

    LOG(DEBUG) << "active network " << m_active_network_id;

    return true;
}

void sta_wlan_hal_nl80211::update_status(const ConnectionStatus &connection_status)
{
    m_active_network_id = connection_status.id;
    m_active_bssid      = connection_status.bssid;
    m_active_channel    = son::wireless_utils::freq_to_channel(connection_status.freq);
    m_active_ssid       = connection_status.ssid;

    LOG(DEBUG) << "active_network_id= " << m_active_network_id
               << ", active_bssid= " << m_active_bssid << ", active_channel= " << m_active_channel
               << ", active_ssid= " << m_active_ssid;
}

bool sta_wlan_hal_nl80211::clear_non_associated_devices() { return true; }

} // namespace nl80211

std::shared_ptr<sta_wlan_hal> sta_wlan_hal_create(const std::string &iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback,
                                                  const bwl::hal_conf_t &hal_conf)
{
    return std::make_shared<nl80211::sta_wlan_hal_nl80211>(iface_name, callback, hal_conf);
}

} // namespace bwl
