/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "sta_wlan_hal_dwpald.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>

extern "C" {
#include <dwpal.h>
#include <dwpald_client.h>
}

#define LOGGING_ID sta_wlan_hal_dwpal
#define PACKAGE_ID "0"

namespace bwl {
namespace dwpal {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct localScanResults {
    uint frequency;
    char bssid[MAC_ADDR_SIZE];
    int8_t rssi;
    char ssid[SSID_MAX_SIZE];
    char vsie[49];
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static sta_wlan_hal::Event dwpal_to_bwl_event(const std::string &opcode)
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
    } else if (opcode == "UNCONNECTED-STA-RSSI") {
        return sta_wlan_hal::Event::STA_Unassoc_RSSI;
    } else if (opcode == "INTERFACE_CONNECTED_OK") {
        return sta_wlan_hal::Event::Interface_Connected_OK;
    } else if (opcode == "INTERFACE_RECONNECTED_OK") {
        return sta_wlan_hal::Event::Interface_Reconnected_OK;
    } else if (opcode == "INTERFACE_DISCONNECTED") {
        return sta_wlan_hal::Event::Interface_Disconnected;
    }

    return sta_wlan_hal::Event::Invalid;
}

// Convert BWL to DWPAL security string
static std::string dwpal_security_val(WiFiSec sec)
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

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// To access object methods from dwpald context callback
// key: ifname, value: sta_wlan_hal_dwpal object context
static std::unordered_map<std::string, sta_wlan_hal_dwpal *> ctx_map;

sta_wlan_hal_dwpal::sta_wlan_hal_dwpal(const std::string &iface_name, hal_event_cb_t callback,
                                       const bwl::hal_conf_t &hal_conf)
    : base_wlan_hal(bwl::HALType::Station, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_dwpal(bwl::HALType::Station, iface_name, callback, hal_conf)
{
    ctx_map[iface_name] = this;
}

sta_wlan_hal_dwpal::~sta_wlan_hal_dwpal()
{
    if (dwpald_hostap_detach(m_radio_info.iface_name.c_str()) != DWPALD_SUCCESS) {
        LOG(ERROR) << " Failed to detach from dwpald for interface" << m_radio_info.iface_name;
    }
    ctx_map.erase(m_radio_info.iface_name);
}

bool sta_wlan_hal_dwpal::start_wps_pbc()
{
    LOG(DEBUG) << "Initiating wps_pbc multi_ap=1 on interface: " << get_iface_name();

    if (!dwpal_send_cmd("WPS_PBC multi_ap=1")) {
        LOG(ERROR) << "start_wps_pbc - wpa_ctrl_send_msg failed for " << get_iface_name();
        return false;
    }
    return true;
}

bool sta_wlan_hal_dwpal::initiate_scan()
{
    LOG(DEBUG) << "Initiating scan on interface: " << get_iface_name();

    // Start the scan
    if (!dwpal_send_cmd("SCAN")) {
        LOG(ERROR) << "initiate_scan - wpa_ctrl_send_msg failed for " << get_iface_name();
        return false;
    }
    return true;
}

bool sta_wlan_hal_dwpal::scan_bss(const sMacAddr &bssid, uint8_t channel,
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
    if (!dwpal_send_cmd("SCAN TYPE=ONLY freq=" + std::to_string(freq) +
                        " bssid=" + tlvf::mac_to_string(bssid))) {
        LOG(ERROR) << "scan_bss - dwpal_send_cmd failed for " << get_iface_name();
        return false;
    }
    return true;
}

int sta_wlan_hal_dwpal::get_scan_results(const std::string &ssid, std::vector<sScanResult> &list,
                                         bool parse_vsie)
{
    LOG(TRACE) << __func__ << " entered for " << get_iface_name();

    char *reply           = nullptr;
    const std::string cmd = "SCAN_RESULTS";

    // Send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << "SCAN_RESULTS failed!";
        return 0;
    }

    size_t replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    /* TEMP: Traces... */
    LOG(DEBUG) << "SCAN_RESULTS replylen=" << (int)replyLen;
    LOG(DEBUG) << "SCAN_RESULTS reply=\n" << reply;

    /*
        bssid=70:54:d2:03:1d:99 frequency=2437 signal_level=-43 flags=[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS] ssid= freqband=2G netmode=bgn VEs=
        bssid=ac:9a:96:f5:0f:10 frequency=2442 signal_level=-38 flags=[WPA2-PSK-CCMP][WPS][ESS] ssid=test_ssid freqband=2G netmode=bgn VEs=[BRCOM:DD090010180200009C0000]
    */
    size_t numOfValidArgs[5] = {0};
    localScanResults scan_results[256];

    FieldsToParse fieldsToParse[] = {
        {(void *)scan_results[0].bssid, &numOfValidArgs[0], DWPAL_STR_PARAM,
         "bssid=", sizeof(scan_results[0].bssid)},
        {(void *)scan_results[0].ssid, &numOfValidArgs[1], DWPAL_STR_PARAM,
         "ssid=", sizeof(scan_results[0].ssid)},
        {(void *)&scan_results[0].frequency, &numOfValidArgs[2], DWPAL_INT_PARAM, "frequency=", 0},
        {(void *)&scan_results[0].rssi, &numOfValidArgs[3], DWPAL_CHAR_PARAM, "signal_level=", 0},
        {(void *)scan_results[0].vsie, &numOfValidArgs[4], DWPAL_STR_PARAM,
         "VEs=", sizeof(scan_results[0].vsie)},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(scan_results)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    for (uint16_t i = 0; i < sizeof(numOfValidArgs) / sizeof(size_t); i++) {
        if ((numOfValidArgs[i] == 0) || (numOfValidArgs[i] != numOfValidArgs[0])) {
            LOG(ERROR) << "Failed reading parameter " << (int)i << ": " << (int)numOfValidArgs[i];
            LOG(ERROR) << "Should be the same as numOfValidArgs[0]= " << (int)numOfValidArgs[0]
                       << " ==> Abort";
            return false;
        }
    }

    size_t valid_aps = numOfValidArgs[0];
    int aps_found    = 0;

    for (size_t i = 0; i < valid_aps; i++) {
        std::string ssid_str = std::string(scan_results[i].ssid);
        if (ssid != ssid_str) {
            // LOG(DEBUG) << "Wanted ssid " << ssid << " and the ssid is " << ssid_str;
            continue;
        }

        sScanResult ap;
        ap.bssid     = tlvf::mac_from_string(scan_results[i].bssid);
        ap.channel   = son::wireless_utils::freq_to_channel(scan_results[i].frequency);
        ap.freq_type = son::wireless_utils::which_freq_type(scan_results[i].frequency);
        ap.rssi      = scan_results[i].rssi;

        list.insert(list.begin(), ap);
        aps_found++;
    }

    return aps_found;
}

bool sta_wlan_hal_dwpal::connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                                 bool mem_only_psk, const std::string &bssid,
                                 ChannelFreqPair channel, bool hidden_ssid)
{
    LOG(DEBUG) << " connect iface " << get_iface_name() << " to SSID = " << ssid
               << ", BSSID = " << bssid << ", Channel = " << int(channel.first)
               << ", freq type = " << channel.second << ", Sec = " << dwpal_security_val(sec)
               << ", mem_only_psk=" << int(mem_only_psk);

    if (ssid.empty() || bssid.empty() || sec == WiFiSec::Invalid) {
        LOG(ERROR) << "Invalid params!";
        return false;
    }

    // First disconnect (or do nothing if not connected)
    if (!disconnect()) {
        LOG(WARNING) << "connect - Failed disconnecting";
        return false;
    }

    // Add a new network
    auto network_id = add_network();
    if (invalid_network_id == network_id) {
        LOG(ERROR) << "Failed adding new network to interface: " << get_iface_name();
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

    // Update active network parameters
    m_active_ssid.assign(ssid);
    m_active_bssid.assign(bssid);
    m_active_pass.assign(pass);
    m_active_channel    = channel.first;
    m_active_security   = sec;
    m_active_network_id = network_id;

    return true;
}

bool sta_wlan_hal_dwpal::disconnect()
{
    LOG(TRACE) << __func__ << ": iface = " << get_iface_name()
               << ", m_active_network_id = " << m_active_network_id;

    ConnectionStatus connection_status;
    if (!read_status(connection_status)) {
        LOG(ERROR) << "disconnect - Failed reading status for " << get_iface_name()
                   << "! can't disconnect";
        return false;
    }

    // Return gracefully if network state is not connected
    if (!is_connected(connection_status.wpa_state)) {
        LOG(DEBUG) << "disconnect - Active network does not exist";
        return true;
    }

    // Return gracefully if no network is connected
    if (m_active_network_id < 0) {
        LOG(DEBUG) << "disconnect - Active network does not exist";
        return true;
    }

    //connection status id must be same as active network id
    if (m_active_network_id != connection_status.id) {
        LOG(DEBUG) << "disconnect - m_active_network_id(" << m_active_network_id << ") != "
                   << "connection_status.id(" << connection_status.id << ")";
        return false;
    }

    const std::string cmd = "REMOVE_NETWORK " + std::to_string(m_active_network_id);
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "disconnect - REMOVE_NETWORK failed for network_id = " << m_active_network_id
                   << " on interface: " << get_iface_name();
    }

    // Clear state
    m_active_ssid       = "";
    m_active_bssid      = "";
    m_active_pass       = "";
    m_active_channel    = 0;
    m_active_security   = WiFiSec::None;
    m_active_network_id = -1;

    return true;
}

bool sta_wlan_hal_dwpal::roam(const sMacAddr &bssid, ChannelFreqPair channel)
{
    if (m_active_network_id == -1) {
        LOG(ERROR) << "Incorrect active network " << m_active_network_id;
        return false;
    }

    if (!is_connected()) {
        LOG(ERROR) << get_iface_name() << " Not connected, can't roam";
        return false;
    }

    auto bssid_str = tlvf::mac_to_string(bssid);

    const std::string cmd = "ROAM " + bssid_str;
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << get_iface_name() << " ROAM failed!";
        return false;
    }

