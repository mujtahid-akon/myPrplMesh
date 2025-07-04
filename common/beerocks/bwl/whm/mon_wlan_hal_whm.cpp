/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2022 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "mon_wlan_hal_whm.h"
#include <bwl/mon_wlan_hal_types.h>

#include <amxd/amxd_object.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bwl/key_value_parser.h>

#include <easylogging++.h>

#include <cmath>
#include <iomanip>
using namespace beerocks;
using namespace wbapi;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// WHM////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace whm {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static mon_wlan_hal::Event wpaCtrl_to_bwl_event(const std::string &opcode)
{
    if (opcode == "BEACON-REQ-TX-STATUS") {
        return mon_wlan_hal::Event::RRM_Beacon_Request_Status;
    } else if (opcode == "BEACON-RESP-RX") {
        return mon_wlan_hal::Event::RRM_Beacon_Response;
    }

    return mon_wlan_hal::Event::Invalid;
}

static bool parse_beacon_request_status(bwl::parsed_line_t &parsed_obj,
                                        bwl::SBeaconRequestStatus11k *msg)
{

    if (parsed_obj.find("_mac") != parsed_obj.end()) {
        tlvf::mac_from_string(msg->sta_mac.oct, parsed_obj.at("_mac"));
    } else {
        LOG(ERROR) << "missing station mac";
        return false;
    }

    if (parsed_obj.find("_arg0") != parsed_obj.end()) {
        // Dialog token
        msg->dialog_token = beerocks::string_utils::stoi(parsed_obj.at("_arg0"));
    } else {
        LOG(ERROR) << "missing dialog token";
        return false;
    }

    // we use parse_event_keyless_params for the variable count of keyless params in RRM
    // ack is a key-val, so handle it separately here
    if (parsed_obj.find("_arg1") != parsed_obj.end()) {
        auto idx = parsed_obj.at("_arg1").find_first_of('=');
        if (idx != std::string::npos) {
            auto ack_substr = parsed_obj.at("_arg1").substr(idx + 1, std::string::npos);
            msg->ack        = beerocks::string_utils::stoi(ack_substr);
        }
    }

    LOG(DEBUG) << "Beacon Request Status RX:" << std::endl
               << "  dialog token = " << msg->dialog_token << std::endl
               << "  StaMAC = " << parsed_obj.at("_mac") << std::endl;

    return true;
}

