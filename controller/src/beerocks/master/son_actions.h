/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _SON_ACTIONS_H_
#define _SON_ACTIONS_H_

#include "controller.h"

#include <bcl/beerocks_message_structs.h>
#include <tlvf/wfa_map/tlvClientAssociationControlRequest.h>

#define CLI_LOG(a) LOG(a)

#define LOG_CLI(LEVEL, msg)                                                                        \
    {                                                                                              \
        std::stringstream ss;                                                                      \
        ss << msg;                                                                                 \
        son_actions::send_cli_debug_message(database, cmdu_tx, ss);                                \
        CLI_LOG(LEVEL) << ss.rdbuf();                                                              \
    }

namespace son {
class son_actions {
public:
    static void handle_completed_connection(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                            task_pool &tasks, std::string client_mac);
    static bool add_station_to_default_location(db &database, std::string client_mac);
    static void unblock_sta(db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::string sta_mac);
    static int steer_sta(db &database, ieee1905_1::CmduMessageTx &cmdu_tx, task_pool &tasks,
                         std::string sta_mac, std::string chosen_hostap,
                         const std::string &triggered_by, const std::string &steering_type,
                         bool disassoc_imminent,
                         int disassoc_timer_ms = beerocks::BSS_STEER_DISASSOC_TIMER_MS,
                         bool steer_restricted = false);
    static void disconnect_client(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                  const std::string &client_mac, const std::string &bssid,
                                  eDisconnectType type, uint32_t reason,
                                  eClientDisconnectSource src = eClient_Disconnect_Source_Ignore);
    static bool set_radio_active(db &database, task_pool &tasks, std::string hostap_mac,
                                 const bool active);
    static void send_cli_debug_message(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                       std::stringstream &ss);

    static void handle_dead_radio(const sMacAddr &mac, bool reported_by_parent, db &database,
                                  task_pool &tasks);
    static void handle_dead_station(std::string mac, bool reported_by_parent, db &database,
                                    task_pool &tasks);
    static bool validate_beacon_measurement_report(beerocks_message::sBeaconResponse11k report,
                                                   const std::string &sta_mac,
                                                   const std::string &bssid);
    static bool has_matching_operating_class(wfa_map::tlvApRadioBasicCapabilities &radio_basic_caps,
                                             const wireless_utils::sBssInfoConf &bss_info_conf);
    static bool send_cmdu_to_agent(const sMacAddr &dest_mac, ieee1905_1::CmduMessageTx &cmdu_tx,
                                   db &database, const std::string &radio_mac = std::string());
    static bool send_ap_config_renew_msg(ieee1905_1::CmduMessageTx &cmdu_tx, db &database);
    static bool send_topology_query_msg(const sMacAddr &dest_mac,
                                        ieee1905_1::CmduMessageTx &cmdu_tx, db &database);

    static int start_btm_request_task(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                      task_pool &tasks, const bool &disassoc_imminent,
                                      const int &disassoc_timer_ms,
                                      const int &bss_term_duration_min,
                                      const int &validity_interval_ms, const int &steering_timer_ms,
                                      const std::string &sta_mac, const std::string &target_bssid,
                                      const std::string &event_source);

    static bool send_client_association_control(
        db &database, ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &agent_mac,
        const sMacAddr &agent_bssid, const std::unordered_set<sMacAddr> &station_list,
        const int &duration_sec,
        wfa_map::tlvClientAssociationControlRequest::eAssociationControl association_flag);

    static bool handle_agent_ap_mld_configuration_tlv(db &database, const sMacAddr &al_mac,
                                                      ieee1905_1::CmduMessageRx &cmdu_rx);

private:
    static bool
    check_hostap_activability(db &database,
                              std::string mac); // note: there isn't a word - "activability"
};

} // namespace son
#endif