    auto freq = son::wireless_utils::channel_to_freq(int(channel.first), channel.second);
    if (!set_network(m_active_network_id, "freq_list", std::to_string(freq))) {
        LOG(ERROR) << "Failed setting frequency on iface " << get_iface_name();
        return false;
    }

    // Update the active channel and bssid
    m_active_bssid.assign(bssid_str);
    m_active_channel = channel.first;

    return true;
}

bool sta_wlan_hal_dwpal::get_4addr_mode()
{

    char *reply = nullptr;

    std::string cmd = "DATA_ROUTING_MODE_GET";

    // Send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << "DATA_ROUTING_MODE_GET failed!";
        return false;
    }

    return (!std::string(reply).compare("mode=4_ADDRESS_MODE"));
}

bool sta_wlan_hal_dwpal::set_4addr_mode(bool enable)
{
    std::string cmd = "DATA_ROUTING_MODE_SET mode=";
    if (enable) {
        cmd += "4_ADDRESS_MODE";
    } else {
        cmd += "3_ADDRESS_MODE";
    }

    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "set_4addr_mode failed for interface: " << get_iface_name();
        return false;
    }

    return true;
}

bool sta_wlan_hal_dwpal::set_3addr_mcast(bool enable)
{
    auto iface_name = get_iface_name();
    LOG(DEBUG) << (enable ? "Allowing" : "Disabling") << " 3address multicast";
    int param = enable;
    if (!dwpal_nl_cmd_set(iface_name, LTQ_NL80211_VENDOR_SUBCMD_SET_ALLOW_3ADDR_MCAST, &param,
                          sizeof(param))) {
        LOG(ERROR) << "LTQ_NL80211_VENDOR_SUBCMD_SET_ALLOW_3ADDR_MCAST failed!";
        return false;
    }

    return true;
}

