/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "son_management.h"
#include "son_actions.h"
#include "tasks/bml_task.h"
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
#include "tasks/pre_association_steering/pre_association_steering_task.h"
#endif
#include "db/network_map.h"
#include "tasks/channel_selection_task.h"
#include "tasks/dynamic_channel_selection_r2_task.h"
#include "tasks/dynamic_channel_selection_task.h"
#include "tasks/ire_network_optimization_task.h"
#include "tasks/load_balancer_task.h"
#include "tasks/optimal_path_task.h"
#include "tasks/statistics_polling_task.h"

#include <beerocks/tlvf/beerocks_message_1905_vs.h>
#include <beerocks/tlvf/beerocks_message_bml.h>
#include <beerocks/tlvf/beerocks_message_cli.h>

#include <tlvf/tlvftypes.h>
#include <tlvf/wfa_map/tlvClientAssociationControlRequest.h>
#include <tlvf/wfa_map/tlvUnassociatedStaLinkMetricsQuery.h>

#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_wifi_channel.h>
#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * @brief Check if the given channel pool fulfils the "all-channel" scan requirement.
 * @param channel_pool_set: Unordered set containing the current channel pool.
 * @return true if pool matches the "all-channel" scan requirement
 * @return false if not.
 */
bool is_scan_all_channels_request(std::unordered_set<uint8_t> &channel_pool_set)
{
    return channel_pool_set.size() == 1 && *channel_pool_set.begin() == SCAN_ALL_CHANNELS;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void son_management::handle_cli_message(int sd, std::shared_ptr<beerocks_header> beerocks_header,
                                        ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
                                        task_pool &tasks)
{
    bool isOK           = true;
    int8_t currentValue = -1; //ignore currentValue field in the response

    //LOG(DEBUG) << "NEW CLI action=" << int(header->action()) << " action_op=" << int(header->action_op());

    auto controller_ctx = database.get_controller_ctx();
    if (!controller_ctx) {
        LOG(ERROR) << "controller_ctx == nullptr";
        return;
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_CLI_HOSTAP_STATS_MEASUREMENT: {
        auto request_in =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_HOSTAP_STATS_MEASUREMENT>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_STATS_MEASUREMENT failed";
            isOK = false;
            break;
        }
        std::string hostap_mac = tlvf::mac_to_string(request_in->ap_mac());
        auto request_out       = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST>(cmdu_tx);
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }

        if (hostap_mac == network_utils::ZERO_MAC_STRING) {
            /*
                 * request from all hostaps
                 */
            LOG(DEBUG) << "CLI requesting stats measurements from all hostaps";
            auto hostaps = database.get_active_radios();
            for (auto hostap : hostaps) {
                LOG(DEBUG) << "CLI requesting stats measurement from " << hostap;
                son_actions::send_cmdu_to_agent(
                    database.get_radio_parent_agent(request_in->ap_mac()), cmdu_tx, database,
                    hostap);
            }
        } else {
            LOG(DEBUG) << "CLI requesting stats measurements from " << hostap_mac;
            const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
            son_actions::send_cmdu_to_agent(database.get_radio_parent_agent(request_in->ap_mac()),
                                            cmdu_tx, database, parent_radio);
        }
        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST: {
        auto cli_request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST failed";
            isOK = false;
            break;
        }
        auto hostap_mac = tlvf::mac_to_string(cli_request->ap_mac());
        auto agent_mac  = database.get_radio_parent_agent(cli_request->ap_mac());

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }
        memset(&request->params(), 0, sizeof(beerocks_message::sNeighborSetParams11k));
        request->params().channel = cli_request->channel();
        request->params().bssid   = cli_request->bssid();
        request->params().vap_id  = cli_request->vap_id();

        const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST: {
        auto cli_request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST failed";
            isOK = false;
            break;
        }
        std::string hostap_mac = tlvf::mac_to_string(cli_request->ap_mac());
        auto agent_mac         = database.get_bss_parent_agent(cli_request->ap_mac());

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            isOK = false;
            break;
        }
        memset(&request->params(), 0, sizeof(beerocks_message::sNeighborSetParams11k));
        request->params().bssid  = cli_request->bssid();
        request->params().vap_id = cli_request->vap_id();

        const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);

        break;
    }
    case beerocks_message::ACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS: {
        auto cli_request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS failed";
            isOK = false;
            break;
        }

        if (cli_request->attempts() != (int32_t)database.get_slave_stop_on_failure_attempts() &&
            cli_request->attempts() != -1) {
            database.set_slave_stop_on_failure_attempts(cli_request->attempts());
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST>(
                cmdu_tx);
            if (request == nullptr) {
                LOG(ERROR) << "Failed building message!";
                isOK = false;
                break;
            }
            request->attempts() = cli_request->attempts();

            for (const auto &agent : database.get_all_connected_agents()) {
                for (const auto &radio_map_elemet : agent->radios) {

                    auto radio = radio_map_elemet.second;
                    auto state = database.get_radio_state(radio->radio_uid);

                    if (state != beerocks::STATE_DISCONNECTED) {
                        son_actions::send_cmdu_to_agent(agent->al_mac, cmdu_tx, database,
                                                        tlvf::mac_to_string(radio->radio_uid));
                    }
                }
            }
        }
        currentValue = (int8_t)database.get_slave_stop_on_failure_attempts();
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_ALLOW_REQUEST: {
        auto cli_request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_CLIENT_ALLOW_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_ALLOW_REQUEST failed";
            isOK = false;
            break;
        }
        std::string client_mac = tlvf::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = tlvf::mac_to_string(cli_request->hostap_mac());
        LOG(DEBUG) << "CLI client allow request for " << client_mac << " to " << hostap_mac;

        auto agent_mac = database.get_bss_parent_agent(cli_request->hostap_mac());
        if (!cmdu_tx.create(0,
                            ieee1905_1::eMessageType::CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE)) {
            LOG(ERROR)
                << "cmdu creation of type CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE, has failed";
            isOK = false;
            break;
        }
        auto association_control_request_tlv =
            cmdu_tx.addClass<wfa_map::tlvClientAssociationControlRequest>();
        if (!association_control_request_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvClientAssociationControlRequest failed";
            isOK = false;
            break;
        }

        association_control_request_tlv->bssid_to_block_client() =
            tlvf::mac_from_string(hostap_mac);
        association_control_request_tlv->association_control() =
            wfa_map::tlvClientAssociationControlRequest::UNBLOCK;
        association_control_request_tlv->alloc_sta_list();
        auto sta_list         = association_control_request_tlv->sta_list(0);
        std::get<1>(sta_list) = tlvf::mac_from_string(client_mac);

        const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_CROSS_RX_RSSI_MEASUREMENT: {

        auto cli_request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CROSS_RX_RSSI_MEASUREMENT failed";
            isOK = false;
            break;
        }

        std::string client_mac          = tlvf::mac_to_string(cli_request->client_mac());
        std::string hostap_mac          = tlvf::mac_to_string(cli_request->hostap_mac());
        std::shared_ptr<Station> client = database.get_station(cli_request->client_mac());
        if (!client || !client->get_bss()) {
            break;
        }
        auto agent_mac = database.get_bss_parent_agent(cli_request->hostap_mac());

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST message!";
            isOK = false;
            break;
        }

        auto sta_parent_wifi_channel =
            database.get_radio_wifi_channel(client->get_bss()->radio.radio_uid);
        if (sta_parent_wifi_channel.is_empty()) {
            LOG(WARNING) << "empty wifi channel of " << client->get_bss()->radio.radio_uid
                         << " in DB";
        }
        request->params().mac     = cli_request->client_mac();
        request->params().ipv4    = network_utils::ipv4_from_string(network_utils::ZERO_IP_STRING);
        request->params().channel = sta_parent_wifi_channel.get_channel();
        request->params().bandwidth = sta_parent_wifi_channel.get_bandwidth();
        request->params().vht_center_frequency =
            cli_request->center_frequency() ? cli_request->center_frequency()
                                            : sta_parent_wifi_channel.get_center_frequency();
        request->params().cross                  = 0;
        request->params().mon_ping_burst_pkt_num = 0;

        const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
        LOG(DEBUG) << "CLI cross rx_rssi measurement request for sta=" << client_mac
                   << " hostap=" << hostap_mac;
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_DISALLOW_REQUEST: {
        auto cli_request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_CLIENT_DISALLOW_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_DISALLOW_REQUEST failed";
            isOK = false;
            break;
        }
        std::string client_mac = tlvf::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = tlvf::mac_to_string(cli_request->hostap_mac());
        LOG(DEBUG) << "CLI client disallow request for " << client_mac << " to " << hostap_mac;

        auto agent_mac = database.get_bss_parent_agent(cli_request->hostap_mac());
        if (!cmdu_tx.create(0,
                            ieee1905_1::eMessageType::CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE)) {
            LOG(ERROR)
                << "cmdu creation of type CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE, has failed";
            isOK = false;
            break;
        }

        auto association_control_request_tlv =
            cmdu_tx.addClass<wfa_map::tlvClientAssociationControlRequest>();
        if (!association_control_request_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvClientAssociationControlRequest failed";
            isOK = false;
            break;
        }

        association_control_request_tlv->bssid_to_block_client() =
            tlvf::mac_from_string(hostap_mac);
        association_control_request_tlv->association_control() =
            wfa_map::tlvClientAssociationControlRequest::TIMED_BLOCK;
        //TODO: Get real validity_period_sec
        association_control_request_tlv->validity_period_sec() = 1;
        association_control_request_tlv->alloc_sta_list();
        auto sta_list         = association_control_request_tlv->sta_list(0);
        std::get<1>(sta_list) = tlvf::mac_from_string(client_mac);

        const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_DISCONNECT_REQUEST: {
        auto cli_request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_CLIENT_DISCONNECT_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_DISCONNECT_REQUEST failed";
            isOK = false;
            break;
        }
        std::string client_mac = tlvf::mac_to_string(cli_request->client_mac());
        std::string hostap_mac = database.get_sta_parent(client_mac);
        LOG(DEBUG) << "CLI client disassociate request, client " << client_mac << " hostap "
                   << hostap_mac;

        son_actions::disconnect_client(database, cmdu_tx, client_mac, hostap_mac,
                                       cli_request->type(), cli_request->reason(),
                                       beerocks_message::eClient_Disconnect_Source_Beerocks_cli);
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_BEACON_11K_REQUEST: {
        auto cli_request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_CLIENT_BEACON_11K_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_CLI_CLIENT_BEACON_11K_REQUEST failed";
            isOK = false;
            break;
        }

        std::string client_mac = tlvf::mac_to_string(cli_request->client_mac());

        /////////////// FOR DEBUG ONLY ////////////////
        if (client_mac == network_utils::ZERO_MAC_STRING) {
            LOG(DEBUG) << "Updating beacon request params on database";
            optimal_path_task::cli_beacon_request_duration  = cli_request->duration();
            optimal_path_task::cli_beacon_request_rand_ival = cli_request->rand_ival();
            optimal_path_task::cli_beacon_request_mode =
                (beerocks::eMeasurementMode11K)cli_request->measurement_mode();
            break;
        } else if (client_mac == network_utils::WILD_MAC_STRING) {
            LOG(DEBUG) << "Updating beacon request params on database to default";
            optimal_path_task::cli_beacon_request_duration  = -1;
            optimal_path_task::cli_beacon_request_rand_ival = -1;
            optimal_path_task::cli_beacon_request_mode      = beerocks::MEASURE_MODE_UNDEFINED;
            break;
        }
        ///////////////////////////////////////////////

        std::string hostap_mac = database.get_sta_parent(client_mac);
        auto agent_mac         = database.get_bss_parent_agent(tlvf::mac_from_string(hostap_mac));
        LOG(DEBUG) << "CLI beacon request for " << client_mac << " to " << hostap_mac;

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_BEACON_11K_REQUEST message!";
            isOK = false;
            break;
        }
        //TODO: set params values
        request->params().measurement_mode =
            cli_request
                ->measurement_mode(); // son::eWiFiMeasurementMode11K. Should be replaced with string "passive"/"active"/"table"
        request->params().channel  = cli_request->channel();
        request->params().op_class = cli_request->op_class();
        request->params().repeats =
            cli_request
                ->repeats(); // '0' = once, '65535' = repeats until cancel request, other (1-65534) = specific num of repeats
        request->params().rand_ival =
            cli_request
                ->rand_ival(); // random interval - specifies the upper bound of the random delay to be used prior to making the measurement, expressed in units of TUs [=1024usec]
        request->params().duration =
            cli_request->duration(); // measurement duration, expressed in units of TUs [=1024usec]
        request->params().sta_mac = cli_request->client_mac();
        request->params().bssid =
            cli_request
                ->bssid(); // the bssid which will be reported. for all bssid, use wildcard "ff:ff:ff:ff:ff:ff"

        request->params().expected_reports_count = 1;

        // Optional:
        if (cli_request->use_optional_ssid()) {
            request->params().use_optional_ssid = 1; // bool
            string_utils::copy_string(request->params().ssid, (char *)cli_request->ssid(),
                                      beerocks::message::WIFI_SSID_MAX_LENGTH);
        } else {
            request->params().use_optional_ssid = 0;
        }

        const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);

        break;
    }
    case beerocks_message::ACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST: {
        auto cli_request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST failed";
            isOK = false;
            break;
        }
        std::string hostap_mac = tlvf::mac_to_string(cli_request->mac());
        auto agent_mac         = database.get_bss_parent_agent(cli_request->mac());
        LOG(DEBUG) << "CLI ap channel switch request for " << hostap_mac;

        /*
        Construct a wifiChannel object with given parameters
        to check if the requested channel switch parameters are a valid ones.
        */
        beerocks::WifiChannel wifi_channel(
            cli_request->cs_params().channel,
            son::wireless_utils::which_freq_type(cli_request->cs_params().vht_center_frequency),
            static_cast<beerocks::eWiFiBandwidth>(cli_request->cs_params().bandwidth));
        if (wifi_channel.is_empty()) {
            LOG(ERROR) << "Invalid channel switch request. channel="
                       << cli_request->cs_params().channel << ", bandwidth="
                       << beerocks::utils::convert_bandwidth_to_string(
                              static_cast<beerocks::eWiFiBandwidth>(
                                  cli_request->cs_params().bandwidth))
                       << ", center_frequency=" << cli_request->cs_params().vht_center_frequency;
            isOK = false;
            break;
        }

        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START message!";
            isOK = false;
            break;
        }

        request->cs_params() = cli_request->cs_params();

        const auto parent_radio = database.get_bss_parent_radio(hostap_mac);
        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
        break;
    }
    case beerocks_message::ACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS failed";
            isOK = false;
            break;
        }
        if (request->isEnable() >= 0) {
            database.settings_diagnostics_measurements(request->isEnable());
#ifndef BEEROCKS_LINUX
            controller_ctx->start_optional_tasks();
            // start_optional_tasks will start / stop diagnostics measurements task based on new settings;
#endif
            LOG(INFO) << "CLI load_diagnostics_measurements changed to "
                      << int(database.settings_diagnostics_measurements());
        }
        currentValue = database.settings_diagnostics_measurements();

        break;
    }
    case beerocks_message::ACTION_CLI_ENABLE_DEBUG: {
        auto request = beerocks_header->addClass<beerocks_message::cACTION_CLI_ENABLE_DEBUG>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_ENABLE_DEBUG failed";
            isOK = false;
            break;
        }

        if (request->isEnable() >= 0) {
            if (request->isEnable()) {
                database.add_cli_socket(sd);
            } else {
                database.remove_cli_socket(sd);
            }
        }
        currentValue = database.get_cli_debug_enable(sd);
        break;
    }
    case beerocks_message::ACTION_CLI_DUMP_NODE_INFO: {
        auto cli_request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_DUMP_NODE_INFO>();
        if (cli_request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_DUMP_NODE_INFO failed";
            isOK = false;
            break;
        }
        std::string mac       = tlvf::mac_to_string(cli_request->mac());
        std::string node_info = database.obj_to_string(cli_request->mac());
        std::size_t length    = node_info.size();
        LOG(TRACE) << "CLI dump node info for " << mac
                   << " node info size = " << std::to_string(length);

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_CLI_RESPONSE_STR>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_CLI_RESPONSE_STR message!";
            isOK = false;
            break;
        }

        //In case we don't have enough space for node length, reserve 1 byte for '\0'
        size_t reserved_size =
            (message_com::get_vs_cmdu_size_on_buffer<beerocks_message::cACTION_CLI_RESPONSE_STR>() -
             1);
        size_t max_size = cmdu_tx.getMessageBuffLength() - reserved_size;

        if (length == 0) {
            node_info = "Error: mac does not exist";
            length    = node_info.size();
        }

        size_t size = (length > max_size) ? max_size : length;

        if (!response->alloc_buffer(size)) {
            LOG(ERROR) << "Failed buffer allocation to size=" << int(size);
            isOK = false;
            break;
        }

        std::copy_n(node_info.c_str(), size, response->buffer(0));
        (response->buffer(0))[size] = 0;

        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CLI_OPTIMAL_PATH_TASK: {
        auto request = beerocks_header->addClass<beerocks_message::cACTION_CLI_OPTIMAL_PATH_TASK>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_OPTIMAL_PATH_TASK failed";
            isOK = false;
            break;
        }
        std::string client_mac = tlvf::mac_to_string(request->client_mac());

        auto client = database.get_station(tlvf::mac_from_string(client_mac));
        if (!client) {
            LOG(ERROR) << "client " << client_mac << " not found";
            isOK = false;
            break;
        }

        if (database.get_sta_state(client_mac) == beerocks::STATE_CONNECTED) {

            if (tasks.is_task_running(client->roaming_task_id)) {
                LOG(TRACE) << "CLI roaming task already running for " << client_mac;
            } else {
                LOG(TRACE) << "CLI start roaming task for " << client_mac;
                auto new_task = std::make_shared<optimal_path_task>(database, cmdu_tx, tasks,
                                                                    client_mac, 0, "");
                tasks.add_task(new_task);
            }
        } else {
            isOK = false;
        }
        break;
    }
    case beerocks_message::ACTION_CLI_LOAD_BALANCER_TASK: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_LOAD_BALANCER_TASK>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_LOAD_BALANCER_TASK failed";
            isOK = false;
            break;
        }
        std::string hostap_mac  = tlvf::mac_to_string(request->ap_mac());
        sMacAddr ire_mac        = database.get_bss_parent_agent(request->ap_mac());
        std::string ire_mac_str = tlvf::mac_to_string(ire_mac);
        LOG(TRACE) << "CLI load notification from hostap " << hostap_mac << " ire mac=" << ire_mac;

        /*
             * start load balancing
             */
        if (database.is_radio_active(tlvf::mac_from_string(hostap_mac)) &&
            database.get_agent_state(ire_mac) == beerocks::STATE_CONNECTED) {
            /*
                 * when a notification arrives, it means a large change in rx_rssi occurred (above the defined thershold)
                 * therefore, we need to create a load balancing task to optimize the network
                 */
            int prev_task_id = database.get_agent_load_balancer_task_id(ire_mac);
            if (tasks.is_task_running(prev_task_id)) {
                LOG(TRACE) << "CLI load balancer task already running for " << ire_mac;
            } else {
                auto new_task = std::make_shared<load_balancer_task>(database, cmdu_tx, tasks,
                                                                     ire_mac_str, "load_balancer");
                tasks.add_task(new_task);
            }
        } else {
            isOK = false;
        }
        break;
    }
    case beerocks_message::ACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK: {
        LOG(TRACE) << "CLI IRE network optimization scan triggered";
        auto new_task = std::make_shared<ire_network_optimization_task>(
            database, cmdu_tx, tasks, "cli - ire_network_optimization");
        tasks.add_task(new_task);
        break;
    }
    case beerocks_message::ACTION_CLI_BACKHAUL_ROAM_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_BACKHAUL_ROAM_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_BACKHAUL_ROAM_REQUEST failed";
            isOK = false;
            break;
        }
        std::string slave_mac  = tlvf::mac_to_string(request->slave_mac());
        std::string hostap_mac = tlvf::mac_to_string(request->bssid());
        std::string triggered_by{" On-Demand backhaul [imminent] CLI "};
        //TODO: we are passing true for imminent by default
        //extend ACTION_CLI_BACKHAUL_ROAM_REQUEST to have imminent variable
        uint8_t disassoc_imminent = uint8_t(1);

        LOG(DEBUG) << "CLI steer IRE request for " << slave_mac << " to hostap: " << hostap_mac;
        son_actions::steer_sta(database, cmdu_tx, tasks, slave_mac, hostap_mac, triggered_by,
                               std::string(), disassoc_imminent);
        break;
    }
    case beerocks_message::ACTION_CLI_CLIENT_BSS_STEER_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_CLI_CLIENT_BSS_STEER_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CLI_CLIENT_BSS_STEER_REQUEST failed";
            isOK = false;
            break;
        }
        std::string client_mac = tlvf::mac_to_string(request->client_mac());
        std::string hostap_mac = tlvf::mac_to_string(request->bssid());
        std::string triggered_by{" On-Demand CLI"};
        uint8_t disassoc_imminent = request->disassoc_timer_ms() ? uint8_t(1) : uint8_t(0);
        LOG(DEBUG) << "CLI steer sta request for " << client_mac << " to hostap: " << hostap_mac
                   << " disassoc_imminent=" << int(disassoc_imminent)
                   << " disassoc_timer=" << int(request->disassoc_timer_ms());
        son_actions::steer_sta(database, cmdu_tx, tasks, client_mac, hostap_mac, triggered_by,
                               std::string(), int(disassoc_imminent),
                               int(request->disassoc_timer_ms()));

        break;
    }
    default: {
        LOG(ERROR) << "Unsupported CLI action_op:" << int(beerocks_header->action_op());
        isOK = false;
        break;
    }
    }

    //Send response message
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_RESPONSE_INT>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return;
    }
    response->isOK()         = isOK;
    response->currentValue() = currentValue;
    controller_ctx->send_cmdu(sd, cmdu_tx);
}

