/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_wlan_hal_dummy.h"

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_os_utils.h>
#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>
#include <math.h>
#include <sstream>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace dummy {

#define CSA_EVENT_FILTERING_TIMEOUT_MS 1000

// Temporary storage for station capabilities
struct SRadioCapabilitiesStrings {
    std::string supported_rates;
    std::string ht_cap;
    std::string ht_mcs;
    std::string vht_cap;
    std::string vht_mcs;
    std::string he_cap;
    std::string he_mcs;
    std::string rrm_caps;
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static ap_wlan_hal::Event dummy_to_bwl_event(const std::string &opcode)
{
    if (opcode == "AP-ENABLED") {
        return ap_wlan_hal::Event::AP_Enabled;
    } else if (opcode == "AP-DISABLED") {
        return ap_wlan_hal::Event::AP_Disabled;
    } else if (opcode == "AP-STA-CONNECTED") {
        return ap_wlan_hal::Event::STA_Connected;
    } else if (opcode == "AP-STA-DISCONNECTED") {
        return ap_wlan_hal::Event::STA_Disconnected;
    } else if (opcode == "UNCONNECTED_STA_RSSI") {
        return ap_wlan_hal::Event::STA_Unassoc_RSSI;
    } else if (opcode == "INTERFACE-ENABLED") {
        return ap_wlan_hal::Event::Interface_Enabled;
    } else if (opcode == "INTERFACE-DISABLED") {
        return ap_wlan_hal::Event::Interface_Disabled;
    } else if (opcode == "ACS-STARTED") {
        return ap_wlan_hal::Event::ACS_Started;
    } else if (opcode == "ACS-COMPLETED") {
        return ap_wlan_hal::Event::ACS_Completed;
    } else if (opcode == "ACS-FAILED") {
        return ap_wlan_hal::Event::ACS_Failed;
    } else if (opcode == "AP-CSA-FINISHED") {
        return ap_wlan_hal::Event::CSA_Finished;
    } else if (opcode == "BSS-TM-RESP") {
        return ap_wlan_hal::Event::BSS_TM_Response;
    } else if (opcode == "DFS-CAC-COMPLETED") {
        return ap_wlan_hal::Event::DFS_CAC_Completed;
    } else if (opcode == "DFS-NOP-FINISHED") {
        return ap_wlan_hal::Event::DFS_NOP_Finished;
    } else if (opcode == "MGMT-FRAME") {
        return ap_wlan_hal::Event::MGMT_Frame;
    } else if (opcode == "WPA_EVENT_EAP_FAILURE") {
        return ap_wlan_hal::Event::WPA_Event_EAP_Failure;
    } else if (opcode == "WPA_EVENT_EAP_FAILURE2") {
        return ap_wlan_hal::Event::WPA_Event_EAP_Failure2;
    } else if (opcode == "WPA_EVENT_EAP_TIMEOUT_FAILURE") {
        return ap_wlan_hal::Event::WPA_Event_EAP_Timeout_Failure;
    } else if (opcode == "WPA_EVENT_EAP_TIMEOUT_FAILURE2") {
        return ap_wlan_hal::Event::WPA_Event_EAP_Timeout_Failure2;
    } else if (opcode == "WPS_EVENT_TIMEOUT") {
        return ap_wlan_hal::Event::WPS_Event_Timeout;
    } else if (opcode == "WPS_EVENT_FAIL") {
        return ap_wlan_hal::Event::WPS_Event_Fail;
    } else if (opcode == "WPA_EVENT_SAE_UNKNOWN_PASSWORD_IDENTIFIER") {
        return ap_wlan_hal::Event::WPA_Event_SAE_Unknown_Password_Identifier;
    } else if (opcode == "WPS_EVENT_CANCEL") {
        return ap_wlan_hal::Event::WPS_Event_Cancel;
    } else if (opcode == "AP-STA-POSSIBLE-PSK-MISMATCH") {
        return ap_wlan_hal::Event::AP_Sta_Possible_Psk_Mismatch;
    }

    return ap_wlan_hal::Event::Invalid;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NOTE: Since *base_wlan_hal_dummy* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_dummy::ap_wlan_hal_dummy(const std::string &iface_name, hal_event_cb_t callback,
                                     const hal_conf_t &hal_conf)
    : base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_dummy(bwl::HALType::AccessPoint, iface_name, callback, hal_conf)
{
    m_filtered_events.insert({});
}

ap_wlan_hal_dummy::~ap_wlan_hal_dummy() {}

HALState ap_wlan_hal_dummy::attach(bool block)
{
    auto state = base_wlan_hal_dummy::attach(block);

    // Initialize status files
    if (m_radio_info.is_5ghz) {
        set_channel(149, beerocks::eWiFiBandwidth::BANDWIDTH_80, 5775);
    } else {
        set_channel(1, beerocks::eWiFiBandwidth::BANDWIDTH_40, 2422);
    }

    std::list<son::wireless_utils::sBssInfoConf> bss_info_conf_list;
    update_vap_credentials(bss_info_conf_list, "", "", "");

    // On Operational send the AP_Attached event to the AP Manager
    if (state == HALState::Operational) {
        event_queue_push(Event::AP_Attached);
    }

    return state;
}

bool ap_wlan_hal_dummy::enable() { return true; }

bool ap_wlan_hal_dummy::disable() { return true; }

bool ap_wlan_hal_dummy::set_start_disabled(bool enable, int vap_id) { return true; }

bool ap_wlan_hal_dummy::set_channel(int chan, beerocks::eWiFiBandwidth bw, int center_channel)
{
    m_radio_info.channel         = chan;
    m_radio_info.bandwidth       = bw;
    m_radio_info.vht_center_freq = center_channel;
    m_radio_info.is_dfs_channel  = son::wireless_utils::is_dfs_channel(chan);
    std::stringstream value;
    value << "channel: " << chan << std::endl;
    value << "bw: " << beerocks::utils::convert_bandwidth_to_string(m_radio_info.bandwidth)
          << std::endl;
    value << "center_channel: " << center_channel << std::endl;
    return write_status_file("channel", value.str());
}

bool ap_wlan_hal_dummy::sta_allow(const sMacAddr &mac, const sMacAddr &bssid)
{
    LOG(DEBUG) << "Got client allow request for " << mac << " on bssid " << bssid;
    return true;
}

bool ap_wlan_hal_dummy::sta_deny(const sMacAddr &mac, const sMacAddr &bssid)
{
    LOG(DEBUG) << "Got client disallow request for " << mac << " on bssid " << bssid
               << " reject_sta: 33";
    return true;
}

bool ap_wlan_hal_dummy::clear_blacklist()
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED";
    return true;
}

bool ap_wlan_hal_dummy::sta_acceptlist_modify(const sMacAddr &mac, const sMacAddr &bssid,
                                              bwl::sta_acl_action action)
{
    return true;
}

bool ap_wlan_hal_dummy::set_macacl_type(const eMacACLType &acl_type, const sMacAddr &bssid)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_disassoc(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_deauth(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_bss_steer(int8_t vap_id, const std::string &mac,
                                      const std::string &bssid, int oper_class, int chan,
                                      int disassoc_timer_btt, int valid_int_btt, int reason)
{
    LOG(DEBUG) << "Got steer request for " << mac << " steer to " << bssid;

    auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));
    auto msg      = reinterpret_cast<sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE *>(msg_buff.get());
    LOG_IF(!msg, FATAL) << "Memory allocation failed!";

    memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));

    msg->params.mac         = tlvf::mac_from_string(mac);
    msg->params.status_code = 0;
    // source_bssid should be the vap bssid and not radio_mac, but
    // dummy mode doesn't use vaps yet
    msg->params.source_bssid = tlvf::mac_from_string(get_radio_mac());

    // Add the message to the queue
    event_queue_push(Event::BSS_TM_Response, msg_buff);

    return true;
}