bool sta_wlan_hal_dwpal::unassoc_rssi_measurement(const std::string &mac, int chan,
                                                  beerocks::eWiFiBandwidth bw,
                                                  int vht_center_frequency, int delay,
                                                  int window_size)
{
    /*
    LOG(TRACE) << __func__ << " mac: " << mac << ", channel: " << chan 
        << ", bw: " << bw << ", vht_center_frequency: " << vht_center_frequency 
        << ", delay: " << delay;

    // Convert values to strings
    std::string chanBandwidth       = std::to_string(bw);
    std::string centerFreq          = std::to_string(son::wireless_utils::channel_to_freq(chan));
    std::string waveVhtCenterFreq   = std::to_string(vht_center_frequency);

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.AccessPoint.AssociatedDevice");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", "MACAddress", mac.c_str(), 0, 0);

    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "CenterFrequency", centerFreq.c_str(), 0, 0);
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "WaveVhtCenterFrequency", waveVhtCenterFreq.c_str(), 0, 0);

    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio", "OperatingChannelBandwidth", chanBandwidth.c_str(), 0, 0);

    // Delay the first measurement...
    UTILS_SLEEP_MSEC(delay);

    m_unassoc_measure_start         = std::chrono::steady_clock::now();
    m_unassoc_measure_delay         = delay;
    m_unassoc_measure_window_size   = window_size;

    // Trigger a measurement
    if (fapi_wlan_unassociated_devices_info_req(get_iface_name().c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_unassociated_devices_info_req failed!";
        return false;
    }
*/
    LOG(ERROR) << "NOT IMPLEMENTED!";
    return false;
}