void son_management::handle_bml_message(int sd, std::shared_ptr<beerocks_header> beerocks_header,
                                        ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
                                        task_pool &tasks)
{
    auto controller_ctx = database.get_controller_ctx();
    if (!controller_ctx) {
        LOG(ERROR) << "controller_ctx == nullptr";
        return;
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_BML_PING_REQUEST: {
        LOG(TRACE) << "ACTION_BML_PING_REQUEST";
        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_PING_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_PING_RESPONSE failed";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_REGISTER_TOPOLOGY_QUERY: {
        LOG(TRACE) << "ACTION_BML_REGISTER_TOPOLOGY_QUERY";

        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::REGISTER_TO_TOPOLOGY_UPDATES,
                         &new_event);
    } break;

    case beerocks_message::ACTION_BML_UNREGISTER_TOPOLOGY_QUERY: {
        LOG(TRACE) << "ACTION_BML_UNREGISTER_TOPOLOGY_QUERY";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::UNREGISTER_TO_TOPOLOGY_UPDATES,
                         &new_event);
    } break;

    case beerocks_message::ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST";

        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::REGISTER_TO_NW_MAP_UPDATES,
                         &new_event);
    } break;

    case beerocks_message::ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST";

        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::UNREGISTER_TO_NW_MAP_UPDATES,
                         &new_event);
    } break;

    case beerocks_message::ACTION_BML_NW_MAP_REQUEST: {
        LOG(TRACE) << "ACTION_BML_NW_MAP_REQUEST";
        network_map::send_bml_network_map_message(database, sd, cmdu_tx, beerocks_header->id());
    } break;

    case beerocks_message::ACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::REGISTER_TO_STATS_UPDATES,
                         &new_event);
    } break;

    case beerocks_message::ACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::UNREGISTER_TO_STATS_UPDATES,
                         &new_event);
    } break;

    case beerocks_message::ACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::REGISTER_TO_EVENTS_UPDATES,
                         &new_event);
    } break;

    case beerocks_message::ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST: {
        LOG(TRACE) << "ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST";
        bml_task::listener_general_register_unregister_event new_event;
        new_event.sd = sd;
        tasks.push_event(database.get_bml_task_id(), bml_task::UNREGISTER_TO_EVENTS_UPDATES,
                         &new_event);
    } break;

    case beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_CLIENT_ROAMING_REQUEST failed";
            break;
        }

        database.settings_client_optimal_path_roaming(request->isEnable());
        database.settings_client_11k_roaming(request->isEnable());
        LOG(INFO) << "BML client_optimal_path_roaming to "
                  << int(database.settings_client_optimal_path_roaming());
        LOG(INFO) << "BML client_11k_roaming to " << int(database.settings_client_11k_roaming());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_CLIENT_ROAMING_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_GET_CLIENT_ROAMING_RESPONSE failed";
            break;
        }
        response->isEnable() = database.settings_client_optimal_path_roaming();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_REQUEST failed";
            break;
        }

        database.settings_client_11k_roaming(request->isEnable());
        LOG(INFO) << "BML client_11k_roaming to " << database.settings_client_11k_roaming();

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE>(cmdu_tx);

        if (!response) {
            LOG(ERROR)
                << "Failed building ACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "addClass ACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE failed";
            break;
        }
        response->isEnable() = database.settings_client_11k_roaming();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST failed";
            break;
        }

        database.settings_legacy_client_roaming(request->isEnable());
        LOG(INFO) << "BML legacy_client_roaming to "
                  << int(database.settings_legacy_client_roaming());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE failed";
            break;
        }
        response->isEnable() = (database.settings_legacy_client_roaming());

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST: {
        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST failed";
            break;
        }

        database.settings_client_optimal_path_roaming_prefer_signal_strength(request->isEnable());
        LOG(INFO) << "BML settings_client_roaming_prefer_signal_strength to "
                  << int(database.settings_client_optimal_path_roaming_prefer_signal_strength());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE>(
            cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building "
                          "ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE>(
            cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building "
                          "ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE message!";
            break;
        }

        response->isEnable() =
            database.settings_client_optimal_path_roaming_prefer_signal_strength();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST failed";
            break;
        }

        database.settings_client_band_steering(request->isEnable());
        LOG(INFO) << "BML settings_client_band_steering changed to "
                  << int(database.settings_client_band_steering());
        database.settings_client_11k_roaming(request->isEnable());
        LOG(INFO) << "BML settings_client_11k_roaming changed to "
                  << int(database.settings_client_band_steering());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_client_band_steering();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_IRE_ROAMING_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_SET_IRE_ROAMING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_IRE_ROAMING_REQUEST failed";
            break;
        }

        database.settings_ire_roaming(request->isEnable());
        LOG(INFO) << "BML settings_client_band_steering changed to "
                  << int(database.settings_ire_roaming());

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_SET_IRE_ROAMING_RESPONSE>(
                cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_IRE_ROAMING_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_IRE_ROAMING_REQUEST: {
        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_GET_IRE_ROAMING_RESPONSE>(
                cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_IRE_ROAMING_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_ire_roaming();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_LOAD_BALANCER_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_SET_LOAD_BALANCER_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_LOAD_BALANCER_REQUEST failed";
            break;
        }

        database.settings_load_balancing(request->isEnable());
        LOG(INFO) << "BML settings_load_balancing changed to "
                  << int(database.settings_load_balancing());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_LOAD_BALANCER_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_LOAD_BALANCER_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_LOAD_BALANCER_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_LOAD_BALANCER_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_LOAD_BALANCER_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_load_balancing();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_SERVICE_FAIRNESS_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_SERVICE_FAIRNESS_REQUEST failed";
            break;
        }

        database.settings_service_fairness(request->isEnable());
        LOG(INFO) << "BML settings_service_fairness changed to "
                  << int(database.settings_service_fairness());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_SERVICE_FAIRNESS_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_service_fairness();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_DFS_REENTRY_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_SET_DFS_REENTRY_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_DFS_REENTRY_REQUEST failed";
            break;
        }

        database.settings_dfs_reentry(request->isEnable());
        LOG(INFO) << "BML settings_dfs_reentry changed to " << int(database.settings_dfs_reentry());

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_SET_DFS_REENTRY_RESPONSE>(
                cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_BML_SET_DFS_REENTRY_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_DFS_REENTRY_REQUEST: {
        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_BML_GET_DFS_REENTRY_RESPONSE>(
                cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_BML_GET_DFS_REENTRY_RESPONSE message!";
            break;
        }

        response->isEnable() = database.settings_dfs_reentry();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_CERTIFICATION_MODE_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_SET_CERTIFICATION_MODE_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_CERTIFICATION_MODE_REQUEST failed";
            break;
        }

        database.setting_certification_mode(request->isEnable());
        LOG(INFO) << "BML setting_certification_mode changed to "
                  << int(database.settings_dfs_reentry());

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_CERTIFICATION_MODE_REQUEST: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE message!";
            break;
        }

        response->isEnable() = database.setting_certification_mode();

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST: {
        LOG(TRACE) << "ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST failed";
            break;
        }

        //save new configured restricted channel to DB.
        if (request->params().is_global) {
            database.set_global_restricted_channels(request->params().restricted_channels);
        } else {
            database.set_radio_conf_restricted_channels(request->params().hostap_mac,
                                                        request->params().restricted_channels);
        }

        //send restricted channel event to channel selection task
        auto new_event        = new channel_selection_task::sConfiguredRestrictedChannels_event;
        new_event->hostap_mac = request->params().hostap_mac;
        std::copy_n(request->params().restricted_channels, sizeof(new_event->restricted_channels),
                    new_event->restricted_channels);
        tasks.push_event(database.get_channel_selection_task_id(),
                         (int)channel_selection_task::eEvent::CONFIGURED_RESTRICTED_CHANNELS_EVENT,
                         (void *)new_event);

        //send response to bml
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE message!";
            break;
        }

        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;

    case beerocks_message::ACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST: {
        LOG(TRACE) << "ACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST";
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE message!";
            break;
        }

        auto vec_restricted_channels =
            request->params().is_global
                ? database.get_global_restricted_channels()
                : database.get_radio_conf_restricted_channels(request->params().hostap_mac);
        std::copy(vec_restricted_channels.begin(), vec_restricted_channels.end(),
                  response->params().restricted_channels);

        //send response to bml
        controller_ctx->send_cmdu(sd, cmdu_tx);
    } break;
    case beerocks_message::ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST: {
        auto bml_request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST>();
        if (bml_request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST failed";
            break;
        }
        if (bml_request->params().module_name == beerocks::BEEROCKS_PROCESS_MASTER) {
            database.set_log_level_state((beerocks::eLogLevel)bml_request->params().log_level,
                                         bml_request->params().enable);
        } else {
            if (bml_request->params().module_name == beerocks::BEEROCKS_PROCESS_ALL) {
                database.set_log_level_state((eLogLevel)bml_request->params().log_level,
                                             bml_request->params().enable);
            }
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL>(cmdu_tx);

            if (request == nullptr) {
                LOG(ERROR) << "Failed building ACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL message!";
                break;
            }
            request->params().module_name = bml_request->params().module_name;
            request->params().log_level   = bml_request->params().log_level;
            request->params().enable      = bml_request->params().enable;

            std::string dst_mac = tlvf::mac_to_string(bml_request->params().mac);
            if (dst_mac == network_utils::WILD_MAC_STRING) {
                auto slaves = database.get_active_radios();
                for (const auto &slave : slaves) {
                    if (database.is_radio_active(tlvf::mac_from_string(slave))) {
                        auto agent_mac =
                            database.get_radio_parent_agent(tlvf::mac_from_string(slave));
                        son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, slave);
                    }
                }
            } else {
                auto agent_mac          = database.get_bss_parent_agent(bml_request->params().mac);
                const auto parent_radio = database.get_bss_parent_radio(dst_mac);
                son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
            }
        }
        // send response
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return;
        }
        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_WIFI_CREDENTIALS_SET_REQUEST: {
        LOG(TRACE) << "ACTION_BML_WIFI_CREDENTIALS_SET_REQUEST";
        son::wireless_utils::sBssInfoConf wifi_credentials;

        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST failed";
            return;
        }
        wifi_credentials.ssid                = request->ssid_str();
        wifi_credentials.network_key         = request->network_key_str();
        wifi_credentials.authentication_type = WSC::eWscAuth(request->authentication_type());
        wifi_credentials.encryption_type     = WSC::eWscEncr(request->encryption_type());
        wifi_credentials.fronthaul           = request->fronthaul();
        wifi_credentials.backhaul            = request->backhaul();

        auto operating_classes = request->operating_classes();
        for (int i = 0; i < request->operating_classes_size(); i++) {
            wifi_credentials.operating_class.push_back(operating_classes[i]);
        }

        LOG(DEBUG) << "Add wifi credentials to the database for AL-MAC: " << request->al_mac();

        database.add_bss_info_configuration(request->al_mac(), wifi_credentials);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE>(cmdu_tx,
                                                                         beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE ! ";
        } else {
            response->error_code() = 1;
            if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
                LOG(ERROR) << "Error sending cmdu message";
            }
        }
        break;
    }
    case beerocks_message::ACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST: {
        LOG(TRACE) << "ACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST failed";
            return;
        }

        auto al_mac = request->al_mac();
        database.clear_bss_info_configuration(al_mac);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE>(cmdu_tx,
                                                                           beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE ! ";
        } else {
            if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
                LOG(ERROR) << "Error sending cmdu message";
            }
        }
        break;
    }
    case beerocks_message::ACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST: {
        LOG(TRACE) << "ACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST";
        uint32_t ret = 0;
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST failed";
            return;
        }

        ret = son_actions::send_ap_config_renew_msg(cmdu_tx, database);
        if (!ret) {
            LOG(ERROR) << "Failed son_actions::send_ap_config_renew_msg ! ";
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE>(cmdu_tx,
                                                                            beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE ! ";
        } else {
            response->error_code() = ret;
            if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
                LOG(ERROR) << "Error sending cmdu message";
            }
        }
        break;
    }
    case beerocks_message::ACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST: {

        uint32_t result = 1; //1-fail 0-success

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST failed";
            return;
        }

        auto vap_list_size = request->vap_list_size();
        if (vap_list_size == 0) {
            LOG(WARNING) << "Received an empty list, clearing the list in the DB";
            database.clear_vap_list();
            result = 0;
        } else {

            LOG(INFO) << "Received " << int(vap_list_size) << " VAPs from BML";

            auto vap_list_tuple = request->vap_list(0);
            if (std::get<0>(vap_list_tuple) == false) {
                LOG(ERROR) << "vap list access fail!";
            } else {

                auto vap_list = std::make_shared<son::db::vaps_list_t>();

                auto bml_vaps = &std::get<1>(vap_list_tuple);
                for (uint8_t i = 0; i < vap_list_size; i++) {
                    vap_list->push_back(
                        std::make_shared<beerocks_message::sConfigVapInfo>(bml_vaps[i]));
                }

                // Store the list in the DB
                database.set_vap_list(vap_list);
                result = 0;
            }
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE>(cmdu_tx,
                                                                             beerocks_header->id());

        response->result() = result;

        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building message cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE ! ";
        } else {
            if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
                LOG(ERROR) << "Error sending cmdu message";
            }
        }
        break;
    }
    case beerocks_message::ACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST: {

        uint32_t result = 1; //1-fail 0-success

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE>(cmdu_tx,
                                                                             beerocks_header->id());

        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building message cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE ! ";
            return;
        }

        auto vap_list = database.get_vap_list();
        if (!vap_list || !vap_list->size()) {
            LOG(DEBUG) << "vap list is empty!";

            // Send a valid response with an empty list
            result = 0;

        } else {

            auto vap_list_size = vap_list->size();
            LOG(INFO) << "Returning " << vap_list_size << " VAPs to BML caller";

            if (response->alloc_vap_list(vap_list_size) == false) {
                LOG(ERROR) << "Failed buffer allocation to size = " << int(vap_list_size);
            } else {
                auto vap_list_tuple = response->vap_list(0);
                if (std::get<0>(vap_list_tuple) == false) {
                    LOG(ERROR) << "vap list access fail!";
                } else {
                    auto vaps = &std::get<1>(vap_list_tuple);
                    uint8_t i = 0;
                    for (auto vap : *vap_list) {
                        vaps[i] = *vap;
                        i++;
                    }

                    // Change the result value to success
                    result = 0;
                }
            }
        }

        response->result() = result;
        if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
            LOG(ERROR) << "Error sending get vaps list response message";
        }
        break;
    }
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    case beerocks_message::ACTION_BML_STEERING_SET_GROUP_REQUEST: {
        LOG(TRACE) << "ACTION_BML_STEERING_SET_GROUP_REQUEST";
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_STEERING_SET_GROUP_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass ACTION_BML_STEERING_SET_GROUP_REQUEST failed";
            break;
        }

        pre_association_steering_task::sSteeringSetGroupRequestEvent new_event;
        new_event.sd                 = sd;
        new_event.steeringGroupIndex = request->steeringGroupIndex();
        new_event.remove             = (request->ap_cfgs_length() > 0) ? 0 : 1;
        bool is_error                = false;
        for (size_t i = 0; i < request->ap_cfgs_length() / sizeof(sSteeringApConfig); i++) {
            if (!std::get<0>(request->ap_cfgs(i))) {
                LOG(ERROR) << "ACTION_BML_STEERING_SET_GROUP_REQUEST can't get AP Configuration No."
                           << i + 1;
                is_error = true;
                break;
            }
            new_event.ap_cfgs.push_back(std::get<1>(request->ap_cfgs(i)));
        }
        if (!is_error) {
            tasks.push_event(database.get_pre_association_steering_task_id(),
                             pre_association_steering_task::eEvents::STEERING_SET_GROUP_REQUEST,
                             &new_event);
        }
        break;
    }
    case beerocks_message::ACTION_BML_STEERING_CLIENT_SET_REQUEST: {
        LOG(TRACE) << "cACTION_BML_STEERING_CLIENT_SET_REQUEST";
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_SET_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_SET_REQUEST failed";
            break;
        }

        pre_association_steering_task::sSteeringClientSetRequestEvent new_event;
        //checking for remove option
        new_event.sd                 = sd;
        new_event.remove             = request->remove();
        new_event.steeringGroupIndex = request->steeringGroupIndex();
        new_event.bssid              = tlvf::mac_to_string(request->bssid());
        new_event.client_mac         = request->client_mac();
        new_event.config             = request->config();

        tasks.push_event(database.get_pre_association_steering_task_id(),
                         pre_association_steering_task::eEvents::STEERING_CLIENT_SET_REQUEST,
                         &new_event);
        break;
    }
    case beerocks_message::ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST: {
        LOG(TRACE) << "ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST";
        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST failed";
            break;
        }

        //TODO: set database to update sd as listener in pre_association_steering_task.
        pre_association_steering_task::sListenerGeneralRegisterUnregisterEvent new_event;
        new_event.sd = sd;
        if (request->unregister()) {
            tasks.push_event(database.get_pre_association_steering_task_id(),
                             pre_association_steering_task::eEvents::STEERING_EVENT_UNREGISTER,
                             &new_event);
        } else {
            tasks.push_event(database.get_pre_association_steering_task_id(),
                             pre_association_steering_task::eEvents::STEERING_EVENT_REGISTER,
                             &new_event);
        }

        break;
    }
    case beerocks_message::ACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST: {
        LOG(TRACE) << "ACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST";
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST failed";
            break;
        }

        pre_association_steering_task::sSteeringClientDisconnectRequestEvent new_event;
        new_event.sd                 = sd;
        new_event.client_mac         = request->client_mac();
        new_event.steeringGroupIndex = request->steeringGroupIndex();
        new_event.bssid              = tlvf::mac_to_string(request->bssid());
        new_event.type               = request->type();
        new_event.reason             = request->reason();

        tasks.push_event(database.get_pre_association_steering_task_id(),
                         pre_association_steering_task::eEvents::STEERING_CLIENT_DISCONNECT_REQUEST,
                         &new_event);
        break;
    }
    case beerocks_message::ACTION_BML_STEERING_CLIENT_MEASURE_REQUEST: {
        LOG(TRACE) << "ACTION_BML_STEERING_CLIENT_MEASURE_REQUEST";
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST failed";
            break;
        }
        std::shared_ptr<Station> client = database.get_station(request->client_mac());
        if (!client || !client->get_bss()) {
            break;
        }

        pre_association_steering_task::sSteeringRssiMeasurementRequestEvent new_event;
        new_event.sd = sd;

        auto sta_parent_wifi_channel =
            database.get_radio_wifi_channel(client->get_bss()->radio.radio_uid);
        if (sta_parent_wifi_channel.is_empty()) {
            LOG(WARNING) << "empty wifi channel of " << sta_parent_wifi_channel << " in DB";
            break;
        }
        new_event.bssid            = tlvf::mac_to_string(request->bssid());
        new_event.params.mac       = request->client_mac();
        new_event.params.ipv4      = network_utils::ipv4_from_string(network_utils::ZERO_IP_STRING);
        new_event.params.channel   = sta_parent_wifi_channel.get_channel();
        new_event.params.bandwidth = sta_parent_wifi_channel.get_bandwidth();
        new_event.params.vht_center_frequency   = sta_parent_wifi_channel.get_center_frequency();
        new_event.params.cross                  = 0;
        new_event.params.mon_ping_burst_pkt_num = 0;

        tasks.push_event(database.get_pre_association_steering_task_id(),
                         pre_association_steering_task::eEvents::STEERING_RSSI_MEASUREMENT_REQUEST,
                         &new_event);
        break;
    }
