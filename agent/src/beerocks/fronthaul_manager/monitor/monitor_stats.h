/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MONITOR_LOAD_H
#define MONITOR_LOAD_H

#include "monitor_db.h"

#include <bcl/beerocks_cmdu_client.h>
#include <bcl/beerocks_message_structs.h>
#include <bcl/network/network_utils.h>

#include <bwl/mon_wlan_hal.h>
#include <tlvf/CmduMessageTx.h>

namespace son {
class monitor_stats {

public:
    explicit monitor_stats(ieee1905_1::CmduMessageTx &cmdu_tx_);
    ~monitor_stats() {}
    bool start(monitor_db *mon_db_, std::shared_ptr<beerocks::CmduClient> slave_client);
    void stop();

    void add_request(uint16_t id, uint8_t sync,
                     const sMacAddr &mac = beerocks::net::network_utils::ZERO_MAC);

    void process(bool instant_handling = false);

    /** Collect AP metrics and create AP Metrics TLV */
    bool add_ap_metrics(ieee1905_1::CmduMessageTx &cmdu_tx, const monitor_vap_node &vap_node,
                        const monitor_radio_node &radio_node,
                        std::shared_ptr<bwl::mon_wlan_hal> &mon_wlan_hal) const;
    bool add_ap_extended_metrics(ieee1905_1::CmduMessageTx &cmdu_tx,
                                 monitor_vap_node &vap_node) const;
    bool add_ap_assoc_sta_traffic_stat(ieee1905_1::CmduMessageTx &cmdu_tx,
                                       const monitor_sta_node &sta_node);
    bool add_ap_assoc_sta_link_metric(ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &bssid,
                                      monitor_sta_node &sta_node);
    bool add_ap_assoc_wifi_6_sta_status_report(ieee1905_1::CmduMessageTx &cmdu_tx,
                                               const monitor_sta_node &sta_node);
    bool add_radio_metrics(ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &radio_mac,
                           monitor_radio_node &radio_node) const;
    bool add_affiliated_ap_metrics(ieee1905_1::CmduMessageTx &cmdu_tx,
                                   monitor_vap_node &vap_node) const;

    int8_t conf_total_ch_load_notification_lo_th_percent    = 20;
    int8_t conf_total_ch_load_notification_hi_th_percent    = 90;
    int8_t conf_total_ch_load_notification_delta_th_percent = 30;
    int8_t conf_min_active_client_count                     = 2;
    int8_t conf_active_client_th                            = 3;
    int8_t conf_client_load_notification_delta_th_percent   = 20;
    uint32_t conf_ap_idle_threshold_B                       = 10000;
    uint32_t conf_ap_active_threshold_B                     = 20000;
    uint16_t conf_ap_idle_stable_time_sec                   = 600;

    //const uint32_t AP_IDLE_BYTES_THRESHOLD = 10000;
    const uint32_t AP_ACTIVE_BYTES_THRESHOLD = 20000;
    //const uint32_t AP_IDLE_TIME_SEC_THRESHOLD = 70; //600;

    uint32_t idle_timer = 0;
    std::chrono::steady_clock::time_point idle_prev_timestamp;
    beerocks::eApActiveMode eApActiveMode = beerocks::eApActiveMode::AP_ACTIVE_MODE;

private:
    void calculate_client_load(monitor_sta_node *sta_node, monitor_radio_node *radio_node,
                               int active_sta_th);

    std::string parent_thread_name;
    monitor_db *mon_db   = nullptr;
    bool stats_requested = false;

    /**
     * CMDU client to send messages to the CMDU server running in slave.
     */
    std::shared_ptr<beerocks::CmduClient> m_slave_client;

    struct sMeasurementsRequest {
        sMeasurementsRequest(uint16_t _mid, const sMacAddr &_mac) : message_id(_mid), mac(_mac) {}
        uint16_t message_id;
        sMacAddr mac;
    };
    std::list<sMeasurementsRequest> requests_list;
    void send_hostap_measurements(const sMeasurementsRequest &request,
                                  const monitor_radio_node::SRadioStats &radio_stats);
    void send_associated_sta_link_metrics(const sMeasurementsRequest &request);

    ieee1905_1::CmduMessageTx &cmdu_tx;
};

} // namespace son

#endif