bool sta_wlan_hal_dwpal::is_connected()
{
    ConnectionStatus connection_status;
    int retries = 10;
    while (!read_status(connection_status)) {
        --retries;
        LOG(ERROR) << "is_connected - read_status failed, retry:" << 10 - retries;
        if (retries == 0) {
            break;
        }
        usleep(1000);
    }

    return is_connected(connection_status.wpa_state);
}

int sta_wlan_hal_dwpal::get_channel() { return m_active_channel; }

bool sta_wlan_hal_dwpal::update_status()
{
    ConnectionStatus connection_status;
    if (!read_status(connection_status)) {
        LOG(ERROR) << "Failed reading connection status for iface: " << get_iface_name();
        return false;
    }
    update_status(connection_status);

    return true;
}

std::string sta_wlan_hal_dwpal::get_ssid() { return m_active_ssid; }

std::string sta_wlan_hal_dwpal::get_bssid() { return m_active_bssid; }

std::string sta_wlan_hal_dwpal::get_wireless_backhaul_mac()
{
    std::string mac;
    if (!beerocks::net::network_utils::linux_iface_get_mac(m_radio_info.iface_name, mac)) {
        LOG(ERROR) << "Failed to get radio mac from ifname " << m_radio_info.iface_name;
    }
    return mac;
}

