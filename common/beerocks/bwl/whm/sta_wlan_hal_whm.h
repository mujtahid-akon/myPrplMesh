/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2022 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_STA_WLAN_HAL_WHM_H_
#define _BWL_STA_WLAN_HAL_WHM_H_

#include "base_wlan_hal_whm.h"
#include <bwl/sta_wlan_hal.h>

namespace bwl {
namespace whm {

/*!
 * Hardware abstraction layer for WLAN Station/Client.
 */
class sta_wlan_hal_whm : public base_wlan_hal_whm, public sta_wlan_hal {

    // Public methods
public:
    /*!
     * Constructor.
     *
     * @param [in] iface_name STA/Client interface name.
     * @param [in] callback Callback for handling internal events.
     */
    sta_wlan_hal_whm(const std::string &iface_name, hal_event_cb_t callback,
                     const bwl::hal_conf_t &hal_conf);
    virtual ~sta_wlan_hal_whm();

    virtual bool start_wps_pbc() override;
    virtual bool detach() override;

    virtual bool initiate_scan() override;
    bool scan_bss(const sMacAddr &bssid, uint8_t channel, beerocks::eFreqType freq_type) override;
    virtual int get_scan_results(const std::string &ssid, std::vector<sScanResult> &list,
                                 bool parse_vsie = false) override;

    virtual bool connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                         bool mem_only_psk, const std::string &bssid, ChannelFreqPair channel,
                         bool hidden_ssid) override;

    virtual bool disconnect() override;

    virtual bool reassociate() override;

    virtual bool roam(const sMacAddr &bssid, ChannelFreqPair channel) override;

    virtual bool get_4addr_mode() override;
    virtual bool set_4addr_mode(bool enable) override;
    virtual bool set_3addr_mcast(bool enable) override;

    virtual bool unassoc_rssi_measurement(const std::string &mac, int chan,
                                          beerocks::eWiFiBandwidth bw, int vht_center_frequency,
                                          int delay, int window_size) override;

    virtual bool is_connected() override;
    virtual int get_channel() override;
    virtual bool update_status() override;
    virtual bool clear_non_associated_devices() override;

    std::string get_ssid() override;
    std::string get_bssid() override;
    std::string get_wireless_backhaul_mac() override;

    const std::vector<int> &get_ext_events_fds() const;
    bool unique_file_descriptors() const { return false; }

protected:
    // Overload for Monitor events
    bool event_queue_push(sta_wlan_hal::Event event, std::shared_ptr<void> data = {})
    {
        return base_wlan_hal::event_queue_push(int(event), data);
    }

private:
    /**
     * @brief subscribe to WiFi.EndPoint.*.ConnectionStatus and IntfName dm object change
     */
    void subscribe_to_ep_events();

    /**
     * *@brief Process the event WiFi.EndPoint.*.ConnectionStatus and IntfName dm event
     */
    bool process_ep_event(const std::string &interface, const std::string &key,
                          const beerocks::wbapi::AmbiorixVariant *new_value,
                          const beerocks::wbapi::AmbiorixVariant *old_value);

    /**
     * @brief subscribe to WiFi.EndPoint.*.WPS. pairingDone dm notification
     */
    void subscribe_to_ep_wps_events();

    /**
     * *@brief Process the event "pairingDone" when received from the dm
     */
    bool process_ep_wps_event(const std::string &interface,
                              const beerocks::wbapi::AmbiorixVariant *data);

    /**
     * @brief Process the event "ScanComplete" when received from the dm
     */
    bool process_scan_complete_event(const std::string &result) override;

    struct Endpoint {
        std::string bssid;
        std::string ssid;
        std::string connection_status;
        uint8_t multi_ap_profile;
        uint16_t multi_ap_primary_vlanid;
        int channel;
        int active_profile_id;
    };

    struct Profile {
        int id;
        std::string alias;
        std::string ssid;
        std::string bssid;
        WiFiSec sec;
        bool mem_only_psk;
        std::string pass;
        bool hidden_ssid;
        ChannelFreqPair channel;
    };

    bool set_profile(Profile &profile);
    int add_profile(const std::string &alias);
    int find_profile_by_alias(const std::string &alias);
    int remove_profile(int profile_id);
    bool set_profile_params(const Profile &profile);
    bool enable_profile(int profile_id);

    bool read_status(Endpoint &endpoint);
    void update_status(const Endpoint &endpoint);
    void clear_conn_state();
    bool is_connected(const std::string &status);

    std::string m_ep_path;
    // Active profile parameters
    std::string m_active_ssid;
    std::string m_active_bssid;
    std::string m_active_pass;
    std::string m_active_connection_status;
    WiFiSec m_active_secutiry = WiFiSec::Invalid;
    uint8_t m_active_channel  = 0;
    int m_active_profile_id   = -1;
    bool m_scan_active        = false;
};

} // namespace whm
} // namespace bwl

#endif // _BWL_STA_WLAN_HAL_WHM_H_
