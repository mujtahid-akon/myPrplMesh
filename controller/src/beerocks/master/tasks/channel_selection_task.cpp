/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "channel_selection_task.h"
#include "../son_actions.h"
#include "bml_task.h"
#include "ire_network_optimization_task.h"
#include "optimal_path_task.h"

#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_wifi_channel.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

#define FSM_MOVE_STATE(new_state)                                                                  \
    ({                                                                                             \
        TASK_LOG(TRACE) << "FSM: " << s_ar_states[int(fsm_state)] << " --> "                       \
                        << s_ar_states[int(eState::new_state)];                                    \
        fsm_state = eState::new_state;                                                             \
    })

#define FSM_IS_IN_STATE(state) (fsm_state == eState::state)
#define FSM_CURR_STATE_STR s_ar_states[int(fsm_state)]

#define EVENT_STR(event) s_ar_events[int(event)]

const char *channel_selection_task::s_ar_states[] = {FOREACH_STATE(GENERATE_STRING)};

const char *channel_selection_task::s_ar_events[] = {FOREACH_EVENT(GENERATE_STRING)};

channel_selection_task::channel_selection_task(db &database_, ieee1905_1::CmduMessageTx &cmdu_tx_,
                                               task_pool &tasks_)
    : task("channel selection task"), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_)
{
    fsm_state = eState::INIT;
}

// void channel_selection_task::handle_responses_timeout(std::unordered_multimap<std::string, message::eActionOp_CONTROL> timed_out_macs)
// {
//     //DO NOT USE THIS FUNCTION, ONLY USE handle_event FUNCTION
// }

// void channel_selection_task::handle_response(std::string mac, std::shared_ptr<beerocks_header> beerocks_header)
// {
//     //DO NOT USE THIS FUNCTION, ONLY USE handle_event FUNCTION
// }

void channel_selection_task::handle_events_timeout(std::multiset<int> pending_events)
{
    // only one event is expected
    for (std::multiset<int>::iterator it = pending_events.begin(); it != pending_events.end();
         ++it) {
        TASK_LOG(ERROR) << "event " << *it << " timed out on radio " << radio_mac
                        << ", going to idle state -> handle_dead_radio";
        son_actions::handle_dead_radio(radio_mac, true, database, tasks);
        //TODO check state of timeout event
        switch (fsm_state) {
        case eState::WAIT_FOR_RESTRICTED_CHANNEL_RESPONSE: {
            break;
        }
        case eState::WAIT_FOR_ACS_RESPONSE: {
            break;
        }
        case eState::WAIT_FOR_CSA_NOTIFICATION: {
            break;
        }
        default: {
            TASK_LOG(ERROR) << "Unexpected event timeout!";
            break;
        }
        }
    }
    cs_waiting_for_event = eEvent::INVALID_EVENT;
    FSM_MOVE_STATE(GOTO_IDLE);
}

void channel_selection_task::handle_event(int event_type, void *obj)
{
    //TASK_LOG(DEBUG) << "event_type " << event_type << " was received";
    if (obj == nullptr) {
        TASK_LOG(ERROR) << "obj == nullptr";
        return;
    }
    sMacAddr event_hostap_mac = *static_cast<sMacAddr *>(obj);
    if (radio_mac == event_hostap_mac) {
        bool handle_event = false;
        switch (eEvent(event_type)) {
        case eEvent::ACS_RESPONSE_EVENT: {
            if (FSM_IS_IN_STATE(WAIT_FOR_ACS_RESPONSE)) {
                handle_event       = true;
                acs_response_event = static_cast<sAcsResponse_event *>(obj);
                FSM_MOVE_STATE(ON_ACS_RESPONSE);
            } else {
                delete static_cast<sAcsResponse_event *>(obj);
            }
            break;
        }
        case eEvent::CSA_EVENT: {
            if (FSM_IS_IN_STATE(WAIT_FOR_CSA_NOTIFICATION)) {
                handle_event = true;
                csa_event    = static_cast<sCsa_event *>(obj);
                FSM_MOVE_STATE(ON_CSA_NOTIFICATION);
            } else {
                TASK_LOG(DEBUG) << " event: " << EVENT_STR(eEvent(event_type))
                                << " in state: " << FSM_CURR_STATE_STR;
                queue_push_event(eEvent(event_type), static_cast<uint8_t *>(obj));
            }
            break;
        }
        case eEvent::RESTRICTED_CHANNEL_RESPONSE_EVENT: {
            if (FSM_IS_IN_STATE(WAIT_FOR_RESTRICTED_CHANNEL_RESPONSE) ||
                FSM_IS_IN_STATE(WAIT_FOR_FAIL_SAFE_CHANNEL_RESPONSE)) {
                handle_event = true;
                restricted_channel_response_event =
                    static_cast<sRestrictedChannelResponse_event *>(obj);
                FSM_MOVE_STATE(ON_RESTRICTED_FAIL_SAFE_CHANNEL_RESPONSE);
            } else if (FSM_IS_IN_STATE(WAIT_FOR_CLEAR_RESTRICTED_CHANNEL_RESPONSE)) {
                handle_event = true;
                restricted_channel_response_event =
                    static_cast<sRestrictedChannelResponse_event *>(obj);
                FSM_MOVE_STATE(ACTIVATE_SLAVE);
            } else {
                delete static_cast<sRestrictedChannelResponse_event *>(obj);
            }
            break;
        }
        case eEvent::SLAVE_JOINED_EVENT: {
            TASK_LOG(WARNING) << "slave rejoined, event: " << EVENT_STR(eEvent(event_type))
                              << " in state: " << FSM_CURR_STATE_STR;
            queue_clear_mac(tlvf::mac_to_string(event_hostap_mac));
            queue_push_event(eEvent(event_type), static_cast<uint8_t *>(obj));
            break;
        }
        case eEvent::AP_ACTIVITY_IDLE_EVENT: {
            TASK_LOG(DEBUG) << " event: " << EVENT_STR(eEvent(event_type))
                            << " in state: " << FSM_CURR_STATE_STR;
            queue_push_event(eEvent(event_type), static_cast<uint8_t *>(obj));
            break;
        }
        case eEvent::DFS_CHANNEL_AVAILABLE_EVENT: {
            TASK_LOG(DEBUG) << " event: " << EVENT_STR(eEvent(event_type))
                            << " in state: " << FSM_CURR_STATE_STR;
            queue_push_event(eEvent(event_type), static_cast<uint8_t *>(obj));
            break;
        }
        case eEvent::DFS_REENTRY_PENDING_STEERED_CLIENT: {
            // TASK_LOG(DEBUG) << " event: " << EVENT_STR(eEvent(event_type)) << " in state: " << FSM_CURR_STATE_STR;
            queue_push_event(eEvent(event_type), static_cast<uint8_t *>(obj));
            break;
        }
        case eEvent::DFS_CAC_PENDING_HOSTAP_EVENT: {
            // TASK_LOG(DEBUG) << " event: " << EVENT_STR(eEvent(event_type)) << " in state: " << FSM_CURR_STATE_STR;
            queue_push_event(eEvent(event_type), static_cast<uint8_t *>(obj));
            break;
        }
        case eEvent::CAC_COMPLETED_EVENT: {
            TASK_LOG(DEBUG) << " event: " << EVENT_STR(eEvent(event_type))
                            << " in state: " << FSM_CURR_STATE_STR;
            queue_push_event(eEvent(event_type), static_cast<uint8_t *>(obj));
            break;
        }
        case eEvent::CONFIGURED_RESTRICTED_CHANNELS_EVENT: {
            TASK_LOG(DEBUG) << " event: " << EVENT_STR(eEvent(event_type))
                            << " in state: " << FSM_CURR_STATE_STR;
            queue_push_event(eEvent(event_type), static_cast<uint8_t *>(obj));
            break;
        }
        default: {
            break;
        }
        }

        if (!handle_event) {
            //TASK_LOG(ERROR) << "unexpected event: " << EVENT_STR(eEvent(event_type)) << " in state: " << FSM_CURR_STATE_STR;
        } else {
            cs_waiting_for_event = eEvent::INVALID_EVENT;
        }
    } else {
        if (cs_waiting_for_event != eEvent::INVALID_EVENT &&
            eEvent(event_type) == cs_waiting_for_event) {
            TASK_LOG(DEBUG) << "event : " << EVENT_STR(eEvent(event_type))
                            << " in state: " << FSM_CURR_STATE_STR
                            << " for mac : " << event_hostap_mac << " calling wait_for_event ";
            wait_for_event((int)cs_waiting_for_event);
        }
        queue_push_event(eEvent(event_type), (uint8_t *)obj);
    }
}
void channel_selection_task::clear_events()
{
    slave_joined_event                = nullptr;
    hostap_channel_request_event      = nullptr;
    restricted_channel_response_event = nullptr;
    acs_response_event                = nullptr;
    csa_event                         = nullptr;
}