bool sta_wlan_hal_dwpal::process_dwpal_event(char *ifname, char *buffer, int bufLen,
                                             const std::string &opcode)
{
    LOG(TRACE) << __func__ << " " << get_iface_name() << " - opcode: |" << opcode << "|";

    auto event = dwpal_to_bwl_event(opcode);

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

        update_status(connection_status);

        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_BACKHAUL_CONNECTED_NOTIFICATION));
        auto msg      = reinterpret_cast<sACTION_BACKHAUL_CONNECTED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_BACKHAUL_CONNECTED_NOTIFICATION));

        // Parse the event
        parsed_line_t parsed_obj;
        int64_t tmp_int;

        parse_event(buffer, parsed_obj);

        // Since prplwrt does not include the changes on the wpa_supplicant which add the Multi-AP
        // Profile to the association response, do not fail if it does not exist, and put default
        // value of profile-1.
        // TODO: When the changes will be ported to prplwrt, need to treat the "Multi-AP Profile"
        // parameter as mandatory.

        // Multi-AP Profile
        if (read_param("multi_ap_profile", parsed_obj, tmp_int)) {
            msg->multi_ap_profile = tmp_int;
        } else {
            msg->multi_ap_profile = 1;
            LOG(ERROR) << "Failed reading 'multi_ap_profile' parameter!";
        }

        // Multi-AP Primary VLAN ID - Not mandatory
        if (read_param("multi_ap_primary_vlanid", parsed_obj, tmp_int)) {
            msg->multi_ap_primary_vlan_id = tmp_int;
        } else {
            msg->multi_ap_primary_vlan_id = 0;
        }

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

        // Read the disconnect reason value
        size_t numOfValidArgs[2]      = {0};
        char bssid[MAC_ADDR_SIZE]     = {0};
        FieldsToParse fieldsToParse[] = {
            {(void *)&bssid, &numOfValidArgs[0], DWPAL_STR_PARAM, "bssid=", sizeof(bssid)},
            {(void *)&msg->disconnect_reason, &numOfValidArgs[1], DWPAL_INT_PARAM, "reason=", 0},

            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        // TODO: fieldsToParse.field should be contained in one struct and struct size should be
        //       passed to the dwpal_string_to_struct_parse - a security fix for multiline reply
        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse,
                                         sizeof(msg->disconnect_reason)) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << get_iface_name() << "CTRL-EVENT-DISCONNECTED buffer= \n" << buffer;
        LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0]
                   << " bssid= " << std::string(bssid);
        LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1]
                   << " disconnect_reason= " << msg->disconnect_reason;
        /* End of TEMP: Traces... */

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << "!";
                return false;
            }
        }

        msg->bssid = tlvf::mac_from_string(bssid);

        // Forward the event
        event_queue_push(event, msg_buff);

    } break;

    case Event::Terminating:
    case Event::ScanResults:
    case Event::ChannelSwitch: {
        // Forward the event
        event_queue_push(event);

    } break;

    /* Events about connection between dwpald and hostapd*/
    case Event::Interface_Connected_OK:
    case Event::Interface_Reconnected_OK: {
        std::vector<int> vap_id = {};
        LOG(INFO) << "INTERFACE_RECONNECTED_OK or INTERFACE_CONNECTED_OK from " << ifname;
        if (update_conn_status(ifname, vap_id)) {
            LOG(INFO) << "dwpald connection status updated successfully";
        } else {
            LOG(INFO) << "dwpald connection status update failed";
        }

        for (auto &vap_it : vap_id) {
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_ENABLED_NOTIFICATION));
            auto msg      = reinterpret_cast<sHOSTAP_ENABLED_NOTIFICATION *>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            msg->vap_id = vap_it;
            event_queue_push(event, msg_buff);
        }
        break;
    }
    case Event::Interface_Disconnected: {
        LOG(INFO) << "INTERFACE_DISCONNECTED from interface " << ifname;
        for (auto &con : conn_state) {
            // Update interface connection status for vap to false
            auto iface_ids = beerocks::utils::get_ids_from_iface_string(con.first);
            if (iface_ids.vap_id == beerocks::IFACE_RADIO_ID) {
                LOG(DEBUG) << "Ignore INTERFACE_Disconnected on radio";
                continue;
            }
            conn_state[con.first] = false;
            LOG(INFO) << "updated connection status for vap " << con.first << " with false";

            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_DISABLED_NOTIFICATION));
            auto msg      = reinterpret_cast<sHOSTAP_DISABLED_NOTIFICATION *>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            msg->vap_id = iface_ids.vap_id;
            event_queue_push(Event::Interface_Disconnected, msg_buff);
        }
        break;
    }

    // Gracefully ignore unhandled events
    // TODO: Probably should be changed to an error once dwpal will stop
    //       sending empty or irrelevant events...
    default:
        LOG(WARNING) << "Unhandled event received: " << opcode;
        break;
    }

    return true;
}

/* hostap event callback executed in dwpald context */
static int hap_evt_callback(char *ifname, char *op_code, char *buffer, size_t len)
{
    std::string opcode(op_code);
    auto it = ctx_map.find(ifname);
    if (it == ctx_map.end()) {
        LOG(ERROR) << "Could not find context for ifname=" << ifname;
        return -1;
    }
    it->second->process_dwpal_event(ifname, buffer, len, opcode);
    return 0;
}

#define HAP_EVENT(event) (char *)event, sizeof(event) - 1, hap_evt_callback

bool sta_wlan_hal_dwpal::dwpald_attach(char *ifname)
{
    static dwpald_hostap_event supplicant_event_handlers[] = {
        {HAP_EVENT("CTRL-EVENT-CONNECTED")},      {HAP_EVENT("CTRL-EVENT-DISCONNECTED")},
        {HAP_EVENT("CTRL-EVENT-TERMINATING")},    {HAP_EVENT("CTRL-EVENT-SCAN-RESULTS")},
        {HAP_EVENT("CTRL-EVENT-CHANNEL-SWITCH")}, {HAP_EVENT("UNCONNECTED-STA-RSSI")},
        {HAP_EVENT("INTERFACE_CONNECTED_OK")},    {HAP_EVENT("INTERFACE_RECONNECTED_OK")},
        {HAP_EVENT("INTERFACE_DISCONNECTED")}};

    if (dwpald_connect("sta_wlan_hal") != DWPALD_SUCCESS) {
        LOG(ERROR) << "Failed to connect to dwpald";
        // Don't return, as backhaul process can try to connect for all sta interface ( per radio )
    } else {
        if (dwpald_start_listener() != DWPALD_SUCCESS) {
            LOG(ERROR) << "Failed to start listener thread in dwpald";
            return false;
        }

        if (dwpald_nl_drv_attach(0, NULL, NULL) != DWPALD_SUCCESS) {
            LOG(ERROR) << "Failed to attach to dwpald for nl";
            return false;
        }
    }
    if (dwpald_hostap_attach(ifname,
                             sizeof(supplicant_event_handlers) / sizeof(dwpald_hostap_event),
                             supplicant_event_handlers, 0) != DWPALD_SUCCESS) {
        LOG(ERROR) << "Failed to attach to dwpald for interface " << ifname;
        return false;
    }
    return true;
}