static bool parse_beacon_measurement_response(bwl::parsed_line_t &parsed_obj,
                                              bwl::SBeaconResponse11k *msg)
{
    if (parsed_obj.find("_mac") != parsed_obj.end()) {
        // STA Mac Address
        tlvf::mac_from_string(msg->sta_mac.oct, parsed_obj.at("_mac"));
    } else {
        LOG(ERROR) << "missing station mac";
        return false;
    }

    if (parsed_obj.find("_arg0") != parsed_obj.end()) {
        // Dialog token
        msg->dialog_token = beerocks::string_utils::stoi(parsed_obj.at("_arg0"));
    } else {
        LOG(ERROR) << "missing dialog token";
        return false;
    }

    if (parsed_obj.find("_arg1") != parsed_obj.end()) {
        // rep_mode
        msg->rep_mode = beerocks::string_utils::stoi(parsed_obj.at("_arg1"));
    }

    // Parse the report
    if (parsed_obj.find("_arg2") == parsed_obj.end()) {
        LOG(WARNING) << "missing 11k report";
        return false;
    }
    auto report = parsed_obj.at("_arg2");
    if (report.length() < 52) {
        LOG(WARNING) << "Invalid 11k report length!";
        return false;
    }

    int idx = 0;

    // op_class
    msg->op_class = std::strtoul(report.substr(idx, 2).c_str(), 0, 16);
    idx += 2;

    // channel
    msg->channel = std::strtoul(report.substr(idx, 2).c_str(), 0, 16);
    idx += 2;

    // start_time
    msg->start_time = std::strtoull(report.substr(idx, 16).c_str(), 0, 16);
    msg->start_time = be64toh(msg->start_time);
    idx += 16;

    // measurement_duration
    msg->duration = std::strtoul(report.substr(idx, 4).c_str(), 0, 16);
    msg->duration = be16toh(msg->duration);
    idx += 4;

    // phy_type
    msg->phy_type = std::strtoul(report.substr(idx, 2).c_str(), 0, 16);
    idx += 2;

    // rcpi
    msg->rcpi = std::strtol(report.substr(idx, 2).c_str(), 0, 16);
    idx += 2;

    // rsni
    msg->rsni = std::strtol(report.substr(idx, 2).c_str(), 0, 16);
    idx += 2;

    // bssid
    msg->bssid.oct[0] = std::strtoul(report.substr(idx + 0, 2).c_str(), 0, 16);
    msg->bssid.oct[1] = std::strtoul(report.substr(idx + 2, 2).c_str(), 0, 16);
    msg->bssid.oct[2] = std::strtoul(report.substr(idx + 4, 2).c_str(), 0, 16);
    msg->bssid.oct[3] = std::strtoul(report.substr(idx + 6, 2).c_str(), 0, 16);
    msg->bssid.oct[4] = std::strtoul(report.substr(idx + 8, 2).c_str(), 0, 16);
    msg->bssid.oct[5] = std::strtoul(report.substr(idx + 10, 2).c_str(), 0, 16);
    idx += 12;

    // ant_id
    msg->ant_id = std::strtoul(report.substr(idx, 2).c_str(), 0, 16);
    idx += 2;

    // parent_tsf
    msg->parent_tsf = std::strtoull(report.substr(idx, 8).c_str(), 0, 16);
    idx += 8;

    LOG(DEBUG) << "Beacon Response:" << std::endl
               << "  dialog token = " << msg->dialog_token << std::endl
               << "  op_class = " << int(msg->op_class) << std::endl
               << "  channel = " << int(msg->channel) << std::endl
               << "  start_time = " << int(msg->start_time) << std::endl
               << "  duration = " << int(msg->duration) << std::endl
               << "  phy_type = " << int(msg->phy_type) << std::endl
               << "  rcpi = " << int(msg->rcpi) << std::endl
               << "  rsni = " << int(msg->rsni) << std::endl
               << "  bssid = " << tlvf::mac_to_string(&(msg->bssid.oct[0])) << std::endl
               << "  ant_id = " << int(msg->ant_id) << std::endl
               << "  parent_tfs = " << int(msg->parent_tsf);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

mon_wlan_hal_whm::mon_wlan_hal_whm(const std::string &iface_name, hal_event_cb_t callback,
                                   const bwl::hal_conf_t &hal_conf)
    : base_wlan_hal(bwl::HALType::Monitor, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_whm(bwl::HALType::Monitor, iface_name, callback, hal_conf)
{
    subscribe_to_ap_events();
    subscribe_to_sta_events();
    subscribe_to_scan_complete_events();
}

mon_wlan_hal_whm::~mon_wlan_hal_whm() {}

static uint8_t scaled_metric(const uint16_t val, const uint16_t tot)
{
    uint32_t aux = val * 2550 / tot; // 2550 isof 255 so we see decimal in second step

    if (aux % 10 > 4) { // round decimal;
        aux += 10;
    }
    aux = aux / 10;

    if (aux > 255) { // clip to 255 max value
        aux = 255;
    }

    return aux;
}

bool mon_wlan_hal_whm::update_radio_stats(SRadioStats &radio_stats)
{
    std::string stats_path = m_radio_path + "Stats.";

    auto stats_obj = m_ambiorix_cl.get_object(stats_path);
    if (!stats_obj) {
        LOG(ERROR) << "failed to get radio Stats object " << stats_path;
        return true;
    }

    stats_obj->read_child(radio_stats.tx_bytes_cnt, "BytesSent");
    stats_obj->read_child(radio_stats.rx_bytes_cnt, "BytesReceived");
    stats_obj->read_child(radio_stats.tx_packets_cnt, "PacketsSent");
    stats_obj->read_child(radio_stats.rx_packets_cnt, "PacketsReceived");
    stats_obj->read_child(radio_stats.errors_sent, "ErrorsSent");
    stats_obj->read_child(radio_stats.errors_received, "ErrorsReceived");
    stats_obj->read_child(radio_stats.noise, "Noise");

    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE); // ex : WiFi.Radio.2.getRadioAirStats()
    if (!m_ambiorix_cl.call(m_radio_path, "getRadioAirStats", args, result)) {
        LOG(ERROR) << "remote function call getRadioAirStats Failed!";
        return true;
    }

    auto air_stats_as_list = result.read_children<AmbiorixVariantListSmartPtr>();
    // read as AmbxList removes [] around the result

    auto data_map_2 = air_stats_as_list->front().read_children<AmbiorixVariantMapSmartPtr>();
    // list has one element, with all lines grouped between curly brackets :
    // {Key1 = Value1, KeyN = ValueN}
    // second transformation exposes the lines of the result as a map
    // storage size, name, exclamation marks required values
    /*  uint16_t, "Load"
        int32_t,  "Noise"  !
        uint16_t, "TxTime" !
        uint16_t, "RxTime" !
        uint16_t, "IntTime"
        uint16_t, "ObssTime" !
        uint16_t, "NoiseTime"
        uint16_t, "FreeTime"
        uint16_t, "TotalTime" !
        uint32_t, "Timestamp"
    */

    std::vector<std::string> required_elements = {"Noise", "TxTime", "RxTime", "ObssTime",
                                                  "TotalTime"};
    for (auto &r_e : required_elements) {
        if (data_map_2->find(r_e) == data_map_2->end()) {
            LOG(ERROR) << "RadioAirStats missing " << r_e;
            return true;
        }
    }

    uint16_t total, txt, rxt, obt;
    int32_t noise;

    (*data_map_2)["TotalTime"].get(total);
    if (total == 0) {
        LOG(WARNING) << "TotalTime 0: avoid using as denominator";
        return true;
    }

    (*data_map_2)["TxTime"].get(txt);
    (*data_map_2)["RxTime"].get(rxt);
    (*data_map_2)["ObssTime"].get(obt);

    (*data_map_2)["Noise"].get(noise);

    radio_stats.anpi_noise    = (noise < 0) ? ((noise + 110) * 2) : 220;
    radio_stats.transmit      = scaled_metric(txt, total);
    radio_stats.receive_self  = scaled_metric(rxt, total);
    radio_stats.receive_other = scaled_metric(obt, total);

    return true;
}

bool mon_wlan_hal_whm::update_vap_stats(const std::string &vap_iface_name, SVapStats &vap_stats)
{
    std::string ssid_stats_path = wbapi_utils::search_path_ssid_by_iface(vap_iface_name) + "Stats.";

    auto ssid_stats_obj = m_ambiorix_cl.get_object(ssid_stats_path);
    if (!ssid_stats_obj) {
        LOG(ERROR) << "failed to get SSID Stats object, path:" << ssid_stats_path;
        return true;
    }

    ssid_stats_obj->read_child(vap_stats.tx_bytes_cnt, "BytesSent");
    ssid_stats_obj->read_child(vap_stats.rx_bytes_cnt, "BytesReceived");
    ssid_stats_obj->read_child(vap_stats.tx_packets_cnt, "PacketsSent");
    ssid_stats_obj->read_child(vap_stats.rx_packets_cnt, "PacketsReceived");
    ssid_stats_obj->read_child(vap_stats.errors_sent, "ErrorsSent");
    ssid_stats_obj->read_child(vap_stats.errors_received, "ErrorsReceived");
    ssid_stats_obj->read_child(vap_stats.retrans_count, "RetransCount");

    // update MLO stats
    std::string ap_mlostats_path =
        wbapi_utils::search_path_ap_by_iface(vap_iface_name) + "MLOStats.";

    auto ap_mlostats_obj = m_ambiorix_cl.get_object(ap_mlostats_path);
    if (!ap_mlostats_obj) {
        LOG(ERROR) << "failed to get AP MLOStats object, path:" << ap_mlostats_path;
        return true;
    }

    ap_mlostats_obj->read_child(vap_stats.mlo_stats.tx_packets_cnt, "PacketsSent");
    ap_mlostats_obj->read_child(vap_stats.mlo_stats.rx_packets_cnt, "PacketsReceived");
    ap_mlostats_obj->read_child(vap_stats.mlo_stats.tx_packets_err_cnt, "ErrorsSent");
    ap_mlostats_obj->read_child(vap_stats.mlo_stats.tx_ucast_bytes, "UnicastBytesSent");
    ap_mlostats_obj->read_child(vap_stats.mlo_stats.rx_ucast_bytes, "UnicastBytesReceived");
    ap_mlostats_obj->read_child(vap_stats.mlo_stats.tx_mcast_bytes, "MulticastBytesSent");
    ap_mlostats_obj->read_child(vap_stats.mlo_stats.rx_mcast_bytes, "MulticastBytesReceived");
    ap_mlostats_obj->read_child(vap_stats.mlo_stats.tx_bcast_bytes, "BroadcastBytesSent");
    ap_mlostats_obj->read_child(vap_stats.mlo_stats.rx_bcast_bytes, "BroadcastBytesReceived");

    return true;
}

bool mon_wlan_hal_whm::update_stations_stats(const std::string &vap_iface_name,
                                             const std::string &sta_mac, SStaStats &sta_stats,
                                             bool is_read_unicast)
{
    auto sta_mac_address = tlvf::mac_from_string(sta_mac);
    nl80211_client::sta_info sta_info;
    if (!m_iso_nl80211_client->get_sta_info(vap_iface_name, sta_mac_address, sta_info)) {
        return true;
    }
    sta_stats.tx_bytes          = sta_info.tx_bytes;
    sta_stats.rx_bytes          = sta_info.rx_bytes;
    sta_stats.tx_packets        = sta_info.tx_packets;
    sta_stats.rx_packets        = sta_info.rx_packets;
    sta_stats.retrans_count     = sta_info.tx_retries;
    sta_stats.tx_phy_rate_100kb = sta_info.tx_bitrate_100kbps;
    sta_stats.rx_phy_rate_100kb = sta_info.rx_bitrate_100kbps;
    sta_stats.dl_bandwidth      = sta_info.dl_bandwidth;
    if (sta_info.signal_dbm != 0) {
        sta_stats.rx_rssi_watt = std::pow(10, (int8_t(sta_info.signal_dbm) / 10.0));
        sta_stats.rx_rssi_watt_samples_cnt++;
    }

    //complement missing info in sta_info struct
    std::string assoc_device_path =
        wbapi_utils::search_path_assocDev_by_mac(vap_iface_name, sta_mac);

    float s_float;
    if (m_ambiorix_cl.get_param(s_float, assoc_device_path, "SignalNoiseRatio")) {
        if (s_float >= beerocks::SNR_MIN) {
            sta_stats.rx_snr_watt = std::pow(10, s_float / float(10));
            sta_stats.rx_snr_watt_samples_cnt++;
        }
    }

    m_ambiorix_cl.get_param(sta_stats.tx_bytes_cnt, assoc_device_path, "TxBytes");
    m_ambiorix_cl.get_param(sta_stats.rx_bytes_cnt, assoc_device_path, "RxBytes");
    m_ambiorix_cl.get_param(sta_stats.rx_packets_cnt, assoc_device_path, "RxPacketCount");
    m_ambiorix_cl.get_param(sta_stats.tx_packets_cnt, assoc_device_path, "TxPacketCount");

    return true;
}

bool mon_wlan_hal_whm::update_station_qos_control_params(const std::string &vap_iface_name,
                                                         const std::string &sta_mac,
                                                         SStaQosCtrlParams &sta_qos_ctrl_params)
{
    //LOG(TRACE) << __func__ << " - NOT IMPLEMENTED";
    return true;
}

bool mon_wlan_hal_whm::sta_channel_load_11k_request(const std::string &vap_iface_name,
                                                    const SStaChannelLoadRequest11k &req)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED";
    return true;
}