bool ap_wlan_hal_dummy::update_vap_credentials(
    std::list<son::wireless_utils::sBssInfoConf> &bss_info_conf_list,
    const std::string &backhaul_wps_ssid, const std::string &backhaul_wps_passphrase,
    const std::string &bridge_ifname)
{
    std::vector<int> configured_vaps;
    for (auto &bss_info_conf : bss_info_conf_list) {
        auto vap_iter =
            std::find_if(m_radio_info.available_vaps.begin(), m_radio_info.available_vaps.end(),
                         [bss_info_conf](const std::pair<int, VAPElement> &iter) {
                             const std::string &found = iter.second.mac;
                             const std::string &bssid = tlvf::mac_to_string(bss_info_conf.bssid);
                             return bssid.size() == found.size() &&
                                    std::equal(bssid.begin(), bssid.end(), found.begin(),
                                               [](char a, char b) -> bool {
                                                   return (tolower(a) == tolower(b));
                                               });
                         });
        if (vap_iter == m_radio_info.available_vaps.end()) {
            LOG(DEBUG) << "Unable to find bssid " << bss_info_conf.bssid;
            continue;
        }
        configured_vaps.push_back(vap_iter->first);

        auto auth_type =
            son::wireless_utils::wsc_to_bwl_authentication(bss_info_conf.authentication_type);
        if (auth_type == "INVALID") {
            LOG(ERROR) << "Bssid " << bss_info_conf.bssid << " has an invalid auth_type "
                       << int(bss_info_conf.authentication_type);
            bss_info_conf.teardown = true;
        }
        auto enc_type = son::wireless_utils::wsc_to_bwl_encryption(bss_info_conf.encryption_type);
        if (enc_type == "INVALID") {
            LOG(ERROR) << "Bssid " << bss_info_conf.bssid << " has an invalid enc_type "
                       << int(bss_info_conf.encryption_type);
            bss_info_conf.teardown = true;
        }

        if (bss_info_conf.teardown) {
            // Clear existing configuration since bss is flagged for teardown.
            vap_iter->second.fronthaul = false;
            vap_iter->second.backhaul  = false;
            vap_iter->second.ssid.clear();
            continue;
        }

        LOG(DEBUG) << "Autoconfiguration for bssid: " << bss_info_conf.bssid
                   << " ssid: " << bss_info_conf.ssid << " auth_type: " << auth_type
                   << " encr_type: " << enc_type << " network_key: " << bss_info_conf.network_key
                   << " fronthaul: " << beerocks::string_utils::bool_str(bss_info_conf.fronthaul)
                   << " backhaul: " << beerocks::string_utils::bool_str(bss_info_conf.backhaul);

        vap_iter->second.fronthaul = bss_info_conf.fronthaul;
        vap_iter->second.backhaul  = bss_info_conf.backhaul;
        vap_iter->second.ssid      = bss_info_conf.ssid;
    }

    /* Tear down all other VAPs */
    for (int vap_id = beerocks::IFACE_VAP_ID_MIN; vap_id < predefined_vaps_num; vap_id++) {
        if (std::find(configured_vaps.begin(), configured_vaps.end(), vap_id) !=
            configured_vaps.end()) {
            // Vap is configured, skip teardown.
            continue;
        }
        m_radio_info.available_vaps[vap_id].fronthaul = false;
        m_radio_info.available_vaps[vap_id].backhaul  = false;
        m_radio_info.available_vaps[vap_id++].ssid.clear();
    }

    /* Write current conf to tmp file*/
    std::stringstream value;
    for (int id = beerocks::IFACE_VAP_ID_MIN; id < predefined_vaps_num; id++) {
        value << "- vap_" << id << ":" << std::endl;
        value << "  iface: " << m_radio_info.available_vaps[id].bss << std::endl;
        value << "  bssid: " << m_radio_info.available_vaps[id].mac << std::endl;
        value << "  ssid: '" << m_radio_info.available_vaps[id].ssid << "'" << std::endl;
        value << "  fronthaul: " << m_radio_info.available_vaps[id].fronthaul << std::endl;
        value << "  backhaul: " << m_radio_info.available_vaps[id].backhaul << std::endl;

        // Generate AP_ENABLED event on that BSS iface
        parsed_obj_map_t parsed_obj;
        parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_OPCODE] = "AP-ENABLED";
        parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_IFACE]  = m_radio_info.available_vaps[id].bss;
        process_dummy_event(parsed_obj);
    }

    LOG(DEBUG) << "Writing to VAP status file:" << std::endl
               << "###########################" << std::endl
               << value.str() << std::endl
               << "###########################";
    write_status_file("vap", value.str());

    return true;
}