bool sta_wlan_hal_dwpal::process_dwpal_nl_event(struct nl_msg *msg, void *arg)
{
    LOG(ERROR) << __func__ << "isn't implemented by this derived and shouldn't be called";
    return false;
}

// Add a new network and return the ID
int sta_wlan_hal_dwpal::add_network()
{
    char *reply = nullptr;

    std::string cmd = "ADD_NETWORK";

    // Send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << "ADD_NETWORK failed!";
        return invalid_network_id;
    }

    // Return the newly added network ID
    return beerocks::string_utils::stoi(reply);
}

bool sta_wlan_hal_dwpal::set_network(int network_id, const std::string &param,
                                     const std::string &value)
{
    const std::string cmd = "SET_NETWORK " + std::to_string(network_id) + " " + param + " " + value;
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "SET_NETWORK failed!";
        return false;
    }
    return true;
}

// Update network parameters
bool sta_wlan_hal_dwpal::set_network_params(int network_id, const std::string &ssid,
                                            const std::string &bssid, WiFiSec sec,
                                            bool mem_only_psk, const std::string &pass,
                                            bool hidden_ssid, int freq)
{
    // Set SSID
    if (!set_network(network_id, "ssid", "\"" + ssid + "\"")) {
        LOG(ERROR) << "set_network_params ssid failed!";
        return false;
    }

    // Set BSSID
    if (!set_network(network_id, "bssid", bssid)) {
        LOG(ERROR) << "set_network_params bssid failed!";
        return false;
    }

    // optional: set frequency
    if (freq > 0) {
        if (!set_network(network_id, "freq_list", std::to_string(freq))) {
            LOG(ERROR) << "Failed setting frequency on iface " << get_iface_name();
            return false;
        }
    }

    // Security and Password
    if (!set_network(network_id, "key_mgmt", dwpal_security_val(sec))) {
        LOG(ERROR) << "set_network_params key_mgmt failed!";
        return false;
    }

    if (hidden_ssid) {
        if (!set_network(network_id, "scan_ssid", "1")) {
            LOG(ERROR) << "set_network_params hidden failed!";
            return false;
        }
    }

    if (mem_only_psk) {
        if (!set_network(network_id, "mem_only_psk", "1")) {
            LOG(ERROR) << "set_network_params hidden failed!";
            return false;
        }
    } else if (!pass.empty()) {
        if (!set_network(network_id, "psk", "\"" + pass + "\"")) {
            LOG(ERROR) << "set_network_params pass failed!";
            return false;
        }
    }

    return true;
}

bool sta_wlan_hal_dwpal::enable_network(int network_id)
{
    const std::string cmd = "ENABLE_NETWORK " + std::to_string(network_id);
    if (!dwpal_send_cmd(cmd)) {
        LOG(ERROR) << "ENABLE_NETWORK Failed on iface " << get_iface_name();
        return false;
    }

    return true;
}

bool sta_wlan_hal_dwpal::is_connected(const std::string &wpa_state)
{
    return (wpa_state.compare("COMPLETED") == 0);
}