bool mon_wlan_hal_whm::sta_beacon_11k_request(const std::string &vap_iface_name,
                                              const SBeaconRequest11k &req, int &dialog_token)
{
    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    args.add_child("mac", tlvf::mac_to_string(req.sta_mac.oct));
    args.add_child("bssid", tlvf::mac_to_string(req.bssid.oct));
    args.add_child("class", uint8_t(req.op_class));
    args.add_child("channel", uint8_t(req.channel));
    args.add_child("ssid", std::string((const char *)req.ssid));
    args.add_child("mode", int(req.measurement_mode));
    args.add_child("neighbor", false);
    args.add_child("duration", uint16_t(req.duration));

    if (req.use_optional_ap_ch_report) {
        std::stringstream optionalElements;
        optionalElements << std::hex << std::setfill('0');
        optionalElements << std::setw(2) << static_cast<unsigned>(AP_CHANNEL_REPORT_TYPE);
        optionalElements << std::setw(2) << static_cast<unsigned>(req.use_optional_ap_ch_report);
        for (auto i = 0; i < req.use_optional_ap_ch_report; i++) {
            optionalElements << std::setw(2) << static_cast<unsigned>(req.ap_ch_report[i]);
        }
        args.add_child("optionalElements", optionalElements.str());
    }

    std::string wifi_ap_path = wbapi_utils::search_path_ap_by_iface(vap_iface_name);
    bool ret = m_ambiorix_cl.call(wifi_ap_path, "sendRemoteMeasumentRequest", args, result);

    LOG(DEBUG) << "11k request for " << tlvf::mac_to_string(req.sta_mac.oct) << " bssid "
               << tlvf::mac_to_string(req.bssid.oct);
    if (!ret) {
        LOG(ERROR) << "sta_beacon_11k_request() failed!";
        return false;
    }
    return true;
}

