/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_task.h"
#include "../db/db.h"
#include "../db/network_map.h"
#include "bml_defs.h"

#include <bcl/network/sockets.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

#include <climits>

using namespace beerocks;
using namespace son;

bml_task::bml_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_, task_pool &tasks_)
    : task("bml task"), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_)
{
}

void bml_task::work()
{
    switch (state) {
    case START: {
        int prev_task_id = database.get_bml_task_id();
        tasks.kill_task(prev_task_id);
        database.assign_bml_task_id(id);

        state = IDLE;
        break;
    }
    case IDLE: {
        wait_for(5000);
        break;
    }
    case LISTENING: {
        wait_for(1000);
        break;
    }
    }
}

void bml_task::handle_event(int event_type, void *obj)
{
    if ((event_type != REGISTER_TO_NW_MAP_UPDATES && event_type != REGISTER_TO_STATS_UPDATES &&
         event_type != REGISTER_TO_EVENTS_UPDATES && event_type != REGISTER_TO_TOPOLOGY_UPDATES) &&
        !database.is_bml_listener_exist()) {
        return;
    }

    //TASK_LOG(TRACE) << "event_type " << event_type << " was received";
    std::vector<int> events_updates_listeners;
    int idx = 0;
    int sd;

    while ((sd = database.get_bml_socket_at(idx)) !=
           beerocks::net::FileDescriptor::invalid_descriptor) {
        if (database.get_bml_events_update_enable(sd)) {
            events_updates_listeners.push_back(sd);
        }
        idx++;
    }

    switch (event_type) {
    case CONNECTION_CHANGE: {
        if (obj) {
            auto event_obj = static_cast<connection_change_event *>(obj);
            TASK_LOG(DEBUG) << "connection change event, mac=" << event_obj->mac;

            if (event_obj->mac.empty()) {
                TASK_LOG(ERROR) << "mac address is empty!";
                break;
            }
            update_bml_nw_map(event_obj->mac, event_obj->force_client_disconnect);
        }
        break;
    }
    case STATS_INFO_AVAILABLE: {
        //TASK_LOG(DEBUG) << "stats info available event";
        if (obj) {
            auto event_obj = static_cast<stats_info_available_event *>(obj);

            int index = 0;
            std::vector<int> stats_updates_listeners;
            int fd;
            while ((fd = database.get_bml_socket_at(index)) !=
                   beerocks::net::FileDescriptor::invalid_descriptor) {
                if (database.get_bml_stats_update_enable(fd)) {
                    stats_updates_listeners.push_back(fd);
                }
                index++;
            }
            if (!stats_updates_listeners.empty()) {
                network_map::send_bml_nodes_statistics_message_to_listeners(
                    database, cmdu_tx, stats_updates_listeners, event_obj->valid_hostaps);
            }
        }
        break;
    }
    case BSS_TM_REQ_EVENT_AVAILABLE: {
        if (obj) {
            auto event_obj = static_cast<bml_task::bss_tm_req_available_event *>(obj);

            if (!events_updates_listeners.empty()) {
                network_map::send_bml_bss_tm_req_message_to_listeners(
                    database, cmdu_tx, events_updates_listeners, event_obj->target_bssid,
                    event_obj->disassoc_imminent);
            }
        }
        break;
    }
    case BH_ROAM_REQ_EVENT_AVAILABLE: {
        if (obj) {
            auto event_obj = static_cast<bml_task::bh_roam_req_available_event *>(obj);

            if (!events_updates_listeners.empty()) {
                network_map::send_bml_bh_roam_req_message_to_listeners(
                    database, cmdu_tx, events_updates_listeners, event_obj->bssid,
                    event_obj->channel);
            }
        }
        break;
    }
    case CLIENT_ALLOW_REQ_EVENT_AVAILABLE: {
        if (obj) {
            auto event_obj = static_cast<bml_task::client_allow_req_available_event *>(obj);

            if (!events_updates_listeners.empty()) {
                network_map::send_bml_client_allow_req_message_to_listeners(
                    database, cmdu_tx, events_updates_listeners, event_obj->sta_mac,
                    event_obj->hostap_mac, event_obj->ip);
            }
        }
        break;
    }
    case CLIENT_DISALLOW_REQ_EVENT_AVAILABLE: {
        if (obj) {
            auto event_obj = static_cast<bml_task::client_disallow_req_available_event *>(obj);

            if (!events_updates_listeners.empty()) {
                network_map::send_bml_client_disallow_req_message_to_listeners(
                    database, cmdu_tx, events_updates_listeners, event_obj->sta_mac,
                    event_obj->hostap_mac);
            }
        }
        break;
    }
    case ACS_START_EVENT_AVAILABLE: {
        if (obj) {
            auto event_obj = static_cast<bml_task::acs_start_event *>(obj);

            if (!events_updates_listeners.empty()) {
                network_map::send_bml_acs_start_message_to_listeners(
                    database, cmdu_tx, events_updates_listeners, event_obj->hostap_mac);
            }
        }
        break;
    }
    case CSA_NOTIFICATION_EVENT_AVAILABLE: {
        if (obj) {
            auto event_obj = static_cast<bml_task::csa_notification_event *>(obj);

            if (!events_updates_listeners.empty()) {
                network_map::send_bml_csa_notification_message_to_listeners(
                    database, cmdu_tx, events_updates_listeners, event_obj->hostap_mac,
                    event_obj->bandwidth, event_obj->channel, event_obj->channel_ext_above_primary,
                    event_obj->vht_center_frequency);
            }
            auto slave_parent_mac =
                database.get_radio_parent_agent(tlvf::mac_from_string(event_obj->hostap_mac));
            update_bml_nw_map(tlvf::mac_to_string(slave_parent_mac));
        }
        break;
    }
    case CAC_STATUS_CHANGED_NOTIFICATION_EVENT_AVAILABLE: {
        if (obj) {
            auto event_obj = static_cast<bml_task::cac_status_changed_notification_event *>(obj);

            if (!events_updates_listeners.empty()) {
                network_map::send_bml_cac_status_changed_notification_message_to_listeners(
                    database, cmdu_tx, events_updates_listeners, event_obj->hostap_mac,
                    event_obj->cac_completed);
            }
            auto slave_parent_mac =
                database.get_radio_parent_agent(tlvf::mac_from_string(event_obj->hostap_mac));
            update_bml_nw_map(tlvf::mac_to_string(slave_parent_mac));
        }
        break;
    }
    case REGISTER_TO_TOPOLOGY_UPDATES: {
        if (!obj) {
            break;
        }
        auto event_obj = static_cast<listener_general_register_unregister_event *>(obj);
        TASK_LOG(DEBUG) << "REGISTER_TO_TOPOLOGY_UPDATES event was received";
        database.add_bml_socket(event_obj->sd);
        if (!database.set_bml_topology_update_enable(event_obj->sd, true)) {
            TASK_LOG(DEBUG) << "fail in topology_updates registration";
        }
        state = LISTENING;
        break;
    }
    case UNREGISTER_TO_TOPOLOGY_UPDATES: {
        if (!obj) {
            break;
        }
        auto event_obj = static_cast<listener_general_register_unregister_event *>(obj);
        TASK_LOG(DEBUG) << "UNREGISTER_TO_TOPOLOGY_UPDATES event was received";

        if (!database.set_bml_topology_update_enable(event_obj->sd, false)) {
            TASK_LOG(DEBUG) << "fail in topology_updates unregistration";
        }
        if (!database.is_bml_listener_exist()) {
            state = IDLE;
        }
        break;
    }
    case REGISTER_TO_NW_MAP_UPDATES: {
        if (obj) {
            auto event_obj = static_cast<listener_general_register_unregister_event *>(obj);
            TASK_LOG(DEBUG) << "REGISTER_TO_NW_MAP_UPDATES event was received";
            database.add_bml_socket(event_obj->sd);
            if (!database.set_bml_nw_map_update_enable(event_obj->sd, true)) {
                TASK_LOG(DEBUG) << "fail in changing nw_update registration";
            }
            state = LISTENING;
        }
        break;
    }
    case UNREGISTER_TO_NW_MAP_UPDATES: {
        if (obj) {
            auto event_obj = static_cast<listener_general_register_unregister_event *>(obj);
            TASK_LOG(DEBUG) << "UNREGISTER_TO_NW_MAP_UPDATES event was received";

            if (!database.set_bml_nw_map_update_enable(event_obj->sd, false)) {
                TASK_LOG(DEBUG) << "fail in changing nw_map_update unregistration";
            }
            if (!database.is_bml_listener_exist()) {
                state = IDLE;
            }
        }
        break;
    }
    case REGISTER_TO_STATS_UPDATES: {
        if (obj) {
            auto event_obj = static_cast<listener_general_register_unregister_event *>(obj);
            TASK_LOG(DEBUG) << "REGISTER_TO_STATS_UPDATES event was received";
            database.add_bml_socket(event_obj->sd);
            if (!database.set_bml_stats_update_enable(event_obj->sd, true)) {
                TASK_LOG(DEBUG) << "fail in changing stats_update registration";
            }
            state = LISTENING;
        }
        break;
    }
    case UNREGISTER_TO_STATS_UPDATES: {
        if (obj) {
            auto event_obj = static_cast<listener_general_register_unregister_event *>(obj);
            TASK_LOG(DEBUG) << "UNREGISTER_TO_STATS_UPDATES event was received";

            if (!database.set_bml_stats_update_enable(event_obj->sd, false)) {
                TASK_LOG(DEBUG) << "fail in changing stats_update unregistration";
            }
            if (!database.is_bml_listener_exist()) {
                state = IDLE;
            }
        }
        break;
    }
    case REGISTER_TO_EVENTS_UPDATES: {
        if (obj) {
            auto event_obj = static_cast<listener_general_register_unregister_event *>(obj);
            TASK_LOG(DEBUG) << "REGISTER_TO_EVENTS_UPDATES event was received";
            database.add_bml_socket(event_obj->sd);
            if (!database.set_bml_events_update_enable(event_obj->sd, true)) {
                TASK_LOG(DEBUG) << "fail in changing events_update registration";
            }
            state = LISTENING;
        }
        break;
    }
    case UNREGISTER_TO_EVENTS_UPDATES: {
        if (obj) {
            auto event_obj = static_cast<listener_general_register_unregister_event *>(obj);
            TASK_LOG(DEBUG) << "UNREGISTER_TO_EVENTS_UPDATES event was received";

            if (!database.set_bml_events_update_enable(event_obj->sd, false)) {
                TASK_LOG(DEBUG) << "fail in changing stats_update unregistration";
            }
            if (!database.is_bml_listener_exist()) {
                state = IDLE;
            }
        }
        break;
    }
    case TOPOLOGY_RESPONSE_UPDATE: {
        if (!obj) {
            break;
        }
        auto event_obj = static_cast<topology_response_update_event *>(obj);
        TASK_LOG(DEBUG) << "TOPOLOGY_RESPONSE_UPDATE event was received";
        int sd_idx = 0;
        std::vector<int> topology_response_updates_listeners;
        while ((sd = database.get_bml_socket_at(sd_idx)) !=
               beerocks::net::FileDescriptor::invalid_descriptor) {
            if (database.get_bml_topology_update_enable(sd)) {
                topology_response_updates_listeners.push_back(sd);
            }
            sd_idx++;
        }

        if (topology_response_updates_listeners.empty()) {
            break;
        }

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_TOPOLOGY_RESPONSE>(
                cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
            return;
        }

        if (event_obj->radio_interfaces.size() > beerocks::message::DEV_MAX_RADIOS) {
            LOG(ERROR) << "number of reported interfaces is higher than expected, "
                          "ACTION_BML_TOPOLOGY_RESPONSE will not be sent";
            return;
        }

        size_t i = 0;
        for (auto iface : event_obj->radio_interfaces) {
            auto iface_mac_str                             = tlvf::mac_to_string(iface);
            response->device_data().radios[i].iface_mac    = iface;
            response->device_data().radios[i].iface_status = database.get_radio_state(iface);
            std::copy_n(database.get_radio_iface_name(iface).c_str(),
                        beerocks::message::IFACE_NAME_LENGTH,
                        response->device_data().radios[i].iface_name);
            i++;
        }
        response->device_data().al_mac = event_obj->al_mac;
        response->result()             = true;
        network_map::send_bml_event_to_listeners(database, cmdu_tx,
                                                 topology_response_updates_listeners);
        break;
    }
    default: {
        TASK_LOG(ERROR) << "UNKNOWN event was received";
        break;
    }
    }
}