bool sta_wlan_hal_dwpal::read_status(ConnectionStatus &connection_status)
{
    LOG(TRACE) << __func__ << " entered for " << get_iface_name();

    char *reply     = nullptr;
    std::string cmd = "STATUS";

    // Send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << "STATUS failed!";
        return false;
    }

    size_t numOfValidArgs[11] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    char bssid[MAC_ADDR_SIZE]     = {0};
    int freq                      = -1;
    char ssid[SSID_MAX_SIZE]      = {0};
    int id                        = -1;
    char mode[32]                 = {0}; // size?
    char pairwise_cipher[32]      = {0}; // size?
    char group_cipher[32]         = {0}; // size?
    char key_mgmt[32]             = {0}; // size?
    char wpa_state[32]            = {0}; // size?
    char address[MAC_ADDR_SIZE]   = {0};
    char uuid[UUID_STR_SIZE]      = {0};
    FieldsToParse fieldsToParse[] = {
        {(void *)wpa_state, &numOfValidArgs[0], DWPAL_STR_PARAM, "wpa_state=", sizeof(wpa_state)},
        {(void *)&freq, &numOfValidArgs[1], DWPAL_INT_PARAM, "freq=", 0},
        {(void *)ssid, &numOfValidArgs[2], DWPAL_STR_PARAM, "ssid=", sizeof(ssid)},
        {(void *)&id, &numOfValidArgs[3], DWPAL_INT_PARAM, "id=", 0},
        {(void *)mode, &numOfValidArgs[4], DWPAL_STR_PARAM, "mode=", sizeof(mode)},
        {(void *)pairwise_cipher, &numOfValidArgs[5], DWPAL_STR_PARAM,
         "pairwise_cipher=", sizeof(pairwise_cipher)},
        {(void *)group_cipher, &numOfValidArgs[6], DWPAL_STR_PARAM, "group_cipher=", sizeof(bssid)},
        {(void *)key_mgmt, &numOfValidArgs[7], DWPAL_STR_PARAM, "key_mgmt=", sizeof(key_mgmt)},
        {(void *)bssid, &numOfValidArgs[8], DWPAL_STR_PARAM, "bssid=", sizeof(bssid)},
        {(void *)address, &numOfValidArgs[9], DWPAL_STR_PARAM, "address=", sizeof(address)},
        {(void *)uuid, &numOfValidArgs[10], DWPAL_STR_PARAM, "uuid=", sizeof(uuid)},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    // TODO: fieldsToParse.field should be contained in one struct and struct size should be
    //       passed to the dwpal_string_to_struct_parse - a security fix for multiline reply
    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(uuid)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    connection_status.bssid           = std::string(bssid);
    connection_status.freq            = freq;
    connection_status.ssid            = std::string(ssid);
    connection_status.id              = id;
    connection_status.mode            = std::string(mode);
    connection_status.pairwise_cipher = std::string(pairwise_cipher);
    connection_status.group_cipher    = std::string(group_cipher);
    connection_status.key_mgmt        = std::string(key_mgmt);
    connection_status.wpa_state       = std::string(wpa_state);
    connection_status.address         = std::string(address);
    connection_status.uuid            = std::string(uuid);

    /* TEMP: Traces... */
    /* Removing the Status reply since it is duplicated */
    //LOG(DEBUG) << "STATUS reply= \n" << reply;
    /* Values not parsed from Status buffer is listed below */
    /* multi_ap_profile=2 multi_ap_primary_vlanid=0 wifi_generation=5 pmf=2 mgmt_group_cipher=BIP sae_group=19 sae_h2e=0 sae_pk=0 ieee80211ac=1 */

    LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0]
               << " wpa_state= " << connection_status.wpa_state;
    LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " freq= " << connection_status.freq;
    LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " ssid= " << connection_status.ssid;
    LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3] << " id= " << connection_status.id;
    LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " mode= " << connection_status.mode;
    LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5]
               << " pairwise_cipher= " << connection_status.pairwise_cipher;
    LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6]
               << " group_cipher= " << connection_status.group_cipher;
    LOG(DEBUG) << "numOfValidArgs[7]= " << numOfValidArgs[7]
               << " key_mgmt= " << connection_status.key_mgmt;
    LOG(DEBUG) << "numOfValidArgs[8]= " << numOfValidArgs[8]
               << " bssid= " << connection_status.bssid;
    LOG(DEBUG) << "numOfValidArgs[9]= " << numOfValidArgs[9]
               << " address= " << connection_status.address;
    LOG(DEBUG) << "numOfValidArgs[10]= " << numOfValidArgs[10]
               << " uuid= " << connection_status.uuid;
    /* End of TEMP: Traces... */

    LOG(DEBUG) << "active network " << m_active_network_id;

    if (numOfValidArgs[0] == 0) {
        LOG(ERROR) << "Failed reading parsed parameter wpa_state ==> Abort";
        return false;
    }

    return true;
}