bool mon_wlan_hal_whm::sta_link_measurements_11k_request(const std::string &vap_iface_name,
                                                         const std::string &sta_mac)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED";
    return true;
}

bool mon_wlan_hal_whm::channel_scan_trigger(int dwell_time_msec,
                                            const std::vector<unsigned int> &channel_pool,
                                            bool cert_mode)
{
    if (channel_pool.empty()) {
        LOG(INFO) << "channel_pool is empty!, scanning all channels";
    }

    if (cert_mode) {
        //Need to customise the channel scan parameters for certification.
    }

    std::string channels;
    if (!channel_pool.empty()) {
        for (auto &input_channel : channel_pool) {
            channels += std::to_string(input_channel);
            channels += ",";
        }
        channels.pop_back();
    }

    if (m_scan_active) {
        AmbiorixVariant result_abort;
        AmbiorixVariant args_abort(AMXC_VAR_ID_HTABLE);
        //scan is already active, as per spec, cancel the old one and start new one
        if (!m_ambiorix_cl.call(m_radio_path, "stopScan", args_abort, result_abort)) {
            LOG(INFO) << " remote function stopScan startScan Failed!";
        }
        m_scan_active = false; // optimistically reset m_scan_active
        LOG(INFO) << "m_scan_active: " << m_scan_active;
    }
    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    if (!channels.empty()) {
        args.add_child("channels", channels);
    }
    if (!m_ambiorix_cl.call(m_radio_path, "startScan", args, result)) {
        LOG(ERROR) << " remote function call startScan Failed!";
        return false;
    }
    event_queue_push(Event::Channel_Scan_Triggered);
    m_scan_active = true;
    LOG(INFO) << "m_scan_active: " << m_scan_active;
    return true;
}

bool mon_wlan_hal_whm::channel_scan_dump_cached_results()
{
    //read stats cached internally
    for (auto &map : m_scan_results) {
        auto results_notif = std::make_shared<sCHANNEL_SCAN_RESULTS_NOTIFICATION>();
        auto &results      = results_notif->channel_scan_results;

        string_utils::copy_string(results.ssid, map["SSID"].c_str(),
                                  beerocks::message::WIFI_SSID_MAX_LENGTH);

        results.bssid = tlvf::mac_from_string(map["BSSID"]);

        int32_t center_channel = std::stoi(map["CentreChannel"]);

        int32_t bandwidth                   = std::stoi(map["Bandwidth"]);
        results.operating_channel_bandwidth = utils_wlan_hal_whm::get_bandwidth_from_int(bandwidth);

        results.channel = std::stoul(map["Channel"]);

        WifiChannel wifi_channel(results.channel, center_channel,
                                 utils::convert_bandwidth_to_enum(bandwidth));
        results.operating_frequency_band =
            utils_wlan_hal_whm::eFreqType_to_eCh_scan_Op_Fr_Ba(wifi_channel.get_freq_type());

        results.signal_strength_dBm = std::stoi(map["RSSI"]);

        if (map.find("SecurityModeEnabled") != map.end()) {
            std::string security_mode_enabled(map["SecurityModeEnabled"]);
            results.security_mode_enabled =
                utils_wlan_hal_whm::get_scan_security_modes_from_str(security_mode_enabled);
        }

        if (map.find("EncryptionMode") != map.end()) {
            std::string encryption_mode(map["EncryptionMode"]);
            results.encryption_mode =
                utils_wlan_hal_whm::get_scan_result_encryption_modes_from_str(encryption_mode);
        }

        if (map.find("OperatingStandards") != map.end()) {
            std::string supported_standards(map["OperatingStandards"]);
            results.supported_standards =
                utils_wlan_hal_whm::get_scan_result_operating_standards_from_str(
                    supported_standards);
        }

        if (map.find("ChannelUtilization") != map.end()) {
            results.channel_utilization = std::stoul(map["ChannelUtilization"]);
            results.load_bss_ie_present = 1;
        }

        if (map.find("StationCount") != map.end()) {
            results.station_count       = std::stoul(map["StationCount"]);
            results.load_bss_ie_present = 1;
        }

        LOG(DEBUG) << "Processing results for BSSID:" << results.bssid
                   << " on Channel: " << results.channel;
        event_queue_push(Event::Channel_Scan_Dump_Result, results_notif);
    }

    for (auto &map : m_spectrum_results) {
        auto results_notif = std::make_shared<sCHANNEL_SCAN_RESULTS_NOTIFICATION>();
        auto &results      = results_notif->channel_scan_results;

        results.spectrum_info_present = 1;

        int32_t bandwidth                   = std::stoi(map["Bandwidth"]);
        results.operating_channel_bandwidth = utils_wlan_hal_whm::get_bandwidth_from_int(bandwidth);

        results.channel = std::stoul(map["Channel"]);

        results.noise_dBm = std::stoi(map["Noiselevel"]);

        uint32_t availability = std::stoul(map["Availability"]);
        results.utilization   = (((100 - availability) * 255) / 100);

        LOG(DEBUG) << "Processing spectrum results for channel:" << results.channel;
        event_queue_push(Event::Channel_Scan_Dump_Result, results_notif);
    }

    return true;
}