bool ap_wlan_hal_dummy::sta_unassoc_rssi_measurement(const std::string &mac, int chan,
                                                     beerocks::eWiFiBandwidth bw,
                                                     int vht_center_frequency, int delay,
                                                     int window_size)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_softblock_add(const std::string &vap_name,
                                          const std::string &client_mac, uint8_t reject_error_code,
                                          uint8_t probe_snr_threshold_hi,
                                          uint8_t probe_snr_threshold_lo,
                                          uint8_t authetication_snr_threshold_hi,
                                          uint8_t authetication_snr_threshold_lo)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_softblock_remove(const std::string &vap_name,
                                             const std::string &client_mac)
{
    return true;
}

bool ap_wlan_hal_dummy::switch_channel(int chan, beerocks::eWiFiBandwidth bw,
                                       int vht_center_frequency, int csa_beacon_count)
{
    LOG(TRACE) << __func__ << " channel: " << chan
               << ", bw: " << beerocks::utils::convert_bandwidth_to_string(bw)
               << ", vht_center_frequency: " << vht_center_frequency;

    m_radio_info.last_csa_sw_reason = ChanSwReason::Unknown;

    event_queue_push(Event::ACS_Started);
    event_queue_push(Event::ACS_Completed);
    event_queue_push(Event::CSA_Finished);

    return set_channel(chan, bw, vht_center_frequency);
}

