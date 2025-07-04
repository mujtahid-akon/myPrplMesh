/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2022 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "sta_wlan_hal_whm.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>

using namespace beerocks;
using namespace wbapi;

namespace bwl {
namespace whm {

static struct {
    std::mutex m_mutex;
    const sta_wlan_hal_whm *owner;
    std::set<sta_wlan_hal_whm const *> instances;
    std::vector<int> empty_vector;
} call_once_impl;

sta_wlan_hal_whm::sta_wlan_hal_whm(const std::string &iface_name, hal_event_cb_t callback,
                                   const bwl::hal_conf_t &hal_conf)
    : base_wlan_hal(bwl::HALType::Station, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_whm(bwl::HALType::Station, iface_name, callback, hal_conf)
{
    int amx_fd = m_ambiorix_cl.get_fd();
    LOG_IF((amx_fd == -1), FATAL) << "Failed to get amx  fd";
    int amxp_fd = m_ambiorix_cl.get_signal_fd();
    LOG_IF((amxp_fd == -1), FATAL) << "Failed to get amx signal fd";

    m_fds_ext_events = {amx_fd, amxp_fd};

    m_ambiorix_cl.resolve_path(wbapi_utils::search_path_ep_by_iface(iface_name), m_ep_path);

    // get radio path from m_ep_path
    std::string radRef;
    auto obj = m_ambiorix_cl.get_object(m_ep_path);
    if (obj) {
        radRef = wbapi_utils::get_path_radio_reference(*obj);
        m_ambiorix_cl.resolve_path(radRef, m_radio_path);

        if (!m_ambiorix_cl.get_param(m_radio_info.iface_name, m_radio_path, "Name")) {
            LOG(ERROR) << "Failed to update m_radio_info interface name";
        }
    } else {
        LOG(ERROR) << "Failed to get object from " << m_ep_path;
    }

    if (!m_ep_path.empty() && hal_conf.is_repeater) {
        // Enable the endpoint instance
        AmbiorixVariant params(AMXC_VAR_ID_HTABLE);
        params.add_child<bool>("Enable", true);
        bool ret = m_ambiorix_cl.update_object(m_ep_path, params);
        LOG_IF((!ret), ERROR) << "Failed to enable endpoint, path:" << m_ep_path;
    }

    LOG(DEBUG) << "init sta_wlan_hal_whm for " << m_ep_path << " with radio " << m_radio_path;
    subscribe_to_ep_events();
    subscribe_to_ep_wps_events();
    subscribe_to_scan_complete_events();
}

const std::vector<int> &sta_wlan_hal_whm::get_ext_events_fds() const
{

    std::lock_guard<std::mutex> lock(call_once_impl.m_mutex);

    if (call_once_impl.owner == nullptr) {
        call_once_impl.owner = this;
    }

    if (call_once_impl.instances.find(this) == call_once_impl.instances.end()) {
        call_once_impl.instances.insert(this);
        LOG(DEBUG) << "add " << this << " to instance list";
    } else {
        LOG(DEBUG) << "second call for " << this;
    }

    if (call_once_impl.owner == this) {
        LOG(DEBUG) << "first call return good values";
        return (m_fds_ext_events);
    } else {
        LOG(DEBUG) << "return empty vector";
        return call_once_impl.empty_vector;
    }
}

sta_wlan_hal_whm::~sta_wlan_hal_whm()
{
    std::lock_guard<std::mutex> lock(call_once_impl.m_mutex);

    if (call_once_impl.owner == this) {
        LOG(DEBUG) << "reset owner " << this;
        call_once_impl.owner = nullptr;
    }

    auto it = call_once_impl.instances.find(this);

    if (it != call_once_impl.instances.end()) {
        call_once_impl.instances.erase(it);
    } else {
        LOG(INFO) << "should not happen";
    }
    sta_wlan_hal_whm::detach();
}

void sta_wlan_hal_whm::subscribe_to_ep_events()
{
    std::string wifi_ep_path  = wbapi_utils::search_path_ep();
    auto event_handler        = std::make_shared<sAmbiorixEventHandler>();
    event_handler->event_type = AMX_CL_OBJECT_CHANGED_EVT;

    event_handler->callback_fn = [this](AmbiorixVariant &event_data) -> void {
        std::string ep_path;
        if (!event_data.read_child(ep_path, "path") || ep_path.empty()) {
            return;
        }

        auto parameters = event_data.find_child("parameters");
        if (!parameters || parameters->empty()) {
            return;
        }
        auto params_map = parameters->read_children<AmbiorixVariantMapSmartPtr>();
        if (!params_map) {
            return;
        }
        // if endpoint path already resolved, then event path must match it
        if (!m_ep_path.empty()) {
            if (m_ep_path != ep_path) {
                return;
            }
        } else {
            //otherwise check first for notif param intfname
            std::string intf_name;
            for (auto &param_it : *params_map) {
                auto key   = param_it.first;
                auto value = param_it.second.find_child("to");
                if (key.empty() || !value || value->empty()) {
                    continue;
                }
                if (key == "IntfName") {
                    intf_name = value->get<std::string>();
                    break;
                }
            }
            //If not in the notif param, try querying itfname
            if (intf_name.empty() && !m_ambiorix_cl.get_param<>(intf_name, ep_path, "IntfName")) {
                return;
            }
            //endpoint itfname must match
            if (intf_name != get_iface_name()) {
                return;
            }
            //Then save the resolved endpoint path
            m_ep_path = ep_path;
        }
        for (auto &param_it : *params_map) {
            auto key       = param_it.first;
            auto new_value = param_it.second.find_child("to");
            auto old_value = param_it.second.find_child("from");

            if (key.empty() || !new_value || new_value->empty() || !old_value ||
                old_value->empty()) {
                continue;
            }
            process_ep_event(get_iface_name(), key, new_value.get(), old_value.get());
        }
    };

    std::string filter = "(path matches '" + wifi_ep_path +
                         "[0-9]+.$')"
                         " && (notification == '" +
                         AMX_CL_OBJECT_CHANGED_EVT +
                         "')"
                         " && ((contains('parameters.ConnectionStatus'))"
                         " || (contains('parameters.IntfName')))";

    m_ambiorix_cl.subscribe_to_object_event(wifi_ep_path, event_handler, filter);
}

void sta_wlan_hal_whm::subscribe_to_ep_wps_events()
{
    std::string wifi_wps_path = m_ep_path + "WPS.";
    auto event_handler        = std::make_shared<sAmbiorixEventHandler>();
    event_handler->event_type = AMX_CL_WPS_PAIRING_DONE;

    event_handler->callback_fn = [this](AmbiorixVariant &event_data) -> void {
        if (!event_data) {
            return;
        }

        process_ep_wps_event(get_iface_name(), &event_data);
    };

    std::string filter = "path matches '" + wifi_wps_path + "'";
    m_ambiorix_cl.subscribe_to_object_event(wifi_wps_path, event_handler, filter);
}

bool sta_wlan_hal_whm::process_scan_complete_event(const std::string &result)
{

    if (result == "error") {
        LOG(DEBUG) << " received ScanComplete event with Error indication!";
        m_scan_active = false;
        return false;
    }
    if (result == "done" && m_scan_active) {
        event_queue_push(Event::ScanResults);
        m_scan_active = false;
    }
    return true;
}

bool sta_wlan_hal_whm::start_wps_pbc()
{
    // MultiAPEnable default value may be false, do not rely only on
    // an outside entity to configure the EndPoint correctly
    AmbiorixVariant enable_map(AMXC_VAR_ID_HTABLE);
    enable_map.add_child("MultiAPEnable", true);
    bool ret = m_ambiorix_cl.update_object(m_ep_path, enable_map);
    if (!ret) {
        LOG(WARNING) << "failed to enable multi ap mode for" << m_ep_path;
    }

    AmbiorixVariant args, result;
    std::string wps_path = m_ep_path + "WPS.";
    ret                  = m_ambiorix_cl.call(wps_path, "pushButton", args, result);

    if (!ret) {
        LOG(ERROR) << "start_wps_pbc() failed!";
        return false;
    }
    return true;
}

bool sta_wlan_hal_whm::detach() { return true; }

bool sta_wlan_hal_whm::initiate_scan()
{

    //scan is already active, as per spec, cancel the old one and start new one
    if (m_scan_active) {
        AmbiorixVariant result_abort;
        AmbiorixVariant args_abort(AMXC_VAR_ID_HTABLE);
        if (!m_ambiorix_cl.call(m_radio_path, "stopScan", args_abort, result_abort)) {
            LOG(INFO) << " remote function stopScan Failed!";
        }
        m_scan_active =
            false; // if for some reasons, no scan is active, stopScan will return error, thus the need to reset m_scan_active
    }

    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    if (!m_ambiorix_cl.call(m_radio_path, "startScan", args, result)) {
        LOG(ERROR) << " remote function call startScan Failed!";
        return false;
    }
    m_scan_active = true;
    return true;
}

bool sta_wlan_hal_whm::scan_bss(const sMacAddr &bssid, uint8_t channel,
                                beerocks::eFreqType freq_type)
{
    if (bssid == beerocks::net::network_utils::ZERO_MAC || channel == 0) {
        LOG(ERROR) << "Invalid parameters";
        return false;
    }

    //scan is already active, as per spec, cancel the old one and start new one
    if (m_scan_active) {
        AmbiorixVariant result_abort;
        AmbiorixVariant args_abort(AMXC_VAR_ID_HTABLE);
        if (!m_ambiorix_cl.call(m_radio_path, "stopScan", args_abort, result_abort)) {
            LOG(INFO) << " remote function stopScan Failed!";
        }
        m_scan_active =
            false; // if for some reasons, no scan is active, stopScan will return error, thus the need to reset m_scan_active
    }

    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    args.add_child("BSSID", tlvf::mac_to_string(bssid));
    args.add_child("channels", channel);
    if (!m_ambiorix_cl.call(m_radio_path, "startScan", args, result)) {
        LOG(ERROR) << " remote function call startScan Failed!";
        return false;
    }
    m_scan_active = true;
    return true;
}

int sta_wlan_hal_whm::get_scan_results(const std::string &ssid, std::vector<sScanResult> &list,
                                       bool parse_vsie)
{
    AmbiorixVariant result;
    list.clear();
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    if (!m_ambiorix_cl.call(m_radio_path, "getScanResults", args, result)) {
        LOG(ERROR) << " remote function call getScanResults Failed!";
        return 0;
    }
    AmbiorixVariantListSmartPtr scan_results_list =
        result.read_children<AmbiorixVariantListSmartPtr>();
    if (!scan_results_list) {
        LOG(ERROR) << "failed reading scan_results!";
        return 0;
    }

    if (scan_results_list->empty()) {
        LOG(ERROR) << "scan_results are empty";
        return 0;
    }

    auto results_as_wrapped_list = scan_results_list->front();

    auto ssid_results_as_list =
        results_as_wrapped_list.read_children<AmbiorixVariantListSmartPtr>();

    for (auto &ssid_results_map : *ssid_results_as_list) {
        auto data_map = ssid_results_map.read_children<AmbiorixVariantMapSmartPtr>();
        auto &map     = *data_map;

        if (map.find("BSSID") == map.end()) {
            LOG(DEBUG) << " no BSSID, skipping...";
            continue;
        }

        sScanResult ap;

        std::string bssid;
        map["BSSID"].get(bssid);
        ap.bssid = tlvf::mac_from_string(bssid);

        int32_t center_channel(0);
        if (map.find("CentreChannel") != map.end()) {
            map["CentreChannel"].get(center_channel);
        }
        int32_t bandwidth(0);
        if (map.find("Bandwidth") != map.end()) {
            map["Bandwidth"].get(bandwidth);
        }
        if (map.find("Channel") != map.end()) {
            int32_t channel(0);
            map["Channel"].get(channel);
            ap.channel = (uint8_t)channel;
            if (bandwidth != 0 && center_channel != 0) {
                WifiChannel wifi_chan(channel, center_channel,
                                      utils::convert_bandwidth_to_enum(bandwidth));
                ap.freq_type = wifi_chan.get_freq_type();
            }
        }

        if (map.find("RSSI") != map.end()) {
            int32_t rssi(UINT8_MAX);
            map["RSSI"].get(rssi);
            ap.rssi = (int8_t)rssi;
        }

        list.push_back(ap);
    }

    return list.size();
}

bool sta_wlan_hal_whm::connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                               bool mem_only_psk, const std::string &bssid, ChannelFreqPair channel,
                               bool hidden_ssid)
{
    LOG(DEBUG) << "Connect interface " << get_iface_name() << " to SSID = " << ssid
               << ", BSSID = " << bssid << ", Channel = " << int(channel.first)
               << ", freq type =" << channel.second << ", Sec = " << sec
               << ", mem_only_psk=" << int(mem_only_psk);

    if (ssid.empty() || sec == WiFiSec::Invalid) {
        LOG(ERROR) << "Invalid params!";
        return false;
    }

    // First disconnect (or do nothing if not connected)
    if (!disconnect()) {
        LOG(WARNING) << "Failed disconnecting before connecting to the new BSSID";
        return false;
    }

    // Set profile
    Profile profile = {-1, "prplMesh", ssid, bssid, sec, mem_only_psk, pass, hidden_ssid, channel};
    if (!set_profile(profile)) {
        LOG(ERROR) << "Unable to connect to profile, on interface: " << get_iface_name();
        return false;
    }

    return true;
}

bool sta_wlan_hal_whm::set_profile(Profile &profile)
{
    // Find profile
    int profile_id = find_profile_by_alias(profile.alias);
    if (profile_id <= 0) {
        // Add a new profile
        profile_id = add_profile(profile.alias);
        if (profile_id <= 0) {
            LOG(ERROR) << "Failed (" << profile_id
                       << ") adding new profile to interface: " << get_iface_name();
            return false;
        }
    }
    // Update profile id
    profile.id = profile_id;

    // Update profile parameters
    if (!set_profile_params(profile)) {
        LOG(ERROR) << "Failed setting profile id = " << profile_id
                   << " on interface: " << get_iface_name();
        return false;
    }

    // Enable the profile
    if (!enable_profile(profile_id)) {
        LOG(ERROR) << "Failed enabling profile id = " << profile_id
                   << " on interface: " << get_iface_name();
        return false;
    }

    m_active_profile_id = profile_id;

    LOG(DEBUG) << "Profile with id " << profile_id << " has been added and enabled on interface "
               << get_iface_name();

    return true;
}

bool sta_wlan_hal_whm::disconnect()
{
    LOG(TRACE) << "Disconnect profile id " << m_active_profile_id
               << " on interface: " << get_iface_name();

    Endpoint endpoint;
    if (!read_status(endpoint)) {
        LOG(ERROR) << "Failed reading status for " << get_iface_name() << "! can't disconnect";
        return false;
    }

    // Return gracefully if endpoint connection_status is not connected
    if (!is_connected(endpoint.connection_status)) {
        LOG(DEBUG) << "Active profile is not connected";
        return true;
    }

    // Return gracefully if no endpoint connection_status is connected
    if (m_active_profile_id <= 0) {
        LOG(DEBUG) << "Active profile does not exist";
        return true;
    }

    // Connection status id must be the same as the active profile id
    if (m_active_profile_id != endpoint.active_profile_id) {
        LOG(ERROR) << "Profile id mismatch: m_active_profile_id(" << m_active_profile_id << ") != "
                   << "endpoint.active_profile_id(" << endpoint.active_profile_id << ")";
        return false;
    }

    if (remove_profile(m_active_profile_id)) {
        LOG(ERROR) << "Failed to disconnect profile " << m_active_profile_id;
        return false;
    }

    // Clear state
    clear_conn_state();

    return true;
}

bool sta_wlan_hal_whm::roam(const sMacAddr &bssid, ChannelFreqPair channel)
{

    if (!is_connected()) {
        LOG(ERROR) << get_iface_name() << " Not connected, can't roam";
        return false;
    }

    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    args.add_child("bssid", tlvf::mac_to_string(bssid));
    args.add_child("tries", 2);        //arbitrary choice
    args.add_child("timeoutInSec", 5); //arbitrary choice
    if (!m_ambiorix_cl.call(m_ep_path, "roamTo", args, result)) {
        LOG(ERROR) << " remote function call roamTo Failed!";
        return false;
    }

    // Update the active channel and bssid
    m_active_bssid   = tlvf::mac_to_string(bssid);
    m_active_channel = channel.first;

    return true;
}

bool sta_wlan_hal_whm::get_4addr_mode()
{
    // API get_4addr_mode is not required with PWHM, see set_4addr_mode first
    // get_4addr_mode is not called in prplMesh
    // Hence no implementation is required.
    return true;
}

bool sta_wlan_hal_whm::set_4addr_mode(bool enable)
{

    // API set_4addr_mode is not required with PWHM
    // PWHM implicitly set the 4addr_mode on endpoint interface in both generic and vendor module
    // It is implicit to manage STA mac management behind repeater
    // Analysis of prplMesh code base only 4_addr_mode is set in FSM of backhaul manager
    // EState::WIRELESS_CONFIG_4ADDR_MODE (sta_wlan_hal->set_4addr_mode(true))
    // No instance of disabling of 4addr_mode exists
    // Niether the get_4addr_mode is called
    // This is set only in dwpal backend when prplMesh backhaul manager is controlling
    // the connection sequence through wpa_supplicabt and is specific to MXL target.
    // see also get_4addr_mode.
    // Hence no implementation is required.
    return true;
}

bool sta_wlan_hal_whm::set_3addr_mcast(bool enable) { return true; }

bool sta_wlan_hal_whm::unassoc_rssi_measurement(const std::string &mac, int chan,
                                                beerocks::eWiFiBandwidth bw,
                                                int vht_center_frequency, int delay,
                                                int window_size)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED";
    return true;
}

bool sta_wlan_hal_whm::reassociate()
{
    Endpoint endpoint;
    if (read_status(endpoint)) {
        if (endpoint.bssid == beerocks::net::network_utils::ZERO_MAC_STRING) {
            LOG(ERROR) << "Got zero BSSID after read status";
            return false;
        }
        update_status(endpoint);
        if (is_connected(endpoint.connection_status)) {
            LOG(TRACE) << "reassociate: - EP already connected";
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_BACKHAUL_CONNECTED_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_BACKHAUL_CONNECTED_NOTIFICATION *>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            memset(msg_buff.get(), 0, sizeof(sACTION_BACKHAUL_CONNECTED_NOTIFICATION));
            if (endpoint.multi_ap_profile) {
                msg->multi_ap_profile = endpoint.multi_ap_profile;
            } else {
                msg->multi_ap_profile = 1;
                LOG(ERROR) << "Failed reading 'multi_ap_profile' parameter!";
            }

            // Multi-AP Primary VLAN ID - Not mandatory
            if (endpoint.multi_ap_primary_vlanid) {
                msg->multi_ap_primary_vlan_id = endpoint.multi_ap_primary_vlanid;
            } else {
                msg->multi_ap_primary_vlan_id = 0;
            }
            event_queue_push(Event::Connected, msg_buff);
            return true;
        } else {
            LOG(TRACE) << "reassociate: - Toggle EP";
            AmbiorixVariant params(AMXC_VAR_ID_HTABLE);
            params.add_child<bool>("Enable", false);
            params.add_child<bool>("Enable", true);
            bool ret = m_ambiorix_cl.update_object(m_ep_path, params);
            if (!ret) {
                LOG(ERROR) << "Failed to toggle endpoint " << get_iface_name();
                return false;
            }
            return false;
        }
    }
    return false;
}

bool sta_wlan_hal_whm::is_connected()
{
    Endpoint endpoint;
    if (!read_status(endpoint)) {
        LOG(ERROR) << "Failed reading endpoint status for iface: " << get_iface_name();
        return false;
    }

    return is_connected(endpoint.connection_status);
}
int sta_wlan_hal_whm::get_channel() { return m_active_channel; }

std::string sta_wlan_hal_whm::get_ssid() { return m_active_ssid; }

std::string sta_wlan_hal_whm::get_bssid() { return m_active_bssid; }

std::string sta_wlan_hal_whm::get_wireless_backhaul_mac()
{
    std::string mac("");
    std::string ssid_ref, ssid_path;
    if (m_ambiorix_cl.get_param(ssid_ref, m_ep_path, "SSIDReference") &&
        m_ambiorix_cl.resolve_path(ssid_ref + ".", ssid_path)) {
        m_ambiorix_cl.get_param(mac, ssid_path, "MACAddress");
    }
    return mac;
}

bool sta_wlan_hal_whm::update_status()
{
    Endpoint endpoint;
    if (!read_status(endpoint)) {
        LOG(ERROR) << "Failed reading endpoint status for iface: " << get_iface_name();
        return false;
    }
    update_status(endpoint);

    return true;
}

int sta_wlan_hal_whm::add_profile(const std::string &alias)
{
    // Path example: WiFi.EndPoint.[IntfName == 'wlan0'].Profile+
    std::string profiles_path = m_ep_path + "Profile.";
    int profile_id            = -1;
    AmbiorixVariant obj_data(AMXC_VAR_ID_HTABLE);
    obj_data.add_child("Alias", alias);
    bool ret = m_ambiorix_cl.add_instance(profiles_path, obj_data, profile_id);
    if (!ret) {
        LOG(ERROR) << "Failed to add profile instance " << get_iface_name();
    }
    return profile_id;
}

int sta_wlan_hal_whm::find_profile_by_alias(const std::string &alias)
{
    std::string profile_find_path = m_ep_path + "Profile.[Alias == '" + alias + "'].";
    int profile_id                = -1;
    std::string profile_path;
    m_ambiorix_cl.resolve_path(profile_find_path, profile_path);
    if (!profile_path.empty()) {
        profile_id = wbapi_utils::get_object_id(profile_path);
    }
    return profile_id;
}

int sta_wlan_hal_whm::remove_profile(int profile_id)
{
    // Path example: WiFi.EndPoint.[IntfName == 'wlan0'].Profile+
    std::string profiles_path = m_ep_path + "Profile.";

    bool ret = m_ambiorix_cl.remove_instance(profiles_path, profile_id);
    if (!ret) {
        LOG(ERROR) << "Failed to remove profile instance with id:" << profile_id;
    }
    return profile_id;
}

bool sta_wlan_hal_whm::set_profile_params(const Profile &profile)
{
    // Path example: WiFi.EndPoint.[IntfName == 'wlan0'].Profile.1.
    std::string profile_path = m_ep_path + "Profile." + std::to_string(profile.id) + ".";
    AmbiorixVariant params(AMXC_VAR_ID_HTABLE);

    // Set SSID
    params.add_child("SSID", profile.ssid);
    bool ret = m_ambiorix_cl.update_object(profile_path, params);
    if (!ret) {
        LOG(ERROR) << "Failed setting alias or ssid on interface " << get_iface_name();
        return false;
    }

    // Set BSSID : optional
    if (!profile.bssid.empty()) {
        params.set_type(AMXC_VAR_ID_HTABLE);
        params.add_child("ForceBSSID", profile.bssid);
        ret = m_ambiorix_cl.update_object(profile_path, params);
        if (!ret) {
            LOG(ERROR) << "Failed setting bssid on interface " << get_iface_name();
            return false;
        }
    }

    // Optional: set channel : not supported by pwhm

    // Set Security
    // Path example: WiFi.EndPoint.[IntfName == 'wlan0'].Profile.1.Security.
    std::string profile_security_path = profile_path + "Security.";

    // Configure Profile.Security object
    params.set_type(AMXC_VAR_ID_HTABLE);
    params.add_child("ModeEnabled", "WPA2-WPA3-Personal");
    params.add_child("KeyPassPhrase", profile.pass);
    params.add_child("SAEPassphrase", profile.pass);
    params.add_child("MFPConfig", "Optional");
    ret = m_ambiorix_cl.update_object(profile_security_path, params);
    if (!ret) {
        LOG(ERROR) << "Failed setting security params on interface " << get_iface_name();
        return false;
    }

    // Optional: set hidden-ssid: not supported by pwhm

    // mem_only_psk not supported by pwhm

    return true;
}

bool sta_wlan_hal_whm::enable_profile(int profile_id)
{
    // Path example: WiFi.EndPoint.[IntfName == 'wlan0'].Profile.1.
    std::string profile_path = m_ep_path + "Profile." + std::to_string(profile_id) + ".";
    AmbiorixVariant params(AMXC_VAR_ID_HTABLE);
    params.add_child<bool>("Enable", true);
    bool ret = m_ambiorix_cl.update_object(profile_path, params);
    if (!ret) {
        LOG(ERROR) << "Failed to enable profile " << get_iface_name();
        return false;
    }

    // Path example: WiFi.EndPoint.[IntfName == 'wlan0'].
    std::string profile_ref;
    if (!m_ambiorix_cl.resolve_path(profile_path, profile_ref)) {
        LOG(ERROR) << "Failed to resolve profile path: " << profile_path;
        return false;
    } else {
        LOG(DEBUG) << "Resolved profile path: " << profile_path
                   << " -> profile_ref: " << profile_ref;
    }

    // We only need the DM path under “WiFi.”, strip leading “Device.” if present
    constexpr const char *device_prefix = "Device.";
    if (profile_ref.rfind(device_prefix, 0) == 0) {
        profile_ref.erase(0, strlen(device_prefix));
        LOG(DEBUG) << "Stripped Device prefix, new profile_ref: " << profile_ref;
    }

    params.set_type(AMXC_VAR_ID_HTABLE);
    ret = params.add_child("ProfileReference", profile_ref);
    if (!ret) {
        LOG(ERROR) << "Failed to add ProfileReference " << get_iface_name();
    }

    ret = m_ambiorix_cl.update_object(m_ep_path, params);
    if (!ret) {
        LOG(ERROR) << "Failed to set profile preference " << get_iface_name();
        return false;
    } else {
        LOG(DEBUG) << "set ProfileReference: " << profile_ref << " as profile reference for "
                   << get_iface_name();
    }

    return true;
}

bool sta_wlan_hal_whm::read_status(Endpoint &endpoint)
{
    // Path example: WiFi.EndPoint.[IntfName == 'wlan0'].
    auto ep_obj = m_ambiorix_cl.get_object(m_ep_path);
    if (!ep_obj) {
        LOG(ERROR) << "failed to get endpoint object";
        return false;
    }

    ep_obj->read_child(endpoint.connection_status, "ConnectionStatus");
    ep_obj->read_child(endpoint.multi_ap_profile, "MultiAPProfile");
    ep_obj->read_child(endpoint.multi_ap_primary_vlanid, "MultiAPVlanId");

    auto ssid_obj = m_ambiorix_cl.get_object(wbapi_utils::get_path_ssid_reference(*ep_obj));
    if (!ssid_obj) {
        LOG(ERROR) << "failed to get ssid object from endpoint " << m_ep_path;
        return false;
    }

    ssid_obj->read_child(endpoint.bssid, "BSSID");
    ssid_obj->read_child(endpoint.ssid, "SSID");

    if (m_radio_path.empty() ||
        !m_ambiorix_cl.get_param(endpoint.channel, m_radio_path, "Channel")) {
        LOG(WARNING) << "failed to update radio channel from: " << m_radio_path;
        return false;
    }

    std::string profile_ref, profile_path;
    if (ep_obj->read_child(profile_ref, "ProfileReference") &&
        m_ambiorix_cl.resolve_path(profile_ref + ".", profile_path)) {
        endpoint.active_profile_id = wbapi_utils::get_object_id(profile_path);
    }

    return true;
}

void sta_wlan_hal_whm::update_status(const Endpoint &endpoint)
{
    m_active_bssid             = endpoint.bssid;
    m_active_ssid              = endpoint.ssid;
    m_active_connection_status = endpoint.connection_status;
    m_active_profile_id        = endpoint.active_profile_id;
    m_active_channel           = endpoint.channel;

    LOG(DEBUG) << "m_active_profile_id= " << m_active_profile_id
               << ", active_bssid= " << m_active_bssid << ", active_channel= " << m_active_channel
               << ", active_ssid= " << m_active_ssid;
}

void sta_wlan_hal_whm::clear_conn_state()
{
    // Clear state
    m_active_ssid              = "";
    m_active_bssid             = "";
    m_active_pass              = "";
    m_active_connection_status = "";
    m_active_channel           = 0;
    m_active_profile_id        = -1;
}

bool sta_wlan_hal_whm::is_connected(const std::string &status)
{
    return (status.compare("Connected") == 0);
}

bool sta_wlan_hal_whm::process_ep_event(const std::string &interface, const std::string &key,
                                        const AmbiorixVariant *new_value,
                                        const AmbiorixVariant *old_value)
{
    if (key == "ConnectionStatus") {
        std::string new_status = new_value->get<std::string>();
        std::string old_status = old_value->get<std::string>();
        if (old_status.empty() || new_status.empty()) {
            return true;
        }
        LOG(INFO) << "Endpoint " << interface << " ConnectionStatus " << new_status;
        if (is_connected(new_status)) {
            Endpoint endpoint;
            if (!read_status(endpoint)) {
                LOG(ERROR) << "Failed reading connection status for iface: " << get_iface_name();
                return false;
            } else if (endpoint.bssid == beerocks::net::network_utils::ZERO_MAC_STRING) {
                LOG(ERROR) << "Got zero BSSID after read status";
                return false;
            }
            update_status(endpoint);

            LOG(DEBUG) << get_iface_name() << " - Connected: bssid = " << m_active_bssid
                       << ", channel = " << m_active_channel;
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_BACKHAUL_CONNECTED_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_BACKHAUL_CONNECTED_NOTIFICATION *>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            memset(msg_buff.get(), 0, sizeof(sACTION_BACKHAUL_CONNECTED_NOTIFICATION));
            if (endpoint.multi_ap_profile) {
                msg->multi_ap_profile = endpoint.multi_ap_profile;
            } else {
                msg->multi_ap_profile = 1;
                LOG(ERROR) << "Failed reading 'multi_ap_profile' parameter!";
            }

            // Multi-AP Primary VLAN ID - Not mandatory
            if (endpoint.multi_ap_primary_vlanid) {
                msg->multi_ap_primary_vlan_id = endpoint.multi_ap_primary_vlanid;
            } else {
                msg->multi_ap_primary_vlan_id = 0;
            }
            event_queue_push(Event::Connected, msg_buff);
        } else if (is_connected(old_status)) {
            auto msg_buff =
                ALLOC_SMART_BUFFER(sizeof(sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION));
            auto msg =
                reinterpret_cast<sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION *>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            memset(msg_buff.get(), 0, sizeof(sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION));
            LOG(DEBUG) << get_iface_name() << " - Disconnected: bssid = " << m_active_bssid
                       << ", channel = " << m_active_channel;
            // TODO: Disconnect reason is not supported by whm, set to UNSPECIFIED_REASON 1: PPM-2416
            msg->disconnect_reason = 1;
            msg->bssid             = tlvf::mac_from_string(m_active_bssid);
            clear_conn_state();
            event_queue_push(Event::Disconnected, msg_buff);
        }
    }
    return true;
}

bool sta_wlan_hal_whm::process_ep_wps_event(const std::string &interface,
                                            const AmbiorixVariant *data)
{
    std::string reason;
    data->read_child(reason, "reason");
    if (reason.empty()) {
        return true;
    }

    LOG(INFO) << "WPS end, interface: " << interface << ", reason: " << reason;

    if (reason == "Success") {
        std::string ssid, key, mode;
        data->read_child(ssid, "SSID");
        data->read_child(key, "KeyPassPhrase");
        data->read_child(mode, "securitymode");
        if (ssid.empty() || key.empty() || mode.empty()) {
            return false;
        }

        WiFiSec sec     = utils_wlan_hal_whm::security_type_from_string(mode);
        Profile profile = {
            .id           = -1,
            .alias        = "WPS",
            .ssid         = ssid,
            .bssid        = "", // will be set in process_ep_event
            .sec          = sec,
            .mem_only_psk = false,
            .pass         = key,
            .hidden_ssid  = false,
        };
        return set_profile(profile);
    }

    return true;
}

bool sta_wlan_hal_whm::clear_non_associated_devices()
{
    std::string nasta_monitor_path = m_radio_path + "NaStaMonitor";
    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    if (!m_ambiorix_cl.call(nasta_monitor_path, "clearNonAssociatedDevices", args, result)) {
        LOG(ERROR) << " remote function call clearNonAssociatedDevices for object "
                   << nasta_monitor_path << " Failed!";
        return false;
    }
    return true;
}

} // namespace whm

std::shared_ptr<sta_wlan_hal> sta_wlan_hal_create(const std::string &iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback,
                                                  const bwl::hal_conf_t &hal_conf)
{
    return std::make_shared<whm::sta_wlan_hal_whm>(iface_name, callback, hal_conf);
}

} // namespace bwl