bool mon_wlan_hal_whm::channel_scan_dump_results()
{
    //Lets update our internal results from the pwhm then dump them
    get_scan_results_from_pwhm();
    channel_scan_dump_cached_results(); //lets dump the results
    return true;
}

bool mon_wlan_hal_whm::generate_connected_clients_events(
    bool &is_finished_all_clients, std::chrono::steady_clock::time_point max_iteration_timeout)
{
    // For the pwhm, we belive the time requirement will be maintained all time, thus we will ignore the max_iteration_timeout
    for (auto &vap : m_vapsExtInfo) {

        std::string vap_path                = vap.second.path;
        std::string associated_devices_path = vap_path + "AssociatedDevice.";

        auto associated_devices_pwhm =
            m_ambiorix_cl.get_object_multi<AmbiorixVariantMapSmartPtr>(associated_devices_path);

        if (associated_devices_pwhm == nullptr) {
            LOG(DEBUG) << "Failed reading: " << associated_devices_path;
            return true;
        }

        auto vap_id = get_vap_id_with_bss(vap.first);
        if (vap_id == beerocks::IFACE_ID_INVALID) {
            LOG(DEBUG) << "Invalid vap_id";
            continue;
        }

        //Lets iterate through all instances
        for (auto &associated_device_pwhm : *associated_devices_pwhm) {
            bool is_active;
            if (!associated_device_pwhm.second.read_child(is_active, "Active") || !is_active) {
                // we are only interested in connected stations
                continue;
            }

            std::string mac_addr;
            if (!associated_device_pwhm.second.read_child(mac_addr, "MACAddress")) {
                LOG(DEBUG) << "Failed reading MACAddress";
                continue;
            }

            auto msg_buff =
                ALLOC_SMART_BUFFER(sizeof(sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION));
            LOG_IF(msg_buff == nullptr, FATAL) << "Memory allocation failed!";
            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION));
            auto msg =
                reinterpret_cast<sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
            msg->vap_id = vap_id;
            msg->mac    = tlvf::mac_from_string(mac_addr);

            auto sta_it = m_stations.find(mac_addr);
            if (sta_it == m_stations.end()) {
                m_stations.insert(
                    std::make_pair(mac_addr, sStationInfo(associated_device_pwhm.first)));
            } else {
                sta_it->second.path = associated_device_pwhm.first; //enforce the path
            }

            event_queue_push(Event::STA_Connected, msg_buff);
        }
    }

    is_finished_all_clients = true;

    return true;
}

bool mon_wlan_hal_whm::pre_generate_connected_clients_events()
{
    // For the pwhm and the evolution of prplmesh, we dont see a need to implement this function, all will be done throughh the main
    // function generate_connected_clients_events
    return true;
}

bool mon_wlan_hal_whm::channel_scan_abort()
{
    if (!m_scan_active) {
        LOG(DEBUG) << "No active channel scan found";
        return true;
    }
    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    if (!m_ambiorix_cl.call(m_radio_path, "stopScan", args, result)) {
        LOG(ERROR) << " remote function call stopScan Failed!";
        return false;
    }
    m_scan_active = false;
    LOG(INFO) << "m_scan_active: " << m_scan_active;
    event_queue_push(Event::Channel_Scan_Aborted);
    LOG(DEBUG) << "Channel Scan Aborted";
    return true;
}

bool mon_wlan_hal_whm::process_ap_event(const std::string &interface, const std::string &key,
                                        const AmbiorixVariant *value)
{
    auto vap_id = get_vap_id_with_bss(interface);
    if (vap_id == beerocks::IFACE_ID_INVALID) {
        return true;
    }
    if (key == "Status") {
        std::string status = value->get<std::string>();
        if (status.empty()) {
            return true;
        }
        LOG(WARNING) << "monitor: vap " << interface << " status " << status;
        if (status == "Enabled") {
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_ENABLED_NOTIFICATION));
            auto msg      = reinterpret_cast<sHOSTAP_ENABLED_NOTIFICATION *>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            memset(msg_buff.get(), 0, sizeof(sHOSTAP_ENABLED_NOTIFICATION));
            msg->vap_id = vap_id;
            event_queue_push(Event::AP_Enabled, msg_buff);
        } else {
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_DISABLED_NOTIFICATION));
            auto msg      = reinterpret_cast<sHOSTAP_DISABLED_NOTIFICATION *>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            memset(msg_buff.get(), 0, sizeof(sHOSTAP_DISABLED_NOTIFICATION));
            msg->vap_id = vap_id;
            event_queue_push(Event::AP_Disabled, msg_buff); // send message to the AP manager
        }
    }
    return true;
}

