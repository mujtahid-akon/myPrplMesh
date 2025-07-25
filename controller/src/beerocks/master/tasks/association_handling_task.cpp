/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "association_handling_task.h"
#include "../db/db_algo.h"
#include "../son_actions.h"
#include "optimal_path_task.h"

#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_wifi_channel.h>
#include <bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message_control.h>

using namespace beerocks;
using namespace net;
using namespace son;

#define BEACON_MEASURE_REQ_TIME_SPAN 3000
#define BEACON_MEASURE_MAX_ATTEMPTS 3
#define REQUEST_RSSI_MEASUREMENT_MAX_ATTEMPTS 5
#define REQUEST_RSSI_MEASUREMENT_DELAY 10000
#define START_MONITORING_RESPONSE_TIMEOUT_MSEC 3000
#define START_MONITORING_MAX_ATTEMPTS 5

association_handling_task::association_handling_task(db &database_,
                                                     ieee1905_1::CmduMessageTx &cmdu_tx_,
                                                     task_pool &tasks_, const std::string &sta_mac_,
                                                     const std::string &task_name_)
    : task(task_name_), database(database_), cmdu_tx(cmdu_tx_), tasks(tasks_), sta_mac(sta_mac_)
{
}

void association_handling_task::work()
{
    switch (state) {
    case START: {

        if (!database.is_sta_wireless(sta_mac)) {
            TASK_LOG(DEBUG) << "client " << sta_mac << " is not wireless, finish the task";
            finish();
            return;
        }

        auto station = database.get_station(tlvf::mac_from_string(sta_mac));
        if (!station) {
            TASK_LOG(DEBUG) << "client " << sta_mac << " is not found";
            finish();
            return;
        }

        // If this task already has been created by another event, let it finish and finish the new
        // instance of it.
        int prev_task_id = station->association_handling_task_id;
        if (tasks.is_task_running(prev_task_id)) {
            finish();
            return;
        }
        station->association_handling_task_id = id;

        original_parent_mac = database.get_sta_parent(sta_mac);

        task_started_timestamp = std::chrono::steady_clock::now();

        if (database.settings_monitor_on_vaps() == false) {
            TASK_LOG(DEBUG) << "started association_handling_task, non main vap connected sta "
                            << sta_mac;
            state = FINISH;
            break;
        }

        TASK_LOG(DEBUG) << "started association_handling_task, rssi measurement on " << sta_mac;
        state        = START_RSSI_MONITORING;
        max_attempts = START_MONITORING_MAX_ATTEMPTS;
        break;
    }

    case START_RSSI_MONITORING: {
        TASK_LOG(DEBUG) << "START_RSSI_MONITORING";
        /*
         * request constant RSSI monitoring for the new client
         */

        std::shared_ptr<Station> station = database.get_station(tlvf::mac_from_string(sta_mac));
        if (!station) {
            return;
        }

        std::string new_hostap_mac = database.get_sta_parent(sta_mac);
        if (new_hostap_mac != original_parent_mac ||
            database.get_sta_state(sta_mac) != beerocks::STATE_CONNECTED) {
            TASK_LOG(DEBUG) << "sta " << sta_mac << " is no longer connected to "
                            << original_parent_mac << " finishing task";
            finish();
            return;
        }

        LOG(DEBUG) << "START_MONITORING_REQUEST hostap_mac=" << new_hostap_mac << " sta_mac "
                   << sta_mac;

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST>(cmdu_tx, id);

        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_START_MONITORING_REQUEST message!";
            return;
        }

        request->params().mac    = tlvf::mac_from_string(sta_mac);
        request->params().ipv4   = network_utils::ipv4_from_string(database.get_sta_ipv4(sta_mac));
        request->params().is_ire = false;

        //add bridge mac for ires
        if (station->is_bSta()) {
            auto bridge_container = database.get_agent_by_parent(tlvf::mac_from_string(sta_mac));
            if (bridge_container) {
                request->params().bridge_4addr_mac = bridge_container->al_mac;
                LOG(DEBUG) << "IRE " << sta_mac << " is on a bridge with mac "
                           << bridge_container->al_mac;
            }
        }

        auto radio_mac = database.get_bss_parent_radio(new_hostap_mac);
        auto agent_mac = database.get_radio_parent_agent(tlvf::mac_from_string(radio_mac));
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac);

        add_pending_mac(radio_mac,
                        beerocks_message::ACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE);
        set_responses_timeout(START_MONITORING_RESPONSE_TIMEOUT_MSEC);
        break;
    }

    case CHECK_11K_BEACON_MEASURE_CAP: {
        TASK_LOG(DEBUG) << "started association_handling_task, looking for beacon measurement "
                           "capabilities on "
                        << sta_mac;
        std::string bssid     = database.get_sta_parent(sta_mac);
        std::string radio_mac = database.get_bss_parent_radio(bssid);
        auto agent_mac        = database.get_bss_parent_agent(tlvf::mac_from_string(bssid));

        auto measurement_request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST>(cmdu_tx, id);

        if (measurement_request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_BEACON_11K_REQUEST message!";
            return;
        }
        auto wifi_channel = database.get_radio_wifi_channel(tlvf::mac_from_string(radio_mac));
        if (wifi_channel.is_empty()) {
            LOG(WARNING) << "empty wifi channel of " << radio_mac << " in DB";
        }
        measurement_request->params().measurement_mode =
            beerocks::MEASURE_MODE_ACTIVE; // son::eMeasurementMode11K "passive"/"active"/"table"
        measurement_request->params().channel = wifi_channel.get_channel();
        measurement_request->params().op_class =
            database.get_radio_operating_class(tlvf::mac_from_string(radio_mac));
        measurement_request->params().rand_ival = beerocks::
            BEACON_MEASURE_DEFAULT_RANDOMIZATION_INTERVAL; // random interval - specifies the upper bound of the random delay to be used prior to making the measurement, expressed in units of TUs [=1024usec]
        measurement_request->params().duration = beerocks::
            BEACON_MEASURE_DEFAULT_ACTIVE_DURATION; // measurement duration, expressed in units of TUs [=1024usec]
        measurement_request->params().sta_mac = tlvf::mac_from_string(sta_mac);
        measurement_request->params().bssid   = tlvf::mac_from_string(bssid);
        //measurement_request.params.use_optional_ssid = true;
        measurement_request->params().expected_reports_count = 1;
        //mapf::utils::copy_string(measurement_request.params.ssid, database.get_hostap_vap_ssid(bssid).c_str(), sizeof(measurement_request.params.ssid));
        add_pending_mac(radio_mac, beerocks_message::ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE);
        TASK_LOG(DEBUG) << "requested beacon measurement request from sta: " << sta_mac
                        << " on bssid: " << bssid;

        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, radio_mac);
        set_responses_timeout(BEACON_MEASURE_REQ_TIME_SPAN);
        break;
    }

    case REQUEST_RSSI_MEASUREMENT_WAIT: {
        if (!database.settings_client_band_steering() &&
            !database.settings_client_optimal_path_roaming()) {
            TASK_LOG(DEBUG) << "band_steering and client_roaming are disabled! skipping on rssi "
                               "measurement, and finish the task";
            state = FINISH;
            break;
        }

        TASK_LOG(DEBUG) << "REQUEST_RSSI_MEASUREMENT_WAIT";
        state               = REQUEST_RSSI_MEASUREMENT;
        int time_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::chrono::steady_clock::now() - task_started_timestamp)
                                  .count();
        int new_delay = REQUEST_RSSI_MEASUREMENT_DELAY - time_elapsed_ms;
        TASK_LOG(DEBUG) << "new_delay=" << new_delay << "ms";
        max_attempts = REQUEST_RSSI_MEASUREMENT_MAX_ATTEMPTS;
        attempts     = 0;
        wait_for(new_delay);
        break;
    }

    case REQUEST_RSSI_MEASUREMENT: {

        /*
         * send measurement request to get a valid RSSI reading
         */
        TASK_LOG(DEBUG) << "starting rssi measurement on " << sta_mac;
        std::string hostap_mac = database.get_sta_parent(sta_mac);
        auto agent_mac         = database.get_bss_parent_agent(tlvf::mac_from_string(hostap_mac));

        if (hostap_mac != original_parent_mac ||
            database.get_sta_state(sta_mac) != beerocks::STATE_CONNECTED) {
            TASK_LOG(DEBUG) << "sta " << sta_mac << " is no longer connected to "
                            << original_parent_mac << " finishing task";
            finish();
            return;
        }

        std::string ipv4 = database.get_sta_ipv4(sta_mac);

        auto measurement_request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>(cmdu_tx, id);
        if (measurement_request == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST message!";
            return;
        }
        const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
        auto radio_wifi_channel =
            database.get_radio_wifi_channel(tlvf::mac_from_string(parent_radio));
        if (radio_wifi_channel.is_empty()) {
            LOG(WARNING) << "empty wifi channel of " << parent_radio << " in DB";
        }
        measurement_request->params().mac       = tlvf::mac_from_string(sta_mac);
        measurement_request->params().ipv4      = network_utils::ipv4_from_string(ipv4);
        measurement_request->params().channel   = radio_wifi_channel.get_channel();
        measurement_request->params().bandwidth = radio_wifi_channel.get_bandwidth();
        measurement_request->params().cross     = 0;

        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);

        TASK_LOG(DEBUG) << "requested rx rssi measurement from " << hostap_mac << " for sta "
                        << sta_mac;

        add_pending_mac(parent_radio,
                        beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE);
        set_responses_timeout(3000);
        break;
    }

    case FINISH: {
        finalize_new_connection();
        finish();
        break;
    }

    default:
        break;
    }
}

