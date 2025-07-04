/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_STA_WLAN_HAL_H_
#define _BWL_STA_WLAN_HAL_H_

#include "base_wlan_hal.h"
#include "sta_wlan_hal_types.h"

namespace bwl {

/*!
 * Hardware abstraction layer for WLAN Station/Client.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class sta_wlan_hal : public virtual base_wlan_hal {

    // Public definitions
public:
    // STA Evnets
    enum class Event {
        Invalid = 0,

        Connected,
        Disconnected,
        Terminating,
        ScanResults,
        ChannelSwitch,

        STA_Unassoc_RSSI,

        Interface_Connected_OK,
        Interface_Reconnected_OK,
        Interface_Disconnected
    };

    using ChannelFreqPair = std::pair<uint8_t, beerocks::eFreqType>;

    // Public methods:
public:
    virtual ~sta_wlan_hal() = default;

    virtual bool start_wps_pbc() = 0;

    virtual bool initiate_scan() = 0;

    /*
     * @brief Trigger a scan for a specific BSS.
     * 
     * Do scan requests for the given BSS on the given channel. It is necessary to do this before 
     * connecting to the BSS, unless a full channel scan (triggered by initiate_scan()) has been 
     * done before.
     *
     * @param[in] bssid The BSSID to scan for.
     * @param[in] channel The channel to scan on.
     * If not provided, all channels are scanned.
     */
    virtual bool scan_bss(const sMacAddr &bssid, uint8_t channel,
                          beerocks::eFreqType freq_type) = 0;

    virtual int get_scan_results(const std::string &ssid, std::vector<sScanResult> &list,
                                 bool parse_vsie = false) = 0;

    virtual bool connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                         bool mem_only_psk, const std::string &bssid, ChannelFreqPair channel,
                         bool hidden_ssid) = 0;

    virtual bool disconnect() = 0;

    virtual bool roam(const sMacAddr &bssid, ChannelFreqPair channel) = 0;

    virtual bool get_4addr_mode()             = 0;
    virtual bool set_4addr_mode(bool enable)  = 0;
    virtual bool set_3addr_mcast(bool enable) = 0;

    virtual bool unassoc_rssi_measurement(const std::string &mac, int chan,
                                          beerocks::eWiFiBandwidth bw, int vht_center_frequency,
                                          int delay, int window_size) = 0;

    virtual bool is_connected()  = 0;
    virtual int get_channel()    = 0;
    virtual bool update_status() = 0;

    virtual std::string get_ssid()              = 0;
    virtual std::string get_bssid()             = 0;
    virtual bool clear_non_associated_devices() = 0;

    /*!
     * Returns the Backhaul Mac address.
     */
    virtual std::string get_wireless_backhaul_mac() = 0;
};

// STA HAL factory types
std::shared_ptr<sta_wlan_hal> sta_wlan_hal_create(const std::string &iface_name,
                                                  base_wlan_hal::hal_event_cb_t cb,
                                                  const bwl::hal_conf_t &hal_conf);

} // namespace bwl

#endif // _BWL_STA_WLAN_HAL_H_