void bml_task::update_bml_nw_map(std::string mac, bool force_client_disconnect)
{
    int idx = 0;
    std::vector<int> nw_map_updates_listeners;
    int sd;
    while ((sd = database.get_bml_socket_at(idx)) !=
           beerocks::net::FileDescriptor::invalid_descriptor) {
        if (database.get_bml_nw_map_update_enable(sd)) {
            nw_map_updates_listeners.push_back(sd);
        }
        idx++;
    }

    if (!nw_map_updates_listeners.empty()) {

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_NW_MAP_UPDATE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_NW_MAP_UPDATE message!";
            return;
        }

        // Set number of transmitted nodes
        response->node_num() = 1;

        size_t node_size;
        if (database.get_agent(tlvf::mac_from_string(mac))) {
            node_size = sizeof(BML_NODE);
        } else {
            node_size = sizeof(BML_NODE) - sizeof(BML_NODE::N_DATA::N_GW_IRE);
        }

        std::ptrdiff_t size_left = cmdu_tx.getMessageBuffLength() - cmdu_tx.getMessageLength() -
                                   ieee1905_1::tlvEndOfMessage::get_initial_size();

        if (!response->alloc_buffer(node_size)) {
            LOG(ERROR) << "Failed buffer allocation to size=" << int(node_size);
            return;
        }

        uint8_t *p = (uint8_t *)response->buffer(0);

        network_map::fill_bml_node_data(database, mac, p, size_left, force_client_disconnect);

        network_map::send_bml_event_to_listeners(database, cmdu_tx, nw_map_updates_listeners);
    }
}