void sta_wlan_hal_dwpal::update_status(const ConnectionStatus &connection_status)
{
    m_active_network_id = connection_status.id;
    m_active_bssid      = connection_status.bssid;
    m_active_channel    = son::wireless_utils::freq_to_channel(connection_status.freq);
    m_active_ssid       = connection_status.ssid;

    LOG(DEBUG) << "m_active_network_id= " << m_active_network_id;
    LOG(DEBUG) << "m_active_bssid= " << m_active_bssid;
    LOG(DEBUG) << "m_active_channel= " << m_active_channel;
    LOG(DEBUG) << "m_active_ssid= " << m_active_ssid;
}

bool sta_wlan_hal_dwpal::clear_non_associated_devices() { return true; }

#if 0
bool sta_wlan_hal_dwpal::parse_fapi_event(const std::string& opcode, std::shared_ptr<void> obj)
{
    switch (event)
    {
            // Unassociated STA Measurement
        case Event::STA_Unassoc_RSSI: {

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(beerocks::message::sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
            auto msg = reinterpret_cast<beerocks::message::sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";

            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(beerocks::message::sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
            msg->params.rx_rssi = beerocks::RSSI_INVALID;

            // Read and store the MAC address
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "MACAddress field is missing from the FAPI object!";
                return false;
            }

            msg->params.mac = tlvf::mac_from_string(tmpBuff);

            // Read the RSSI value
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "SignalStrength", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "SignalStrength field is missing from the FAPI object!";
                return false;
            }

            // Split the RSSI values
            auto rssiVec = beerocks::string_utils::str_split(tmpBuff, ' ');

            // -128 -128 -128 -128
            int   rssi_cnt  = 0;
            float rssi_watt = 0;
            float rssi_tmp  = 0;
            for (auto v : rssiVec) {
                rssi_tmp = float(beerocks::string_utils::stoi(v));
                if(rssi_tmp > beerocks::RSSI_MIN) {
                    rssi_watt += pow(10, rssi_tmp / float(10));
                    rssi_cnt++;
                }
            }

            // Read the number of received packets
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice.Stats", 0, "PacketsReceived", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "PacketsReceived field is missing from the FAPI object!";
                return false;
            }

            int val = beerocks::string_utils::stoi(tmpBuff);
            msg->params.rx_packets = (val >= 127) ? 127 : val;

            // Measurement succeeded
            if ((rssi_cnt > 0) && (msg->params.rx_packets > 1)) {
                rssi_watt = (rssi_watt / float(rssi_cnt)); 
                msg->params.rx_rssi = int(10 * log10(rssi_watt));

                auto measurement_recv_timestamp = std::chrono::steady_clock::now();
                m_unassoc_measure_window_size   = std::chrono::duration_cast<std::chrono::milliseconds>
                    (measurement_recv_timestamp - m_unassoc_measure_start).count();

                Measurement failed
            } else {
                auto now   = std::chrono::steady_clock::now();
                auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_unassoc_measure_start).count();
                auto temp_sum_delay = (delta - m_unassoc_measure_delay);

                if (temp_sum_delay > m_unassoc_measure_window_size) {
                    LOG(ERROR) << "event_obj->params.rx_packets = -2 , delay = " << int(temp_sum_delay);
                    msg->params.rx_packets = -2; // means the actual measurment started later then aspected 
                }
            }

            Add the message to the queue
                event_queue_push(event, msg_buff);

        } break;

            // Gracefully ignore unhandled events
            // TODO: Probably should be changed to an error once FAPI will stop
            //       sending empty or irrelevant events...
        default: LOG(DEBUG) << "Unhandled event received: " << opcode; break;
    }

    return true;
}
#endif

} // namespace dwpal

std::shared_ptr<sta_wlan_hal> sta_wlan_hal_create(const std::string &iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback,
                                                  const bwl::hal_conf_t &hal_conf)
{
    return std::make_shared<dwpal::sta_wlan_hal_dwpal>(iface_name, callback, hal_conf);
}

} // namespace bwl