bool ap_wlan_hal_dummy::cancel_cac(int chan, beerocks::eWiFiBandwidth bw, int vht_center_frequency,
                                   int secondary_chan)
{
    return set_channel(chan, bw, vht_center_frequency);
}

bool ap_wlan_hal_dummy::failsafe_channel_set(int chan, int bw, int vht_center_frequency)
{
    return true;
}

bool ap_wlan_hal_dummy::failsafe_channel_get(int &chan, int &bw) { return false; }

// zero wait dfs APIs
bool ap_wlan_hal_dummy::is_zwdfs_supported() { return false; }
bool ap_wlan_hal_dummy::set_zwdfs_antenna(bool enable) { return false; }
bool ap_wlan_hal_dummy::is_zwdfs_antenna_enabled() { return false; }

bool ap_wlan_hal_dummy::hybrid_mode_supported() { return true; }

bool ap_wlan_hal_dummy::restricted_channels_set(char *channel_list) { return true; }

bool ap_wlan_hal_dummy::restricted_channels_get(char *channel_list) { return false; }

bool ap_wlan_hal_dummy::read_acs_report() { return true; }

bool ap_wlan_hal_dummy::set_tx_power_limit(int tx_pow_limit)
{
    LOG(TRACE) << " setting power limit: " << tx_pow_limit << " dBm";
    m_radio_info.tx_power = tx_pow_limit;
    std::stringstream value;
    value << "tx_power: " << m_radio_info.tx_power << std::endl;
    write_status_file("tx_power", value.str());
    return true;
}

bool ap_wlan_hal_dummy::set_vap_enable(const std::string &iface_name, const bool enable)
{
    return true;
}

bool ap_wlan_hal_dummy::get_vap_enable(const std::string &iface_name, bool &enable) { return true; }

bool ap_wlan_hal_dummy::generate_connected_clients_events(
    bool &is_finished_all_clients, std::chrono::steady_clock::time_point max_iteration_timeout)
{
    return true;
}

bool ap_wlan_hal_dummy::pre_generate_connected_clients_events() { return true; }

bool ap_wlan_hal_dummy::start_wps_pbc()
{
    LOG(DEBUG) << "Start WPS PBC";
    return true;
}

bool ap_wlan_hal_dummy::set_mbo_assoc_disallow(const std::string &bssid, bool enable)
{
    LOG(DEBUG) << "Set MBO ASSOC DISALLOW for bssid " << bssid << " to " << enable;
    return true;
}

bool ap_wlan_hal_dummy::set_radio_mbo_assoc_disallow(bool enable)
{
    LOG(DEBUG) << "Set MBO ASSOC DISALLOW for radio to " << enable;
    return true;
}

bool ap_wlan_hal_dummy::set_primary_vlan_id(uint16_t primary_vlan_id)
{
    LOG(DEBUG) << "set_primary_vlan_id " << primary_vlan_id;
    return true;
}

bool ap_wlan_hal_dummy::process_dummy_data(parsed_obj_map_t &parsed_obj) { return true; }