#endif //FEATURE_PRE_ASSOCIATION_STEERING

    case beerocks_message::ACTION_BML_TRIGGER_TOPOLOGY_QUERY: {

        auto bml_request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_TRIGGER_TOPOLOGY_QUERY>();

        auto al_mac = bml_request->al_mac();

        LOG(INFO) << "ACTION_BML_TRIGGER_TOPOLOGY_QUERY al_mac:" << al_mac;

        // In case bridge is not yet connected (bus not active) query will not
        // be sent to a local agent, sending empty bml response instead.
        if ((database.get_local_bridge_mac() == al_mac) &&
            (database.get_agent_state(al_mac) != beerocks::STATE_CONNECTED)) {
            LOG(WARNING) << "Bridge is not connected yet, TOPOLOGY_QUERY_MESSAGE will not be sent";
            auto response =
                message_com::create_vs_message<beerocks_message::cACTION_BML_TOPOLOGY_RESPONSE>(
                    cmdu_tx, beerocks_header->id());
            if (!response) {
                LOG(ERROR) << "Failed building message "
                              "cACTION_BML_TOPOLOGY_RESPONSE !";
            }
            response->result()             = false;
            response->device_data().al_mac = bml_request->al_mac();
            controller_ctx->send_cmdu(sd, cmdu_tx);
            return;
        }

        son_actions::send_topology_query_msg(al_mac, cmdu_tx, database);

    } break;

    case beerocks_message::ACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST: {

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_TRIGGER_CHANNEL_SELECTION_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building cACTION_BML_TRIGGER_CHANNEL_SELECTION_RESPONSE";
        }

        auto freq_type = database.get_radio_wifi_channel(request->radio_mac()).get_freq_type();

        LOG(INFO) << "ACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST "
                  << ", radio_mac=" << request->radio_mac() << ", channel=" << request->channel()
                  << ", bandwidth=" << request->bandwidth()
                  << ", csa_count=" << request->csa_count() << ", band type of radio mac= "
                  << beerocks::utils::convert_frequency_type_to_string(freq_type);

        if (!database.get_agent_by_radio_uid(request->radio_mac())) {
            response->code() = uint8_t(eChannelSwitchStatus::ERROR);
            controller_ctx->send_cmdu(sd, cmdu_tx);
            return;
        }

        bool need_new_preference = false;
        uint8_t operating_class  = 0;
        if (request->channel() != 0) {
            // Specific On-Demand Channel-Selection request

            // Get operating-class & check validity
            operating_class = wireless_utils::get_operating_class_by_channel(
                beerocks::WifiChannel(request->channel(), freq_type, request->bandwidth()));
            if (operating_class == 0) {
                LOG(ERROR) << "channel #" << request->channel() << " and bandwidth "
                           << beerocks::utils::convert_bandwidth_to_string(request->bandwidth())
                           << ", do not have a valid Operating Class";

                response->code() = uint8_t(eChannelSwitchStatus::INVALID_BANDWIDTH_AND_CHANNEL);
                controller_ctx->send_cmdu(sd, cmdu_tx);
                return;
            }

            // Check if preference has expired for the radio
            if (database.is_preference_reported_expired(request->radio_mac())) {
                LOG(DEBUG) << "Preference Report has expired, request new preference!";
                need_new_preference = true;
            } else {
                // Because preference is still valid, we need to check the channel's preference
                int8_t channel_preference = database.get_channel_preference(
                    request->radio_mac(), operating_class, request->channel());
                if (channel_preference <=
                    (int8_t)beerocks::eChannelPreferenceRankingConsts::NON_OPERABLE) {
                    LOG(ERROR) << "channel #" << request->channel() << " and bandwidth "
                               << beerocks::utils::convert_bandwidth_to_string(request->bandwidth())
                               << ", are "
                               << ((channel_preference ==
                                    (int8_t)beerocks::eChannelPreferenceRankingConsts::NON_OPERABLE)
                                       ? "Non-Operable"
                                       : "Invalid");

                    response->code() = uint8_t(eChannelSwitchStatus::INOPERABLE_CHANNEL);
                    controller_ctx->send_cmdu(sd, cmdu_tx);
                    return;
                }
            }
        } else {
            LOG(INFO) << "On-Demand-Auto Channel-Selection request detected";
        }

        // Send back response.
        response->code() = uint8_t(eChannelSwitchStatus::SUCCESS);
        controller_ctx->send_cmdu(sd, cmdu_tx);

        LOG(DEBUG) << "Sending Channel-Selection events to task";
        if (need_new_preference) {
            dynamic_channel_selection_r2_task::sPreferenceRequestEvent new_event;
            new_event.radio_mac = request->radio_mac();
            tasks.push_event(database.get_dynamic_channel_selection_r2_task_id(),
                             dynamic_channel_selection_r2_task::eEvent::REQUEST_NEW_PREFERENCE,
                             &new_event);
        }
        dynamic_channel_selection_r2_task::sOnDemandChannelSelectionEvent new_event;
        new_event.radio_mac       = request->radio_mac();
        new_event.channel         = request->channel();
        new_event.operating_class = operating_class;
        new_event.csa_count       = request->csa_count();
        tasks.push_event(
            database.get_dynamic_channel_selection_r2_task_id(),
            dynamic_channel_selection_r2_task::eEvent::TRIGGER_ON_DEMAND_CHANNEL_SELECTION,
            &new_event);
        break;
    }
    case beerocks_message::ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST";

        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE";
        }

        auto radio_mac         = request->radio_mac();
        auto dwell_time_ms     = request->params().dwell_time_ms;
        auto interval_time_sec = request->params().interval_time_sec;
        auto channel_pool      = request->params().channel_pool;
        auto channel_pool_size = request->params().channel_pool_size;

        LOG(DEBUG) << "request radio_mac:" << radio_mac;
        auto op_error_code = eChannelScanOperationCode::SUCCESS;

        if (dwell_time_ms != CHANNEL_SCAN_INVALID_PARAM &&
            op_error_code == eChannelScanOperationCode::SUCCESS) {
            if (!database.set_channel_scan_dwell_time_msec(radio_mac, dwell_time_ms, false)) {
                op_error_code = eChannelScanOperationCode::INVALID_PARAMS_DWELLTIME;
            }
        }
        if (interval_time_sec != CHANNEL_SCAN_INVALID_PARAM &&
            op_error_code == eChannelScanOperationCode::SUCCESS) {
            if (!database.set_channel_scan_interval_sec(radio_mac, interval_time_sec)) {
                op_error_code = eChannelScanOperationCode::INVALID_PARAMS_SCANTIME;
            }
        }
        if (channel_pool != nullptr && channel_pool_size != CHANNEL_SCAN_INVALID_PARAM &&
            op_error_code == eChannelScanOperationCode::SUCCESS) {
            auto channel_pool_set =
                std::unordered_set<uint8_t>(channel_pool, channel_pool + channel_pool_size);
            // Check if "all-channel" scan is requested
            if (is_scan_all_channels_request(channel_pool_set)) {
                if (!database.get_pool_of_all_supported_channels(channel_pool_set, radio_mac)) {
                    op_error_code = eChannelScanOperationCode::INVALID_PARAMS_CHANNELPOOL;
                }
            }
            // Set the channel pool
            if (op_error_code == eChannelScanOperationCode::SUCCESS) {
                if (!database.set_channel_scan_pool(radio_mac, channel_pool_set, false)) {
                    op_error_code = eChannelScanOperationCode::INVALID_PARAMS_CHANNELPOOL;
                }
            }
        }
        response->op_error_code() = uint8_t(op_error_code);
        //send response to bml
        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST";

        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE !";
        }

        auto radio_mac = request->radio_mac();
        response->params().dwell_time_ms =
            database.get_channel_scan_dwell_time_msec(radio_mac, false);
        response->params().interval_time_sec = database.get_channel_scan_interval_sec(radio_mac);
        auto &channel_pool                   = database.get_channel_scan_pool(radio_mac, false);
        std::copy(channel_pool.begin(), channel_pool.end(), response->params().channel_pool);
        response->params().channel_pool_size = channel_pool.size();
        LOG(DEBUG) << "request radio_mac:" << radio_mac;
        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST";

        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE !";
            return;
        }

        auto radio_mac = request->radio_mac();
        auto enable    = (request->isEnable() == 1);
        LOG(DEBUG) << "request radio_mac:" << radio_mac;
        bool success = database.set_channel_scan_is_enabled(radio_mac, enable);
        response->op_error_code() =
            uint8_t((success) ? eChannelScanOperationCode::SUCCESS
                              : eChannelScanOperationCode::INVALID_PARAMS_ENABLE);
        if (success) {
            dynamic_channel_selection_r2_task::sContinuousScanRequestStateChangeEvent new_event;
            new_event.enable    = enable;
            new_event.radio_mac = radio_mac;

            tasks.push_event(
                database.get_dynamic_channel_selection_r2_task_id(),
                (int)dynamic_channel_selection_r2_task::eEvent::CONTINUOUS_STATE_CHANGED_PER_RADIO,
                (void *)&new_event);
        }
        //send response to bml
        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST";

        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE !";
            return;
        }

        auto radio_mac = request->radio_mac();
        LOG(DEBUG) << "request radio_mac:" << radio_mac;
        response->isEnable() = (database.get_channel_scan_is_enabled(radio_mac)) ? 1 : 0;

        //send response to bml
        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST failed";
            break;
        }

        // Get information from request
        auto radio_mac      = request->radio_mac();
        auto is_single_scan = request->scan_mode() == 1;
        LOG(DEBUG) << "request radio_mac:" << radio_mac << ", "
                   << ((is_single_scan) ? "single-scan" : "continuous-scan");

        // Clear flags
        auto result_status = eChannelScanStatusCode::SUCCESS;
        auto op_error_code = eChannelScanOperationCode::SUCCESS;

        // Get scan statuses
        auto scan_in_progress = database.get_channel_scan_in_progress(radio_mac, is_single_scan);
        if (scan_in_progress) {
            LOG(DEBUG) << "Can't get scan results, scan is not finished!";
            op_error_code = eChannelScanOperationCode::SCAN_IN_PROGRESS;
        }

        auto last_scan_success =
            database.get_channel_scan_results_status(radio_mac, is_single_scan);
        if (last_scan_success != eChannelScanStatusCode::SUCCESS) {
            LOG(ERROR) << "Last scan did not finish successfully!";
            result_status = last_scan_success;
        }

        // Get results
        auto scan_results      = database.get_channel_scan_report(radio_mac, is_single_scan);
        auto scan_results_size = scan_results.size();

        LOG(DEBUG) << "scan_results received for hostap_mac= " << radio_mac << std::endl
                   << "scan_results_size= " << scan_results_size;
        auto gen_new_results_response = [&cmdu_tx, &beerocks_header]()
            -> std::shared_ptr<beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE> {
            auto res_msg = message_com::create_vs_message<
                beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE>(
                cmdu_tx, beerocks_header->id());
            return res_msg;
        };
        auto send_results_response =
            [&controller_ctx, &sd, &cmdu_tx](
                std::shared_ptr<beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE>
                    &res_msg,
                const eChannelScanStatusCode result_status,
                const eChannelScanOperationCode op_error_code, bool is_last = true) {
                res_msg->result_status() = uint8_t(result_status);
                res_msg->op_error_code() = uint8_t(op_error_code);
                res_msg->last()          = (is_last) ? 1 : 0;
                controller_ctx->send_cmdu(sd, cmdu_tx);
            };

        /**
         * If there was an error before, send the results with a failed status
         * No need to print errors on the following conditions
         * eChannelScanOperationCode::SCAN_IN_PROGRESS,
         * eChannelScanStatusCode::RESULTS_EMPTY
         */
        bool results_op_is_not_successful =
            (op_error_code != eChannelScanOperationCode::SUCCESS &&
             op_error_code != eChannelScanOperationCode::SCAN_IN_PROGRESS);
        bool results_are_invalid = (result_status != eChannelScanStatusCode::SUCCESS &&
                                    result_status != eChannelScanStatusCode::RESULTS_EMPTY);
        if (results_op_is_not_successful || results_are_invalid) {
            LOG(ERROR) << "Something went wrong, sending CMDU with error code: ["
                       << (int)op_error_code << "] & result status [" << (int)result_status << "].";
            auto response = gen_new_results_response();
            send_results_response(response, result_status, op_error_code);

            break;
        }

        //Results availability check
        if (scan_results_size == 0) {
            LOG(DEBUG) << "no scan results are available";
            auto response = gen_new_results_response();
            send_results_response(response, eChannelScanStatusCode::RESULTS_EMPTY, op_error_code);
            break;
        }

        size_t reserved_size = (message_com::get_vs_cmdu_size_on_buffer<
                                beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE>());
        auto response        = gen_new_results_response();
        size_t max_size      = cmdu_tx.getMessageBuffLength() - reserved_size;
        for (auto &dump : scan_results) {

            if (max_size < sizeof(dump)) {

                LOG(DEBUG) << "Reached limit on CMDU, Sending..";
                send_results_response(response, result_status, op_error_code, false);
                LOG(DEBUG) << "Creating new CMDU";
                response = gen_new_results_response();
                max_size = cmdu_tx.getMessageBuffLength() - reserved_size;
            }
            //LOG(DEBUG) << "Allocating space";
            if (!response->alloc_results()) {
                LOG(ERROR) << "Failed buffer allocation";
                op_error_code = eChannelScanOperationCode::ERROR;
                break;
            }
            max_size -= sizeof(dump);

            auto num_of_res = response->results_size();
            if (!std::get<0>(response->results(num_of_res - 1))) {
                LOG(ERROR) << "Failed accessing results buffer";
                op_error_code = eChannelScanOperationCode::ERROR;
                break;
            }
            auto &dump_msg = std::get<1>(response->results(num_of_res - 1));

            dump_msg = dump;
        }
        LOG(DEBUG) << "Finished all results, Sending final CMDU";
        send_results_response(response, result_status, op_error_code, true);
        break;
    }
    case beerocks_message::ACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE>(cmdu_tx,
                                                                            beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE !";
            return;
        }

        // Get single scan status
        auto single_scan_in_progress =
            database.get_channel_scan_in_progress(request->scan_params().radio_mac, true);
        if (single_scan_in_progress) {
            LOG(DEBUG) << "Single scan is still running!";
            response->op_error_code() = uint8_t(eChannelScanOperationCode::SCAN_IN_PROGRESS);
            controller_ctx->send_cmdu(sd, cmdu_tx);
            break;
        }

        auto radio_mac         = request->scan_params().radio_mac;
        auto dwell_time_ms     = request->scan_params().dwell_time_ms;
        auto channel_pool_size = request->scan_params().channel_pool_size;
        auto channel_pool      = request->scan_params().channel_pool;
        auto channel_pool_set =
            std::unordered_set<uint8_t>(channel_pool, channel_pool + channel_pool_size);

        LOG(DEBUG) << "set_channel_scan_dwell_time_msec " << dwell_time_ms;
        if (!database.set_channel_scan_dwell_time_msec(radio_mac, dwell_time_ms, true)) {
            LOG(ERROR) << "set_channel_scan_dwell_time_msec failed";
            response->op_error_code() =
                uint8_t(eChannelScanOperationCode::INVALID_PARAMS_DWELLTIME);
            controller_ctx->send_cmdu(sd, cmdu_tx);
            break;
        }

        if (is_scan_all_channels_request(channel_pool_set))
            if (!database.get_pool_of_all_supported_channels(channel_pool_set, radio_mac)) {
                LOG(ERROR) << "set_channel_scan_pool failed";
                response->op_error_code() =
                    uint8_t(eChannelScanOperationCode::INVALID_PARAMS_CHANNELPOOL);
                controller_ctx->send_cmdu(sd, cmdu_tx);
                break;
            }
        if (!database.set_channel_scan_pool(radio_mac, channel_pool_set, true)) {
            LOG(ERROR) << "set_channel_scan_pool failed";
            response->op_error_code() =
                uint8_t(eChannelScanOperationCode::INVALID_PARAMS_CHANNELPOOL);
            controller_ctx->send_cmdu(sd, cmdu_tx);
            break;
        }

        LOG(DEBUG) << "Clearing DB results for a single scan";
        if (!database.clear_channel_scan_results(radio_mac, true)) {
            LOG(ERROR) << "failed to clear scan results";
            response->op_error_code() = uint8_t(eChannelScanOperationCode::ERROR);
            controller_ctx->send_cmdu(sd, cmdu_tx);
            break;
        }

        LOG(DEBUG) << "Triggering Scan in task";
        dynamic_channel_selection_r2_task::sSingleScanRequestEvent new_event;
        new_event.radio_mac = radio_mac;
        tasks.push_event(database.get_dynamic_channel_selection_r2_task_id(),
                         dynamic_channel_selection_r2_task::eEvent::TRIGGER_SINGLE_SCAN,
                         &new_event);
        response->op_error_code() = uint8_t(eChannelScanOperationCode::SUCCESS);
        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE !";
            break;
        }

        auto send_response = [&](bool result) {
            //TODO: replace all BML_CLIENT requests to use boolean: (0=failure, 1-=success)
            response->result() = (result) ? 0 : 1;
            if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
                LOG(ERROR) << "Error sending get client list response message";
            }
        };

        auto client_list = database.get_clients_with_persistent_data_configured();
        if (client_list.empty()) {
            LOG(DEBUG) << "client list is empty!";
            // Send a valid response with an empty list
            send_response(true);
            break;
        }

        auto client_list_size = client_list.size();
        LOG(INFO) << "Returning " << client_list_size << " clients to BML caller";

        if (!response->alloc_client_list(client_list_size)) {
            LOG(ERROR) << "Failed buffer allocation to size = " << int(client_list_size);
            send_response(false);
            break;
        }

        auto client_list_tuple = response->client_list(0);
        if (!std::get<0>(client_list_tuple)) {
            LOG(ERROR) << "client list access fail!";
            send_response(false);
            break;
        }

        auto clients = &std::get<1>(client_list_tuple);
        for (size_t i = 0; i < client_list.size(); i++) {
            clients[i] = client_list[i];
        }

        send_response(true);
        break;
    }
    case beerocks_message::ACTION_BML_CLIENT_SET_CLIENT_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CLIENT_SET_CLIENT_REQUEST";

        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_CLIENT_SET_CLIENT_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CLIENT_SET_CLIENT_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CLIENT_SET_CLIENT_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_CLIENT_SET_CLIENT_RESPONSE !";
            break;
        }

        auto send_response = [&](bool result) -> bool {
            response->result() = (result) ? 0 : 1;
            return controller_ctx->send_cmdu(sd, cmdu_tx);
        };

        // If none of the parameters is configured - return error.
        // This flow should be blocked by the BML interface and should not be reached.
        if ((request->client_config().stay_on_initial_radio == PARAMETER_NOT_CONFIGURED) &&
            (request->client_config().stay_on_selected_device == PARAMETER_NOT_CONFIGURED) &&
            (request->client_config().selected_bands == PARAMETER_NOT_CONFIGURED) &&
            (request->client_config().time_life_delay_minutes == PARAMETER_NOT_CONFIGURED)) {
            LOG(ERROR)
                << "Received ACTION_BML_CLIENT_SET_CLIENT request without parameters to configure";
            send_response(false);
            break;
        }

        // Get client mac.
        auto client_mac = request->sta_mac();

        // If client doesn't have node in runtime DB - add node to runtime DB.
        if (!database.has_station(client_mac)) {
            LOG(DEBUG) << "Setting a client which doesn't exist in DB, adding client to DB";
            if (!database.add_station(network_utils::ZERO_MAC, client_mac)) {
                LOG(ERROR) << "Failed to add client node for client " << client_mac;
                send_response(false);
                break;
            }
        }

        // Get Station object
        auto client = database.get_station(client_mac);
        if (!client) {
            LOG(ERROR) << "client " << client_mac << " not found";
            break;
        }

        // Set stay_on_initial_radio if requested.
        if (request->client_config().stay_on_initial_radio != PARAMETER_NOT_CONFIGURED) {
            auto stay_on_initial_radio =
                (eTriStateBool(request->client_config().stay_on_initial_radio) ==
                 eTriStateBool::TRUE);
            if (!database.set_sta_stay_on_initial_radio(*client, stay_on_initial_radio, false)) {
                LOG(ERROR) << " Failed to set stay-on-initial-radio to " << stay_on_initial_radio
                           << " for client " << client_mac;
                send_response(false);
                break;
            }
        }

        // Set stay_on_selected_device if requested.
        if (request->client_config().stay_on_selected_device != PARAMETER_NOT_CONFIGURED) {
            LOG(DEBUG)
                << "The stay-on-selected-device configuration is not yet supported in the DB";

            // TODO: add stay_on_selected_device support in the persistent DB.
            // auto stay_on_selected_device =
            //     (eTriStateBool(request->client_config().stay_on_selected_device) ==
            //      eTriStateBool::TRUE);
            // if (!database.set_client_stay_on_selected_device(client_mac, stay_on_selected_device,
            //                                                  false)) {
            //     LOG(ERROR) << " Failed to set stay-on-selected-device to "
            //                << stay_on_selected_device << " for client " << client_mac;
            //     send_response(false);
            //     break;
            // }
        }

        // Set selected_bands if requested.
        if (request->client_config().selected_bands != PARAMETER_NOT_CONFIGURED) {
            auto selected_bands = eClientSelectedBands(request->client_config().selected_bands);
            if (!database.set_sta_selected_bands(*client, selected_bands, false)) {
                LOG(ERROR) << " Failed to set selected-bands to " << selected_bands
                           << " for client " << client_mac;
                send_response(false);
                break;
            }
        }

        // Set time_life_delay_minutes if requested.
        if (request->client_config().time_life_delay_minutes != PARAMETER_NOT_CONFIGURED) {
            auto time_life_delay_minutes =
                std::chrono::minutes(request->client_config().time_life_delay_minutes);
            if (!database.set_client_time_life_delay(*client, time_life_delay_minutes, false)) {
                LOG(ERROR) << " Failed to set max-time-life for client " << client_mac;
                send_response(false);
                break;
            }
        }

        //if persistent_db is enabled, call the "update_client_persistent_db"
        if (database.config.persistent_db) {
            if (!database.update_client_persistent_db(*client)) {
                LOG(ERROR)
                    << "Information is saved to runtime-DB but failed to set to persistent DB";
                send_response(false);
                break;
            }
        }

        send_response(true);
        break;
    }
    case beerocks_message::ACTION_BML_CLIENT_GET_CLIENT_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CLIENT_GET_CLIENT_REQUEST";

        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_CLIENT_GET_CLIENT_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CLIENT_GET_CLIENT_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CLIENT_GET_CLIENT_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_CLIENT_GET_CLIENT_RESPONSE !";
            break;
        }

        auto client_mac = request->sta_mac();
        auto client     = database.get_station(client_mac);
        if (!database.has_station(client_mac) || !client) {
            LOG(DEBUG) << "Requested client " << client_mac << " is not listed in the DB";
            response->result() = 1; //Fail.
            controller_ctx->send_cmdu(sd, cmdu_tx);
            break;
        }

        // A configured client must have a valid timestamp configured
        auto client_timestamp = client->parameters_last_edit;
        if (client_timestamp == std::chrono::system_clock::time_point::min()) {
            LOG(DEBUG) << "Requested client " << client_mac
                       << " doesn't have a valid timestamp listed in the DB";
            response->result() = 1; //Fail.
            controller_ctx->send_cmdu(sd, cmdu_tx);
            break;
        }

        // Client mac
        response->client().sta_mac = client_mac;
        // Timestamp
        response->client().timestamp_sec = client_timestamp.time_since_epoch().count();
        // Stay on initial radio
        response->client().stay_on_initial_radio = int(client->stay_on_initial_radio);
        // Initial radio
        response->client().initial_radio = client->initial_radio;
        // Selected bands
        response->client().selected_bands =
            static_cast<eClientSelectedBands>(client->selected_bands);
        // Timelife Delay in minutes
        response->client().time_life_delay_minutes =
            static_cast<int>(client->time_life_delay_minutes.count());

        // Currently not supported in DB
        // Stay on selected device
        response->client().stay_on_selected_device = PARAMETER_NOT_CONFIGURED;
        // Does client support only single band
        response->client().single_band = PARAMETER_NOT_CONFIGURED;

        response->result() = 0; //Success.
        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST: {
        LOG(TRACE) << "ACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST";
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_CLIENT_CLEAR_CLIENT_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_CLIENT_CLEAR_CLIENT_RESPONSE !";
            break;
        }

        auto mac = request->sta_mac();

        response->result() = database.clear_client_persistent_db(mac) ? 0 : 1;
        if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
            LOG(ERROR) << "Error sending clear client response message for mac= " << mac;
        }
        break;
    }
    case beerocks_message::ACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST: {
        LOG(TRACE) << "ACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST";
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_SET_SELECTION_CHANNEL_POOL_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_SET_SELECTION_CHANNEL_POOL_RESPONSE !";
            break;
        }

        auto radio_mac         = request->radio_mac();
        auto channel_pool      = request->channel_pool();
        auto channel_pool_size = request->channel_pool_size();
        auto channel_pool_set =
            std::unordered_set<uint8_t>(channel_pool, channel_pool + channel_pool_size);
        std::stringstream ss;
        for (auto channel : channel_pool_set) {
            ss << (int)channel << " ";
        }
        LOG(INFO) << "Setting channel pool to: " << ss.str();
        response->success() =
            (database.set_selection_channel_pool(radio_mac, channel_pool_set) ? 0 : -1);

        LOG(INFO) << "Sending SET_SELECTION_CHANNEL_POOL_RESPONSE with response "
                  << (int)response->success();
        if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
            LOG(ERROR) << "Error sending response message";
        }
        break;
    }
    case beerocks_message::ACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST: {
        LOG(TRACE) << "ACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST";
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_SELECTION_CHANNEL_POOL_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_GET_SELECTION_CHANNEL_POOL_RESPONSE !";
            break;
        }

        auto radio_mac = request->radio_mac();
        std::unordered_set<uint8_t> channel_pool_set;
        if (!database.get_selection_channel_pool(radio_mac, channel_pool_set)) {
            LOG(INFO) << "Failed to get Selection Channel-Pool.";
            response->success() = 1; //Failure.
        } else {
            LOG(INFO) << "Got Selection Channel-Pool successfully.";
            response->success() = 0; //Success.
            std::vector<uint8_t> tmp_vec(channel_pool_set.begin(), channel_pool_set.end());
            response->set_channel_pool(tmp_vec.data(), tmp_vec.size());
            response->channel_pool_size() = tmp_vec.size();
        }
        LOG(INFO) << "Sending GET_SELECTION_CHANNEL_POOL_RESPONSE";
        if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
            LOG(ERROR) << "Error sending response message";
        }
        break;
    }
    case beerocks_message::ACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST: {

        LOG(DEBUG) << "received ACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST failed";
            break;
        }

        controller_ctx->add_unassociated_station(request->mac_address(), request->channel(),
                                                 request->operating_class(),
                                                 request->agent_mac_address());
        break;
    }
    case beerocks_message::ACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST: {
        LOG(DEBUG) << "received ACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST";

        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST failed";
            break;
        }

        controller_ctx->remove_unassociated_station(request->mac_address(),
                                                    request->agent_mac_address());
        break;
    }

    case beerocks_message::ACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST: {
        LOG(TRACE) << "ACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST failed";
            break;
        }

        //send a request to gather the newest stats from agents
        controller_ctx->get_unassociated_stations_stats();

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_RESPONSE !";
            break;
        }

        auto un_stations_stats = database.get_unassociated_stations_stats();
        size_t size            = un_stations_stats.size();
        if (!response->alloc_sta_list(size)) {
            LOG(ERROR) << " failed allocating memory or size " << un_stations_stats.size();
            break;
        }

        size_t count = 0;
        for (auto &un_station : un_stations_stats) {
            auto &out_stat               = std::get<1>(response->sta_list(count));
            out_stat.sta_mac             = tlvf::mac_from_string(un_station.first);
            out_stat.uplink_rcpi_dbm_enc = un_station.second->uplink_rcpi_dbm_enc;
            snprintf(out_stat.time_stamp, sizeof(out_stat.time_stamp), "%s",
                     un_station.second->time_stamp.c_str());
            //Also logging it
            LOG(TRACE) << " Station with mac address : " << out_stat.sta_mac
                       << " has a signal strength of " << out_stat.uplink_rcpi_dbm_enc
                       << " TimeStamp: " << out_stat.time_stamp;
            count++;
        }

        if (!controller_ctx->send_cmdu(sd, cmdu_tx)) {
            LOG(ERROR) << "Error cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_RESPONSE message";
        }
        break;
    }
    case beerocks_message::ACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST: {

        LOG(DEBUG) << "ACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_UNASSOC_STA_RCPI_QUERY_RESPONSE>(cmdu_tx,
                                                                           beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_UNASSOC_STA_RCPI_QUERY_RESPONSE !";
            return;
        }
        response->op_error_code() = uint8_t(eUnAssocStaLinkMetricErrCode::SUCCESS);

        //send response to bml
        controller_ctx->send_cmdu(sd, cmdu_tx);

        auto sta_mac = request->sta_mac();
        auto opclass = request->opclass();
        auto channel = request->channel();
        LOG(DEBUG) << "request for unassociated sta_mac: " << sta_mac;
        LinkMetricsTask::sUnAssociatedLinkMetricsQueryEvent new_event;
        new_event.channel         = channel;
        new_event.opClass         = opclass;
        new_event.unassoc_sta_mac = sta_mac;

        tasks.push_event(database.get_link_metrics_task_id(),
                         (int)LinkMetricsTask::eEvent::UNASSOC_STA_LINK_METRICS_QUERY,
                         (void *)&new_event);
        break;
    }
    case beerocks_message::ACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST: {

        LOG(DEBUG) << "ACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST failed";
            break;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message "
                          "cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_RESPONSE !";
            return;
        }
        response->op_error_code() =
            uint8_t(eUnAssocStaLinkMetricErrCode::LINK_METRICS_COLLECTION_NOT_DONE);
        if (database.m_measurement_done) {
            response->op_error_code() =
                uint8_t(eUnAssocStaLinkMetricErrCode::RESULT_NOT_AVAILABLE_FOR_STA);
            auto &unassoc_sta_metric = database.get_unassoc_sta_map();
            std::string mac          = tlvf::mac_to_string(request->sta_mac());
            auto it                  = unassoc_sta_metric.find(mac);
            if (it != unassoc_sta_metric.end()) {
                response->op_error_code()     = uint8_t(eUnAssocStaLinkMetricErrCode::SUCCESS);
                auto sta                      = it->second;
                response->sta_mac()           = request->sta_mac();
                response->opclass()           = database.m_opclass;
                response->channel()           = sta.channel;
                response->rcpi()              = sta.rcpi;
                response->measurement_delta() = sta.measurement_delta;
            }
        }

        //send response to bml
        controller_ctx->send_cmdu(sd, cmdu_tx);
        break;
    }

    default: {
        LOG(ERROR) << "Unsupported BML action_op:" << int(beerocks_header->action_op());
        break;
    }
    }
}