void association_handling_task::handle_response(std::string mac,
                                                std::shared_ptr<beerocks_header> beerocks_header)
{
    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE: {

        auto response =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE>();
        if (!response) {
            TASK_LOG(ERROR)
                << "addClass failed for cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE";
            return;
        }

        TASK_LOG(DEBUG) << "received ACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE, success="
                        << string_utils::bool_str(response->success());

        if (!response->success()) {
            if (++attempts >= max_attempts) {
                TASK_LOG(ERROR) << "state START_RSSI_MONITORING reached maximum attempts = "
                                << attempts << " aborting task!";

                TASK_LOG(WARNING) << "client " << sta_mac
                                  << " is not monitored, and could not be steered!!!";
                finish();
            } else {
                wait_for(START_MONITORING_RESPONSE_TIMEOUT_MSEC); // Wait before next request
            }

            break;
        }

        std::shared_ptr<Station> station = database.get_station(tlvf::mac_from_string(sta_mac));
        if (!station) {
            LOG(ERROR) << "No station found with mac " << sta_mac;
            break;
        }
        if (database.settings_client_11k_roaming() &&
            (database.get_sta_beacon_measurement_support_level(sta_mac) ==
             beerocks::BEACON_MEAS_UNSUPPORTED) &&
            !station->is_bSta()) {

            state        = CHECK_11K_BEACON_MEASURE_CAP;
            max_attempts = BEACON_MEASURE_MAX_ATTEMPTS;
            attempts     = 0;
        } else {
            state = REQUEST_RSSI_MEASUREMENT_WAIT;
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        TASK_LOG(DEBUG) << "response for rx_rssi measurement from " << original_parent_mac
                        << " for sta " << sta_mac << " was received";
        state = FINISH;
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE: {
        const std::string parent_mac = database.get_sta_parent(sta_mac);
        auto response =
            beerocks_header
                ->getClass<beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE>();
        if (!response) {
            TASK_LOG(ERROR) << "getClass failed for cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE";
            return;
        }

        LOG(DEBUG) << "beacon response , ID: " << id << std::endl
                   << "sta_mac: " << response->params().sta_mac << std::endl
                   << "measurement_rep_mode: " << (int)response->params().rep_mode << std::endl
                   << "op_class: " << (int)response->params().op_class << std::endl
                   << "channel: "
                   << (int)response->params().channel
                   //<< std::endl << "start_time: "           << (int)response->params.start_time
                   << std::endl
                   << "duration: "
                   << (int)response->params().duration
                   //<< std::endl << "phy_type: "             << (int)response->params.phy_type
                   //<< std::endl << "frame_type: "           << (int)response->params.frame_type
                   << std::endl
                   << "rcpi: " << (int)response->params().rcpi << std::endl
                   << "rsni: " << (int)response->params().rsni << std::endl
                   << "bssid: " << response->params().bssid
            //<< std::endl << "ant_id: "               << (int)response->params.ant_id
            //<< std::endl << "tsf: "                  << (int)response->params.parent_tsf

            //<< std::endl << "new_ch_width: "                         << (int)response->params.new_ch_width
            //<< std::endl << "new_ch_center_freq_seg_0: "             << (int)response->params.new_ch_center_freq_seg_0
            //<< std::endl << "new_ch_center_freq_seg_1: "             << (int)response->params.new_ch_center_freq_seg_1
            ;

        TASK_LOG(DEBUG) << "response for beacon measurement request was received from hostapd "
                        << mac;

        if (son_actions::validate_beacon_measurement_report(response->params(), sta_mac,
                                                            parent_mac)) {
            TASK_LOG(DEBUG) << "sta " << sta_mac << " supports beacon measurement!";
            uint8_t support_level = beerocks::BEACON_MEAS_BSSID_SUPPORTED;

            if (response->params().rsni) {
                //on nexus 5x devices rsni always 0, and they are not supports measurement by ssid (special handling)
                support_level |= beerocks::BEACON_MEAS_SSID_SUPPORTED;
            }
            database.set_sta_beacon_measurement_support_level(
                sta_mac, beerocks::eBeaconMeasurementSupportLevel(support_level));
            state = REQUEST_RSSI_MEASUREMENT_WAIT;
            break;
        }

        if (attempts++ > max_attempts) {
            TASK_LOG(DEBUG) << "state CHECK_11K_BEACON_MEASURE_CAP reached maximum attempts="
                            << attempts << " setting sta " << sta_mac
                            << " as beacon measurement unsupported ";
            state = REQUEST_RSSI_MEASUREMENT_WAIT;
        }
        break;
    }
    default: {
        LOG(ERROR) << "Unsupported action_op:" << int(beerocks_header->action_op());
        break;
    }
    }
}

void association_handling_task::finalize_new_connection()
{
    auto client = database.get_station(tlvf::mac_from_string(sta_mac));
    if (!client) {
        LOG(WARNING) << "Client " << sta_mac << " does not exist";
        return;
    }

    /*
     * see if special handling is required if client just came back from a handover
     */
    if (!database.get_sta_handoff_flag(*client)) {
        if (!client->is_bSta()) {
            // The client's stay-on-initial-radio can be enabled prior to the client connection.
            // If this is the case, when the client connects the initial-radio should be configured (if not already configured)
            // to allow the functionality of stay-on-initial-radio.
            // Note: The initial-radio is persistent configuration and if is already set, the client-connection flow should
            // not override the existing configuration.
            if ((client->stay_on_initial_radio == eTriStateBool::TRUE) &&
                (client->initial_radio == network_utils::ZERO_MAC)) {
                auto bssid            = database.get_sta_parent(sta_mac);
                auto parent_radio_mac = database.get_bss_parent_radio(bssid);
                // If stay_on_initial_radio is enabled and initial_radio is not set yet, set to parent radio mac (not bssid)
                if (!database.set_sta_initial_radio(*client,
                                                    tlvf::mac_from_string(parent_radio_mac),
                                                    database.config.persistent_db)) {
                    LOG(WARNING) << "Failed to set client " << client->mac << "  initial radio to "
                                 << parent_radio_mac;
                }
            }
            auto new_task = std::make_shared<optimal_path_task>(
                database, cmdu_tx, tasks, sta_mac, 6000, "handle_completed_connection");
            tasks.add_task(new_task);
        }
    } else {
        LOG(INFO) << "handoff complete for " << sta_mac;

        /* 
         * kill existing roaming task 
         */
        int prev_roaming_task = client->roaming_task_id;
        LOG(DEBUG) << "kill prev_roaming_task " << prev_roaming_task;
        tasks.kill_task(prev_roaming_task);

        /*
         * kill load balancer
         */
        int prev_load_balancer_task =
            database.get_station_load_balancer_task_id(tlvf::mac_from_string(sta_mac));
        tasks.kill_task(prev_load_balancer_task);

        database.set_sta_handoff_flag(*client, false);
    }
}
void association_handling_task::handle_responses_timeout(
    std::unordered_multimap<std::string, beerocks_message::eActionOp_CONTROL> timed_out_macs)
{
    ++attempts;

    switch (state) {
    case START_RSSI_MONITORING: {
        TASK_LOG(DEBUG) << "response for start monitoring from " << original_parent_mac
                        << " for sta " << sta_mac << " timed out! attempts=" << attempts;
        if (attempts >= max_attempts) {
            TASK_LOG(ERROR) << "state START_RSSI_MONITORING reached maximum attempts = " << attempts
                            << " aborting task!";
            TASK_LOG(WARNING) << "client " << sta_mac
                              << " is not monitored, and could not be steered!!!";
            finish();
        }
        break;
    }
    case REQUEST_RSSI_MEASUREMENT: {
        TASK_LOG(DEBUG) << "response for rx rssi measurement from " << original_parent_mac
                        << " for sta " << sta_mac << " timed out! attempts=" << attempts;
        if (attempts >= max_attempts) {
            /*
            * TODO
            * this shouldn't really happen unless the new IRE/hostap is malfunctioning
            * might require further handling
            */
            TASK_LOG(ERROR) << "state REQUEST_RSSI_MEASUREMENT reached maximum attempts="
                            << attempts << " aborting task!";
            finalize_new_connection();
            finish();
        }
        break;
    }
    case CHECK_11K_BEACON_MEASURE_CAP: {
        TASK_LOG(DEBUG) << "response for beacon measurement request from " << sta_mac
                        << " timed out! attempts=" << attempts;
        if (attempts >= max_attempts) {
            TASK_LOG(DEBUG) << "state CHECK_11K_BEACON_MEASURE_CAP reached maximum attempts="
                            << attempts << " setting sta " << sta_mac
                            << " as beacon measurement unsupported ";
            database.set_sta_beacon_measurement_support_level(sta_mac,
                                                              beerocks::BEACON_MEAS_UNSUPPORTED);
            state = REQUEST_RSSI_MEASUREMENT_WAIT;
        }
        break;
    }
    default: {
        TASK_LOG(ERROR) << "Unknown state: " << int(state);
        break;
    }
    }
}