bool ap_wlan_hal_dummy::process_dummy_event(parsed_obj_map_t &parsed_obj)
{
    char *tmp_str;

    // Filter out empty events
    std::string opcode;
    if (!(parsed_obj.find(DUMMY_EVENT_KEYLESS_PARAM_OPCODE) != parsed_obj.end() &&
          !(opcode = parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_OPCODE]).empty())) {
        return true;
    }

    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto event = dummy_to_bwl_event(opcode);

    switch (event) {
    // STA Connected
    case Event::STA_Connected: {
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));

        msg->params.vap_id = beerocks::IFACE_VAP_ID_MIN;
        LOG(DEBUG) << "iface name = " << get_iface_name()
                   << ", vap_id = " << int(msg->params.vap_id);

        if (!dummy_obj_read_str(DUMMY_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading mac parameter!";
            return false;
        }
        msg->params.mac = tlvf::mac_from_string(tmp_str);
        const char assoc_req[] =
            "00003A01029A96FB591100504322565F029A96FB591110E431141400000E4D756C74692D41502D3234472D"
            "31010802040B0C121618242102001430140100000FAC040100000FAC040100000FAC02000032043048606C"
            "3B10515153547374757677787C7D7E7F80823B160C01020304050C161718191A1B1C1D1E1F202180818246"
            "057000000000460571505000047F0A04000282214000408000DD070050F2020001002D1A2D1103FFFF0000"
            "000000000000000000000000000018E6E10900BF0CB079D133FAFF0C03FAFF0C03FF1C2303080000008064"
            "3000000D009F000C0000FAFFFAFF391CC7711C07C70110DD07506F9A16030103";

        //convert the hex string to binary
        auto binary_str                      = get_binary_association_frame(assoc_req);
        msg->params.association_frame_length = binary_str.length();

        std::copy_n(&binary_str[0], binary_str.length(), msg->params.association_frame);
        bool caps_valid = true;
        SRadioCapabilitiesStrings caps_strings;
        if (!dummy_obj_read_str("SupportedRates", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading SupportedRates parameter!";
            caps_valid = false;
        } else {
            caps_strings.supported_rates.assign(tmp_str);
        }

        if (!dummy_obj_read_str("HT_CAP", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading HT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.ht_cap.assign(tmp_str);
        }

        if (!dummy_obj_read_str("HT_MCS", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading HT_MCS parameter!";
            caps_valid = false;
        } else {
            caps_strings.ht_mcs.assign(tmp_str);
        }

        if (!dummy_obj_read_str("VHT_CAP", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading VHT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.vht_cap.assign(tmp_str);
        }

        if (!dummy_obj_read_str("VHT_MCS", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading VHT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.vht_mcs.assign(tmp_str);
        }

        if (!dummy_obj_read_str("HE_CAP", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading HE_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.he_cap.assign(tmp_str);
        }

        if (!dummy_obj_read_str("HE_MCS", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading HE_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.he_mcs.assign(tmp_str);
        }

        if (caps_valid) {
            //get_sta_caps(caps_strings, msg->params.capabilities, get_radio_info().is_5ghz);
        } else {
            LOG(ERROR) << "One or more of required capability strings is missing!";

            // Setting minimum default values
            msg->params.capabilities.ant_num             = 1;
            msg->params.capabilities.wifi_standard       = STANDARD_N;
            msg->params.capabilities.default_mcs         = MCS_6;
            msg->params.capabilities.ht_ss               = 1;
            msg->params.capabilities.ht_bw               = beerocks::BANDWIDTH_20;
            msg->params.capabilities.ht_mcs              = beerocks::MCS_7;
            msg->params.capabilities.ht_low_bw_short_gi  = 1;
            msg->params.capabilities.ht_high_bw_short_gi = 0;
            if (m_radio_info.is_5ghz) {
                msg->params.capabilities.wifi_standard |= STANDARD_AC;
                msg->params.capabilities.vht_ss               = 1;
                msg->params.capabilities.vht_bw               = beerocks::BANDWIDTH_80;
                msg->params.capabilities.vht_mcs              = beerocks::MCS_9;
                msg->params.capabilities.vht_low_bw_short_gi  = 1;
                msg->params.capabilities.vht_high_bw_short_gi = 0;

                msg->params.capabilities.wifi_standard |= STANDARD_AX;
                msg->params.capabilities.he_ss  = 1;
                msg->params.capabilities.he_bw  = beerocks::BANDWIDTH_80;
                msg->params.capabilities.he_mcs = beerocks::MCS_11;
            }
        }

        // Add the message to the queue
        event_queue_push(Event::STA_Connected, msg_buff);
    } break;
    case Event::STA_Disconnected: {
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));

        msg->params.vap_id = beerocks::IFACE_VAP_ID_MIN;
        LOG(DEBUG) << "iface name = " << get_iface_name()
                   << ", vap_id = " << int(msg->params.vap_id);

        if (!dummy_obj_read_str(DUMMY_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading mac parameter!";
            return false;
        }

        // Store the MAC address of the disconnected STA
        msg->params.mac = tlvf::mac_from_string(tmp_str);

        // Add the message to the queue
        event_queue_push(Event::STA_Disconnected, msg_buff);
    } break;

    // STA 802.11 management frame event
    case Event::MGMT_Frame: {
        // Read frame data
        if (!dummy_obj_read_str("DATA", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading DATA parameter!";
            return false;
        }

        // Create the management frame notification event
        auto mgmt_frame = create_mgmt_frame_notification(tmp_str);
        if (!mgmt_frame) {
            LOG(WARNING) << "Failed creating management frame notification!";
            return true; // Just a warning, do not fail
        }

        event_queue_push(Event::MGMT_Frame, mgmt_frame);
    } break;

    case Event::WPA_Event_EAP_Failure:
    case Event::WPA_Event_EAP_Failure2:
    case Event::WPA_Event_EAP_Timeout_Failure:
    case Event::WPA_Event_EAP_Timeout_Failure2:
    case Event::WPS_Event_Timeout:
    case Event::WPS_Event_Fail:
    case Event::WPA_Event_SAE_Unknown_Password_Identifier:
    case Event::WPS_Event_Cancel:
    case Event::AP_Sta_Possible_Psk_Mismatch: {
        LOG(DEBUG) << "Station connection failure event";
        std::string sta_mac = parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_MAC];
        if (sta_mac.empty()) {
            LOG(ERROR) << "Station mac parameter not found!";
            return false;
        }

        auto sta_conn_fail   = std::make_shared<sStaConnectionFail>();
        std::string vap_name = parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_IFACE];
        LOG(ERROR) << "vap_name = " << vap_name;
        std::string bssid;

        for (const auto &iter : m_radio_info.available_vaps) {
            if (vap_name == iter.second.bss) {
                bssid = iter.second.mac;
                break;
            }
        }
        LOG(ERROR) << "bssid = " << bssid;

        sta_conn_fail->bssid   = tlvf::mac_from_string(bssid);
        sta_conn_fail->sta_mac = tlvf::mac_from_string(sta_mac);

        event_queue_push(event, sta_conn_fail);
    } break;

    case Event::AP_Disabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_DISABLED_NOTIFICATION));
        auto msg      = reinterpret_cast<sHOSTAP_DISABLED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_DISABLED_NOTIFICATION));

        std::string interface = parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_IFACE];
        if (interface.empty()) {
            LOG(ERROR) << "Could not find interface name.";
            return false;
        }

        m_radio_info.radio_state = eRadioState::DISABLED;

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        event_queue_push(Event::AP_Disabled, msg_buff); // send message to the AP manager

    } break;

    case Event::AP_Enabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_ENABLED_NOTIFICATION));
        auto msg      = reinterpret_cast<sHOSTAP_ENABLED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_ENABLED_NOTIFICATION));

        std::string interface = parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_IFACE];
        if (interface.empty()) {
            LOG(ERROR) << "Could not find interface name.";
            return false;
        }

        m_radio_info.radio_state = eRadioState::ENABLED;

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        event_queue_push(Event::AP_Enabled, msg_buff);
    } break;

    // Gracefully ignore unhandled events
    default: {
        LOG(DEBUG) << "Unhandled event received: " << opcode;
    } break;
    }

    return true;
}