bool mon_wlan_hal_whm::process_sta_event(const std::string &interface, const std::string &sta_mac,
                                         const std::string &key, const AmbiorixVariant *value)
{
    auto vap_id = get_vap_id_with_bss(interface);
    if (vap_id == beerocks::IFACE_ID_INVALID) {
        return true;
    }
    if (key == "AuthenticationState") {
        bool connected = value->get<bool>();
        if (connected) {
            LOG(WARNING) << "monitor: Connected station " << sta_mac << " over vap " << interface;
            auto msg_buff =
                ALLOC_SMART_BUFFER(sizeof(sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION));
            auto msg =
                reinterpret_cast<sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            memset(msg_buff.get(), 0, sizeof(sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION));
            msg->vap_id = vap_id;
            msg->mac    = tlvf::mac_from_string(sta_mac);
            event_queue_push(Event::STA_Connected, msg_buff);
        } else {
            LOG(WARNING) << "monitor: disconnected station " << sta_mac << " from vap "
                         << interface;
            auto msg_buff =
                ALLOC_SMART_BUFFER(sizeof(sACTION_MONITOR_CLIENT_DISCONNECTED_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_MONITOR_CLIENT_DISCONNECTED_NOTIFICATION *>(
                msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
            memset(msg_buff.get(), 0, sizeof(sACTION_MONITOR_CLIENT_DISCONNECTED_NOTIFICATION));
            msg->mac = tlvf::mac_from_string(sta_mac);
            event_queue_push(Event::STA_Disconnected, msg_buff);
        }
    }
    return true;
}

bool mon_wlan_hal_whm::process_wpa_ctrl_event(const beerocks::wbapi::AmbiorixVariant &event_data)
{
    std::string event_str;
    if (!event_data.read_child<>(event_str, "eventData") || event_str.empty()) {
        LOG(WARNING) << "Unable to retrieve wpaCtrl event data from pwhm notification";
        return false;
    }

    bwl::parsed_line_t parsed_obj;

    auto idx_start = event_str.find_first_of(">");
    if (idx_start != std::string::npos) {
        idx_start++;
    } else {
        LOG(WARNING) << "empty event! event_string: " << event_str;
        return false;
    }

    // parse_event() func supports only a fixed number of keyless params
    // use parse_event_keyless_params instead
    parse_event_keyless_params(event_str, idx_start, parsed_obj, true);

    // Filter out empty events
    std::string opcode;
    if (!(parsed_obj.find(bwl::EVENT_KEYLESS_PARAM_OPCODE) != parsed_obj.end() &&
          !(opcode = parsed_obj.at(bwl::EVENT_KEYLESS_PARAM_OPCODE)).empty())) {
        LOG(ERROR) << "no opcode";
        return true;
    }

    auto event = wpaCtrl_to_bwl_event(opcode);

    std::string interface;
    if (!event_data.read_child<>(interface, "ifName") || interface.empty()) {
        LOG(WARNING) << "Unable to retrieve ifName from pwhm notification";
        return false;
    }
    LOG(DEBUG) << "interface " << interface << " wpaCtrl event: " << event_str;

    switch (event) {
    case Event::RRM_Beacon_Request_Status: {
        /* TODO: The AP Manager already handles all the related stuff for
         * Beacon Metrics, remove this case.
         */
        break;

        // Allocate response object
        auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SBeaconRequestStatus11k));
        auto resp      = reinterpret_cast<SBeaconRequestStatus11k *>(resp_buff.get());
        LOG_IF(!resp, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(resp_buff.get(), 0, sizeof(SBeaconRequestStatus11k));

        if (parse_beacon_request_status(parsed_obj, resp)) {
            // Add the message to the queue
            event_queue_push(event, resp_buff);
        }

    } break;
    case Event::RRM_Beacon_Response: {
        /* TODO: The AP Manager already handles all the related stuff for
         * Beacon Metrics, remove this case.
         */
        break;

        // Allocate response object
        auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SBeaconResponse11k));
        auto resp      = reinterpret_cast<SBeaconResponse11k *>(resp_buff.get());
        LOG_IF(!resp, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(resp_buff.get(), 0, sizeof(SBeaconResponse11k));

        if (parse_beacon_measurement_response(parsed_obj, resp)) {
            event_queue_push(event, resp_buff);
        }

    } break;
    default: {
        // other WPA CTRL messages handled in other hal_whm classes
        break;
    };
    }
    return true;
}

bool mon_wlan_hal_whm::set(const std::string &param, const std::string &value, int vap_id)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED";
    return true;
}

bool mon_wlan_hal_whm::set_available_estimated_service_parameters(
    wfa_map::tlvApMetrics::sEstimatedService &estimated_service_parameters)
{
    estimated_service_parameters.include_ac_bk = 1;
    estimated_service_parameters.include_ac_be = 1;
    estimated_service_parameters.include_ac_vo = 1;
    estimated_service_parameters.include_ac_vi = 1;

    return true;
}

bool mon_wlan_hal_whm::set_estimated_service_parameters(uint8_t *esp_info_field)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED";
    return true;
}

