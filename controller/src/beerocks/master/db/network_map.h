/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _NETWORK_MAP_H_
#define _NETWORK_MAP_H_

#include "db.h"

namespace son {
class network_map {
public:
    static void send_bml_network_map_message(db &database, int fd,
                                             ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id);

    static std::ptrdiff_t fill_bml_node_data(db &database, std::string node_mac, uint8_t *tx_buffer,
                                             const std::ptrdiff_t &buffer_size,
                                             bool force_client_disconnect);
    static std::ptrdiff_t fill_bml_station_data(db &database, std::shared_ptr<Station> station,
                                                uint8_t *tx_buffer,
                                                const std::ptrdiff_t &buffer_size,
                                                bool force_client_disconnect = false);
    static std::ptrdiff_t fill_bml_agent_data(db &database, std::shared_ptr<Agent> agent,
                                              uint8_t *tx_buffer, const std::ptrdiff_t &buffer_size,
                                              bool force_client_disconnect = false);

    static void
    send_bml_nodes_statistics_message_to_listeners(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                                   const std::vector<int> &bml_listeners,
                                                   std::set<std::string> valid_hostaps);
    static std::ptrdiff_t fill_bml_radio_statistics(db &database,
                                                    std::shared_ptr<Agent::sRadio> radio,
                                                    uint8_t *tx_buffer, std::ptrdiff_t buf_size);
    static std::ptrdiff_t fill_bml_station_statistics(db &database, std::shared_ptr<Station> pSta,
                                                      uint8_t *tx_buffer, std::ptrdiff_t buf_size);

    static void send_bml_event_to_listeners(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                            const std::vector<int> &bml_listeners);

    static void send_bml_bss_tm_req_message_to_listeners(db &database,
                                                         ieee1905_1::CmduMessageTx &cmdu_tx,
                                                         const std::vector<int> &bml_listeners,
                                                         std::string target_bssid,
                                                         uint8_t disassoc_imminent);

    static void send_bml_bh_roam_req_message_to_listeners(db &database,
                                                          ieee1905_1::CmduMessageTx &cmdu_tx,
                                                          const std::vector<int> &bml_listeners,
                                                          std::string bssid, uint8_t channel);

    static void send_bml_client_allow_req_message_to_listeners(
        db &database, ieee1905_1::CmduMessageTx &cmdu_tx, const std::vector<int> &bml_listeners,
        std::string sta_mac, std::string hostap_mac, std::string ip);

    static void send_bml_client_disallow_req_message_to_listeners(
        db &database, ieee1905_1::CmduMessageTx &cmdu_tx, const std::vector<int> &bml_listeners,
        std::string sta_mac, std::string hostap_mac);

    static void send_bml_acs_start_message_to_listeners(db &database,
                                                        ieee1905_1::CmduMessageTx &cmdu_tx,
                                                        const std::vector<int> &bml_listeners,
                                                        std::string hostap_mac);

    static void send_bml_csa_notification_message_to_listeners(
        db &database, ieee1905_1::CmduMessageTx &cmdu_tx, const std::vector<int> &bml_listeners,
        std::string hostap_mac, uint8_t bandwidth, uint8_t channel,
        uint8_t channel_ext_above_primary, uint16_t vht_center_frequency);

    static void send_bml_cac_status_changed_notification_message_to_listeners(
        db &database, ieee1905_1::CmduMessageTx &cmdu_tx, const std::vector<int> &bml_listeners,
        std::string hostap_mac, uint8_t cac_completed);
};

} // namespace son
#endif