bool ap_wlan_hal_dummy::set_cce_indication(uint16_t advertise_cce)
{
    LOG(DEBUG) << "ap_wlan_hal_dummy: set_cce_indication, advertise_cce=" << advertise_cce;
    return true;
}

bool ap_wlan_hal_dummy::set(const std::string &param, const std::string &value, int vap_id)
{
    LOG(TRACE) << __func__;
    return true;
}

int ap_wlan_hal_dummy::add_bss(std::string &ifname, son::wireless_utils::sBssInfoConf &bss_conf,
                               std::string &bridge, bool vbss)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::remove_bss(std::string &ifname)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::add_key(const std::string &ifname, const sKeyInfo &key_info)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::add_station(const std::string &ifname, const sMacAddr &mac,
                                    std::vector<uint8_t> &raw_assoc_req)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::get_key(const std::string &ifname, sKeyInfo &key_info)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::send_delba(const std::string &ifname, const sMacAddr &dst,
                                   const sMacAddr &src, const sMacAddr &bssid)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

void ap_wlan_hal_dummy::send_unassoc_sta_link_metric_query(
    std::shared_ptr<wfa_map::tlvUnassociatedStaLinkMetricsQuery> &query)
{
    auto opclass       = query->operating_class_of_channel_list();
    auto chan_list_len = query->channel_list_length();
    if (m_measurement_start == true) {
        LOG(DEBUG) << "Unassociate Sta link metrics measurement already running";
        return;
    }
    m_measurement_start = true;
    LOG(DEBUG) << "Unassociate Sta link metrics: opclass = " << opclass
               << ", channel_list_len = " << chan_list_len;
    for (int i = 0; i < chan_list_len; i = i + 1) {
        auto channel_list    = std::get<1>(query->channel_list(i));
        auto sta_list_length = channel_list.sta_list_length();
        auto channel         = channel_list.channel_number();
        LOG(INFO) << "channel = " << channel << ", With below STA list len=" << sta_list_length;
        for (int j = 0; j < sta_list_length; j = j + 1) {
            auto sta = std::get<1>(channel_list.sta_list(j));
            LOG(INFO) << "Sta[" << j << "] = " << sta;

            std::string mac = tlvf::mac_to_string(sta);
            // Fill relevant information
            sUnAssocStaInfo unassoc_sta;
            m_opclass                               = opclass;
            unassoc_sta.channel                     = channel;
            unassoc_sta.rcpi                        = 90;
            unassoc_sta.last_sta                    = true;
            unassoc_sta.m_unassoc_sta_metrics_start = std::chrono::steady_clock::now();
            // Add to the MAP
            m_unassoc_sta_map[mac] = unassoc_sta;
        }
    }

    // simulating response and pushing event to handle the reponse case
    event_queue_push(Event::STA_Unassoc_Link_Metrics);
}

