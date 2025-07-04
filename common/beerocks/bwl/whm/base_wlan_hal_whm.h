/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2022 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_WHM_H_
#define _BWL_BASE_WLAN_HAL_WHM_H_

#include "utils_wlan_hal_whm.h"
#include <bcl/beerocks_state_machine.h>
#include <bwl/base_wlan_hal.h>
#include <bwl/key_value_parser.h>
#include <bwl/nl80211_client.h>

#include "ambiorix_client.h"
#include "wbapi_utils.h"

#include <chrono>
#include <memory>

namespace bwl {
namespace whm {

enum class whm_fsm_state { Delay, Init, GetRadioInfo, Attach, Operational, Detach };

enum class whm_fsm_event { Attach, Detach };

struct VAPExtInfo {
    std::string path;
    std::string ssid_path;
    std::string status;
    bool teardown = false;

    bool operator==(const VAPExtInfo &other) const { return (path == other.path); }

    bool operator!=(const VAPExtInfo &other) const { return !(*this == other); }
};

struct sStationInfo {
    explicit sStationInfo(const std::string &path_in) : path(path_in) {}
    std::string path;

    bool operator==(const sStationInfo &other) const { return (path == other.path); }

    bool operator!=(const sStationInfo &other) const { return !(*this == other); }
};

/*!
 * Base class for the whm abstraction layer.
 */
class base_wlan_hal_whm : public virtual base_wlan_hal,
                          protected beerocks::beerocks_fsm<whm_fsm_state, whm_fsm_event>,
                          public KeyValueParser {

    // Public methods:
public:
    virtual ~base_wlan_hal_whm();

    virtual HALState attach(bool block = false) override;
    virtual bool detach() override;
    virtual bool ping() override;
    virtual bool reassociate() override;
    virtual bool refresh_radio_info() override;
    virtual bool refresh_vaps_info(int id) override;
    virtual bool
    get_vap_status(const std::list<son::wireless_utils::sBssInfoConf> &bss_info_conf_list) override;
    virtual bool process_ext_events(int fd = 0) override;
    virtual bool process_nl_events() override { return true; };
    virtual std::string get_radio_mac() override;

    /**
     * @brief Gets channel utilization.
     *
     * @see base_wlan_hal::get_channel_utilization
     *
     * Returns a fake channel utilization value, varying from 0 to UINT8_MAX on each call.
     *
     * @param[out] channel_utilization Channel utilization value.
     *
     * @return True on success and false otherwise.
     */
    bool get_channel_utilization(uint8_t &channel_utilization) override;

    // Protected methods
protected:
    base_wlan_hal_whm(HALType type, const std::string &iface_name, hal_event_cb_t callback,
                      const hal_conf_t &hal_conf = {});

    virtual bool set(const std::string &param, const std::string &value,
                     int vap_id = beerocks::IFACE_RADIO_ID) override;
    int whm_get_vap_id(const std::string &iface);
    bool whm_get_radio_ref(const std::string &iface, std::string &ref);
    bool whm_get_radio_path(const std::string &iface, std::string &path);
    bool refresh_vap_info(int id, const beerocks::wbapi::AmbiorixVariant &ap_obj);
    bool get_radio_vaps(beerocks::wbapi::AmbiorixVariantMap &aps);
    bool get_accesspoint_by_ssid(std::string &ssid_path, std::string &ap_path);
    bool has_enabled_vap() const;
    bool check_enabled_vap(const std::string &bss) const;

    beerocks::wbapi::AmbiorixClient m_ambiorix_cl;
    std::unique_ptr<nl80211_client> m_iso_nl80211_client; //impl nl80211 client apis with whm dm
    std::string m_radio_path;
    std::unordered_map<std::string, VAPExtInfo> m_vapsExtInfo; // key = vap_ifname
    std::unordered_map<std::string, sStationInfo> m_stations;  // key = sta_mac
    void subscribe_to_radio_events();
    virtual bool process_radio_event(const std::string &interface, const std::string &key,
                                     const beerocks::wbapi::AmbiorixVariant *value);
    void subscribe_to_radio_channel_change_events();
    virtual bool process_radio_channel_change_event(const beerocks::wbapi::AmbiorixVariant *value);

    void subscribe_to_ap_events();
    virtual bool process_ap_event(const std::string &interface, const std::string &key,
                                  const beerocks::wbapi::AmbiorixVariant *value);

    void subscribe_to_sta_events();
    virtual bool process_sta_event(const std::string &interface, const std::string &sta_mac,
                                   const std::string &key,
                                   const beerocks::wbapi::AmbiorixVariant *value);

    /**
     * @brief subscribe to WiFi.Radio. ScanComplete dm notification
     */
    void subscribe_to_scan_complete_events();
    /**
     * @brief Process the event "ScanComplete" when received from the dm
     */
    virtual bool process_scan_complete_event(const std::string &result);

    /**
     * @brief Process event "wpaCtrlEvents"
     */
    virtual bool process_wpa_ctrl_event(const beerocks::wbapi::AmbiorixVariant &event_data);

    /**
     * @brief subscribe to WiFi.Radio.XXXXX.NaStaMonitor.RssiEventing RssiUpdate dm notification
     */
    virtual void subscribe_to_rssi_eventing_events();

    /**
     * @brief Process the event "RssiUpdate" when received from the dm
     */
    virtual void process_rssi_eventing_event(const std::string &interface,
                                             beerocks::wbapi::AmbiorixVariant *value);

    // Private data-members:
private:
    bool fsm_setup();
    void populate_channels_max_tx_power();

protected:
    std::shared_ptr<beerocks::wbapi::sAmbiorixEventHandler> m_rssi_event_handler;
    std::string m_radio_mac_address;
};

} // namespace whm
} // namespace bwl

#endif // _BWL_BASE_WLAN_HAL_WHM_H_