void channel_selection_task::work()
{
    switch (fsm_state) {
    case eState::INIT: {
        database.assign_channel_selection_task_id(id);
        assign_config_global_restricted_channel_to_db();
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::GOTO_IDLE: {
        radio_mac = beerocks::net::network_utils::ZERO_MAC;
        ccl.clear();
        if (event_handle_in_progress) {
            event_handle_in_progress = false;
            queue_pop_event();
        }
        clear_events();
        FSM_MOVE_STATE(IDLE);
        break;
    }
    case eState::IDLE: {
        if (queue_pending_event_count() > 0) {
            eEvent event_type;
            auto event_obj = queue_get_event(event_type);
            //TASK_LOG(DEBUG) << "event_obj = " << int(event_obj) ;
            memcpy(&radio_mac, event_obj, sizeof(radio_mac));
            event_handle_in_progress = true;
            switch (event_type) {
            case eEvent::SLAVE_JOINED_EVENT: {
                slave_joined_event = reinterpret_cast<sSlaveJoined_event *>(event_obj);
                FSM_MOVE_STATE(ON_SLAVE_JOINED);
                break;
            }
            case eEvent::CSA_EVENT: {
                csa_event = reinterpret_cast<sCsa_event *>(event_obj);
                FSM_MOVE_STATE(ON_CSA_UNEXPECTED_NOTIFICATION);
                break;
            }
            case eEvent::HOSTAP_CHANNEL_REQUEST_EVENT: {
                hostap_channel_request_event =
                    reinterpret_cast<sHostapChannelRequest_event *>(event_obj);
                FSM_MOVE_STATE(ON_HOSTAP_CHANNEL_REQUEST);
                break;
            }
            case eEvent::DELETED_EVENT: {
                TASK_LOG(DEBUG) << "DELETED_EVENT for radio mac = " << radio_mac;
                FSM_MOVE_STATE(GOTO_IDLE);
                break;
            }
            case eEvent::AP_ACTIVITY_IDLE_EVENT: {
                ap_activity_idle = reinterpret_cast<sApActivityIdle_event *>(event_obj);
                FSM_MOVE_STATE(GOTO_IDLE);
                break;
            }
            case eEvent::DFS_REENTRY_PENDING_STEERED_CLIENT: {
                dfs_reentry_pending_steered_clients =
                    reinterpret_cast<sDfsReEntrySampleSteeredClients_event *>(event_obj);
                if (reentry_steered_client_check()) {
                    TASK_LOG(DEBUG) << "condition true radio mac = " << radio_mac;
                    FSM_MOVE_STATE(SEND_ACS);
                    break;
                }
                //pending cases stays in IDLE while condition did not meet or timeout expired.
                //TASK_LOG(DEBUG) << "condition false, radio mac = " << radio_mac;
                if (!dfs_reentry_pending_steered_clients->timeout_expired) {
                    queue_pop_event();
                } else {
                    FSM_MOVE_STATE(GOTO_IDLE);
                }

                break;
            }
            case eEvent::DFS_CAC_PENDING_HOSTAP_EVENT: {
                dfs_cac_pending_hostap = reinterpret_cast<sDfsCacPendinghostap_event *>(event_obj);
                auto cac_pending       = cac_pending_hostap_check();
                if (dfs_cac_pending_hostap->timeout_expired) {
                    TASK_LOG(DEBUG) << "handle_dead_radio, radio mac = " << radio_mac;
                    son_actions::handle_dead_radio(radio_mac, true, database, tasks);
                }
                if (cac_pending) {
                    queue_pop_event();
                } else {
                    FSM_MOVE_STATE(GOTO_IDLE);
                }
                break;
            }

            case eEvent::CAC_COMPLETED_EVENT: {
                if (!hostaps_cac_pending.empty()) {
                    cac_completed_event = reinterpret_cast<sCacCompleted_event *>(event_obj);
                    TASK_LOG(DEBUG) << "CAC_COMPLETED_EVENT for radio mac = " << radio_mac;
                    FSM_MOVE_STATE(ON_CAC_COMPLETED_NOTIFICATION);
                    break;
                }
                TASK_LOG(DEBUG) << "CAC_COMPLETED_EVENT for radio mac = " << radio_mac
                                << "no pending hostaps cac ignored";
                FSM_MOVE_STATE(GOTO_IDLE);
                break;
            }
            case eEvent::DFS_CHANNEL_AVAILABLE_EVENT: {
                dfs_channel_available = reinterpret_cast<sDfsChannelAvailable_event *>(event_obj);
                TASK_LOG(DEBUG) << "DFS_CHANNEL_AVAILABLE_EVENT for radio mac = " << radio_mac;
                FSM_MOVE_STATE(ON_DFS_CHANNEL_AVAILABLE);
                break;
            }
            case eEvent::CONFIGURED_RESTRICTED_CHANNELS_EVENT: {
                configured_restricted_channels =
                    reinterpret_cast<sConfiguredRestrictedChannels_event *>(event_obj);
                TASK_LOG(DEBUG) << "CONFIGURED_RESTRICTED_CHANNELS_EVENT for radio mac = "
                                << radio_mac;
                FSM_MOVE_STATE(ON_CONFIGURED_RESTRICTED_CHANNELS);
                break;
            }
            case eEvent::INVALID_EVENT: {
                TASK_LOG(ERROR) << "only pending event in queue stay in IDLE "
                                << EVENT_STR(eEvent(event_type))
                                << " in state: " << FSM_CURR_STATE_STR
                                << " from radio: " << radio_mac;
                break;
            }
            default: {
                TASK_LOG(ERROR) << "unexpected event: " << EVENT_STR(eEvent(event_type))
                                << " in state: " << FSM_CURR_STATE_STR
                                << " from radio: " << radio_mac;
                FSM_MOVE_STATE(GOTO_IDLE);
            }
            }
        }
        break;
    }
    case eState::ON_SLAVE_JOINED: {
        auto vht_center_frequency = slave_joined_event->cs_params.vht_center_frequency;
        auto freq                 = wireless_utils::channel_to_freq(
            slave_joined_event->channel,
            son::wireless_utils::which_freq_type(vht_center_frequency));

        auto channel_ext_above_primary = slave_joined_event->cs_params.channel_ext_above_primary;

        auto channel_ext_above_secondary = (freq < vht_center_frequency) ? true : false;

        beerocks::WifiChannel wifi_channel = beerocks::WifiChannel(
            slave_joined_event->channel, vht_center_frequency,
            static_cast<beerocks::eWiFiBandwidth>(slave_joined_event->cs_params.bandwidth),
            channel_ext_above_secondary);

        if (!database.set_radio_wifi_channel(radio_mac, wifi_channel)) {
            TASK_LOG(ERROR) << "set radio wifi channel failed, mac=" << radio_mac;
        } else {
            // update bml listeners
            bml_task::connection_change_event new_event;
            new_event.mac = tlvf::mac_to_string(database.get_radio_parent_agent(radio_mac));
            tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
            TASK_LOG(DEBUG) << "BML, sending CONNECTION_CHANGE for mac " << new_event.mac;
        }
        TASK_LOG(DEBUG) << "vht_center_frequency = " << uint16_t(vht_center_frequency);

        TASK_LOG(DEBUG) << "hostap_mac = " << slave_joined_event->hostap_mac
                        << " channel_ext_above_primary = " << int(channel_ext_above_primary);
        FSM_MOVE_STATE(ON_HOSTAP_CHANNEL_REQUEST);
        break;
    }
    case eState::ON_HOSTAP_CHANNEL_REQUEST: {
        channel_switch_required = false;
        get_hostap_params();

        auto radio = database.get_radio_by_uid(radio_mac);
        if (!radio) {
            TASK_LOG(ERROR) << "radio " << radio_mac << " not found";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        if (!radio->is_acs_enabled) {
            if (wireless_utils::is_dfs_channel(hostap_params.channel)) {
                TASK_LOG(INFO) << "not waiting for CAC completed on static DFS channel "
                                  "configuration, setting CAC completed flag to true";
                database.set_radio_cac_completed(radio_mac, true);
            }
        }

        ccl.clear();
        ccl_fill_supported_channels();
        ccl_fill_active_channels();

        //By default set acs param for acs start
        channel_switch_request.channel              = 0;
        channel_switch_request.bandwidth            = beerocks::BANDWIDTH_20;
        channel_switch_request.vht_center_frequency = 0;

        FSM_MOVE_STATE(ACTIVATE_SLAVE);
        break;
    }
    case eState::COMPUTE_IRE_CANDIDATE_CHANNELS: {

        bool eth_bh = !hostap_params.backhaul_is_wireless;

        TASK_LOG(DEBUG)
            << "eth_bh " << int(eth_bh) << " hostap_params.backhaul_freq_type = "
            << beerocks::utils::convert_frequency_type_to_string(hostap_params.freq_type)
            << " hostap_params.freq_type = "
            << beerocks::utils::convert_frequency_type_to_string(hostap_params.freq_type)
            << " hostap_params.channel = " << int();

        if ((!eth_bh) && (hostap_params.backhaul_freq_type == beerocks::FREQ_5G) &&
            (hostap_params.freq_type == beerocks::FREQ_5G)) {
            // 5G / 5G concurrent, remove BH subband
            ccl_remove_5G_subband(hostap_params.backhaul_subband);
        }

        bool has_free_channel = false;
        //General selection logic
        switch (hostap_params.freq_type) {
        case beerocks::FREQ_24G:
            has_free_channel = ccl_has_free_channels_2G();
            break;
        case beerocks::FREQ_5G:
            has_free_channel = ccl_has_free_channels_5G(beerocks::BANDWIDTH_160);
            break;
        case beerocks::FREQ_6G:
            TASK_LOG(ERROR) << "6Ghz is not supported";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        default:
            LOG(ERROR) << "Invalid frequency type "
                       << beerocks::utils::convert_frequency_type_to_string(
                              hostap_params.freq_type);
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        if (has_free_channel) {
            // Send restricted channles and send ACS
            FSM_MOVE_STATE(SEND_RESTRICTED_FAIL_SAFE_CHANNEL);
        } else {
            if (hostap_params.freq_type == eFreqType::FREQ_5G) {
                if (!ccl_fill_channel_switch_request_with_least_used_channel()) {
                    FSM_MOVE_STATE(GOTO_IDLE);
                    break;
                } else {
                    channel_switch_required = true;
                }
            } else {
                //avoid sending restrected channels when channels are not free
                FSM_MOVE_STATE(SEND_ACS);
                break;
            }
            FSM_MOVE_STATE(SEND_RESTRICTED_CHANNEL_FOR_USED_BANDS);
        }
        break;
    }
    case eState::SEND_FAIL_SAFE_CHANNEL: {

        // CMDU Messages
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST>(
            cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }
        request->params().failsafe_channel =
            wireless_utils::freq_to_channel(database.config.fail_safe_5G_frequency);
        request->params().failsafe_channel_bandwidth = database.config.fail_safe_5G_bw;
        request->params().vht_center_frequency       = database.config.fail_safe_5G_vht_frequency;
        memset(request->params().restricted_channels, 0, message::RESTRICTED_CHANNEL_LENGTH);
        auto agent_mac = database.get_radio_parent_agent(radio_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database,
                                        tlvf::mac_to_string(radio_mac));

        set_events_timeout(RESTRICTED_CHANNEL_RESPONSE_WAIT_TIME);
        cs_wait_for_event(eEvent::RESTRICTED_CHANNEL_RESPONSE_EVENT);

        FSM_MOVE_STATE(WAIT_FOR_FAIL_SAFE_CHANNEL_RESPONSE);
        break;
    }
    case eState::SEND_RESTRICTED_FAIL_SAFE_CHANNEL: {

        // CMDU Messages
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST>(
            cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }
        request->params().failsafe_channel =
            hostap_params.freq_type == beerocks::FREQ_5G
                ? wireless_utils::freq_to_channel(database.config.fail_safe_5G_frequency)
                : 0;
        request->params().failsafe_channel_bandwidth =
            (hostap_params.freq_type == beerocks::FREQ_5G) ? database.config.fail_safe_5G_bw : 0;
        request->params().vht_center_frequency = (hostap_params.freq_type == beerocks::FREQ_5G)
                                                     ? database.config.fail_safe_5G_vht_frequency
                                                     : 0;
        memset(request->params().restricted_channels, 0, message::RESTRICTED_CHANNEL_LENGTH);
        fill_restricted_channels_from_ccl_and_supported(request->params().restricted_channels);
        TASK_LOG(INFO) << "***** send_restricted_channel to hostap: " << radio_mac;
        for (auto i = 0; i < message::RESTRICTED_CHANNEL_LENGTH; i++) {
            if (!request->params().restricted_channels[i]) {
                continue;
            }
            TASK_LOG(INFO) << " restricted_channels: "
                           << int(request->params().restricted_channels[i]);
        }
        auto agent_mac = database.get_radio_parent_agent(radio_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database,
                                        tlvf::mac_to_string(radio_mac));

        set_events_timeout(RESTRICTED_CHANNEL_RESPONSE_WAIT_TIME);
        cs_wait_for_event(eEvent::RESTRICTED_CHANNEL_RESPONSE_EVENT);

        FSM_MOVE_STATE(WAIT_FOR_RESTRICTED_CHANNEL_RESPONSE);
        break;
    }
    case eState::SEND_CLEAR_RESTRICTED_CHANNEL: {

        // CMDU Messages
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST>(
            cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }
        request->params().failsafe_channel           = 0;
        request->params().failsafe_channel_bandwidth = 0;
        memset(request->params().restricted_channels, 0, message::RESTRICTED_CHANNEL_LENGTH);
        TASK_LOG(INFO) << "***** clear 2.4G restricted channel for " << radio_mac;
        auto agent_mac = database.get_radio_parent_agent(radio_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database,
                                        tlvf::mac_to_string(radio_mac));

        set_events_timeout(RESTRICTED_CHANNEL_RESPONSE_WAIT_TIME);
        cs_wait_for_event(eEvent::RESTRICTED_CHANNEL_RESPONSE_EVENT);

        FSM_MOVE_STATE(WAIT_FOR_CLEAR_RESTRICTED_CHANNEL_RESPONSE);
        break;
    }
    case eState::SEND_RESTRICTED_CHANNEL_FOR_USED_BANDS: {

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST>(
            cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }
        request->params().failsafe_channel =
            (hostap_params.freq_type == beerocks::FREQ_5G)
                ? wireless_utils::freq_to_channel(database.config.fail_safe_5G_frequency)
                : 0;
        ;
        request->params().failsafe_channel_bandwidth =
            (hostap_params.freq_type == beerocks::FREQ_5G) ? database.config.fail_safe_5G_bw : 0;
        request->params().vht_center_frequency = (hostap_params.freq_type == beerocks::FREQ_5G)
                                                     ? database.config.fail_safe_5G_vht_frequency
                                                     : 0;
        memset(request->params().restricted_channels, 0, message::RESTRICTED_CHANNEL_LENGTH);
        fill_restricted_channels_from_ccl_busy_bands(request->params().restricted_channels);
        TASK_LOG(INFO) << "***** send_restricted_channel to hostap: " << radio_mac;
        for (auto i = 0; i < message::RESTRICTED_CHANNEL_LENGTH; i++) {
            if (request->params().restricted_channels[i] == 0) {
                continue;
            }
            TASK_LOG(INFO) << " restricted_channels: "
                           << int(request->params().restricted_channels[i]);
        }
        auto agent_mac = database.get_radio_parent_agent(radio_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database,
                                        tlvf::mac_to_string(radio_mac));

        set_events_timeout(RESTRICTED_CHANNEL_RESPONSE_WAIT_TIME);
        cs_wait_for_event(eEvent::RESTRICTED_CHANNEL_RESPONSE_EVENT);

        FSM_MOVE_STATE(WAIT_FOR_RESTRICTED_CHANNEL_RESPONSE);
        break;
    }
    case eState::WAIT_FOR_RESTRICTED_CHANNEL_RESPONSE: {
        TASK_LOG(ERROR) << "This should not happen!";
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::WAIT_FOR_CLEAR_RESTRICTED_CHANNEL_RESPONSE: {
        TASK_LOG(ERROR) << "This should not happen!";
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::ON_RESTRICTED_FAIL_SAFE_CHANNEL_RESPONSE: {
        if (restricted_channel_response_event->success) {
            FSM_MOVE_STATE(ACTIVATE_SLAVE);
        } else {
            FSM_MOVE_STATE(GOTO_IDLE);
        }
        break;
    }
    case eState::SEND_ACS: {

        // CMDU Message
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }
        request->cs_params().channel              = 0;
        request->cs_params().bandwidth            = beerocks::BANDWIDTH_20;
        request->cs_params().vht_center_frequency = 0;
        auto agent_mac                            = database.get_radio_parent_agent(radio_mac);
        if (!son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database,
                                             tlvf::mac_to_string(radio_mac))) {
            LOG(ERROR) << "Send cmdu failed!";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        set_events_timeout(ACS_RESPONSE_WAIT_TIME);
        cs_wait_for_event(eEvent::ACS_RESPONSE_EVENT);

        // update bml listeners
        bml_task::acs_start_event acs_start_event;
        acs_start_event.hostap_mac = tlvf::mac_to_string(radio_mac);
        tasks.push_event(database.get_bml_task_id(), bml_task::ACS_START_EVENT_AVAILABLE,
                         &acs_start_event);

        FSM_MOVE_STATE(WAIT_FOR_ACS_RESPONSE);
        break;
    }
    case eState::SEND_CHANNEL_SWITCH: {
        auto agent_mac = database.get_radio_parent_agent(radio_mac);

        // CMDU Message
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }

        request->cs_params() = channel_switch_request;
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database,
                                        tlvf::mac_to_string(radio_mac));

        set_events_timeout(ACS_RESPONSE_WAIT_TIME);
        cs_wait_for_event(eEvent::CSA_EVENT);

        FSM_MOVE_STATE(WAIT_FOR_CSA_NOTIFICATION);
        break;
    }
    case eState::WAIT_FOR_ACS_RESPONSE: {
        TASK_LOG(ERROR) << "This should not happen!";
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::ON_ACS_RESPONSE: {

        cs_wait_for_event(eEvent::CSA_EVENT);
        set_events_timeout(CSA_NOTIFICATION_RESPONSE_WAIT_TIME);

        FSM_MOVE_STATE(WAIT_FOR_CSA_NOTIFICATION);
        break;
    }
    case eState::WAIT_FOR_CSA_NOTIFICATION: {
        TASK_LOG(ERROR) << "This should not happen!";
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::ON_CSA_NOTIFICATION: {
        TASK_LOG(INFO) << "CSA for hostap = " << radio_mac
                       << " bandwidth = " << int(csa_event->cs_params.bandwidth)
                       << " channel = " << int(csa_event->cs_params.channel)
                       << " channel_ext_above_primary = "
                       << int(csa_event->cs_params.channel_ext_above_primary)
                       << " vht_center_frequency = "
                       << uint16_t(csa_event->cs_params.vht_center_frequency);

        beerocks::WifiChannel wifi_channel = beerocks::WifiChannel(
            csa_event->cs_params.channel, csa_event->cs_params.vht_center_frequency,
            static_cast<beerocks::eWiFiBandwidth>(csa_event->cs_params.bandwidth),
            csa_event->cs_params.channel_ext_above_primary > 0 ? true : false);

        if (!database.set_radio_wifi_channel(radio_mac, wifi_channel)) {
            TASK_LOG(ERROR) << "set radio wifi channel failed, mac=" << radio_mac;
        }

        // update bml listeners
        bml_task::csa_notification_event csa_notification_event;
        csa_notification_event.hostap_mac = tlvf::mac_to_string(radio_mac);
        csa_notification_event.bandwidth  = int(csa_event->cs_params.bandwidth);
        csa_notification_event.channel    = int(csa_event->cs_params.channel);
        csa_notification_event.channel_ext_above_primary =
            int(csa_event->cs_params.channel_ext_above_primary);
        csa_notification_event.vht_center_frequency =
            uint16_t(csa_event->cs_params.vht_center_frequency);
        tasks.push_event(database.get_bml_task_id(), bml_task::CSA_NOTIFICATION_EVENT_AVAILABLE,
                         &csa_notification_event);
        FSM_MOVE_STATE(ACTIVATE_SLAVE);
        break;
    }
    case eState::ACTIVATE_SLAVE: {
        if (!son_actions::set_radio_active(database, tasks, tlvf::mac_to_string(radio_mac), true)) {
            TASK_LOG(ERROR) << "set radio active failed, mac=" << radio_mac;
        }
        //the connected looks irelevant for the hostap - adding this line so it would appear on network map
        if (!database.set_radio_state(tlvf::mac_to_string(radio_mac), beerocks::STATE_CONNECTED)) {
            TASK_LOG(ERROR) << "set radio state failed, mac=" << radio_mac;
        }
        if (database.settings_ire_roaming()) {
            TASK_LOG(DEBUG) << "add ire_network_optimization_task";
            auto new_task = std::make_shared<ire_network_optimization_task>(
                database, cmdu_tx, tasks, "hostapd_tx_on_ack - ire_network_optimization");
            tasks.add_task(new_task);
        }

        auto radio = database.get_radio_by_uid(radio_mac);
        if (!radio) {
            TASK_LOG(ERROR) << "radio " << radio_mac << " not found";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        if (!radio->is_acs_enabled) {
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        if (csa_event) {
            auto freq_type =
                son::wireless_utils::which_freq_type(csa_event->cs_params.vht_center_frequency);
            if (freq_type == beerocks::FREQ_5G) {
                TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " csa_event != null";
                if (csa_event->cs_params.is_dfs_channel) {
                    wait_for_cac_completed(csa_event->cs_params.channel,
                                           csa_event->cs_params.bandwidth);
                    if (database.get_radio_on_dfs_reentry(radio_mac)) {
                        TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " DFS reentry flow";
                        if (database.get_radio_cac_completed(radio_mac)) {
                            TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                                            << " was on reentry back on dfs channel";
                            database.set_radio_on_dfs_reentry(radio_mac, false);
                            FSM_MOVE_STATE(STEER_STA_BACK_AFTER_DFS_REENTRY);
                        } else {
                            TASK_LOG(DEBUG)
                                << "radio_mac - " << radio_mac << " cac completed pending";
                            FSM_MOVE_STATE(GOTO_IDLE);
                        }
                        break;
                    }
                } else {
                    if (database.get_radio_on_dfs_reentry(radio_mac)) {
                        TASK_LOG(DEBUG)
                            << "radio_mac - " << radio_mac << " was on reentry back on dfs channel";
                        database.set_radio_on_dfs_reentry(radio_mac, false);
                        FSM_MOVE_STATE(STEER_STA_BACK_AFTER_DFS_REENTRY);
                        break;
                    }
                    TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " non DFS not a reentry flow";
                }
            } else if (freq_type == beerocks::FREQ_24G) {
                TASK_LOG(DEBUG) << "2GHz AP ignore";
            } else if (freq_type == beerocks::FREQ_6G) {
                TASK_LOG(DEBUG) << "6GHz AP ignore";
            } else {
                TASK_LOG(ERROR) << "Invalid frequency type "
                                << beerocks::utils::convert_frequency_type_to_string(freq_type);
            }
        } else {
            // CAC completed will not arrive in passive mode, so set the indication to 'completed'
            database.set_radio_cac_completed(radio_mac, true);
        }
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::ON_CAC_COMPLETED_NOTIFICATION: {

        auto it_cac = hostaps_cac_pending.find(tlvf::mac_to_string(radio_mac));

        if (it_cac != std::end(hostaps_cac_pending)) {
            database.set_radio_cac_completed(tlvf::mac_from_string(it_cac->first), true);
            it_cac = hostaps_cac_pending.erase(it_cac);
            TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                            << " cac completed - found in pending cac - erasing, update DB";

            // update bml listeners
            bml_task::cac_status_changed_notification_event cac_status_changed_event;
            cac_status_changed_event.hostap_mac    = tlvf::mac_to_string(radio_mac);
            cac_status_changed_event.cac_completed = 1;
            tasks.push_event(database.get_bml_task_id(),
                             bml_task::CAC_STATUS_CHANGED_NOTIFICATION_EVENT_AVAILABLE,
                             &cac_status_changed_event);
            //optimal path for all non dfs reentry clients
            run_optimal_path_for_connected_clients();

            if (database.get_radio_on_dfs_reentry(radio_mac)) {
                TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                                << " was on reentry back on dfs channel";
                database.set_radio_on_dfs_reentry(radio_mac, false);
                //optimal path for all non dfs reentry clients
                run_optimal_path_for_connected_clients();
                FSM_MOVE_STATE(STEER_STA_BACK_AFTER_DFS_REENTRY);
                break;
            }
        } else {
            TASK_LOG(ERROR) << "radio_mac - " << radio_mac
                            << " cac completed - not found - not suppose to happen ignore";
        }

        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::ON_DFS_CHANNEL_AVAILABLE: {
        TASK_LOG(INFO) << "radio_mac - " << radio_mac
                       << " channel available - clear radar affected";
        auto vec_channels = wireless_utils::get_5g_20MHz_channels(
            beerocks::eWiFiBandwidth(dfs_channel_available->params.bandwidth),
            dfs_channel_available->params.vht_center_frequency);
        database.set_supported_channel_radar_affected(radio_mac, vec_channels, false);
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::ON_CSA_UNEXPECTED_NOTIFICATION: {
        //updating ap and children nodes.
        TASK_LOG(INFO) << "UNEXPECTED CSA event";
        TASK_LOG(INFO) << "CSA for radio = " << radio_mac
                       << " bandwidth = " << int(csa_event->cs_params.bandwidth)
                       << " channel = " << int(csa_event->cs_params.channel)
                       << " channel_ext_above_primary = "
                       << int(csa_event->cs_params.channel_ext_above_primary);
        auto freq = wireless_utils::channel_to_freq(
            csa_event->cs_params.channel,
            wireless_utils::which_freq_type(csa_event->cs_params.vht_center_frequency));

        auto prev_wifi_channel = database.get_radio_wifi_channel(radio_mac);
        if (prev_wifi_channel.is_empty()) {
            LOG(WARNING) << "empty wifi channel of " << tlvf::mac_to_string(radio_mac) << " in DB";
        }

        auto prev_vht_center_frequency = prev_wifi_channel.get_center_frequency();
        auto prev_channel              = prev_wifi_channel.get_channel();
        auto prev_bandwidth            = prev_wifi_channel.get_bandwidth();
        auto channel_ext_above_secondary =
            (freq < csa_event->cs_params.vht_center_frequency) ? true : false;

        beerocks::WifiChannel wifi_channel = beerocks::WifiChannel(
            csa_event->cs_params.channel, csa_event->cs_params.vht_center_frequency,
            static_cast<beerocks::eWiFiBandwidth>(csa_event->cs_params.bandwidth),
            channel_ext_above_secondary);

        if (!database.set_radio_wifi_channel(radio_mac, wifi_channel)) {
            TASK_LOG(ERROR) << "set radio wifi channel failed, mac=" << radio_mac;
        }

        // update bml listeners
        bml_task::csa_notification_event csa_notification_event;
        csa_notification_event.hostap_mac = tlvf::mac_to_string(radio_mac);
        csa_notification_event.bandwidth  = int(csa_event->cs_params.bandwidth);
        csa_notification_event.channel    = int(csa_event->cs_params.channel);
        csa_notification_event.channel_ext_above_primary =
            int(csa_event->cs_params.channel_ext_above_primary);
        csa_notification_event.vht_center_frequency =
            uint16_t(csa_event->cs_params.vht_center_frequency);
        tasks.push_event(database.get_bml_task_id(), bml_task::CSA_NOTIFICATION_EVENT_AVAILABLE,
                         &csa_notification_event);

        auto radio = database.get_radio_by_uid(radio_mac);
        if (!radio) {
            TASK_LOG(ERROR) << "radio " << radio_mac << " not found";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        if (!radio->is_acs_enabled) {
            TASK_LOG(DEBUG) << " vht_center_frequency";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        TASK_LOG(DEBUG) << " vht_center_frequency = "
                        << uint16_t(csa_event->cs_params.vht_center_frequency)
                        << " csa_event->cs_params.switch_reason "
                        << int(csa_event->cs_params.switch_reason);
        if (csa_event->cs_params.switch_reason == beerocks::CH_SWITCH_REASON_RADAR) {
            database.set_radar_hit_stats(radio_mac, prev_channel, prev_bandwidth, false);
            TASK_LOG(DEBUG)
                << "radio_mac - " << radio_mac
                << " csa - reason radar, moved to fail safe , update channel radar affected";
            auto vec_channels = wireless_utils::calc_5g_20MHz_subband_channels(
                prev_bandwidth, prev_vht_center_frequency,
                beerocks::eWiFiBandwidth(csa_event->cs_params.bandwidth),
                csa_event->cs_params.vht_center_frequency);

            auto freq_type =
                son::wireless_utils::which_freq_type(csa_event->cs_params.vht_center_frequency);
            if (freq_type != eFreqType::FREQ_5G) {
                LOG(ERROR) << "the freq type "
                           << beerocks::utils::convert_frequency_type_to_string(freq_type)
                           << " should be 5G";
            }
            database.set_supported_channel_radar_affected(radio_mac, vec_channels, true);
            FSM_MOVE_STATE(ACTIVATE_SLAVE);
            break;
        }

        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::ON_FAIL_SAFE_CHANNEL: {
        if (!database.settings_dfs_reentry()) {
            LOG(DEBUG) << "DFS reentry feature is not enabled";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }
        if (database.get_radio_on_dfs_reentry(radio_mac)) {
            LOG(DEBUG) << "radio " << radio_mac << "is already on dfs reentry";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        if (!database.settings_client_band_steering()) {
            auto clients = database.get_stations_on_radio(radio_mac, beerocks::TYPE_CLIENT);
            auto it =
                std::find_if(clients.begin(), clients.end(), [&](const std::string &client_mac) {
                    auto client_state = (uint8_t)database.get_sta_state(client_mac);
                    return client_state >= eNodeState::STATE_CONNECTING &&
                           client_state <= STATE_CONNECTED;
                });

            if (it != clients.end()) {
                LOG(DEBUG) << "band steering feature is not enabled and client are connected to "
                              "radio_mac="
                           << radio_mac << ", can't run DFS reentry";
                FSM_MOVE_STATE(GOTO_IDLE);
                break;
            }
        }

        ccl_fill_affected_supported_channels();
        auto has_unaffected_channels = ccl_has_free_dfs_channels(beerocks::BANDWIDTH_160);
        auto ap_idle_mode = (database.get_radio_activity_mode(radio_mac) == AP_IDLE_MODE);
        TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                        << " has_unaffected_channels = " << int(has_unaffected_channels)
                        << " ap_idle_mode = " << int(ap_idle_mode);
        if (has_unaffected_channels && ap_idle_mode) {
            TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                            << " found at list one 80 Mhz un-affected band  ";
            FSM_MOVE_STATE(STEER_STA_BEFORE_DFS_REENTRY);
            database.set_radio_on_dfs_reentry(radio_mac, true);
            break;
        }
        TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                        << " ap activity mode is active or all dfs channels are radar affected";
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::STEER_STA_BEFORE_DFS_REENTRY: {
        database.clear_radio_dfs_reentry_clients(radio_mac);

        auto set_reentry_clients = database.get_stations_on_radio(radio_mac, STATE_CONNECTED);
        if (set_reentry_clients.empty()) {
            TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                            << " no client connected to reentry hostap ";
            FSM_MOVE_STATE(SEND_ACS);
            break;
        }
        TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                        << " client connected to reentry hostap steering to 2.4 hostap ";
        database.set_radio_dfs_reentry_clients(radio_mac, set_reentry_clients);
        auto hostaps_sibling = database.get_radio_siblings(radio_mac);
        auto hostap_mac_2g   = std::find_if(
            std::begin(hostaps_sibling), std::end(hostaps_sibling),
            [&](std::string hostap_sibling) {
                auto hostap_sibling_wifi_channel =
                    database.get_radio_wifi_channel(tlvf::mac_from_string(hostap_sibling));
                if (hostap_sibling_wifi_channel.is_empty()) {
                    LOG(WARNING) << "empty wifi channel of " << hostap_sibling << " in DB";
                }
                return (hostap_sibling_wifi_channel.get_freq_type() == eFreqType::FREQ_24G &&
                        database.is_radio_active(tlvf::mac_from_string(hostap_sibling)));
            });
        if (hostap_mac_2g == std::end(hostaps_sibling)) {
            TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                            << " no 2.4G hostap found - band not active ";
            FSM_MOVE_STATE(GOTO_IDLE);
            break;
        }

        std::for_each(std::begin(set_reentry_clients), std::end(set_reentry_clients),
                      [&](std::string set_reentry_client) {
                          auto disassoc_imminent = true;
                          int disassoc_timer_ms  = DISASSOC_STEER_TIMER_MS;
                          auto steer_restricted  = true;
                          std::string triggered_by{" Before DFS Reentry "};
                          TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " steer sta "
                                          << set_reentry_client << " to 2.4G - " << *hostap_mac_2g;
                          son_actions::steer_sta(database, cmdu_tx, tasks, set_reentry_client,
                                                 *hostap_mac_2g, triggered_by, std::string(),
                                                 disassoc_imminent, disassoc_timer_ms,
                                                 steer_restricted);
                      });

        FSM_MOVE_STATE(GOTO_IDLE);

        //inject event for sample client on reentry hostap after steering them to 2.4G hostap
        auto new_event             = new sDfsReEntrySampleSteeredClients_event;
        new_event->hostap_mac      = radio_mac;
        new_event->timestamp       = std::chrono::steady_clock::now();
        new_event->timeout_expired = false;
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::DFS_REENTRY_PENDING_STEERED_CLIENT,
                         (void *)new_event);

        break;
    }
    case eState::STEER_STA_BACK_AFTER_DFS_REENTRY: {
        auto set_reentry_clients = database.get_radio_dfs_reentry_clients(radio_mac);
        if (set_reentry_clients.empty()) {
            TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                            << "no dfs_reentry_clients - not supposed to happen!!!";
        }
        std::for_each(std::begin(set_reentry_clients), std::end(set_reentry_clients),
                      [&](std::string set_reentry_client) {
                          auto disassoc_imminent = true;
                          int disassoc_timer_ms  = DISASSOC_STEER_TIMER_MS;
                          std::string triggered_by{" After DFS Rentry [imminent] "};
                          TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " steer sta "
                                          << set_reentry_client << " back to - " << radio_mac;
                          son_actions::steer_sta(database, cmdu_tx, tasks, set_reentry_client,
                                                 tlvf::mac_to_string(radio_mac), triggered_by,
                                                 std::string(), disassoc_imminent,
                                                 disassoc_timer_ms);
                      });
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    case eState::ON_CONFIGURED_RESTRICTED_CHANNELS: {
        //preperation for restricted channel change on run time.
        //TODO
        // 1. send new restrected channel to slave.
        // 2. when ap on IDLE mode steer all connected stations to 2.4G ap.
        // 3. trigger ACS.
        TASK_LOG(DEBUG) << "do nothing for now!!! ";
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }

    default: {
        TASK_LOG(ERROR) << "Unknown channel selection state: " << int(fsm_state);
        FSM_MOVE_STATE(GOTO_IDLE);
        break;
    }
    }
}

void channel_selection_task::queue_push_event(eEvent event_type, uint8_t *event_obj)
{
    event_queue.push_back(std::make_pair(event_type, event_obj));
}

uint8_t *channel_selection_task::queue_get_event(eEvent &event_type)
{
    if (event_queue.size() > 0) {
        auto it    = event_queue.front();
        event_type = it.first;
        return it.second;
    } else {
        event_type = eEvent::INVALID_EVENT;
        return nullptr;
    }
}

void channel_selection_task::queue_pop_event()
{
    if (event_queue.size() > 0) {
        auto it = event_queue.front();
        if (it.second != nullptr) {
            delete[] it.second;
        }
        //TASK_LOG(DEBUG) << "******event_queue.pop()*********";
        event_queue.pop_front();
    }
}

int channel_selection_task::queue_pending_event_count() { return event_queue.size(); }

void channel_selection_task::queue_clear()
{
    TASK_LOG(DEBUG) << "queue_clear()";
    while (event_queue.size() > 0) {
        queue_pop_event();
    }
}

void channel_selection_task::queue_clear_mac(const std::string &mac)
{
    TASK_LOG(DEBUG) << "queue_clear_mac(), mac = " << mac;
    for (auto it = event_queue.begin(); it != event_queue.end(); it++) {
        auto event_mac = *reinterpret_cast<sMacAddr *>(it->second);
        if (event_mac == tlvf::mac_from_string(mac)) {
            TASK_LOG(DEBUG) << "DELETED_EVENT";
            it->first = eEvent::DELETED_EVENT;
        }
    }
}

bool channel_selection_task::reentry_steered_client_check()
{
    auto now                       = std::chrono::steady_clock::now();
    auto dfs_reentry_clients_delta = std::chrono::duration_cast<std::chrono::milliseconds>(
                                         now - dfs_reentry_pending_steered_clients->timestamp)
                                         .count();
    if (dfs_reentry_clients_delta < REENTRY_STEERED_CLIENTS_WAIT) {
        auto set_reentry_clients = database.get_stations_on_radio(radio_mac, STATE_CONNECTED);

        if (set_reentry_clients.empty()) {
            TASK_LOG(DEBUG) << "no client connected to reentry hostap ";
            return true;
        }

        //inject the event back to the end of the queue
        //TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " sta's are still connected injecting sta sample event ";
        auto new_event             = new sDfsReEntrySampleSteeredClients_event;
        new_event->hostap_mac      = radio_mac;
        new_event->timestamp       = dfs_reentry_pending_steered_clients->timestamp;
        new_event->timeout_expired = false;
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::DFS_REENTRY_PENDING_STEERED_CLIENT,
                         (void *)new_event);
    } else {
        dfs_reentry_pending_steered_clients->timeout_expired = true;
        TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " sta's sample timeout expired "
                        << int(dfs_reentry_clients_delta);
    }
    return false;
}

bool channel_selection_task::cac_pending_hostap_check()
{
    if (!hostaps_cac_pending.empty()) {
        auto now            = std::chrono::steady_clock::now();
        auto it_cac_pending = hostaps_cac_pending.find(tlvf::mac_to_string(radio_mac));
        if (it_cac_pending == std::end(hostaps_cac_pending)) {
            TASK_LOG(INFO) << "cac complete already arrived for radio_mac = " << radio_mac;
            return true;
        }
        auto cac_complete_delta =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - it_cac_pending->second)
                .count();
        if (cac_complete_delta > CAC_COMPLETED_WAIT_TIME) {
            TASK_LOG(INFO) << "cac_complete_delta = " << int(cac_complete_delta)
                           << " > ( CAC_COMPLETED_WAIT_TIME = 11 min )";
            dfs_cac_pending_hostap->timeout_expired = true;
            it_cac_pending                          = hostaps_cac_pending.erase(it_cac_pending);
            return true;
        } else {
            //inject again the event to check for cac completed.
            //TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " inject cac sample event";
            auto new_event             = new sDfsCacPendinghostap_event;
            new_event->hostap_mac      = radio_mac;
            new_event->timeout_expired = false;
            //TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " timeout_expired = " << int(new_event->timeout_expired);
            tasks.push_event(database.get_channel_selection_task_id(),
                             (int)channel_selection_task::eEvent::DFS_CAC_PENDING_HOSTAP_EVENT,
                             (void *)new_event);
            return true;
        }
    }
    TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " no hostap pending cac ";
    return false;
}
bool channel_selection_task::is_2G_channel(int channel)
{
    return (wireless_utils::which_freq(channel) == eFreqType::FREQ_24G);
}

bool channel_selection_task::is_5G_channel(int channel)
{
    return (wireless_utils::which_freq(channel) == eFreqType::FREQ_5G);
}

void channel_selection_task::get_hostap_params()
{
    auto radio_wifi_channel = database.get_radio_wifi_channel(radio_mac);
    if (radio_wifi_channel.is_empty()) {
        LOG(WARNING) << "empty wifi channel of " << tlvf::mac_to_string(radio_mac) << " in DB";
    }
    hostap_params.channel   = radio_wifi_channel.get_channel();
    hostap_params.freq_type = radio_wifi_channel.get_freq_type();
    TASK_LOG(DEBUG) << "get_hostap_params()  for = " << radio_mac;
    //use slave_joined_event as backhaul may have not connected yet
    if (slave_joined_event) {
        hostap_params.backhaul_is_wireless = slave_joined_event->backhaul_is_wireless;
        hostap_params.backhaul_channel     = slave_joined_event->backhaul_channel;
        hostap_params.low_pass_filter_on   = slave_joined_event->low_pass_filter_on;
        hostap_params.bandwidth            = slave_joined_event->cs_params.bandwidth;
        TASK_LOG(DEBUG) << "hostap_params.low_pass_filter_on = "
                        << int(hostap_params.low_pass_filter_on)
                        << " slave_joined_event->low_pass_filter_on = "
                        << int(slave_joined_event->low_pass_filter_on);
    } else {
        auto backhaul_wifi_channel = database.get_sta_wifi_channel(hostap_params.backhaul_mac);
        if (backhaul_wifi_channel.is_empty()) {
            LOG(ERROR) << "empty wifi channel of " << hostap_params.backhaul_mac << " in DB";
            return;
        }

        std::shared_ptr<Agent> agent       = database.get_agent_by_radio_uid(radio_mac);
        std::shared_ptr<Station> bh_sta    = database.get_station(agent->parent_mac);
        hostap_params.backhaul_is_wireless = (bh_sta && bh_sta->get_bss());
        hostap_params.backhaul_channel     = backhaul_wifi_channel.get_channel();
        hostap_params.backhaul_freq_type   = backhaul_wifi_channel.get_freq_type();
        //TODO add low_pass_filter_on to the DB, only needed for DFS
    }

    // which_subband works only for 5G
    if (hostap_params.freq_type == beerocks::FREQ_5G) {
        hostap_params.backhaul_subband =
            son::wireless_utils::which_subband(hostap_params.backhaul_channel);
    } else {
        hostap_params.backhaul_subband = beerocks::eSubbandType::SUBBAND_UNKNOWN;
    }

    TASK_LOG(DEBUG) << "radio_mac = " << radio_mac
                    << " hostap_params.channel = " << int(hostap_params.channel)
                    << " hostap_params.backhaul_channel = " << int(hostap_params.backhaul_channel)
                    << " hostap_params.bandwidth = " << int(hostap_params.bandwidth);
}

void channel_selection_task::ccl_fill_supported_channels()
{
    TASK_LOG(DEBUG) << "*****************ccl_fill_supported_channels**************************** :";
    //Get the supported channels list from the db
    auto hostap_supported_channels = database.get_radio_supported_channels(radio_mac);

    /*1. Fill active channel list with the suppoted channels and
        2. Initialize all the supported channels as available in active list to start with*/
    for (const auto &hostap_channel : hostap_supported_channels) {
        if (hostap_channel.get_bandwidth() == beerocks::BANDWIDTH_20 &&
            hostap_channel.get_channel() > 0) {
            sCandidateChannel cc;
            cc.primary_channel   = 0;
            cc.secondary_channel = 0;
            cc.disallow          = false;
            ccl.insert({hostap_channel.get_channel(), cc});
        }
    }
}

void channel_selection_task::ccl_fill_affected_supported_channels()
{
    TASK_LOG(DEBUG)
        << "*****************ccl_fill_affected_supported_channels**************************** :";
    auto hostap_supported_channels = database.get_radio_supported_channels(radio_mac);

    /*1. Fill active channel list with the radar affected suppoted channels */
    for (auto hostap_channel : hostap_supported_channels) {
        if (hostap_channel.get_bandwidth() == beerocks::BANDWIDTH_20 &&
            hostap_channel.get_channel() > 0) {
            ccl[hostap_channel.get_channel()].radar_affected = hostap_channel.get_radar_affected();
            TASK_LOG(DEBUG) << " channel = " << int(hostap_channel.get_channel())
                            << " hostap_channel.radar_affected "
                            << int(hostap_channel.get_radar_affected());
        }
    }
}

void channel_selection_task::ccl_fill_active_channels()
{
    TASK_LOG(DEBUG) << "*****************ccl_fill_active_channels**************************** :";
    for (const auto &hostap : database.get_active_radios()) {
        auto wifi_channel = database.get_radio_wifi_channel(tlvf::mac_from_string(hostap));
        if (wifi_channel.is_empty()) {
            LOG(ERROR) << "empty wifi channel of " << hostap
                       << " in DB. possibly not initialized yet. skip this iteration";
            continue;
        }
        TASK_LOG(DEBUG) << "hostap=" << hostap << " " << wifi_channel;

        // add active hostap channels to ccl
        auto channel_list_20MHz = son::wireless_utils::split_channel_to_20MHz(wifi_channel);
        for (auto &channel_20MHz : channel_list_20MHz) {
            TASK_LOG(DEBUG) << "channel_20MHz = " << int(channel_20MHz.first);
            auto it = ccl.find(channel_20MHz.first);
            if (it == ccl.end()) {
                TASK_LOG(DEBUG) << "didn't found channel_20MHz " << int(channel_20MHz.first);
                continue;
            }
            TASK_LOG(DEBUG) << "channel_20MHz.second  = " << int(channel_20MHz.second);
            if (wifi_channel.get_freq_type() == beerocks::FREQ_24G) {
                get_overlapping_channels_for_24G(it->first);
            }
            if (channel_20MHz.second == beerocks::CH_PRIMARY) {
                it->second.primary_channel++;
                TASK_LOG(DEBUG) << "inc primary_channel  " << int(it->second.primary_channel);
            } else if (channel_20MHz.second == beerocks::CH_SECONDARY) {
                it->second.secondary_channel++;
                TASK_LOG(DEBUG) << "inc secondary_channel  " << int(it->second.secondary_channel);
            }
        }

        for (auto &ccl_unit : ccl) {
            TASK_LOG(DEBUG) << "channel  = " << int(ccl_unit.first)
                            << " primary_channel = " << int(ccl_unit.second.primary_channel)
                            << " secondary_channel = " << int(ccl_unit.second.secondary_channel)
                            << " disallow = " << int(ccl_unit.second.disallow)
                            << " overlap = " << int(ccl_unit.second.overlap);
        }
    }
}

void channel_selection_task::ccl_remove_5G_subband(beerocks::eSubbandType band_type)
{
    TASK_LOG(DEBUG) << "*****************ccl_remove_5G_subband**************************** :";
    for (auto it = ccl.begin(); it != ccl.end(); ++it) {
        auto channel = it->first;
        if (is_2G_channel(channel))
            continue;
        if (wireless_utils::which_subband(channel) == band_type) {
            TASK_LOG(DEBUG) << " channel = " << int(channel) << " band_type = " << int(band_type)
                            << " disallow = true";
            it->second.disallow = true;
        }
    }
}

bool channel_selection_task::ccl_has_free_channels_5G(beerocks::eWiFiBandwidth bw)
{
    TASK_LOG(DEBUG) << "*****************ccl_has_free_channels_5G**************************** :";
    // check if there is a free channel in the list with at least "bw" value
    // channels in the list are in order
    int target_free_20MHz_channels =
        beerocks::utils::count_target_bandwidth(bw, beerocks::eWiFiBandwidth::BANDWIDTH_20);
    int free_20MHz_channels = 0;
    //for ( auto it = ccl.begin(); it!= ccl.end(); ++it ){
    for (auto i = START_OF_LOW_BAND_NON_DFS; i <= END_OF_HIGH_BAND; i++) {
        auto it = ccl.find(i);
        if (it == ccl.end()) {
            continue;
        }
        auto cc = it->second;
        if (cc.primary_channel > 0 || cc.secondary_channel > 0 || cc.disallow) {
            free_20MHz_channels = 0;
        } else {
            free_20MHz_channels++;
            TASK_LOG(DEBUG) << " channel = " << int(it->first)
                            << " free_20MHz_channels = " << int(free_20MHz_channels);
        }
        if (free_20MHz_channels >= target_free_20MHz_channels) {
            TASK_LOG(DEBUG) << "TRUE!! last_channel = " << int(it->first)
                            << " free_20MHz_channels = " << int(free_20MHz_channels)
                            << " target_free_20MHz_channels = " << int(target_free_20MHz_channels);
            return true;
        }
    }
    return false;
}

bool channel_selection_task::ccl_has_free_dfs_channels(beerocks::eWiFiBandwidth bw)
{
    TASK_LOG(DEBUG) << "*****************ccl_has_free_dfs_channels**************************** :";
    // check if there is a free dfs channel in the list with at least "bw" value
    // channels in the list are in order
    int target_free_20MHz_channels =
        beerocks::utils::count_target_bandwidth(bw, beerocks::eWiFiBandwidth::BANDWIDTH_20);
    int free_20MHz_channels       = 0;
    int channel_alignment_counter = 0;
    for (auto i = START_OF_LOW_DFS_SUBBAND; i <= END_OF_HIGH_DFS_SUBBAND; i++) {
        auto it = ccl.find(i);
        if (it == ccl.end()) {
            continue;
        }
        auto cc = it->second;
        channel_alignment_counter++;
        if (cc.radar_affected) {
            free_20MHz_channels = 0;
        } else {
            free_20MHz_channels++;
            TASK_LOG(DEBUG) << " channel = " << int(it->first)
                            << " free_20MHz_channels = " << int(free_20MHz_channels)
                            << " radar_affected = " << int(cc.radar_affected)
                            << " channel_alignment_counter=" << channel_alignment_counter
                            << " channel_alignment_counter=" << channel_alignment_counter;
        }
        if (free_20MHz_channels >= target_free_20MHz_channels &&
            channel_alignment_counter == target_free_20MHz_channels) {
            TASK_LOG(DEBUG) << "TRUE!! last_channel = " << int(it->first)
                            << " free_20MHz_channels = " << int(free_20MHz_channels)
                            << " target_free_20MHz_channels = " << int(target_free_20MHz_channels);
            return true;
        }
        if (channel_alignment_counter >= target_free_20MHz_channels) {
            channel_alignment_counter = 0;
        }
    }
    return false;
}

bool channel_selection_task::ccl_has_free_channels_2G()
{
    TASK_LOG(DEBUG) << "*****************ccl_has_free_channels_2G**************************** :";
    for (auto ch : ccl) {
        if (!(is_2G_channel(ch.first))) {
            continue;
        }
        if (ch.second.overlap == 0) {
            return true;
        }
    }
    return false;
}
bool channel_selection_task::ccl_fill_channel_switch_request_with_least_used_channel()
{
    uint8_t min_used_channel_score = 255;
    uint8_t selected_channel       = 0;
    const int channel_step         = CHANNEL_20MHZ_STEP;
    std::unordered_map<uint8_t, sLeastUsedCandidates> min_channels;
    TASK_LOG(DEBUG) << "*****************ccl_fill_channel_switch_request_with_least_used_channel***"
                       "************************* :";

    //finding secondary min and calculating right and left score for least used channel
    for (uint32_t channel = END_OF_HIGH_BAND_NON_DFS; channel >= START_OF_LOW_BAND_NON_DFS;
         channel--) {
        auto it = ccl.find(channel);
        if (it == ccl.end())
            continue;
        //auto cc = it->second;
        //calculating center ,right and left score for least used channel
        TASK_LOG(DEBUG) << "channel = " << channel;
        //center score (secondary = 1 primary = 2 point)
        it->second.ch_score = it->second.secondary_channel + (it->second.primary_channel * 2);
        TASK_LOG(DEBUG) << "it->second.ch_score = " << int(it->second.ch_score);
        auto score_it = ccl.find((channel - channel_step));
        if (score_it != ccl.end()) {
            it->second.ext_above_low_score = score_it->second.primary_channel;
            TASK_LOG(DEBUG) << "it->first = " << int(it->first)
                            << " score_it->first = " << int(score_it->first)
                            << " score_it->second.primary_channel = "
                            << int(score_it->second.primary_channel);
        }
        score_it = ccl.find((channel + channel_step));
        if (score_it != ccl.end()) {
            it->second.ext_above_high_score = score_it->second.primary_channel;
            TASK_LOG(DEBUG) << "it->first = " << int(it->first)
                            << " score_it->first = " << int(score_it->first)
                            << " score_it->second.primary_channel = "
                            << int(score_it->second.primary_channel);
        }
        //finding secondary min
        if (it->second.ch_score < min_used_channel_score) {
            if (it->second.disallow == true) {
                continue;
            }
            min_used_channel_score = it->second.ch_score;
            selected_channel       = it->first;
        }
    }
    TASK_LOG(DEBUG) << "min_used_channel = " << int(selected_channel)
                    << " min_used_channel_score = " << int(min_used_channel_score);
    //finding multiple min
    for (auto ch : ccl) {
        TASK_LOG(DEBUG) << "ch.first = " << int(ch.first)
                        << " ch.second.ch_score = " << int(ch.second.ch_score)
                        << " min_used_channel_score = " << int(min_used_channel_score)
                        << " ch.second.disallow = " << int(ch.second.disallow);
        if (ch.second.ch_score == min_used_channel_score && !(ch.second.disallow)) {
            min_channels[ch.first].channel_ext_above_secondary =
                false; // insert key and default value - deploy left(L)
            min_channels[ch.first].score = ch.second.ext_above_low_score;
            TASK_LOG(DEBUG) << "in_channels[ch.first].channel_ext_above_secondary = "
                            << int(min_channels[ch.first].channel_ext_above_secondary)
                            << " min_channels[ch.first].score = "
                            << int(ch.second.ext_above_low_score)
                            << " ch.first = " << int(ch.first);
            //determine if to deploy right or left
            bool high_band      = wireless_utils::is_high_subband(ch.first);
            uint8_t end_channel = high_band ? END_OF_HIGH_BAND_NON_DFS : END_OF_LOW_BAND_NON_DFS;

            if ((end_channel - ch.first) > channel_step) {
                min_channels[ch.first].channel_ext_above_secondary = true; //deploy right(H)
                min_channels[ch.first].score                       = ch.second.ext_above_high_score;
                TASK_LOG(DEBUG) << "in_channels[ch.first].channel_ext_above_secondary = "
                                << int(min_channels[ch.first].channel_ext_above_secondary)
                                << " min_channels[ch.first].score = "
                                << int(ch.second.ext_above_high_score)
                                << " ch.first = " << int(ch.first);
            }
        }
    }
    if (min_channels.empty()) {
        TASK_LOG(WARNING) << "min_channels is empty!";
        return false;
        // single candidate
    } else if (min_channels.size() == 1) {
        channel_switch_request.channel   = min_channels.begin()->first;
        channel_switch_request.bandwidth = beerocks::BANDWIDTH_80;
        align_channel_to_80Mhz();
        channel_switch_request.vht_center_frequency = wireless_utils::channel_to_vht_center_freq(
            channel_switch_request.channel,
            wireless_utils::which_freq_type(channel_switch_request.vht_center_frequency),
            beerocks::eWiFiBandwidth(channel_switch_request.bandwidth),
            min_channels.begin()->second.channel_ext_above_secondary);
        TASK_LOG(DEBUG) << "channel_switch_request.channel = " << int(min_channels.begin()->first)
                        << " channel_switch_request.channel_ext_above_secondary = "
                        << int(min_channels.begin()->second.channel_ext_above_secondary)
                        << " channel_switch_request.vht_center_frequency = "
                        << int(channel_switch_request.vht_center_frequency);
    } else {
        //multiple candidate find lowest score
        uint8_t min_channel_score         = 255;
        uint8_t min_channel_score_gw_band = 255;
        auto it_min_gw_band               = min_channels.begin();
        auto it_min                       = min_channels.begin();
        //for(auto min_ch : min_channels) {
        for (auto it = min_channels.begin(); it != min_channels.end(); ++it) {
            TASK_LOG(DEBUG) << "min_channel.first = " << int(it->first)
                            << " min_channel.second.score = " << int(it->second.score)
                            << " min_channel.second.channel_ext_above_secondary = "
                            << int(it->second.channel_ext_above_secondary);
            auto gw_slave_5G_channel = get_gw_slave_5g_channel();
            TASK_LOG(DEBUG) << "gw_slave_5G_channel = " << int(gw_slave_5G_channel);
            if (gw_slave_5G_channel != 0 &&
                (wireless_utils::is_high_subband(it->first) ==
                 wireless_utils::is_high_subband(gw_slave_5G_channel))) {
                if (it->second.score < min_channel_score_gw_band) {
                    min_channel_score_gw_band = it->second.score;
                    it_min_gw_band            = it;
                }
            } else {
                if (it->second.score < min_channel_score) {
                    min_channel_score = it->second.score;
                    it_min            = it;
                }
            }
        }
        auto it_res = (min_channel_score != 255) ? it_min : it_min_gw_band;
        TASK_LOG(DEBUG) << "min_channel_score = " << int(min_channel_score)
                        << " min_channel_score_gw_band = " << int(min_channel_score_gw_band);
        channel_switch_request.channel   = it_res->first;
        channel_switch_request.bandwidth = beerocks::BANDWIDTH_80;
        align_channel_to_80Mhz();
        channel_switch_request.vht_center_frequency = wireless_utils::channel_to_vht_center_freq(
            channel_switch_request.channel,
            wireless_utils::which_freq_type(channel_switch_request.vht_center_frequency),
            beerocks::eWiFiBandwidth(channel_switch_request.bandwidth),
            it_res->second.channel_ext_above_secondary);
    }
    TASK_LOG(DEBUG) << "channel_switch_request.channel = " << int(channel_switch_request.channel)
                    << " channel_switch_request.bandwidth = "
                    << int(channel_switch_request.bandwidth)
                    << " channel_switch_request.vht_center_frequency = "
                    << int(channel_switch_request.vht_center_frequency);

    return true;
}

bool channel_selection_task::fill_restricted_channels_from_ccl_and_supported(uint8_t *channel_list)
{
    std::set<uint8_t> channel_list_set;
    int idx = 0;
    for (auto it = ccl.begin(); it != ccl.end(); ++it) {
        auto cc = it->second;
        if (is_2G_channel(it->first)) {
            if (it->second.overlap) {
                channel_list_set.insert(it->first);
            }
        } else if (cc.primary_channel > 0 || cc.secondary_channel > 0 || cc.disallow) {
            TASK_LOG(DEBUG) << "**fill_restricted_channels_from_ccl_and_supported**";
            TASK_LOG(DEBUG) << "channel = " << int(it->first)
                            << " primary_channel = " << int(cc.primary_channel)
                            << " secondary_channel = " << int(cc.secondary_channel)
                            << " disallow = " << int(cc.disallow);

            channel_list_set.insert(it->first);
        }
    }
    auto global_restricted_chn = database.get_global_restricted_channels();
    auto db_restricted         = database.get_radio_conf_restricted_channels(radio_mac);
    auto configured_restricted_chn =
        !global_restricted_chn.empty() ? global_restricted_chn : db_restricted;
    if (!configured_restricted_chn.empty()) {
        for (auto elm : configured_restricted_chn) {
            if (!elm || ccl.find(elm) == ccl.end()) {
                continue;
            }
            TASK_LOG(DEBUG) << "config_channel = " << int(elm);
            channel_list_set.insert(elm);
        }
    }
    for (auto ch : channel_list_set) {
        channel_list[idx] = ch;
        idx++;
    }
    return true;
}

bool channel_selection_task::get_overlapping_channels_for_24G(uint8_t channel)
{
    if (channel > LAST_2G_CHANNEL) {
        return false;
    }

    int channel_low  = int(channel) - 4;
    int channel_high = int(channel) + 4;

    for (int i = channel_low; i <= channel_high; i++) {
        if (i > 0 && i <= LAST_2G_CHANNEL) {
            auto it = ccl.find(i);
            if (it != ccl.end()) {
                it->second.overlap = 1;
            }
        }
    }
    return true;
}

bool channel_selection_task::fill_restricted_channels_from_ccl_busy_bands(uint8_t *channel_list)
{
    int channel_step = CHANNEL_20MHZ_STEP;

    auto channel              = channel_switch_request.channel;
    auto vht_center_frequency = channel_switch_request.vht_center_frequency;
    auto freq                 = wireless_utils::channel_to_freq(
        channel, wireless_utils::which_freq_type(vht_center_frequency));

    auto channel_ext_above_secondary = (freq < vht_center_frequency) ? true : false;

    std::set<uint8_t> least_used_channels;
    TASK_LOG(DEBUG) << "*****************fill_restricted_channels_from_ccl_busy_bands**************"
                       "************** :";

    least_used_channels.insert(channel);
    TASK_LOG(DEBUG) << "insert channel = " << int(channel);
    if (channel_ext_above_secondary) {
        for (int i = (channel_step - 1); i > 0; i--) {
            channel += 4;
            least_used_channels.insert(channel);
            TASK_LOG(DEBUG) << "insert channel = " << int(channel);
        }
    } else {
        for (int i = (channel_step - 1); i > 0; i--) {
            channel -= 4;
            least_used_channels.insert(channel);
            TASK_LOG(DEBUG) << "insert channel = " << int(channel);
        }
    }

    for (auto it = ccl.begin(); it != ccl.end(); ++it) {

        //auto least_used_channels_it = least_used_channels.find(channel_temp);
        if (least_used_channels.find(it->first) != least_used_channels.end()) {
            continue;
        }
        TASK_LOG(DEBUG) << "*channel_list = " << int(it->first);
        *channel_list = it->first;
        channel_list++;

        TASK_LOG(DEBUG) << "channel  = " << int(it->first)
                        << " primary_channel = " << int(it->second.primary_channel)
                        << " secondary_channel = " << int(it->second.secondary_channel)
                        << " disallow = " << int(it->second.disallow);
    }
    return true;
}

uint8_t channel_selection_task::get_gw_slave_5g_channel()
{
    auto gw = database.get_gw();
    if (!gw) {
        return 0;
    }

    for (const auto &radio : gw->radios) {
        std::string radio_mac = tlvf::mac_to_string(radio.first);
        auto wifi_channel     = database.get_radio_wifi_channel(tlvf::mac_from_string(radio_mac));
        if (!wifi_channel.is_empty() && wifi_channel.get_freq_type() == eFreqType::FREQ_5G) {
            return wifi_channel.get_channel();
        }
    }

    return 0;
}

void channel_selection_task::align_channel_to_80Mhz()
{
    TASK_LOG(DEBUG) << "*****************align_channel_to_80Mhz**************************** :";
    auto least_used_channel = channel_switch_request.channel;
    bool high_band          = wireless_utils::is_high_subband(least_used_channel);
    uint8_t end_channel     = high_band ? END_OF_HIGH_BAND_NON_DFS : END_OF_LOW_BAND_NON_DFS;
    auto channel_step       = CHANNEL_20MHZ_STEP;

    if ((end_channel - least_used_channel) > channel_step) {
        channel_switch_request.channel = end_channel - CHANNEL_80MHZ_STEP;
    } else {
        channel_switch_request.channel = end_channel;
    }
}

void channel_selection_task::cs_wait_for_event(eEvent cs_event)
{
    cs_waiting_for_event = cs_event;
    wait_for_event((int)cs_event);
}

void channel_selection_task::wait_for_cac_completed(uint8_t channel, uint8_t bandwidth)
{
    TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " channel = " << int(channel)
                    << " bandwidth = " << int(bandwidth);
    database.set_radar_hit_stats(radio_mac, channel, bandwidth, true);
    //hostap handle the CAC-completed event async pushing to deck.
    TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " enter to cac pending";
    hostaps_cac_pending.insert({tlvf::mac_to_string(radio_mac), std::chrono::steady_clock::now()});
    database.set_radio_cac_completed(radio_mac, false);

    // update bml listeners
    bml_task::cac_status_changed_notification_event cac_status_changed_event;
    cac_status_changed_event.hostap_mac    = tlvf::mac_to_string(radio_mac);
    cac_status_changed_event.cac_completed = 0;
    tasks.push_event(database.get_bml_task_id(),
                     bml_task::CAC_STATUS_CHANGED_NOTIFICATION_EVENT_AVAILABLE,
                     &cac_status_changed_event);

    //inject event to check for cac completed.
    TASK_LOG(DEBUG) << "radio_mac - " << radio_mac << " inject cac sample event";
    auto new_event             = new sDfsCacPendinghostap_event;
    new_event->hostap_mac      = radio_mac;
    new_event->timeout_expired = false;
    TASK_LOG(DEBUG) << "radio_mac - " << radio_mac
                    << " timeout_expired = " << int(new_event->timeout_expired);
    tasks.push_event(database.get_channel_selection_task_id(),
                     (int)channel_selection_task::eEvent::DFS_CAC_PENDING_HOSTAP_EVENT,
                     (void *)new_event);
}

void channel_selection_task::assign_config_global_restricted_channel_to_db()
{
    uint8_t restricted_channels[beerocks::message::RESTRICTED_CHANNEL_LENGTH] = {0};
    std::copy(database.config.global_restricted_channels.begin(),
              database.config.global_restricted_channels.end(), restricted_channels);
    database.set_global_restricted_channels(restricted_channels);
}
void channel_selection_task::run_optimal_path_for_connected_clients()
{
    auto hostaps_sibling = database.get_radio_siblings(radio_mac);
    auto hostap_mac_2g   = std::find_if(
        std::begin(hostaps_sibling), std::end(hostaps_sibling), [&](std::string hostap_sibling) {
            auto hostap_sibling_wifi_channel =
                database.get_radio_wifi_channel(tlvf::mac_from_string(hostap_sibling));
            if (hostap_sibling_wifi_channel.is_empty()) {
                LOG(WARNING) << "empty wifi channel of " << hostap_sibling << "in DB";
            }
            return (hostap_sibling_wifi_channel.get_freq_type() == eFreqType::FREQ_24G &&
                    database.is_radio_active(tlvf::mac_from_string(hostap_sibling)));
        });

    if (hostap_mac_2g != std::end(hostaps_sibling)) {
        auto dfs_reentry_clients = database.get_radio_dfs_reentry_clients(radio_mac);
        auto conn_clients =
            database.get_stations_on_radio(tlvf::mac_from_string(*hostap_mac_2g), STATE_CONNECTED);
        for (auto &dfs_reentry_client : dfs_reentry_clients) {
            TASK_LOG(DEBUG) << "dfs_reentry_client - " << dfs_reentry_client;
            conn_clients.erase(dfs_reentry_client);
        }

        for (auto &conn_client : conn_clients) {
            TASK_LOG(DEBUG) << "conn_client - " << conn_client;
        }

        for (auto &conn_client : conn_clients) {
            TASK_LOG(DEBUG) << "optimal_path_task - conn_client - " << conn_client;
            auto new_task = std::make_shared<optimal_path_task>(
                database, cmdu_tx, tasks, conn_client, 6000, "channel_selection");
            tasks.add_task(new_task);
        }
    }
}

bool channel_selection_task::handle_ieee1905_1_msg(const sMacAddr &src_mac,
                                                   ieee1905_1::CmduMessageRx &cmdu_rx)
{
    return false;
}