bool ap_wlan_hal_dummy::prepare_unassoc_sta_link_metrics_response(
    std::shared_ptr<wfa_map::tlvUnassociatedStaLinkMetricsResponse> &response)
{
    response->operating_class_of_channel_list() = m_opclass;
    auto now                                    = std::chrono::steady_clock::now();
    for (auto &sta_entry : m_unassoc_sta_map) {
        if (!response->alloc_sta_list(1)) {
            LOG(ERROR) << "Failed allocate_sta_list";
            return false;
        }
        auto &unassoc_sta               = std::get<1>(response->sta_list(0));
        unassoc_sta.channel_number      = sta_entry.second.channel;
        unassoc_sta.uplink_rcpi_dbm_enc = sta_entry.second.rcpi;
        unassoc_sta.sta_mac             = tlvf::mac_from_string(sta_entry.first);
        unassoc_sta.measurement_to_report_delta_msec =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - sta_entry.second.m_unassoc_sta_metrics_start)
                .count();
    }

    // clear the map and mark measurement_start as false
    m_unassoc_sta_map.clear();
    m_measurement_start = false;

    return true;
}

bool ap_wlan_hal_dummy::set_beacon_da(const std::string &ifname, const sMacAddr &mac)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::update_beacon(const std::string &ifname)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::set_no_deauth_unknown_sta(const std::string &ifname, bool value)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::configure_service_priority(const uint8_t *data)
{
    std::stringstream ss;
    for (auto i = 0; i < 21; i++) {
        if (i != 0) {
            ss << ",";
        }
        ss << i << "," << int(data[i]);
    }
    // sadly, std::move has no effect prio to C++17
    LOG(DEBUG) << "Setting QOS_MAP_SET " << std::move(ss).str();
    return true;
}

bool ap_wlan_hal_dummy::set_spatial_reuse_config(
    son::wireless_utils::sSpatialReuseParams &spatial_reuse_params)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

bool ap_wlan_hal_dummy::get_spatial_reuse_config(
    son::wireless_utils::sSpatialReuseParams &spatial_reuse_params)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED!";
    return false;
}

} // namespace dummy

std::shared_ptr<ap_wlan_hal> ap_wlan_hal_create(std::string iface_name, bwl::hal_conf_t hal_conf,
                                                base_wlan_hal::hal_event_cb_t callback)
{
    return std::make_shared<dummy::ap_wlan_hal_dummy>(iface_name, callback, hal_conf);
}

} // namespace bwl