/*  will get the unassociated stations stats from Ambirorix
*/
bool mon_wlan_hal_whm::sta_unassoc_rssi_measurement(
    std::unordered_map<std::string, uint8_t> &new_list)
{
    /*
        Example of NonAssociatedDevice object:
        WiFi.Radio.wifi0.NaStaMonitor.NonAssociatedDevice
        WiFi.Radio.wifi0.NaStaMonitor.NonAssociatedDevice.{i}.
        WiFi.Radio.wifi0.NaStaMonitor.NonAssociatedDevice.{i}.MACAddress=AA:BB:CC:DD:EE:FF
        WiFi.Radio.wifi0.NaStaMonitor.NonAssociatedDevice.{i}.SignalStrength=0
        WiFi.Radio.wifi0.NaStaMonitor.NonAssociatedDevice.{i}.TimeStamp=0001-01-01T00:00:00Z
    */

    std::vector<sUnassociatedStationStats> stats;

    std::list<std::string> amx_un_stations_to_be_removed;

    std::string non_associated_device_path = m_radio_path + "NaStaMonitor.NonAssociatedDevice.";

    std::string nasta_monitor_path = m_radio_path + "NaStaMonitor";
    //Now add the new unassociated stations
    for (auto &new_station : new_list) {
        std::string mac_address(new_station.first);

        AmbiorixVariant result;
        AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
        args.add_child("macaddress", mac_address);
        if (!m_ambiorix_cl.call(nasta_monitor_path, "createNonAssociatedDevice", args, result)) {
            LOG(ERROR) << " remote function call createNonAssociatedDevice for object "
                       << nasta_monitor_path << " Failed!";
            continue;
        }

        LOG(TRACE) << "Non Associated Station with MACAddress: " << mac_address << "added to "
                   << non_associated_device_path;
    }

    auto non_ass_devices =
        m_ambiorix_cl.get_object_multi<AmbiorixVariantMapSmartPtr>(non_associated_device_path);
    if (!non_ass_devices) {
        return false;
    }

    //Lets iterate through all instances
    for (auto &non_ass_device : *non_ass_devices) {
        uint8_t signal_strength(0);
        uint8_t channel(0);
        uint8_t operating_class(0);
        std::string time_stamp_str;
        std::string mac_address_amx;
        non_ass_device.second.read_child(mac_address_amx, "MACAddress");
        if (mac_address_amx.empty()) {
            continue;
        }
        non_ass_device.second.read_child(signal_strength, "SignalStrength");
        non_ass_device.second.read_child(channel, "Channel");
        non_ass_device.second.read_child(operating_class, "OperatingClass");
        non_ass_device.second.read_child(time_stamp_str, "TimeStamp");

        amxc_ts_t time;
        memset(&time, 0, sizeof(amxc_ts_t));
        amxc_ts_parse(&time, time_stamp_str.c_str(), time_stamp_str.size());
        uint32_t timestamp_ms = time.sec * 1000 + time.nsec / 1000000;
        // TimeStamp is datetime as per RFC3339 and maybe contains fractions of seconds information

        if (new_list.find(mac_address_amx) != new_list.end()) {
            //NonAssociatedDevice exists -->get the result and update the data
            sUnassociatedStationStats new_stat = {
                tlvf::mac_from_string(mac_address_amx),
                signal_strength,
                channel,
                operating_class,
                timestamp_ms,
            };
            stats.push_back(new_stat);
            LOG(DEBUG) << " read unassociated station stats for mac_address: " << mac_address_amx
                       << " SignalStrength: " << signal_strength << " channel: " << channel
                       << " operating_class: " << operating_class
                       << " TimeStamp(string): " << time_stamp_str
                       << " and TimeStamp(seconds): " << time.sec
                       << " and TimeStamp(milliseconds): " << timestamp_ms;
            new_list.erase(mac_address_amx); // consumed!
        } else { // -->controller is not interested on it any more--> remove it from the dm
            amx_un_stations_to_be_removed.push_back(mac_address_amx);
        }
    }

    // Now lets remove all stations the controller do not want them anymore
    for (auto &station_to_remove : amx_un_stations_to_be_removed) {
        AmbiorixVariant result;
        AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
        args.add_child("MACAddress", station_to_remove);
        if (!m_ambiorix_cl.call(nasta_monitor_path, "deleteNonAssociatedDevice", args, result)) {
            LOG(ERROR) << " remote function call deleteNonAssociatedDevice"
                       << " for object " << nasta_monitor_path
                       << " and  MACAddress: " << station_to_remove << " Failed!!";
            continue;
        } else {
            LOG(TRACE) << "Successfully removed unassociated station with mac: "
                       << station_to_remove
                       << " and path: " << nasta_monitor_path + station_to_remove;
        }
    }
    sUnassociatedStationsStats stats_out{stats};
    auto msg_buff = ALLOC_SMART_BUFFER(sizeof(stats_out));
    if (!msg_buff) {
        LOG(FATAL) << "Memory allocation failed for "
                      "sUnassociatedStationsStats!";
        return false;
    }
    auto msg = reinterpret_cast<sUnassociatedStationsStats *>(msg_buff.get());
    memset(msg_buff.get(), 0, sizeof(stats_out));
    std::copy(stats_out.un_stations_stats.begin(), stats_out.un_stations_stats.end(),
              back_inserter(msg->un_stations_stats));

    event_queue_push(Event::Unassociation_Stations_Stats,
                     msg_buff); // send message internally the monitor

    return true;
}

bool mon_wlan_hal_whm::process_scan_complete_event(const std::string &result)
{
    if (result == "error") {
        LOG(DEBUG) << " received ScanComplete event with Error indication!";
        m_scan_active = false;
        LOG(INFO) << "m_scan_active: " << m_scan_active;
        event_queue_push(Event::Channel_Scan_Aborted);
        return false;
    }
    if (result == "done" && m_scan_active) {
        m_scan_results.clear();
        event_queue_push(Event::Channel_Scan_New_Results_Ready);
        m_scan_active = false;
        LOG(INFO) << "m_scan_active: " << m_scan_active;
        channel_scan_dump_results();
        event_queue_push(Event::Channel_Scan_Finished);
    } else if (!m_scan_active) {
        LOG(INFO) << "results from a scan not requested by bwl";
    }
    return true;
}

bool mon_wlan_hal_whm::get_scan_results_from_pwhm()
{
    AmbiorixVariant result;
    AmbiorixVariant args(AMXC_VAR_ID_HTABLE);
    if (!m_ambiorix_cl.call(m_radio_path, "getScanCombinedData", args, result)) {
        LOG(ERROR) << " remote function call getScanResults Failed!";
        return false;
    }
    AmbiorixVariantListSmartPtr results_list = result.read_children<AmbiorixVariantListSmartPtr>();
    if (!results_list) {
        LOG(ERROR) << "failed reading scan_results!";
        return false;
    }

    if (results_list->empty()) {
        LOG(ERROR) << "scan_results are empty";
        return false;
    }

    m_scan_results.clear();
    m_spectrum_results.clear();

    auto results_as_wrapped_list =
        results_list->front().read_children<AmbiorixVariantMapSmartPtr>();

    if (results_as_wrapped_list->empty()) {
        LOG(ERROR) << "results_as_wrapped_list are empty";
        return false;
    }

    auto ssid_results_as_list =
        (*results_as_wrapped_list)["BSS"].read_children<AmbiorixVariantListSmartPtr>();

    if (ssid_results_as_list->empty()) {
        LOG(ERROR) << "ssid_results_as_list are empty";
        return false;
    }

    for (auto &ssid_results_map : *ssid_results_as_list) {
        auto data_map = ssid_results_map.read_children<AmbiorixVariantMapSmartPtr>();
        auto &map     = *data_map;

        std::unordered_map<std::string, std::string> map_cach_bssid;

        std::string bssid;
        if (map.find("BSSID") != map.end()) {
            map["BSSID"].get(bssid);
            map_cach_bssid["BSSID"] = bssid;
        } else {
            LOG(DEBUG) << "BSSID is missing,skipping";
            continue;
        }

        if (map.find("SSID") != map.end()) {
            std::string ssid;
            map["SSID"].get(ssid);
            map_cach_bssid["SSID"] = ssid;
        } else {
            LOG(DEBUG) << "SSID is missing,skipping";
            continue;
        }

        if (map.find("CentreChannel") != map.end()) {
            std::string center_channel;
            map["CentreChannel"].get(center_channel);
            map_cach_bssid["CentreChannel"] = center_channel;
        } else {
            LOG(DEBUG) << "CentreChannel is missing,skipping";
            continue;
        }

        std::string bandwidth;
        if (map.find("Bandwidth") != map.end()) {
            map["Bandwidth"].get(bandwidth);
            map_cach_bssid["Bandwidth"] = bandwidth;
        } else {
            LOG(DEBUG) << "Bandwidth is missing,skipping";
            continue;
        }
        if (map.find("Channel") != map.end()) {
            std::string channel;
            map["Channel"].get(channel);
            map_cach_bssid["Channel"] = channel;
        } else {
            LOG(DEBUG) << "Channel is missing,skipping";
            continue;
        }

        if (map.find("RSSI") != map.end()) {
            std::string rssi;
            map["RSSI"].get(rssi);
            map_cach_bssid["RSSI"] = rssi;
        } else {
            LOG(DEBUG) << "RSSI is missing,skipping";
            continue;
        }

        if (map.find("SecurityModeEnabled") != map.end()) {
            std::string security_mode_enabled;
            map["SecurityModeEnabled"].get(security_mode_enabled);
            map_cach_bssid["SecurityModeEnabled"] = security_mode_enabled;
        }

        if (map.find("EncryptionMode") != map.end()) {
            std::string encryption_mode;
            map["EncryptionMode"].get(encryption_mode);
            map_cach_bssid["EncryptionMode"] = encryption_mode;
        }

        if (map.find("OperatingStandards") != map.end()) {
            std::string operating_standards;
            map["OperatingStandards"].get(operating_standards);
            map_cach_bssid["OperatingStandards"] = operating_standards;
        }

        if (map.find("ChannelUtilization") != map.end()) {
            std::string ChannelUtilization;
            map["ChannelUtilization"].get(ChannelUtilization);
            map_cach_bssid["ChannelUtilization"] = ChannelUtilization;
        }

        if (map.find("StationCount") != map.end()) {
            std::string StationCount;
            map["StationCount"].get(StationCount);
            map_cach_bssid["StationCount"] = StationCount;
        }

        m_scan_results.push_back(map_cach_bssid);
    }

    auto spectrum_results_as_list =
        (*results_as_wrapped_list)["Spectrum"].read_children<AmbiorixVariantListSmartPtr>();

    if (spectrum_results_as_list->empty()) {
        LOG(ERROR) << "spectrum_results_as_list are empty";
        return false;
    }

    for (auto &spectrum_results_map : *spectrum_results_as_list) {
        auto data_map = spectrum_results_map.read_children<AmbiorixVariantMapSmartPtr>();
        auto &map     = *data_map;

        std::unordered_map<std::string, std::string> map_cach_spectrum;

        if (map.find("bandwidth") != map.end()) {
            std::string bandwidth;
            map["bandwidth"].get(bandwidth);
            map_cach_spectrum["Bandwidth"] = bandwidth;
        } else {
            LOG(DEBUG) << "Bandwidth is missing,skipping";
            continue;
        }

        if (map.find("channel") != map.end()) {
            std::string channel;
            map["channel"].get(channel);
            map_cach_spectrum["Channel"] = channel;
        } else {
            LOG(DEBUG) << "Channel is missing,skipping";
            continue;
        }

        if (map.find("noiselevel") != map.end()) {
            std::string noise_level;
            map["noiselevel"].get(noise_level);
            map_cach_spectrum["Noiselevel"] = noise_level;
        } else {
            LOG(DEBUG) << "Noiselevel is missing,skipping";
            continue;
        }

        if (map.find("availability") != map.end()) {
            std::string availability;
            map["availability"].get(availability);
            map_cach_spectrum["Availability"] = availability;
        } else {
            LOG(DEBUG) << "Availability is missing,skipping";
            continue;
        }

        m_spectrum_results.push_back(map_cach_spectrum);
    }

    return true;
}

} // namespace whm

std::shared_ptr<mon_wlan_hal> mon_wlan_hal_create(const std::string &iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback,
                                                  const bwl::hal_conf_t &hal_conf)
{
    return std::make_shared<whm::mon_wlan_hal_whm>(iface_name, callback, hal_conf);
}

} // namespace bwl
