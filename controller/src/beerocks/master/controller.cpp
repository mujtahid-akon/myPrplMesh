/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2022 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "controller.h"
#include "periodic/persistent_data_commit_operation.h"
#include "periodic/persistent_database_aging.h"
#include "son_actions.h"
#include "son_management.h"
#include "tasks/agent_monitoring_task.h"
#include "tasks/bml_task.h"
#include "tasks/btm_request_task.h"
#include "tasks/client_association_task.h"
#include "tasks/client_steering_task.h"
#include "tasks/dhcp_task.h"
#include "tasks/load_balancer_task.h"
#include "tasks/optimal_path_task.h"
#include "tasks/service_prioritization_task.h"
#include "tasks/topology_task.h"
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
#include "tasks/pre_association_steering/pre_association_steering_task.h"
#endif
#include "db/db_algo.h"
#include "db/network_map.h"
#include "tasks/client_locating_task.h"
#include "tasks/dynamic_channel_selection_task.h"

#include <bcl/beerocks_backport.h>
#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_version.h>
#include <bcl/beerocks_wifi_channel.h>
#include <bcl/network/sockets.h>
#include <bcl/son/son_wireless_utils.h>
#include <bcl/transaction.h>
#include <bpl/bpl_board.h>
#include <bpl/bpl_cfg.h>

#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message_1905_vs.h>
#include <beerocks/tlvf/beerocks_message_control.h>

#include <tlvf/AssociationRequestFrame/AssocReqFrame.h>
#include <tlvf/ieee_1905_1/eMessageType.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddress.h>
#include <tlvf/ieee_1905_1/tlvAutoconfigFreqBand.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <tlvf/ieee_1905_1/tlvSearchedRole.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/wfa_map/tlv1905LayerSecurityCapability.h>
#include <tlvf/wfa_map/tlvAgentApMldConfiguration.h>
#include <tlvf/wfa_map/tlvAkmSuiteCapabilities.h>
#include <tlvf/wfa_map/tlvApCapability.h>
#include <tlvf/wfa_map/tlvApExtendedMetrics.h>
#include <tlvf/wfa_map/tlvApMetrics.h>
#include <tlvf/wfa_map/tlvApRadioIdentifier.h>
#include <tlvf/wfa_map/tlvApVhtCapabilities.h>
#include <tlvf/wfa_map/tlvApWifi6Capabilities.h>
#include <tlvf/wfa_map/tlvAssociatedStaLinkMetrics.h>
#include <tlvf/wfa_map/tlvAssociatedStaTrafficStats.h>
#include <tlvf/wfa_map/tlvAssociatedWiFi6StaStatusReport.h>
#include <tlvf/wfa_map/tlvBackhaulStaRadioCapabilities.h>
#include <tlvf/wfa_map/tlvBackhaulSteeringResponse.h>
#include <tlvf/wfa_map/tlvBssid.h>
#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/wfa_map/tlvChannelScanCapabilities.h>
#include <tlvf/wfa_map/tlvChannelSelectionResponse.h>
#include <tlvf/wfa_map/tlvClientCapabilityReport.h>
#include <tlvf/wfa_map/tlvClientInfo.h>
#include <tlvf/wfa_map/tlvControllerCapability.h>
#include <tlvf/wfa_map/tlvDeviceInventory.h>
#include <tlvf/wfa_map/tlvDscpMappingTable.h>
#include <tlvf/wfa_map/tlvEHTOperations.h>
#include <tlvf/wfa_map/tlvErrorCode.h>
#include <tlvf/wfa_map/tlvHigherLayerData.h>
#include <tlvf/wfa_map/tlvOperatingChannelReport.h>
#include <tlvf/wfa_map/tlvProfile2ApCapability.h>
#include <tlvf/wfa_map/tlvProfile2ApRadioAdvancedCapabilities.h>
#include <tlvf/wfa_map/tlvProfile2CacCapabilities.h>
#include <tlvf/wfa_map/tlvProfile2CacStatusReport.h>
#include <tlvf/wfa_map/tlvProfile2ChannelScanResult.h>
#include <tlvf/wfa_map/tlvProfile2MultiApProfile.h>
#include <tlvf/wfa_map/tlvProfile2RadioMetrics.h>
#include <tlvf/wfa_map/tlvProfile2ReasonCode.h>
#include <tlvf/wfa_map/tlvProfile2StatusCode.h>
#include <tlvf/wfa_map/tlvQoSManagementDescriptor.h>
#include <tlvf/wfa_map/tlvRadioOperationRestriction.h>
#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/wfa_map/tlvServicePrioritizationRule.h>
#include <tlvf/wfa_map/tlvSpatialReuseReport.h>
#include <tlvf/wfa_map/tlvSpatialReuseRequest.h>
#include <tlvf/wfa_map/tlvStaMacAddressType.h>
#include <tlvf/wfa_map/tlvSteeringBTMReport.h>
#include <tlvf/wfa_map/tlvSupportedService.h>
#include <tlvf/wfa_map/tlvTimestamp.h>
#include <tlvf/wfa_map/tlvTunnelledData.h>
#include <tlvf/wfa_map/tlvTunnelledProtocolType.h>
#include <tlvf/wfa_map/tlvTunnelledSourceInfo.h>
#include <tlvf/wfa_map/tlvUnassociatedStaLinkMetricsQuery.h>
#include <tlvf/wfa_map/tlvWifi7AgentCapabilities.h>

#include <net/if.h> // if_nametoindex

#ifdef ENABLE_VBSS
#include "../../../vbss/vbss_actions.h"
#include "../../../vbss/vbss_task.h"
#endif
namespace son {

/**
 * Time between successive timer executions of the tasks timer
 */
constexpr auto tasks_timer_period = std::chrono::milliseconds(250);

/**
 * Time between successive timer executions of the operations timer
 */
constexpr auto operations_timer_period = std::chrono::milliseconds(1000);

Controller::Controller(db &database_,
                       std::unique_ptr<beerocks::btl::BrokerClientFactory> broker_client_factory,
                       std::unique_ptr<beerocks::UccServer> ucc_server,
                       std::unique_ptr<beerocks::CmduServer> cmdu_server,
                       std::shared_ptr<beerocks::TimerManager> timer_manager,
                       std::shared_ptr<beerocks::EventLoop> event_loop)
    : cmdu_tx(m_tx_buffer, sizeof(m_tx_buffer)),
      cert_cmdu_tx(m_cert_tx_buffer, sizeof(m_cert_tx_buffer)), database(database_),
      m_controller_ucc_listener(database_, cert_cmdu_tx, std::move(ucc_server)),
      m_broker_client_factory(std::move(broker_client_factory)),
      m_cmdu_server(std::move(cmdu_server)), m_timer_manager(timer_manager),
      m_event_loop(event_loop)
{
    LOG_IF(!m_broker_client_factory, FATAL) << "Broker client factory is a null pointer!";
    LOG_IF(!m_cmdu_server, FATAL) << "CMDU server is a null pointer!";
    LOG_IF(!m_timer_manager, FATAL) << "Timer manager is a null pointer!";
    LOG_IF(!m_event_loop, FATAL) << "Event loop is a null pointer!";

    database.set_controller_ctx(this);

    start_mandatory_tasks();
    start_optional_tasks();

    beerocks::CmduServer::EventHandlers handlers{
        .on_client_connected    = [&](int fd) { handle_connected(fd); },
        .on_client_disconnected = [&](int fd) { handle_disconnected(fd); },
        .on_cmdu_received =
            [&](int fd, uint32_t iface_index, const sMacAddr &dst_mac, const sMacAddr &src_mac,
                ieee1905_1::CmduMessageRx &cmdu_rx) {
                handle_cmdu(fd, iface_index, dst_mac, src_mac, cmdu_rx);
            },
    };
    m_cmdu_server->set_handlers(handlers);
}

Controller::~Controller()
{
    m_cmdu_server->clear_handlers();

    LOG(DEBUG) << "closing";
}

bool Controller::start()
{
    // In case of error in one of the steps of this method, we have to undo all the previous steps
    // (like when rolling back a database transaction, where either all steps get executed or none
    // of them gets executed)
    beerocks::Transaction transaction;

    LOG(DEBUG) << "persistent db enable=" << database.config.persistent_db;
    if (database.config.persistent_db) {
        LOG(DEBUG) << "loading clients from persistent db";
        if (!database.load_persistent_db_clients()) {
            LOG(WARNING) << "failed to load clients from persistent db";
        } else {
            LOG(DEBUG) << "load clients from persistent db finished successfully";
        }
        if (!database.restore_steer_history()) {
            LOG(WARNING) << "Failed to load steer history from persistent db or no entries found.";
        } else {
            LOG(DEBUG) << "Load steer history from persistent db finished successfully";
        }

        if (operations.is_operation_alive(database.get_persistent_db_aging_operation_id())) {
            LOG(DEBUG) << "persistent DB aging operation already running";
        } else {
            auto aging_interval_seconds =
                std::chrono::seconds(database.config.persistent_db_aging_interval);
            auto new_operation = std::make_shared<persistent_database_aging_operation>(
                aging_interval_seconds, database);
            operations.add_operation(new_operation);
        }

        if (operations.is_operation_alive(database.get_persistent_db_data_commit_operation_id())) {
            LOG(DEBUG) << "persistent DB data commit operation already running";
        } else {
            auto commit_interval_seconds =
                std::chrono::seconds(database.config.persistent_db_commit_changes_interval_seconds);
            auto commit_operation = std::make_shared<persistent_data_commit_operation>(
                database, commit_interval_seconds);
            operations.add_operation(commit_operation);
        }
    }

    // GW & GW Switch nodes are need to be added in case of Controller only mode
    // Normally node/database objects are added with SLAVE JOIN messages
    // In case of Controller only mode, prplMesh agent will not start JOIN process.
    if (database.config.management_mode == BPL_MGMT_MODE_MULTIAP_CONTROLLER) {
        LOG(INFO) << "Controller only Mode is selected. Add GW node to database";

        auto agent = database.m_agents.add(database.get_local_bridge_mac());
        database.set_prplmesh(database.get_local_bridge_mac());
        database.set_agent_manufacturer(*agent, "prplMesh");
        agent->is_gateway = true;

        auto eth_switch_mac = beerocks::net::network_utils::get_eth_sw_mac_from_bridge_mac(
            database.get_local_bridge_mac());
        auto eth_switch_mac_str = tlvf::mac_to_string(eth_switch_mac);
        database.add_eth_switch(eth_switch_mac, database.get_local_bridge_mac());
        database.set_eth_switch_state(eth_switch_mac_str, beerocks::STATE_CONNECTED);
        database.set_eth_switch_name(eth_switch_mac, "GW_CONTROLLER_ETH");
    }

    // Create a timer to run internal tasks periodically
    m_tasks_timer = m_timer_manager->add_timer(
        "Controller Tasks", tasks_timer_period, tasks_timer_period,
        [&](int fd, beerocks::EventLoop &loop) {
            // Allow tasks to execute up to 80% of the timer period
            m_task_pool.run_tasks(int(double(tasks_timer_period.count()) * 0.8));
            return true;
        });
    if (m_tasks_timer == beerocks::net::FileDescriptor::invalid_descriptor) {
        LOG(ERROR) << "Failed to create the tasks timer";
        return false;
    }
    LOG(DEBUG) << "Tasks timer created with fd = " << m_tasks_timer;
    transaction.add_rollback_action([&]() { m_timer_manager->remove_timer(m_tasks_timer); });

    // Create a timer to execute periodic operations
    // TODO: as an enhancement, each periodic operation should have its own timer (PPM-717)
    m_operations_timer =
        m_timer_manager->add_timer("Periodic Operations", operations_timer_period,
                                   operations_timer_period, [&](int fd, beerocks::EventLoop &loop) {
                                       operations.run_operations();
                                       return true;
                                   });
    if (m_operations_timer == beerocks::net::FileDescriptor::invalid_descriptor) {
        LOG(ERROR) << "Failed to create the operations timer";
        return false;
    }
    LOG(DEBUG) << "Operations timer created with fd = " << m_operations_timer;
    transaction.add_rollback_action([&]() { m_timer_manager->remove_timer(m_operations_timer); });

    // Create an instance of a broker client connected to the broker server that is running in the
    // transport process
    m_broker_client = m_broker_client_factory->create_instance();
    if (!m_broker_client) {
        LOG(ERROR) << "Failed to create instance of broker client";
        return false;
    }
    transaction.add_rollback_action([&]() { m_broker_client.reset(); });

    beerocks::btl::BrokerClient::EventHandlers handlers;
    // Install a CMDU-received event handler for CMDU messages received from the transport process.
    // These messages are actually been sent by a remote process and the broker server running in
    // the transport process just forwards them to the broker client.
    handlers.on_cmdu_received = [&](uint32_t iface_index, const sMacAddr &dst_mac,
                                    const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx) {
        handle_cmdu_from_broker(iface_index, dst_mac, src_mac, cmdu_rx);
    };

    // Install a connection-closed event handler.
    // Currently there is no recovery mechanism if connection with broker server gets interrupted
    // (something that happens if the transport process dies). Just log a message and exit
    handlers.on_connection_closed = [&]() {
        LOG(ERROR) << "Broker client got disconnected!";
        return false;
    };

    m_broker_client->set_handlers(handlers);
    transaction.add_rollback_action([&]() { m_broker_client->clear_handlers(); });

    // Subscribe for the reception of CMDU messages that this process is interested in
    if (!m_broker_client->subscribe(std::set<ieee1905_1::eMessageType>{
            ieee1905_1::eMessageType::ACK_MESSAGE,
            ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE,
            ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE,
            ieee1905_1::eMessageType::AP_CAPABILITY_REPORT_MESSAGE,
            ieee1905_1::eMessageType::AP_METRICS_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::BEACON_METRICS_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::CHANNEL_PREFERENCE_REPORT_MESSAGE,
            ieee1905_1::eMessageType::CHANNEL_SELECTION_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::CHANNEL_SCAN_REPORT_MESSAGE,
            ieee1905_1::eMessageType::CLIENT_CAPABILITY_REPORT_MESSAGE,
            ieee1905_1::eMessageType::CLIENT_STEERING_BTM_REPORT_MESSAGE,
            ieee1905_1::eMessageType::HIGHER_LAYER_DATA_MESSAGE,
            ieee1905_1::eMessageType::LINK_METRIC_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::UNASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::OPERATING_CHANNEL_REPORT_MESSAGE,
            ieee1905_1::eMessageType::STEERING_COMPLETED_MESSAGE,
            ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE,
            ieee1905_1::eMessageType::TOPOLOGY_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE,
            ieee1905_1::eMessageType::BACKHAUL_STEERING_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::TUNNELLED_MESSAGE,
            ieee1905_1::eMessageType::BACKHAUL_STA_CAPABILITY_REPORT_MESSAGE,
            ieee1905_1::eMessageType::BSS_CONFIGURATION_REQUEST_MESSAGE,
            ieee1905_1::eMessageType::FAILED_CONNECTION_MESSAGE,
            ieee1905_1::eMessageType::QOS_MANAGEMENT_NOTIFICATION_MESSAGE,
            ieee1905_1::eMessageType::CLIENT_SECURITY_CONTEXT_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::VIRTUAL_BSS_MOVE_PREPARATION_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::VIRTUAL_BSS_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::VIRTUAL_BSS_MOVE_CANCEL_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::TRIGGER_CHANNEL_SWITCH_ANNOUNCEMENT_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::EARLY_AP_CAPABILITY_REPORT_MESSAGE,
        })) {
        LOG(ERROR) << "Failed subscribing to the Bus";
        return false;
    }

    transaction.commit();

    LOG(DEBUG) << "started";

    return true;
}

bool Controller::stop()
{
    bool ok = true;

    if (m_broker_client) {
        m_broker_client->clear_handlers();
        m_broker_client.reset();
    }

    if (m_operations_timer != beerocks::net::FileDescriptor::invalid_descriptor) {
        if (!m_timer_manager->remove_timer(m_operations_timer)) {
            ok = false;
        }
    }

    if (m_tasks_timer != beerocks::net::FileDescriptor::invalid_descriptor) {
        if (!m_timer_manager->remove_timer(m_tasks_timer)) {
            ok = false;
        }
    }

    LOG(DEBUG) << "stopped";

    return ok;
}

void Controller::start_mandatory_tasks()
{
    LOG(DEBUG) << "Start mandatory periodic tasks";

    LOG_IF(!m_task_pool.add_task(std::make_shared<bml_task>(database, cmdu_tx, m_task_pool)), FATAL)
        << "Failed adding BML task!";

    LOG_IF(!m_task_pool.add_task(std::make_shared<topology_task>(database, cmdu_tx, m_task_pool)),
           FATAL)
        << "Failed adding topology task!";

    LOG_IF(!m_task_pool.add_task(
               std::make_shared<client_association_task>(database, cmdu_tx, m_task_pool)),
           FATAL)
        << "Failed adding client association task!";

    LOG_IF(!m_task_pool.add_task(
               std::make_shared<agent_monitoring_task>(database, cmdu_tx, m_task_pool)),
           FATAL)
        << "Failed adding agent monitoring task!";

#ifdef ENABLE_VBSS
    LOG_IF(
        !m_task_pool.add_task(std::make_shared<vbss_task>(database, m_task_pool, m_timer_manager)),
        FATAL)
        << "Failed adding vbss task!";
#else
    LOG(INFO) << "VBSS is not enabled";
#endif

    LOG_IF(!m_task_pool.add_task(std::make_shared<DhcpTask>(database, m_timer_manager)), FATAL)
        << "Failed adding dhcp task!";

    LOG_IF(!m_task_pool.add_task(std::make_shared<service_prioritization_task>(database, cmdu_tx)),
           FATAL)
        << "Failed adding service_prioritization_task!";
}

void Controller::start_optional_tasks()
{
    LOG(DEBUG) << "Start/Stop configurable periodic tasks";

    if (database.config.management_mode != BPL_MGMT_MODE_NOT_MULTIAP) {
        if (!m_link_metrics_task) {
            LOG(DEBUG) << "Start Link metrics task";
            m_link_metrics_task =
                std::make_shared<LinkMetricsTask>(database, cmdu_tx, cert_cmdu_tx, m_task_pool);
            LOG_IF(!m_task_pool.add_task(m_link_metrics_task), FATAL)
                << "Failed adding Link metrics task!";
        } else {
            LOG(DEBUG) << "Link metrics task already running";
        }
    } else {
        if (m_link_metrics_task) {
            LOG(DEBUG) << "Stop Link metrics task";
            m_task_pool.kill_task(m_link_metrics_task->id);
            m_link_metrics_task = {};
        } else {
            LOG(DEBUG) << "Link metric task disabled";
        }
    }

#ifndef BEEROCKS_LINUX
    // update running status for statistics_polling_task
    LOG(INFO) << "statistics polling task status update";
    task_status_helper<statistics_polling_task>(database.settings_diagnostics_measurements(),
                                                m_statistics_polling_task_id,
                                                "statistics_polling_task");
    database.assign_statistics_polling_task_id(m_statistics_polling_task_id);

#endif

    // update running status for channel_selection_task
    LOG(INFO) << "channel selection task status update";
    task_status_helper<channel_selection_task>(database.settings_channel_select_task(),
                                               m_channel_selection_task_id,
                                               "channel_selection_task");
    database.assign_channel_selection_task_id(m_channel_selection_task_id);

    // update running status for dynamic_channel_selection_task_2
    LOG(INFO) << "dynamic channel selection r2 task status update";
    task_status_helper<dynamic_channel_selection_r2_task>(
        database.settings_dynamic_channel_select_task(), m_dynamic_channel_selection_task_id,
        "dynamic_channel_selection_task_r2");
    database.assign_dynamic_channel_selection_r2_task_id(m_dynamic_channel_selection_task_id);

    // update running status for network_health_check_task
    LOG(INFO) << "network health check task status update";
    task_status_helper<network_health_check_task>(database.settings_health_check(),
                                                  m_network_health_check_task_id,
                                                  "network_health_check_task");
    // database has no member network_health_check_task_id, so we are not setting it
}

bool Controller::send_cmdu(int fd, ieee1905_1::CmduMessageTx &cmdu_tx)
{
    return m_cmdu_server->send_cmdu(fd, cmdu_tx);
}

bool Controller::send_cmdu_to_broker(ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &dst_mac,
                                     const sMacAddr &src_mac, const std::string &iface_name)
{
    if (!m_broker_client) {
        LOG(ERROR) << "Unable to send CMDU to broker server";
        return false;
    }

    uint32_t iface_index = 0;
    if (!iface_name.empty()) {
        iface_index = if_nametoindex(iface_name.c_str());
    }

    return m_broker_client->send_cmdu(cmdu_tx, dst_mac, src_mac, iface_index);
}

void Controller::handle_connected(int fd) { LOG(INFO) << "UDS socket connected, fd = " << fd; }

void Controller::handle_disconnected(int fd)
{
    LOG(INFO) << "UDS socket disconnected, fd = " << fd;

    // Removing the socket only from the vector of socket in the database if exists,
    // not from socket thread.
    database.remove_cli_socket(fd);
    database.remove_bml_socket(fd);

#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    pre_association_steering_task::sListenerGeneralRegisterUnregisterEvent new_event;
    new_event.sd = fd;
    m_task_pool.push_event(database.get_pre_association_steering_task_id(),
                           pre_association_steering_task::eEvents::STEERING_REMOVE_SOCKET,
                           &new_event);
#endif
}

bool Controller::handle_cmdu(int fd, uint32_t iface_index, const sMacAddr &dst_mac,
                             const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool vendor_specific = false;

    if (cmdu_rx.getMessageType() == ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE) {
        vendor_specific = true;
    }

    if (vendor_specific) {
        auto beerocks_header = beerocks::message_com::parse_intel_vs_message(cmdu_rx);
        if (!beerocks_header) {
            LOG(ERROR) << "Not a vendor specific message";
            return false;
        }
        switch (beerocks_header->action()) {
        case beerocks_message::ACTION_CLI: {
            son_management::handle_cli_message(fd, beerocks_header, cmdu_tx, database, m_task_pool);
        } break;
        case beerocks_message::ACTION_BML: {
            son_management::handle_bml_message(fd, beerocks_header, cmdu_tx, database, m_task_pool);
        } break;
        case beerocks_message::ACTION_CONTROL: {
            handle_cmdu_control_message(src_mac, beerocks_header);
        } break;
        default: {
            LOG(ERROR) << "Unknown message, action: " << int(beerocks_header->action());
        }
        }
    } else {
        LOG(DEBUG) << "received 1905.1 cmdu message";
        if (cmdu_rx.getMessageType() ==
            ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE) {
            auto time_since_m1 = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - cmdu_rx.received_time);
            if (time_since_m1 >
                std::chrono::seconds{beerocks::ieee1905_1_consts::AUTOCONFIG_M2_TIMEOUT_SECONDS}) {
                LOG(INFO) << "Time since M1 was received (" << time_since_m1.count()
                          << " seconds) is more than "
                          << beerocks::ieee1905_1_consts::AUTOCONFIG_M2_TIMEOUT_SECONDS
                          << " seconds, ignoring M1.";
                database.update_last_contact_time(src_mac);
                return false;
            }
        }
        handle_cmdu_1905_1_message(src_mac, cmdu_rx);
        m_task_pool.handle_ieee1905_1_msg(src_mac, cmdu_rx);

        database.update_last_contact_time(src_mac);
    }

    return true;
}

bool Controller::handle_cmdu_from_broker(uint32_t iface_index, const sMacAddr &dst_mac,
                                         const sMacAddr &src_mac,
                                         ieee1905_1::CmduMessageRx &cmdu_rx)
{
    if (src_mac == beerocks::net::network_utils::ZERO_MAC) {
        LOG(ERROR) << "src_mac is zero!";
        return false;
    }

    if (dst_mac == beerocks::net::network_utils::ZERO_MAC) {
        LOG(ERROR) << "dst_mac is zero!";
        return false;
    }

    // Filter out messages that are not addressed to the controller
    if (dst_mac != beerocks::net::network_utils::MULTICAST_1905_MAC_ADDR &&
        dst_mac != database.get_local_bridge_mac()) {
        return false;
    }

    // TODO: Add optimization of PID filtering for cases like the following:
    // If VS message was sent by Controllers local agent to the controller, it is looped back.

    // Handle CMDU as if it had been received from any other process.
    // The socket descriptor is not needed because this process never responds to the transport
    // process.
    return handle_cmdu(beerocks::net::FileDescriptor::invalid_descriptor, iface_index, dst_mac,
                       src_mac, cmdu_rx);
}

bool Controller::handle_cmdu_1905_1_message(const sMacAddr &src_mac,
                                            ieee1905_1::CmduMessageRx &cmdu_rx)
{
    switch (cmdu_rx.getMessageType()) {
    case ieee1905_1::eMessageType::ACK_MESSAGE:
        return handle_cmdu_1905_ack_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE:
        return handle_cmdu_1905_autoconfiguration_search(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE:
        return handle_cmdu_1905_autoconfiguration_WSC(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::AP_CAPABILITY_REPORT_MESSAGE:
        return handle_cmdu_1905_ap_capability_report(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::AP_METRICS_RESPONSE_MESSAGE:
        return handle_cmdu_1905_ap_metric_response(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::BEACON_METRICS_RESPONSE_MESSAGE:
        return handle_cmdu_1905_beacon_response(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::CHANNEL_SCAN_REPORT_MESSAGE:
        return handle_cmdu_1905_channel_scan_report(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::CLIENT_STEERING_BTM_REPORT_MESSAGE:
        return handle_cmdu_1905_client_steering_btm_report_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::HIGHER_LAYER_DATA_MESSAGE:
        return handle_cmdu_1905_higher_layer_data_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::OPERATING_CHANNEL_REPORT_MESSAGE:
        return handle_cmdu_1905_operating_channel_report(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::STEERING_COMPLETED_MESSAGE:
        return handle_cmdu_1905_steering_completed_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::BACKHAUL_STEERING_RESPONSE_MESSAGE:
        return handle_cmdu_1905_backhaul_sta_steering_response(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::TUNNELLED_MESSAGE:
        return handle_cmdu_1905_tunnelled_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::BACKHAUL_STA_CAPABILITY_REPORT_MESSAGE:
        return handle_cmdu_1905_backhaul_sta_capability_report_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::BSS_CONFIGURATION_REQUEST_MESSAGE:
        return handle_cmdu_1905_bss_configuration_request_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::FAILED_CONNECTION_MESSAGE:
        return handle_cmdu_1905_failed_connection_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::ASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE:
        return handle_cmdu_1905_associated_sta_link_metrics_response_message(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::EARLY_AP_CAPABILITY_REPORT_MESSAGE:
        return handle_cmdu_1905_early_ap_capability_report_message(src_mac, cmdu_rx);

    // Empty cases are used to prevent error logs. Below message types are processed within tasks.
    case ieee1905_1::eMessageType::TOPOLOGY_RESPONSE_MESSAGE:
    case ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE:
    case ieee1905_1::eMessageType::LINK_METRIC_RESPONSE_MESSAGE:
    case ieee1905_1::eMessageType::UNASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE:
    case ieee1905_1::eMessageType::CLIENT_CAPABILITY_REPORT_MESSAGE:
    case ieee1905_1::eMessageType::CHANNEL_PREFERENCE_REPORT_MESSAGE:
    case ieee1905_1::eMessageType::CHANNEL_SELECTION_RESPONSE_MESSAGE:

        return true;
    default:
        break;
    }

    LOG(WARNING) << "Unknown 1905 message received message_type=" << std::hex
                 << int(cmdu_rx.getMessageType()) << " .Ignoring ";
    return true;
}

bool Controller::handle_cmdu_1905_autoconfiguration_search(const sMacAddr &src_mac,
                                                           ieee1905_1::CmduMessageRx &cmdu_rx)
{
    LOG(DEBUG) << "Received AP_AUTOCONFIGURATION_SEARCH_MESSAGE";

    auto tlvAlMacAddress = cmdu_rx.getClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMacAddress) {
        LOG(ERROR) << "getClass<tlvAlMacAddress> failed";
        return false;
    }
    auto tlvSearchedRole = cmdu_rx.getClass<ieee1905_1::tlvSearchedRole>();
    if (!tlvSearchedRole) {
        LOG(ERROR) << "getClass<tlvSearchedRole> failed";
        return false;
    }
    auto tlvAutoconfigFreqBand = cmdu_rx.getClass<ieee1905_1::tlvAutoconfigFreqBand>();
    if (!tlvAutoconfigFreqBand) {
        LOG(ERROR) << "getClass<tlvAutoconfigFreqBand> failed";
        return false;
    }
    auto tlvSupportedServiceIn = cmdu_rx.getClass<wfa_map::tlvSupportedService>();
    if (!tlvSupportedServiceIn) {
        LOG(ERROR) << "getClass<tlvSupportedService> failed";
        return false;
    }
    auto tlvSearchedService = cmdu_rx.getClass<wfa_map::tlvSearchedService>();
    if (!tlvSearchedService) {
        LOG(ERROR) << "getClass<tlvSearchedService> failed";
        return false;
    }

    auto al_mac = tlvAlMacAddress->mac();
    LOG(DEBUG) << "mac=" << al_mac;

    LOG(DEBUG) << "searched_role=" << int(tlvSearchedRole->value());
    if (tlvSearchedRole->value() != ieee1905_1::tlvSearchedRole::REGISTRAR) {
        LOG(ERROR) << "invalid tlvSearchedRole value";
        return false;
    }

    auto &auto_config_freq_band = tlvAutoconfigFreqBand->value();
    LOG(DEBUG) << "band=" << int(auto_config_freq_band);

    bool supported_controller_service = false;
    bool supported_agent_service      = false;
    for (int i = 0; i < tlvSupportedServiceIn->supported_service_list_length(); i++) {
        auto supportedServiceTuple = tlvSupportedServiceIn->supported_service_list(i);
        if (!std::get<0>(supportedServiceTuple)) {
            LOG(ERROR) << "Invalid tlvSupportedService";
            return false;
        }
        auto supportedService = std::get<1>(supportedServiceTuple);
        if (supportedService == wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_AGENT) {
            supported_agent_service = true;
        } else if (supportedService ==
                       wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_CONTROLLER ||
                   supportedService ==
                       wfa_map::tlvSupportedService::eSupportedService::EM_AP_CONTROLLER) {
            supported_controller_service = true;
        } else {
            LOG(INFO) << "Not supported service, received value:" << std::hex
                      << int(supportedService);
        }
    }
    if (supported_controller_service && al_mac != database.get_local_bridge_mac()) {
        LOG(ERROR) << "[Multiple Controllers Detected] Received "
                      "AP_AUTOCONFIGURATION_SEARCH_MESSAGE from another Controller: "
                   << al_mac;
    }
    if (!supported_agent_service) {
        LOG(WARNING) << "MULTI_AP_AGENT is not supported as service";
        return false;
    }

    bool searched_controller_service = false;
    for (int i = 0; i < tlvSearchedService->searched_service_list_length(); i++) {
        auto searchedServiceTuple = tlvSearchedService->searched_service_list(i);
        if (!std::get<0>(searchedServiceTuple)) {
            LOG(ERROR) << "Invalid tlvSearchedService";
            return false;
        }
        if (std::get<1>(searchedServiceTuple) ==
            wfa_map::tlvSearchedService::eSearchedService::MULTI_AP_CONTROLLER) {
            searched_controller_service = true;
        } else {
            LOG(INFO) << "Not supported searched service, received value:" << std::hex
                      << int(std::get<1>(searchedServiceTuple));
        }
    }
    if (!searched_controller_service) {
        LOG(WARNING) << "MULTI_AP_CONTROLLER is not searched as service";
        return false;
    }

    auto cmdu_header = cmdu_tx.create(
        cmdu_rx.getMessageId(), ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RESPONSE_MESSAGE);

    auto tlvSupportedRole = cmdu_tx.addClass<ieee1905_1::tlvSupportedRole>();
    if (!tlvSupportedRole) {
        LOG(ERROR) << "addClass ieee1905_1::tlvSupportedRole failed";
        return false;
    }
    tlvSupportedRole->value() = ieee1905_1::tlvSupportedRole::REGISTRAR;

    auto tlvSupportedFreqBand = cmdu_tx.addClass<ieee1905_1::tlvSupportedFreqBand>();
    if (!tlvSupportedFreqBand) {
        LOG(ERROR) << "addClass ieee1905_1::tlvSupportedFreqBand failed";
        return false;
    }

    switch (auto_config_freq_band) {
    case ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ: {
        tlvSupportedFreqBand->value() = ieee1905_1::tlvSupportedFreqBand::BAND_2_4G;
        break;
    }
    case ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_5_GHZ: {
        tlvSupportedFreqBand->value() = ieee1905_1::tlvSupportedFreqBand::BAND_5G;
        break;
    }
    case ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_6_GHZ: {
        tlvSupportedFreqBand->value() = ieee1905_1::tlvSupportedFreqBand::BAND_6G;
        break;
    }
    case ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_60_GHZ: {
        tlvSupportedFreqBand->value() = ieee1905_1::tlvSupportedFreqBand::BAND_60G;
        break;
    }
    default: {
        LOG(ERROR) << "unknown autoconfig freq band, value=" << int(auto_config_freq_band);
        return false;
    }
    }

    auto tlvSupportedServiceOut = cmdu_tx.addClass<wfa_map::tlvSupportedService>();
    if (!tlvSupportedServiceOut) {
        LOG(ERROR) << "addClass wfa_map::tlvSupportedService failed";
        return false;
    }
    if (!tlvSupportedServiceOut->alloc_supported_service_list()) {
        LOG(ERROR) << "alloc_supported_service_list failed";
        return false;
    }
    auto supportedServiceTuple = tlvSupportedServiceOut->supported_service_list(0);
    if (!std::get<0>(supportedServiceTuple)) {
        LOG(ERROR) << "Failed accessing supported_service_list";
        return false;
    }
    std::get<1>(supportedServiceTuple) =
        wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_CONTROLLER;

    // Add MultiAp Profile TLV only if the agent added it to the seach message.
    // Although R2 is profile1 compatible, we found out that some certified agent
    // fail to parse the response in case the TLV is present.
    auto tlvProfile2MultiApProfileAgent = cmdu_rx.getClass<wfa_map::tlvProfile2MultiApProfile>();
    if (tlvProfile2MultiApProfileAgent) {
        auto tlvProfile2MultiApProfileController =
            cmdu_tx.addClass<wfa_map::tlvProfile2MultiApProfile>();
        if (!tlvProfile2MultiApProfileController) {
            LOG(ERROR) << "addClass wfa_map::tlvProfile2MultiApProfile failed";
            return false;
        }

        tlvProfile2MultiApProfileController->profile() = wfa_map::tlvProfile2MultiApProfile::
            eMultiApProfile::MULTIAP_PROFILE_1; // Profile-1 of EasyMesh R4
        auto profile2_ap_capability_tlv = cmdu_tx.addClass<wfa_map::tlvProfile2ApCapability>();
        if (profile2_ap_capability_tlv) {
            profile2_ap_capability_tlv->capabilities_bit_field().byte_counter_units =
                wfa_map::tlvProfile2ApCapability::eByteCounterUnits::KIBIBYTES;
        }

        // Indicate support for KiB / MiB byte counters
        auto tlvControllerCapability = cmdu_tx.addClass<wfa_map::tlvControllerCapability>();
        if (!tlvControllerCapability) {
            LOG(ERROR) << "addClass wfa_map::tlvControllerCapability failed";
            return false;
        }
        tlvControllerCapability->flags().early_ap_capability = 1;
    }

    auto beerocks_header = beerocks::message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header) {
        if (beerocks_header->action_op() !=
            beerocks_message::ACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST) {
            LOG(WARNING) << "Invalid action op";
            return false;
        }
        // mark slave as prplMesh
        LOG(DEBUG) << "prplMesh agent: received ACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST from "
                   << src_mac;
        database.set_prplmesh(src_mac);
        // response with handshake response to mark the controller as prplmesh
        auto response = beerocks::message_com::add_vs_tlv<
            beerocks_message::cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE>(cmdu_tx);
        if (!response) {
            LOG(ERROR) << "Failed adding cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE";
            return false;
        }
        beerocks::message_com::get_beerocks_header(cmdu_tx)->actionhdr()->direction() =
            beerocks::BEEROCKS_DIRECTION_AGENT;

    } else {
        LOG(DEBUG) << "Not prplMesh agent " << src_mac;
    }
    LOG(DEBUG) << "sending autoconfig response message";

    if (tlvProfile2MultiApProfileAgent) {
        std::shared_ptr<Agent> agent;
        // if al_mac is same as local bridge mac then add node it as GW else as IRE node
        if (database.get_local_bridge_mac() == al_mac) {
            agent = database.add_gateway(al_mac);
        } else {
            agent = database.add_agent(al_mac);
        }
        if (!agent) {
            LOG(ERROR) << "Failed adding agent: " << al_mac;
            return false;
        }

        agent->profile = tlvProfile2MultiApProfileAgent->profile();
        LOG(DEBUG) << "Agent profile is updated with enum " << agent->profile;

        if (!database.dm_set_device_multi_ap_profile(*agent)) {
            LOG(ERROR) << "Failed to set Multi-AP profile in DM for Agent " << agent->al_mac;
            return false;
        }
    }

    return son_actions::send_cmdu_to_agent(al_mac, cmdu_tx, database);
}

/**
 * @brief Encrypt the config data using AES and add to the WSC M2/M8 TLV
 *        The encrypted data length is the config data length padded to 16 bytes boundary.
 *
 * @param[out] iv initialization vector from M2/M8 to be filled
 * @param[out] encrypted_settings encrypted settings from M2/M8 to be filled
 * @param[in] config_data config data in network byte order (swapped)
 * @param[in] authkey 32 bytes calculated authentication key
 * @param[in] keywrapkey 16 bytes calculated key wrap key
 * @return true on success
 * @return false on failure
 */
bool Controller::autoconfig_wsc_add_encrypted_settings(uint8_t &iv,
                                                       std::vector<uint8_t> &encrypted_settings,
                                                       WSC::configData &config_data,
                                                       uint8_t authkey[32], uint8_t keywrapkey[16])
{
    // Step 1 - key wrap authenticator calculation
    uint8_t *plaintext = config_data.getMessageBuff();
    int plaintextlen   = config_data.getMessageLength();

    uint8_t *kwa = config_data.key_wrap_authenticator();
    // The keywrap authenticator is part of the config_data (last member of the
    // config_data to be precise).
    // However, since we need to calculate it over the part of config_data without the keywrap
    // authenticator, substruct it's size from the computation length
    size_t config_data_len_for_kwa = plaintextlen - config_data.key_wrap_authenticator_size();
    // Add KWA which is the 1st 64 bits of HMAC of config_data using AuthKey
    if (!mapf::encryption::kwa_compute(authkey, plaintext, config_data_len_for_kwa, kwa)) {
        LOG(ERROR) << "KeyWrapAuth computation failed!";
        return false;
    }

    // The KWA is computed on the swapped config_data (network byte order).
    // So at this point the KWA class is already swapped. We don't need to swap the recently
    // calculated data since the data is a char array and there is no need to swap it.

    // Step 2 - AES encryption using temporary buffer. This is needed since we only
    // know the encrypted length after encryption.
    // Calculate initialization vector (IV), and encrypt the plaintext using aes128 cbc.
    // leave room for up to 16 bytes internal padding length - see aes_encrypt()
    // Create encrypted_settings
    int cipherlen = plaintextlen + 16;
    uint8_t ciphertext[cipherlen];
    if (!mapf::encryption::create_iv(&iv, WSC::WSC_ENCRYPTED_SETTINGS_IV_LENGTH)) {
        LOG(ERROR) << "create iv failure";
        return false;
    }
    if (!mapf::encryption::aes_encrypt(keywrapkey, &iv, plaintext, plaintextlen, ciphertext,
                                       cipherlen)) {
        LOG(ERROR) << "aes encrypt failure";
        return false;
    }
    encrypted_settings = std::vector<uint8_t>(ciphertext, ciphertext + cipherlen);

    return true;
}

/**
 * @brief Calculate keys and update M2/M8 attributes.
 *
 * @param[in] m1 WSC M1 attribute list received from the radio agent
 * @param[in] enrollee_nonce enrollee nonce
 * @param[in] registrar_nonce registrar nonce
 * @param[in] pub_key public key
 * @param[in] dh diffie helman key exchange class containing the keypair
 * @param[out] authkey 32 bytes calculated authentication key
 * @param[out] keywrapkey 16 bytes calculated key wrap key
 * @return true on success
 * @return false on failure
 */
void Controller::autoconfig_wsc_calculate_keys(WSC::m1 &m1, uint8_t &enrollee_nonce,
                                               uint8_t &registrar_nonce, uint8_t &pub_key,
                                               const mapf::encryption::diffie_hellman &dh,
                                               uint8_t authkey[32], uint8_t keywrapkey[16])
{
    std::copy_n(m1.enrollee_nonce(), WSC::eWscLengths::WSC_NONCE_LENGTH, &enrollee_nonce);
    std::copy_n(dh.nonce(), dh.nonce_length(), &registrar_nonce);
    mapf::encryption::wps_calculate_keys(
        dh, m1.public_key(), WSC::eWscLengths::WSC_PUBLIC_KEY_LENGTH, m1.enrollee_nonce(),
        m1.mac_addr().oct, &registrar_nonce, authkey, keywrapkey);
    copy_pubkey(dh, &pub_key);
}

WSC::configData::config
prepare_wsc_config(const sMacAddr &ruid, const wireless_utils::sBssInfoConf *bss_info_conf, bool m8)
{
    // Create ConfigData
    WSC::configData::config cfg;
    if (bss_info_conf) {
        cfg.ssid        = bss_info_conf->ssid;
        cfg.auth_type   = bss_info_conf->authentication_type;
        cfg.encr_type   = bss_info_conf->encryption_type;
        cfg.network_key = bss_info_conf->network_key;
        if (m8) {
            cfg.bss_type = WSC::eWscVendorExtSubelementBssType::BACKHAUL_STA;
        } else {
            cfg.bss_type = 0;
            if (bss_info_conf->fronthaul) {
                cfg.bss_type |= WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS;
            }
            if (bss_info_conf->backhaul) {
                cfg.bss_type |= WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS;
            }
            if (bss_info_conf->bSTA) {
                cfg.bss_type |= WSC::eWscVendorExtSubelementBssType::BACKHAUL_STA;
            }
            if (bss_info_conf->profile1_backhaul_sta_association_disallowed) {
                cfg.bss_type |= WSC::eWscVendorExtSubelementBssType::
                    PROFILE1_BACKHAUL_STA_ASSOCIATION_DISALLOWED;
            }
            if (bss_info_conf->profile2_backhaul_sta_association_disallowed) {
                cfg.bss_type |= WSC::eWscVendorExtSubelementBssType::
                    PROFILE2_BACKHAUL_STA_ASSOCIATION_DISALLOWED;
            }
        }
        LOG(DEBUG) << "WSC config_data:" << std::hex << std::endl
                   << "     ssid: " << cfg.ssid << std::endl
                   << "     authentication_type: " << int(cfg.auth_type) << std::endl
                   << "     encryption_type: " << int(cfg.encr_type) << std::dec << std::endl
                   << "     bss_type: " << std::hex << int(cfg.bss_type);
    } else {
        // Tear down. No need to set any parameter except the teardown bit and the MAC address.
        cfg.bss_type = WSC::eWscVendorExtSubelementBssType::TEARDOWN;
        LOG(DEBUG) << "WSC config_data: tear down";
    }

    // The MAC address in the config data is tricky... According to "Wi-Fi Simple Configuration
    // Technical Specification v2.0.6", section 7.2.2 "Validation of Configuration Data" the MAC
    // address should be validated to match the Enrollee's own MAC address. "IEEE Std 1905.1-2013"
    // section 10.1.2, Table 10-1 "IEEE 802.11 settings (ConfigData) in M2 frame" says that it
    // should be "AP’s MAC address (BSSID)". The Multi-AP doesn't say anything about the MAC
    // addresses in M2, but it does say that the Enrollee MAC address in the M1 message must be the
    // AL-MAC address.
    //
    // Clearly, we can't use the real BSSID for the MAC address, since it's the responsibility of
    // the agent to use one of its assigned unique addresses as BSSID, and we don't have that
    // information in the controller. So we could use the AL-MAC addresses or the Radio UID. It
    // seems the most logical to make sure it matches the MAC address in the M1, since that stays
    // the closest to the WSC-specified behaviour.
    //
    // Note that the BBF 1905.1 implementation (meshComms) simply ignores the MAC address in M2.
    cfg.bssid = ruid;

    return cfg;
}

/**
 * @brief add WSC M2/M8 TLV to the current CMDU
 *
 *        the config_data contains the secret ssid, authentication and encryption types,
 *        the network key, bssid and the key_wrap_auth attribute.
 *        It does encryption using the keywrapkey and HMAC with the authkey generated
 *        in the WSC keys calculation from the M1 and M2/M8 nonce values, the radio agent's
 *        mac, and a random initialization vector.
 *        The encrypted config_data blob is copied to the encrypted_data attribute
 *        in the M2/M8 TLV, which marks the WSC M2/M8 TLV ready to be sent to the agent.
 *
 * @param m1 WSC M1 attribute list received from the radio agent as part of the WSC autoconfiguration
 *        CMDU
 * @param bss_info_conf BSS configuration
 * @return true on success
 * @return false on failure
 */
bool Controller::autoconfig_wsc_add_m2(WSC::m1 &m1,
                                       const wireless_utils::sBssInfoConf *bss_info_conf)
{
    auto tlv = cmdu_tx.addClass<ieee1905_1::tlvWsc>();
    if (!tlv) {
        LOG(ERROR) << "Failed creating tlvWsc";
        return false;
    }
    // Allocate maximum allowed length for the payload, so it can accommodate variable length
    // data inside the internal TLV list.
    // On finalize(), the buffer is shrunk back to its real size.
    size_t payload_length =
        tlv->getBuffRemainingBytes() - ieee1905_1::tlvEndOfMessage::get_initial_size();
    tlv->alloc_payload(payload_length);

    WSC::m2::config m2_cfg;
    m2_cfg.msg_type = WSC::eWscMessageType::WSC_MSG_TYPE_M2;
    // enrolee_nonce and registrar_nonce are set in autoconfig_wsc_calculate_keys()
    // public_key is set in autoconfig_wsc_calculate_keys()
    // connection_type and configuration_methods have default values
    // TODO the following should be taken from the database
    m2_cfg.manufacturer        = "prplMesh";
    m2_cfg.model_name          = "Ubuntu";
    m2_cfg.model_number        = "18.04";
    m2_cfg.serial_number       = "prpl12345";
    m2_cfg.primary_dev_type_id = WSC::WSC_DEV_NETWORK_INFRA_GATEWAY;
    m2_cfg.device_name         = "prplmesh-controller";
    m2_cfg.encr_type_flags     = uint16_t(WSC::eWscEncr::WSC_ENCR_NONE) |
                             uint16_t(WSC::eWscEncr::WSC_ENCR_AES) |
                             uint16_t(WSC::eWscEncr::WSC_ENCR_TKIP);
    m2_cfg.auth_type_flags =
        WSC::eWscAuth(WSC::eWscAuth::WSC_AUTH_OPEN | WSC::eWscAuth::WSC_AUTH_WPA2PSK |
                      WSC::eWscAuth::WSC_AUTH_SAE);
    // TODO Maybe the band should be taken from bss_info_conf.operating_class instead?

    if (m1.rf_bands() & WSC::WSC_RF_BAND_2GHZ) {
        m2_cfg.bands = WSC::WSC_RF_BAND_2GHZ;
    } else if (m1.rf_bands() & WSC::WSC_RF_BAND_5GHZ) {
        m2_cfg.bands = WSC::WSC_RF_BAND_5GHZ;
    } else if (m1.rf_bands() & WSC::WSC_RF_BAND_2GHZ_5GHZ) {
        m2_cfg.bands = WSC::WSC_RF_BAND_2GHZ_5GHZ;
    } else if (m1.rf_bands() & WSC::WSC_RF_BAND_6GHZ) {
        m2_cfg.bands = WSC::WSC_RF_BAND_6GHZ;
    } else {
        LOG(WARNING) << "WSC's M1 RF bands is not 2.4GHz, 5GHz, 2.4GHz and 5Ghz or 6GHz. Assigning "
                        "RF bands default value to 2.4GHz and 5Ghz";
        m2_cfg.bands = WSC::WSC_RF_BAND_2GHZ_5GHZ;
    }

    // association_state, configuration_error, device_password_id, os_version and vendor_extension
    // have default values

    ///////////////////////////////
    // @brief encryption support //
    ///////////////////////////////
    mapf::encryption::diffie_hellman dh;
    uint8_t authkey[32];
    uint8_t keywrapkey[16];
    autoconfig_wsc_calculate_keys(m1, *m2_cfg.enrollee_nonce, *m2_cfg.registrar_nonce,
                                  *m2_cfg.pub_key, dh, authkey, keywrapkey);

    // Encrypted settings
    // Encrypted settings are the ConfigData + IV. First create the ConfigData,
    // Then copy it to the encrypted data, add an IV and encrypt.
    // Finally, add HMAC

    uint8_t buf[1024];
    WSC::configData::config cfg = prepare_wsc_config(m1.mac_addr(), bss_info_conf, false);
    auto config_data            = WSC::configData::create(cfg, buf, sizeof(buf));
    if (!config_data) {
        LOG(ERROR) << "Failed to create configData";
        return false;
    }
    config_data->finalize();

    if (!autoconfig_wsc_add_encrypted_settings(*m2_cfg.iv, m2_cfg.encrypted_settings, *config_data,
                                               authkey, keywrapkey))
        return false;

    auto m2 = WSC::m2::create(*tlv, m2_cfg);
    if (!m2)
        return false;

    // Finalize m2 since it needs to be in network byte order for global authentication
    m2->finalize();

    if (!autoconfig_wsc_authentication(m1, m2, authkey))
        return false;

    return true;
}

bool Controller::autoconfig_wsc_add_m8(WSC::m1 &m1,
                                       const wireless_utils::sBssInfoConf &bss_info_conf)
{
    auto tlv = cmdu_tx.addClass<ieee1905_1::tlvWsc>();
    if (!tlv) {
        LOG(ERROR) << "Failed creating tlvWsc";
        return false;
    }
    // Allocate maximum allowed length for the payload, so it can accommodate variable length
    // data inside the internal TLV list.
    // On finalize(), the buffer is shrunk back to its real size.
    size_t payload_length =
        tlv->getBuffRemainingBytes() - ieee1905_1::tlvEndOfMessage::get_initial_size();
    tlv->alloc_payload(payload_length);

    WSC::m8::config m8_cfg;
    m8_cfg.msg_type = WSC::eWscMessageType::WSC_MSG_TYPE_M8;

    ///////////////////////////////
    // @brief encryption support //
    ///////////////////////////////
    mapf::encryption::diffie_hellman dh;
    uint8_t authkey[32];
    uint8_t keywrapkey[16];
    autoconfig_wsc_calculate_keys(m1, *m8_cfg.enrollee_nonce, *m8_cfg.registrar_nonce,
                                  *m8_cfg.pub_key, dh, authkey, keywrapkey);

    // Encrypted settings
    // Encrypted settings are the ConfigData + IV. First create the ConfigData,
    // Then copy it to the encrypted data, add an IV and encrypt.
    // Finally, add HMAC

    uint8_t buf[1024];
    WSC::configData::config cfg = prepare_wsc_config(m1.mac_addr(), &bss_info_conf, true);
    auto config_data            = WSC::configData::create(cfg, buf, sizeof(buf));
    if (!config_data) {
        LOG(ERROR) << "Failed to create configData";
        return false;
    }
    config_data->finalize();

    if (!autoconfig_wsc_add_encrypted_settings(*m8_cfg.iv, m8_cfg.encrypted_settings, *config_data,
                                               authkey, keywrapkey))
        return false;

    auto m8 = WSC::m8::create(*tlv, m8_cfg);
    if (!m8)
        return false;

    // Finalize m8 since it needs to be in network byte order for global authentication
    m8->finalize();

    if (!autoconfig_wsc_authentication(m1, m8, authkey))
        return false;

    return true;
}

/**
 * @brief Parse AP-Autoconfiguration WSC which should include one AP Radio Basic Capabilities
 *        TLV and one WSC TLV containing M1. If this is Intel agent, it will also have vendor specific tlv.
 *
 * @param sd socket descriptor
 * @param cmdu_rx received CMDU which contains M1
 * @return true on success
 * @return false on failure
 */
bool Controller::handle_cmdu_1905_autoconfiguration_WSC(const sMacAddr &src_mac,
                                                        ieee1905_1::CmduMessageRx &cmdu_rx)
{
    LOG(DEBUG) << "Received AP_AUTOCONFIGURATION_WSC_MESSAGE";
    auto tlvWsc = cmdu_rx.getClass<ieee1905_1::tlvWsc>();
    if (!tlvWsc) {
        LOG(ERROR) << "getClass<ieee1905_1::tlvWsc> failed";
        return false;
    }
    auto m1 = WSC::m1::parse(*tlvWsc);
    if (!m1) {
        LOG(INFO) << "Not a valid M1 - Ignoring WSC CMDU";
        return false;
    }

    auto radio_basic_caps = cmdu_rx.getClass<wfa_map::tlvApRadioBasicCapabilities>();
    if (!radio_basic_caps) {
        LOG(ERROR) << "getClass<wfa_map::tlvApRadioBasicCapabilities> failed";
        return false;
    }
    auto al_mac = m1->mac_addr();
    auto ruid   = radio_basic_caps->radio_uid();
    LOG(INFO) << "AP_AUTOCONFIGURATION_WSC M1 al_mac=" << al_mac << " ruid=" << ruid;
    LOG(DEBUG) << "   device " << m1->manufacturer() << " " << m1->model_name() << " "
               << m1->device_name() << " " << m1->serial_number();

    // If the agent already exists, return the old one
    auto agent   = database.m_agents.add(al_mac);
    agent->state = beerocks::STATE_DISCONNECTED;

    database.set_agent_manufacturer(*agent, m1->manufacturer());

    // Profile-2 Multi AP profile is added for higher than Profile-1 agents.
    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1 &&
        !handle_tlv_profile2_ap_capability(agent, cmdu_rx)) {
        LOG(ERROR) << "Profile2 AP Capability is not supplied for Agent " << al_mac
                   << " with profile enum " << agent->profile;
    }

    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1 &&
        !handle_tlv_profile2_ap_radio_advanced_capabilities(*agent, cmdu_rx)) {
        LOG(ERROR) << "Profile2 AP Radio Advanced Capabilities is not supplied for Agent " << al_mac
                   << " with profile enum " << agent->profile;
    }

    //TODO autoconfig process the rest of the class
    //TODO autoconfig Keep intel agent support only as intel enhancements
    /**
     * @brief Reply with AP-Autoconfiguration WSC with a single AP Radio Identifier TLV
     * and one (TODO do we need more?) WSC TLV containing M2.
     */
    if (!cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE)) {
        LOG(ERROR) << "Create AP_AUTOCONFIGURATION_WSC_MESSAGE response";
        return false;
    }
    // All attributes which are not explicitely set below are set to
    // default by the TLV factory, see WSC_Attributes.yml
    auto tlvRuid = cmdu_tx.addClass<wfa_map::tlvApRadioIdentifier>();
    if (!tlvRuid) {
        LOG(ERROR) << "error creating tlvApRadioIdentifier TLV";
        return false;
    }

    tlvRuid->radio_uid() = ruid;

    auto &bss_info_confs = database.get_bss_info_configuration(m1->mac_addr());
    uint8_t num_bsss     = 0;

    // Update BSSes in the Agent
    if (agent->radios.find(ruid) == agent->radios.end()) {
        database.add_radio(ruid, al_mac);
    }
    auto radio = agent->radios.get(ruid);
    if (!radio) {
        LOG(ERROR) << "No radio found for ruid=" << ruid << " on " << al_mac;
        return false;
    }
    /*  Obtain the freq_type from the M1 message itself.
        This will help us to know the radio's frequency
        type before we run channel selection task.
        This is useful when the operating class count
        is given as 0 in the Channel preference report
        of the agent.
    */
    if (m1->rf_bands() & WSC::WSC_RF_BAND_2GHZ) {
        radio->band = beerocks::FREQ_24G;
    } else if (m1->rf_bands() & WSC::WSC_RF_BAND_5GHZ) {
        radio->band = beerocks::FREQ_5G;
    } else if (m1->rf_bands() & WSC::WSC_RF_BAND_6GHZ) {
        radio->band = beerocks::FREQ_6G;
    }

    database.clear_configured_bss_info(ruid);

    for (auto &bss_info_conf : bss_info_confs) {
        // Check if the radio supports it
        if (!son_actions::has_matching_operating_class(*radio_basic_caps, bss_info_conf)) {
            LOG(INFO) << "Skipping " << bss_info_conf.ssid << " due to operclass mismatch";
            continue;
        }
        if ((m1->auth_type_flags() & bss_info_conf.authentication_type) !=
            bss_info_conf.authentication_type) {
            LOG(INFO) << std::hex << "Auth mismatch for " << bss_info_conf.ssid << ": get 0x"
                      << m1->auth_type_flags() << " need at least 0x"
                      << uint16_t(bss_info_conf.authentication_type);
        }
        if (!(m1->encr_type_flags() & uint16_t(bss_info_conf.encryption_type))) {
            LOG(INFO) << std::hex << "Encr mismatch for " << bss_info_conf.ssid << ": get 0x"
                      << m1->encr_type_flags() << " need 0x"
                      << uint16_t(bss_info_conf.encryption_type);
        }
        if (num_bsss >= radio_basic_caps->maximum_number_of_bsss_supported()) {
            LOG(INFO) << "Configured #BSS exceeds maximum for " << al_mac << " radio " << ruid;
            break;
        }

        // Check if the agent is external one or not to decide propagating Backhaul BSS
        if (database.settings_daisy_chaining_disabled() && !agent->is_gateway &&
            bss_info_conf.backhaul && !bss_info_conf.fronthaul) {
            LOG(INFO) << "Daisy chaining is disabled for " << al_mac << " radio " << ruid
                      << " skip BH BSS";
            continue;
        }

        // Clear backhaul flag for FH+BH configurations
        else if (database.settings_daisy_chaining_disabled() && !agent->is_gateway &&
                 bss_info_conf.backhaul && bss_info_conf.fronthaul) {
            LOG(INFO) << "Daisy chaining is disabled. Clear BH flag for" << al_mac << " radio "
                      << ruid;
            bss_info_conf.backhaul = false;
        }

        if (!agent->bSTA_reconfiguration_supported && bss_info_conf.backhaul) {
            bss_info_conf.bSTA = true;
        }

        if (!autoconfig_wsc_add_m2(*m1, &bss_info_conf)) {
            LOG(ERROR) << "Failed setting M2 attributes";
            return false;
        }

        if (agent->bSTA_reconfiguration_supported && bss_info_conf.backhaul &&
            !autoconfig_wsc_add_m8(*m1, bss_info_conf)) {
            LOG(ERROR) << "Failed setting M8 attributes";
            return false;
        }
        database.add_configured_bss_info(ruid, bss_info_conf);
        num_bsss++;
    }

    // If no BSS (either because none are configured, or because they don't match), tear down.
    if (num_bsss == 0) {
        if (!autoconfig_wsc_add_m2(*m1, nullptr)) {
            LOG(ERROR) << "Failed setting M2 attributes";
            return false;
        }
    } else {
        agent_monitoring_task::add_traffic_policy_tlv(database, cmdu_tx, m1);
        agent_monitoring_task::add_profile_2default_802q_settings_tlv(database, cmdu_tx,
                                                                      m1->mac_addr());
    }

    auto beerocks_header = beerocks::message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header) {
        LOG(INFO) << "Intel radio agent join (al_mac=" << al_mac << " ruid=" << ruid;
        if (!handle_intel_slave_join(src_mac, radio_basic_caps, *beerocks_header, cmdu_tx, agent)) {
            LOG(ERROR) << "Intel radio agent join failed (al_mac=" << al_mac << " ruid=" << ruid
                       << ")";
            return false;
        }
    } else {
        LOG(INFO) << "Non-Intel radio agent join (al_mac=" << al_mac << " ruid=" << ruid << ")";
        // Multi-AP Agent doesn't say anything about the bridge, so we have to rely on Intel Slave Join for that.
        // We'll use AL-MAC as the bridge
        // TODO convert source address into AL-MAC address
        if (!handle_non_intel_slave_join(src_mac, radio_basic_caps, *m1, agent, ruid, cmdu_tx)) {
            LOG(ERROR) << "Non-Intel radio agent join failed (al_mac=" << al_mac << " ruid=" << ruid
                       << ")";
            return false;
        }
    }

    agent->device_info.manufacturer       = m1->manufacturer();
    agent->device_info.manufacturer_model = m1->model_name();
    agent->device_info.serial_number      = m1->serial_number();
    database.dm_set_profile1_device_info(*agent);

    return true;
}

bool Controller::handle_cmdu_1905_ack_message(const sMacAddr &src_mac,
                                              ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();

    // extract error code, if any
    std::stringstream errorSS;
    auto error_tlv = cmdu_rx.getClass<wfa_map::tlvErrorCode>();
    if (error_tlv) {
        errorSS << "0x" << error_tlv->reason_code();
    } else {
        errorSS << "no error";
    }

    LOG(DEBUG) << "Received ACK_MESSAGE, mid=" << std::hex << int(mid)
               << " tlv error code: " << errorSS.str();

    // TODO: Send ACK message/event to dynamic_channel_selection_r2_task.

    //TODO: the ACK should be sent to the correct task and will be done as part of agent certification
    return true;
}

bool Controller::handle_cmdu_1905_steering_completed_message(const sMacAddr &src_mac,
                                                             ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received STEERING_COMPLETED_MESSAGE, mid=" << std::hex << int(mid);
    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);

    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message back to agent, mid=" << std::hex << int(mid);
    return son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);
}

bool Controller::handle_cmdu_1905_client_steering_btm_report_message(
    const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received CLIENT_STEERING_BTM_REPORT_MESSAGE, mid=" << std::hex << int(mid);

    auto steering_btm_report = cmdu_rx.getClass<wfa_map::tlvSteeringBTMReport>();
    if (!steering_btm_report) {
        LOG(ERROR) << "addClass wfa_map::tlvSteeringBTMReportfailed";
        return false;
    }

    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);

    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message back to agent";
    son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);

    std::string client_mac = tlvf::mac_to_string(steering_btm_report->sta_mac());
    wfa_map::tlvSteeringBTMReport::eBTMStatusCode status_code =
        steering_btm_report->btm_status_code();

    LOG(DEBUG) << "BTM_REPORT from source bssid " << steering_btm_report->bssid()
               << " for client_mac=" << client_mac << " status_code=" << (int)status_code;

    auto client = database.get_station(tlvf::mac_from_string(client_mac));
    if (!client) {
        LOG(ERROR) << "sta " << client_mac << " not found";
        return false;
    }

    int steering_task_id = client->steering_task_id;
    // Check if task is running before pushing the event
    if (m_task_pool.is_task_running(steering_task_id)) {
        m_task_pool.push_event(steering_task_id, client_steering_task::BTM_REPORT_RECEIVED,
                               (void *)&status_code);
    }
    database.update_node_11v_responsiveness(*client, true);

    if (status_code != wfa_map::tlvSteeringBTMReport::ACCEPT) {
        LOG(DEBUG) << "sta " << client_mac << " rejected BSS steer request";
        LOG(DEBUG) << "killing roaming task";

        m_task_pool.kill_task(client->roaming_task_id);

        m_task_pool.push_event(steering_task_id, client_steering_task::BSS_TM_REQUEST_REJECTED);
    }

    int btm_request_task_id = client->btm_request_task_id;
    // Check if task is running before pushing the event
    if (m_task_pool.is_task_running(btm_request_task_id)) {
        m_task_pool.push_event(btm_request_task_id, btm_request_task::BTM_REPORT_RECEIVED,
                               (void *)&status_code);
        // no BSS_TM_REQUEST_REJECTED event for the btm_request_task since there is no particualr handling for
        // this value of the BTM_RESPONSE status;
    }

    return true;
}

bool Controller::handle_cmdu_1905_channel_scan_report(const sMacAddr &src_mac,
                                                      ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto current_message_mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received CHANNEL_SCAN_REPORT_MESSAGE, agent src_mac=" << src_mac
              << ", mid=" << std::hex << current_message_mid;

    // Build and send ACK message CMDU to the originator.
    auto cmdu_tx_header =
        cmdu_tx.create(current_message_mid, ieee1905_1::eMessageType::ACK_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }

    // Zero Error Code TLVs in this ACK message
    LOG(DEBUG) << "Sending ACK message to the originator, mid=" << std::hex << current_message_mid;
    if (!send_cmdu_to_broker(cmdu_tx, src_mac, database.get_local_bridge_mac())) {
        LOG(ERROR) << "Failed to send ACK_MESSAGE back to agent";
        return false;
    }

    // get Timestamp TLV
    auto timestamp_tlv = cmdu_rx.getClass<wfa_map::tlvTimestamp>();
    if (!timestamp_tlv) {
        LOG(ERROR) << "getClass wfa_map::tlvTimestamp has failed";
        return false;
    }
    auto ISO_8601_timestamp = timestamp_tlv->timestamp_str();
    LOG(INFO) << "Report Timestamp: " << ISO_8601_timestamp;

    int result_count = 0;
    for (auto const &result_tlv : cmdu_rx.getClassList<wfa_map::tlvProfile2ChannelScanResult>()) {
        auto neighbors_list_length = result_tlv->neighbors_list_length();
        LOG(DEBUG) << "Received Result TLV for:" << std::endl
                   << "RUID: " << result_tlv->radio_uid() << ", "
                   << "Scan status: " << result_tlv->success() << ", "
                   << "Operating Class: " << result_tlv->operating_class() << ", "
                   << "Channel: " << result_tlv->channel() << ", "
                   << " containing " << neighbors_list_length << " neighbors";
        /**
         * To correctly store the results of the most current report, we need to know whether to
         * override any existing records.
         * In case of fragmentation in prplmesh the entire report could be split into several
         * report messages, thus to confirm if we should override the existing records we compare
         * the recorded timestamp against the received timestamp as fragmented reports share the
         * same timestamp.
         *
         * Reports with the same timestamp are recorded in the report-record-index
         */
        bool should_override_existing_records = true;
        if (database.has_channel_report_record(result_tlv->radio_uid(), ISO_8601_timestamp,
                                               result_tlv->operating_class(),
                                               result_tlv->channel())) {
            LOG(DEBUG) << "Report record found for " << ISO_8601_timestamp << " ["
                       << result_tlv->operating_class() << "," << result_tlv->channel() << "] "
                       << " from radio: " << result_tlv->radio_uid()
                       << ", Not overriding existing records.";
            should_override_existing_records = false;
        } else {
            LOG(DEBUG) << "No previous report record were found for " << ISO_8601_timestamp << " ["
                       << result_tlv->operating_class() << "," << result_tlv->channel() << "] "
                       << " from radio:" << result_tlv->radio_uid() << ".";
        }

        // If scan status is not successful, Add an empty channel scan report entry in the
        // Controller's DB with the report timestamp.
        if (result_tlv->success() != wfa_map::tlvProfile2ChannelScanResult::eScanStatus::SUCCESS) {
            if (!database.add_empty_channel_report_entry(
                    result_tlv->radio_uid(), result_tlv->operating_class(), result_tlv->channel(),
                    ISO_8601_timestamp)) {
                LOG(ERROR) << "Failed to add empty channel report entry!";
                return false;
            }
            continue;
        }

        std::vector<wfa_map::cNeighbors> neighbor_vec;
        for (int nbr_idx = 0; nbr_idx < neighbors_list_length; nbr_idx++) {
            auto neighbor_tuple = result_tlv->neighbors_list(nbr_idx);
            if (!std::get<0>(neighbor_tuple)) {
                LOG(ERROR) << "getting neighbor entry #" << nbr_idx << " has failed!";
                return false;
            }

            auto neighbor = std::get<1>(neighbor_tuple);
            neighbor_vec.push_back(neighbor);
        }
        if (!database.add_channel_report(result_tlv->radio_uid(), result_tlv->operating_class(),
                                         result_tlv->channel(), neighbor_vec, result_tlv->noise(),
                                         result_tlv->utilization(), ISO_8601_timestamp,
                                         should_override_existing_records)) {
            LOG(ERROR) << "Failed to add channel report entry #" << result_count << "!";
            return false;
        }
        if (!database.dm_add_scan_result(
                result_tlv->radio_uid(), result_tlv->operating_class(), result_tlv->channel(),
                result_tlv->noise(), result_tlv->utilization(), neighbor_vec, ISO_8601_timestamp)) {
            LOG(ERROR) << "Failed to add ScanResult entry #" << result_count << " !";
        }
        result_count++;
    }
    LOG(DEBUG) << "Done with Channel Scan Results TLVs";

    /**
     * To support scan reports that may exceed the maximum size supported by the transport layer
     * (currently not addressed in the EasyMesh specifications), for prplmesh agent, we might
     * de-fragment the report across several report messages.
     * For non-prplmesh agents, receiving the report message means the scan is complete.
     * For prplmesh agents need to check the report_done flag inside tlvChannelScanReportDone
     */
    bool report_done = true;
    if (database.is_prplmesh(src_mac)) {
        /**
         * For prplmesh agents, we wish to support de-fragmentation of channel scan results across
         * multiple report CMDUs when the size of the results exceeds the max TX buffer size.
         * This means that only the last report message should clear the report records
         * NOTE: Clearing the report record will not erase the existing scan results, but the next
         * incoming report will.
         */
        auto beerocks_header = beerocks::message_com::parse_intel_vs_message(cmdu_rx);
        if (!beerocks_header) {
            LOG(ERROR) << "expecting wfa_map::tlvChannelScanReportDone";
            return false;
        }
        auto vs_tlv = beerocks_header->addClass<beerocks_message::tlvVsChannelScanReportDone>();
        if (!vs_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvChannelScanReportDone failed";
            return false;
        }
        report_done = vs_tlv->report_done();
    }

    if (report_done) {
        LOG(DEBUG) << "Sending RECEIVED_CHANNEL_SCAN_REPORT event to DCS R2 task.";
        dynamic_channel_selection_r2_task::sScanReportEvent new_event = {};
        new_event.ISO_8601_timestamp                                  = ISO_8601_timestamp;
        new_event.agent_mac                                           = src_mac;
        m_task_pool.push_event(
            database.get_dynamic_channel_selection_r2_task_id(),
            dynamic_channel_selection_r2_task::eEvent::RECEIVED_CHANNEL_SCAN_REPORT, &new_event);
    }
    LOG(DEBUG) << "Report handling is done";
    return true;
}

void Controller::set_esp(const std::string &param_name, const sMacAddr &reporting_agent_bssid,
                         uint8_t *est_service_info_field)
{
    union {
        uint8_t bytes[4];
        uint32_t value;
    } estimated_service_param = {0};

    estimated_service_param.bytes[0] = est_service_info_field[0];
    estimated_service_param.bytes[1] = est_service_info_field[1];
    estimated_service_param.bytes[2] = est_service_info_field[2];
    database.set_estimated_service_param(reporting_agent_bssid, param_name,
                                         estimated_service_param.value);
}

bool Controller::handle_cmdu_1905_ap_metric_response(const sMacAddr &src_mac,
                                                     ieee1905_1::CmduMessageRx &cmdu_rx)
{

    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received AP_METRICS_RESPONSE_MESSAGE, mid=" << std::dec << int(mid);

    bool ret_val = true;

    //getting reference for ap metric data storage from db
    auto &ap_metric_data = database.get_ap_metric_data_map();

    for (auto ap_metric_tlv : cmdu_rx.getClassList<wfa_map::tlvApMetrics>()) {
        //parse tx_ap_metric_data
        sMacAddr reporting_agent_bssid = ap_metric_tlv->bssid();

        if (!database.set_radio_utilization(reporting_agent_bssid,
                                            ap_metric_tlv->channel_utilization())) {
            LOG(ERROR) << "Failed to set radio utilization dor bssid: " << reporting_agent_bssid;
            ret_val = false;
            continue;
        }

        LOG(DEBUG) << "received tlvApMetrics from BSSID =" << reporting_agent_bssid;

        //fill tx data from TLV
        if (!ap_metric_data[reporting_agent_bssid].add_ap_metric_data(ap_metric_tlv)) {
            LOG(ERROR) << "adding apMetricData from tlv has failed";
            ret_val = false;
            continue;
        }

        if (ap_metric_tlv->estimated_service_parameters().include_ac_be) {
            set_esp("EstServiceParametersBE", reporting_agent_bssid,
                    ap_metric_tlv->estimated_service_info_field());
        } else {
            LOG(WARNING) << "Include bit for the Estimated Service Parameters AC = BE should "
                            "always be 1";
        }
        if (ap_metric_tlv->estimated_service_parameters().include_ac_bk) {
            set_esp("EstServiceParametersBK", reporting_agent_bssid,
                    ap_metric_tlv->estimated_service_info_field(3));
        }
        if (ap_metric_tlv->estimated_service_parameters().include_ac_vo) {
            set_esp("EstServiceParametersVO", reporting_agent_bssid,
                    ap_metric_tlv->estimated_service_info_field(6));
        }
        if (ap_metric_tlv->estimated_service_parameters().include_ac_vi) {
            set_esp("EstServiceParametersVI", reporting_agent_bssid,
                    ap_metric_tlv->estimated_service_info_field(9));
        }
    }

    for (auto radio_tlv : cmdu_rx.getClassList<wfa_map::tlvProfile2RadioMetrics>()) {
        ret_val &= database.set_radio_metrics(radio_tlv->radio_uid(), radio_tlv->noise(),
                                              radio_tlv->transmit(), radio_tlv->receive_self(),
                                              radio_tlv->receive_other());
    }

    auto agent = database.m_agents.get(src_mac);
    if (!agent) {
        LOG(ERROR) << "Agent with mac is not found in database mac=" << src_mac;
        return false;
    }

    ret_val &= handle_tlv_ap_extended_metrics(agent, cmdu_rx);
    ret_val &= handle_tlv_associated_sta_link_metrics(src_mac, cmdu_rx);
    ret_val &= handle_tlv_associated_sta_extended_link_metrics(src_mac, cmdu_rx);
    ret_val &= handle_tlv_associated_sta_traffic_stats(src_mac, cmdu_rx);
    ret_val &= handle_tlv_associated_wifi6_sta_status_report(src_mac, cmdu_rx);

    // For now, this is only used for certification so update the certification cmdu.
    if (database.setting_certification_mode() &&
        (database.config.management_mode != BPL_MGMT_MODE_NOT_MULTIAP)) {
        m_link_metrics_task->construct_combined_infra_metric();
    }

    return ret_val;
}

bool Controller::handle_tlv_ap_ht_capabilities(ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool ret_val = true;

    for (const auto &ap_ht_caps_tlv : cmdu_rx.getClassList<wfa_map::tlvApHtCapabilities>()) {
        if (!database.set_ap_ht_capabilities(ap_ht_caps_tlv->radio_uid(),
                                             ap_ht_caps_tlv->flags())) {
            LOG(ERROR) << "Couldn't set values for AP HT Capabilities in Controller Data Model.";
            ret_val = false;
        }
    }
    return ret_val;
}

bool Controller::handle_tlv_ap_wifi6_capabilities(ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool ret_val = true;

    for (const auto &wifi6_caps_tlv : cmdu_rx.getClassList<wfa_map::tlvApWifi6Capabilities>()) {
        if (!database.set_ap_wifi6_capabilities(*wifi6_caps_tlv)) {
            LOG(ERROR) << "Couldn't set values for ap WIFI6capabilities data model";
            ret_val = false;
        }
    }
    return ret_val;
}

bool Controller::handle_tlv_wifi7_agent_capabilities(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                     std::shared_ptr<Agent> agent)
{
    auto wifi7_agt_caps_tlv = cmdu_rx.getClass<wfa_map::tlvWifi7AgentCapabilities>();
    if (!wifi7_agt_caps_tlv) {
        LOG(DEBUG) << "Cound't find WiFi 7 agent capabitites TLV";
        return true;
    }

    if (!database.set_wifi7_agent_capabilities(*wifi7_agt_caps_tlv, agent)) {
        LOG(ERROR) << "Couldn't set values for WiFi 7 agent capabilities data model";
        return false;
    }

    return true;
}

bool Controller::handle_tlv_eht_operations(ieee1905_1::CmduMessageRx &cmdu_rx,
                                           const sMacAddr &al_mac)
{
    auto eht_operations_tlv = cmdu_rx.getClass<wfa_map::tlvEHTOperations>();
    if (!eht_operations_tlv) {
        LOG(DEBUG) << "Cound't find EHT Operations TLV";
        return true;
    }

    if (!database.set_eht_operations(*eht_operations_tlv, al_mac)) {
        LOG(ERROR) << "Couldn't set values for eht operations data model";
        return false;
    }

    return true;
}

bool Controller::handle_tlv_apCapability(ieee1905_1::CmduMessageRx &cmdu_rx,
                                         std::shared_ptr<Agent> agent, bool early)
{
    auto tlv_ap_capability = cmdu_rx.getClass<wfa_map::tlvApCapability>();
    if (!tlv_ap_capability) {
        LOG(ERROR) << "addClass wfa_map::tlvApCapability failed";
        return false;
    }

    for (auto &radio : agent->radios) {
        radio.second->ap_capabilities.support_agent_initiated_rcpi_based_steering =
            tlv_ap_capability->value().support_agent_initiated_rcpi_based_steering;
        radio.second->ap_capabilities.support_unassociated_sta_link_metrics_on_non_operating_bssid =
            tlv_ap_capability->value().support_unassociated_sta_link_metrics_on_non_operating_bssid;
        radio.second->ap_capabilities.support_unassociated_sta_link_metrics_on_operating_bssid =
            tlv_ap_capability->value().support_unassociated_sta_link_metrics_on_operating_bssid;
    }

    if (early && tlv_ap_capability->value().support_agent_backhaul_sta_reconfiguration) {
        agent->bSTA_reconfiguration_supported = true;
    }
    //TODO : shall we update the datamodel here ???

    return true;
}

bool Controller::handle_cmdu_1905_associated_sta_link_metrics_response_message(
    const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received ASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE, mid=" << std::hex << mid;
    handle_tlv_associated_sta_link_metrics(src_mac, cmdu_rx);
    handle_tlv_associated_sta_extended_link_metrics(src_mac, cmdu_rx);
    return true;
}

bool Controller::handle_tlv_ap_extended_metrics(std::shared_ptr<Agent> agent,
                                                ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool ret_val = true;

    for (auto ap_extended_metric_tlv : cmdu_rx.getClassList<wfa_map::tlvApExtendedMetrics>()) {

        // Recalculate counters according to Agent Byte Units.
        ret_val &= database.set_vap_stats_info(
            ap_extended_metric_tlv->bssid(),
            database.recalculate_attr_to_byte_units(agent->byte_counter_units,
                                                    ap_extended_metric_tlv->unicast_bytes_sent()),
            database.recalculate_attr_to_byte_units(
                agent->byte_counter_units, ap_extended_metric_tlv->unicast_bytes_received()),
            database.recalculate_attr_to_byte_units(agent->byte_counter_units,
                                                    ap_extended_metric_tlv->multicast_bytes_sent()),
            database.recalculate_attr_to_byte_units(
                agent->byte_counter_units, ap_extended_metric_tlv->multicast_bytes_received()),
            database.recalculate_attr_to_byte_units(agent->byte_counter_units,
                                                    ap_extended_metric_tlv->broadcast_bytes_sent()),
            database.recalculate_attr_to_byte_units(
                agent->byte_counter_units, ap_extended_metric_tlv->broadcast_bytes_received()));
    }

    return ret_val;
}

bool Controller::handle_tlv_associated_sta_link_metrics(const sMacAddr &src_mac,
                                                        ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool ret_val = true;

    for (auto &sta_link_metric : cmdu_rx.getClassList<wfa_map::tlvAssociatedStaLinkMetrics>()) {

        // STA Metrics can hold information from different BSS sources, mostly when steering
        // Metrics of last registered and active value needs to be considered.
        auto response_list = sta_link_metric->bssid_info_list(0);

        if (!std::get<0>(response_list)) {
            LOG(ERROR) << "Failed to get bssid info list.";
            continue;
        }

        auto bssid_info = std::get<1>(response_list);

        // Verify reported BSSID and data model registered STAs BSSID is same.
        if (database.get_sta_parent(tlvf::mac_to_string(sta_link_metric->sta_mac())) !=
            tlvf::mac_to_string(bssid_info.bssid)) {
            LOG(INFO) << "Reported STA BSSID is not matching with datamodel. Reported bssid:"
                      << bssid_info.bssid;
            continue;
        }
        if (!database.dm_set_sta_link_metrics(sta_link_metric->sta_mac(),
                                              bssid_info.downlink_estimated_mac_data_rate_mbps,
                                              bssid_info.uplink_estimated_mac_data_rate_mbps,
                                              bssid_info.sta_measured_uplink_rcpi_dbm_enc)) {
            LOG(ERROR) << "Failed to set link metrics for STA:" << sta_link_metric->sta_mac();
            ret_val = false;
        }
    }
    return ret_val;
}

bool Controller::handle_tlv_associated_sta_extended_link_metrics(const sMacAddr &src_mac,
                                                                 ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool ret_val = true;

    for (auto &sta_extended_link_metric :
         cmdu_rx.getClassList<wfa_map::tlvAssociatedStaExtendedLinkMetrics>()) {

        if (sta_extended_link_metric->metrics_list_length() == 0) {
            LOG(INFO) << "No metrics provided for the station "
                      << sta_extended_link_metric->associated_sta();
            continue;
        }

        auto metrics_list = sta_extended_link_metric->metrics_list(0);
        if (!std::get<0>(metrics_list)) {
            LOG(ERROR) << "Failed to get metrics info list.";
            continue;
        }

        auto metrics = std::get<1>(metrics_list);

        // Verify reported BSSID and data model registered STAs BSSID is same.
        auto station = database.get_station(sta_extended_link_metric->associated_sta());
        if (!station) {
            LOG(ERROR) << "Failed to get station on db with mac: "
                       << sta_extended_link_metric->associated_sta();
            continue;
        }

        auto parent_bss = station->get_bss();
        if (!parent_bss) {
            LOG(ERROR) << "Connected BSS of the station is not found, sta mac="
                       << sta_extended_link_metric->associated_sta();
            continue;
        }
        if (parent_bss->bssid != metrics.bssid) {
            LOG(INFO) << "Reported STA BSSID is not matching with datamodel. Reported bssid:"
                      << metrics.bssid;
            continue;
        }
        if (!database.dm_set_sta_extended_link_metrics(sta_extended_link_metric->associated_sta(),
                                                       metrics)) {
            LOG(ERROR) << "Failed to set extended link metrics for STA:"
                       << sta_extended_link_metric->associated_sta();
            ret_val = false;
        }
    }
    return ret_val;
}

bool Controller::handle_tlv_associated_sta_traffic_stats(const sMacAddr &src_mac,
                                                         ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool ret_val = true;

    auto agent = database.m_agents.get(src_mac);

    if (!agent) {
        LOG(ERROR) << "Agent with mac is not found in database mac=" << src_mac;
        return false;
    }

    for (auto &sta_traffic_stat : cmdu_rx.getClassList<wfa_map::tlvAssociatedStaTrafficStats>()) {

        db::sAssociatedStaTrafficStats stats;

        // Recalculate counters according to Agent Byte Units.
        stats.m_byte_received = database.recalculate_attr_to_byte_units(
            agent->byte_counter_units, sta_traffic_stat->byte_received());

        stats.m_byte_sent = database.recalculate_attr_to_byte_units(agent->byte_counter_units,
                                                                    sta_traffic_stat->byte_sent());

        stats.m_packets_received     = sta_traffic_stat->packets_received();
        stats.m_packets_sent         = sta_traffic_stat->packets_sent();
        stats.m_retransmission_count = sta_traffic_stat->retransmission_count();
        stats.m_rx_packets_error     = sta_traffic_stat->rx_packets_error();
        stats.m_tx_packets_error     = sta_traffic_stat->tx_packets_error();

        if (!database.dm_set_sta_traffic_stats(sta_traffic_stat->sta_mac(), stats)) {
            LOG(ERROR) << "Failed to set traffic stats for STA:" << sta_traffic_stat->sta_mac();
            ret_val = false;
        }
    }
    return ret_val;
}

bool Controller::handle_tlv_associated_wifi6_sta_status_report(const sMacAddr &src_mac,
                                                               ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool ret_val = true;

    auto agent = database.m_agents.get(src_mac);
    if (!agent) {
        LOG(ERROR) << "Agent with mac is not found in database mac=" << src_mac;
        return false;
    }

    for (auto &wifi6_sta_status_rep :
         cmdu_rx.getClassList<wfa_map::tlvAssociatedWiFi6StaStatusReport>()) {

        auto station = database.get_station(wifi6_sta_status_rep->sta_mac());
        if (!station) {
            LOG(ERROR) << "Failed to get station on db with mac: "
                       << wifi6_sta_status_rep->sta_mac();
            return false;
        }

        std::vector<wfa_map::tlvAssociatedWiFi6StaStatusReport::sTidQueueSize> tid_queue_vector;
        uint8_t tid_list_length = wifi6_sta_status_rep->tid_queue_size_list_length();

        LOG(DEBUG) << "TID list length: " << tid_list_length
                   << " of MAC address of the associated STA: " << wifi6_sta_status_rep->sta_mac();

        for (uint8_t tid_index = 0; tid_index < tid_list_length; tid_index++) {
            auto tid_tuple = wifi6_sta_status_rep->tid_queue_size_list(tid_index);
            if (!std::get<0>(tid_tuple)) {
                LOG(ERROR) << "Invalid TID with queue size in tlvAssociatedWiFi6StaStatusReport";
                continue;
            }

            auto &qos_ctrl_params = std::get<1>(tid_tuple);

            LOG(DEBUG) << "TID: " << qos_ctrl_params.tid
                       << ", Queue Size: " << qos_ctrl_params.queue_size;

            tid_queue_vector.push_back(qos_ctrl_params);
        }

        if (!database.dm_add_tid_queue_sizes(*station, tid_queue_vector)) {
            LOG(ERROR) << "Failed to set TID and corresponding Queue Size for STA:"
                       << wifi6_sta_status_rep->sta_mac();
            ret_val = false;
        }
    }
    return ret_val;
}

bool Controller::handle_tlv_ap_vht_capabilities(ieee1905_1::CmduMessageRx &cmdu_rx)
{
    bool ret_val = true;

    for (const auto &vht_caps_tlv : cmdu_rx.getClassList<wfa_map::tlvApVhtCapabilities>()) {
        if (!database.set_ap_vht_capabilities(*vht_caps_tlv)) {
            LOG(ERROR) << "Couldn't set values for ap VHTcapabilities data model";
            ret_val = false;
        }
    }
    return ret_val;
}

bool Controller::handle_tlv_agent_ap_mld_configuration(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                       const sMacAddr &src_mac)
{
    auto agent_ap_mld_configuration_tlv = cmdu_rx.getClass<wfa_map::tlvAgentApMldConfiguration>();
    if (!agent_ap_mld_configuration_tlv) {
        LOG(ERROR) << "addClass wfa_map::tlvAgentApMldConfiguration failed";
        return false;
    }

    // placeholder //

    return true;
}

bool Controller::handle_cmdu_1905_ap_capability_report(const sMacAddr &src_mac,
                                                       ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received AP_CAPABILITY_REPORT_MESSAGE, mid=" << std::dec << int(mid);

    return handle_ap_capability_report(src_mac, cmdu_rx, false);
}

bool Controller::handle_cmdu_1905_operating_channel_report(const sMacAddr &src_mac,
                                                           ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received OPERATING_CHANNEL_REPORT_MESSAGE, mid=" << std::dec << int(mid);

    for (auto operating_channel_report_tlv :
         cmdu_rx.getClassList<wfa_map::tlvOperatingChannelReport>()) {
        auto &ruid    = operating_channel_report_tlv->radio_uid();
        auto tx_power = operating_channel_report_tlv->current_transmit_power();

        /*
            Here need to remove the CurrentOperatingClass data from the Controller Data Model which was
            set in previous OPERATING_CHANNEL_REPORT_MESSAGE.
         */
        database.remove_current_op_classes(ruid);

        LOG(INFO) << "operating channel report, ruid=" << ruid << ", tx_power=" << std::dec
                  << int(tx_power);

        auto operating_classes_list_length =
            operating_channel_report_tlv->operating_classes_list_length();

        for (uint8_t oc = 0; oc < operating_classes_list_length; oc++) {
            auto operating_class_tuple = operating_channel_report_tlv->operating_classes_list(oc);
            if (!std::get<0>(operating_class_tuple)) {
                LOG(ERROR) << "getting operating class entry has failed!";
                return false;
            }

            auto &operating_class_struct = std::get<1>(operating_class_tuple);
            auto operating_class         = operating_class_struct.operating_class;
            auto channel                 = operating_class_struct.channel_number;
            LOG(INFO) << "operating_class=" << int(operating_class)
                      << ", operating_channel=" << int(channel);

            database.add_current_op_class(ruid, operating_class, channel, tx_power);
        }
    }

    for (const auto &spatial_reuse_report_tlv :
         cmdu_rx.getClassList<wfa_map::tlvSpatialReuseReport>()) {

        if (!database.add_spatial_reuse_parameters(*spatial_reuse_report_tlv)) {
            LOG(ERROR) << "Couldn't set values for ap SpatialReuse data model";
        }
    }

    // send ACK_MESSAGE back to the Agent
    if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }

    return son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);
}

bool Controller::handle_cmdu_1905_higher_layer_data_message(const sMacAddr &src_mac,
                                                            ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received HIGHER_LAYER_DATA_MESSAGE , mid=" << std::hex << int(mid);

    auto tlvHigherLayerData = cmdu_rx.getClass<wfa_map::tlvHigherLayerData>();
    if (!tlvHigherLayerData) {
        LOG(ERROR) << "addClass wfa_map::tlvHigherLayerData failed";
        return false;
    }

    const auto protocol       = tlvHigherLayerData->protocol();
    const auto payload_length = tlvHigherLayerData->payload_length();
    LOG(DEBUG) << "Protocol: " << std::hex << protocol;
    LOG(DEBUG) << "Payload-Length: " << int(payload_length);

    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message to the agent, mid=" << std::hex << int(mid);
    return son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);
}

bool Controller::handle_cmdu_1905_backhaul_sta_steering_response(const sMacAddr &src_mac,
                                                                 ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received BACKHAUL_STA_STEERING_MESSAGE from " << src_mac << ", mid=" << std::hex
               << mid;

    auto tlv_backhaul_sta_steering_resp = cmdu_rx.getClass<wfa_map::tlvBackhaulSteeringResponse>();
    if (!tlv_backhaul_sta_steering_resp) {
        LOG(ERROR) << "Failed getClass<wfa_map::tlvBackhaulSteeringResponse>";
        return false;
    }

    auto bh_steering_resp_code = tlv_backhaul_sta_steering_resp->result_code();
    LOG(DEBUG) << "BACKHAUL_STA_STEERING_MESSAGE result_code: " << int(bh_steering_resp_code);

    if (bh_steering_resp_code) {
        auto error_code_tlv = cmdu_rx.getClass<wfa_map::tlvErrorCode>();
        if (!error_code_tlv) {
            LOG(ERROR) << "Failed getClass<wfa_map::tlvErrorCode>";
            return false;
        }
        LOG(DEBUG) << "BACKHAUL_STA_STEERING_MESSAGE error_code: "
                   << int(error_code_tlv->reason_code());
    }

    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }

    LOG(DEBUG) << "sending ACK message to the agent, mid=" << std::hex << mid;

    return son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);
}

bool Controller::handle_cmdu_1905_tunnelled_message(const sMacAddr &src_mac,
                                                    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received Tunnelled Message from " << src_mac << ", mid=" << std::hex << mid;

    // Parse the Source Info TLV
    auto source_info_tlv = cmdu_rx.getClass<wfa_map::tlvTunnelledSourceInfo>();
    if (!source_info_tlv) {
        LOG(ERROR) << "Failed parsing tlvTunnelledSourceInfo!";
        return false;
    }

    // Parse the Type
    auto type_tlv = cmdu_rx.getClass<wfa_map::tlvTunnelledProtocolType>();
    if (!type_tlv) {
        LOG(ERROR) << "Failed parsing tlvTunnelledProtocolType!";
        return false;
    }

    // Parse the Data
    auto data_tlv = cmdu_rx.getClass<wfa_map::tlvTunnelledData>();
    if (!data_tlv) {
        LOG(ERROR) << "Failed parsing tlvTunnelledData!";
        return false;
    }

    LOG(DEBUG) << "Tunnelled Message STA MAC: " << source_info_tlv->mac() << ", Type: " << std::hex
               << int(type_tlv->protocol_type()) << ", Data Length: " << std::dec
               << data_tlv->data_length() << ", Data: " << std::endl
               << beerocks::utils::dump_buffer(data_tlv->data(0), data_tlv->data_length());

    if (type_tlv->protocol_type() ==
        wfa_map::tlvTunnelledProtocolType::eTunnelledProtocolType::ASSOCIATION_REQUEST) {
        auto assoc_frame = assoc_frame::AssocReqFrame::parse(
            data_tlv->data(), data_tlv->data_length(),
            assoc_frame::AssocReqFrame::eFrameType::ASSOCIATION_REQUEST);
        if (!assoc_frame) {
            LOG(ERROR) << "Failed to parse Association Request frame";
        }
    }
    if (type_tlv->protocol_type() ==
        wfa_map::tlvTunnelledProtocolType::eTunnelledProtocolType::REASSOCIATION_REQUEST) {
        auto reassoc_frame = assoc_frame::AssocReqFrame::parse(
            data_tlv->data(), data_tlv->data_length(),
            assoc_frame::AssocReqFrame::eFrameType::REASSOCIATION_REQUEST);
        if (!reassoc_frame) {
            LOG(ERROR) << "Failed to parse Reassociation Request frame";
        }
    }

    // Note that we can't save the association frame in the DB after
    // we parsed it, because the tunneled message can be (and is)
    // received before the topology notification message that allows
    // us to create the station in the DB is received by the
    // controller.
    // This is not always a problem for prplMesh agents which use a
    // VS messages to add the station before the topology
    // notification, but it can be for non-prplMesh agents.
    return true;
}

bool Controller::handle_cmdu_1905_backhaul_sta_capability_report_message(
    const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received BACKHAUL_STA_CAPABILITY_REPORT_MESSAGE, mid=" << std::hex << mid
               << " from " << src_mac;

    for (auto bh_sta_radio_cap_tlv :
         cmdu_rx.getClassList<wfa_map::tlvBackhaulStaRadioCapabilities>()) {
        if (!bh_sta_radio_cap_tlv) {
            LOG(ERROR) << "Failed to get tlvBackhaulStaRadioCapabilities!";
            return false;
        }

        auto radio = database.get_radio_by_uid(bh_sta_radio_cap_tlv->ruid());
        if (!radio) {
            return false;
        }

        if (bh_sta_radio_cap_tlv->sta_mac_included()) {
            radio->backhaul_station_mac = *bh_sta_radio_cap_tlv->sta_mac();
        } else {
            LOG(INFO) << "STA MAC is not included in Backhaul STA Capability Report.";
            radio->backhaul_station_mac = beerocks::net::network_utils::ZERO_MAC;
        }

        auto sta_parent = database.get_sta_parent(tlvf::mac_to_string(radio->backhaul_station_mac));
        LOG(DEBUG) << "Backhaul STA of radio with ruid=" << bh_sta_radio_cap_tlv->ruid()
                   << " is sta_mac=" << radio->backhaul_station_mac << " parent_mac=" << sta_parent;

        if (radio->backhaul_station_mac != beerocks::net::network_utils::ZERO_MAC) {
            database.add_backhaul_station(radio->backhaul_station_mac,
                                          tlvf::mac_from_string(sta_parent), src_mac);
        }
        database.dm_set_radio_bh_sta(*radio, radio->backhaul_station_mac);
    }
    return true;
}

bool Controller::handle_cmdu_1905_failed_connection_message(const sMacAddr &src_mac,
                                                            ieee1905_1::CmduMessageRx &cmdu_rx)
{
    LOG(DEBUG) << "Received Failed Connection Message for STA";

    auto bssid_tlv = cmdu_rx.getClass<wfa_map::tlvBssid>();
    if (!bssid_tlv) {
        LOG(ERROR) << "Failed to get tlvBssid!";
        return false;
    }

    auto sta_mac_tlv = cmdu_rx.getClass<wfa_map::tlvStaMacAddressType>();
    if (!sta_mac_tlv) {
        LOG(ERROR) << "Failed to get tlvStaMacAddressType!";
        return false;
    }
    LOG(DEBUG) << "Sta Connection Failure: offending Sta MAC = " << sta_mac_tlv->sta_mac()
               << " bssid = " << bssid_tlv->bssid();

    auto status_code              = 0x0001; // Set default to Unspecified failure.
    auto profile2_status_code_tlv = cmdu_rx.getClass<wfa_map::tlvProfile2StatusCode>();
    auto agent                    = database.m_agents.get(src_mac);

    if (!agent) {
        LOG(ERROR) << "Agent with mac is not found in database mac=" << src_mac;
        return false;
    }
    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1 &&
        !profile2_status_code_tlv) {
        LOG(ERROR) << "Profile2 Status Code tlv is not supplied for Agent " << src_mac
                   << " with profile enum " << agent->profile;
    }
    if (profile2_status_code_tlv) {
        status_code = profile2_status_code_tlv->status_code();
    }

    auto reason_code              = wfa_map::tlvProfile2ReasonCode::UNSPECIFIED_REASON;
    auto profile2_reason_code_tlv = cmdu_rx.getClass<wfa_map::tlvProfile2ReasonCode>();

    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1 &&
        !profile2_reason_code_tlv) {
        LOG(ERROR) << "Profile2 Reason Code tlv is not supplied for Agent " << src_mac
                   << " with profile enum " << agent->profile;
    }
    if (profile2_reason_code_tlv) {
        reason_code = profile2_reason_code_tlv->reason_code();
    }
    if (status_code != 0) {
        if (!database.dm_add_failed_connection_event(bssid_tlv->bssid(), sta_mac_tlv->sta_mac(),
                                                     reason_code, status_code)) {
            LOG(ERROR) << "Failed to add FailedConnectionEvent.";
            return false;
        }
    }
    return true;
}

bool Controller::handle_cmdu_1905_beacon_response(const sMacAddr &src_mac,
                                                  ieee1905_1::CmduMessageRx &cmdu_rx)
{
    // here we need to extract and keep the data received from the STA
    // but currently we'll just print that we are here
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "got beacon response from STA. mid: 0x" << std::hex << mid;
    return true;
}

bool Controller::handle_intel_slave_join(
    const sMacAddr &src_mac, std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
    beerocks::beerocks_header &beerocks_header, ieee1905_1::CmduMessageTx &cmdu_tx,
    const std::shared_ptr<Agent> &agent)
{
    // Prepare outcoming response vs tlv
    auto join_response =
        beerocks::message_com::add_vs_tlv<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_RESPONSE>(
            cmdu_tx);
    if (!join_response) {
        LOG(ERROR) << "Failed adding intel vendor specific TLV";
        return false;
    }

    if (beerocks_header.action_op() != beerocks_message::ACTION_CONTROL_SLAVE_JOINED_NOTIFICATION) {
        LOG(ERROR) << "Unexpected Intel action op " << beerocks_header.action_op();
        return false;
    }

    auto notification =
        beerocks_header.addClass<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION>();
    if (!notification) {
        LOG(ERROR) << "addClass cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION failed";
        return false;
    }

    // mark slave as prplMesh
    // This is redundent for the normal initilization flow, but is needed for the renew flow
    LOG(DEBUG) << "prplMesh agent: received cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION from "
               << src_mac;
    database.set_prplmesh(src_mac);

    std::string slave_version =
        std::string(notification->slave_version(beerocks::message::VERSION_LENGTH));
    sMacAddr radio_mac        = notification->hostap().iface_mac;
    sMacAddr parent_bssid_mac = notification->backhaul_params().backhaul_bssid;
    std::string backhaul_mac  = tlvf::mac_to_string(notification->backhaul_params().backhaul_mac);
    std::string backhaul_ipv4 =
        beerocks::net::network_utils::ipv4_to_string(notification->backhaul_params().backhaul_ipv4);
    beerocks::eIfaceType backhaul_iface_type =
        (beerocks::eIfaceType)notification->backhaul_params().backhaul_iface_type;
    bool is_gw_slave           = (backhaul_iface_type == beerocks::IFACE_TYPE_GW_BRIDGE);
    beerocks::eType ire_type   = is_gw_slave ? beerocks::TYPE_GW : beerocks::TYPE_IRE;
    int backhaul_channel       = notification->backhaul_params().backhaul_channel;
    sMacAddr bridge_mac        = agent->al_mac;
    std::string bridge_mac_str = tlvf::mac_to_string(bridge_mac);
    std::string bridge_ipv4 =
        beerocks::net::network_utils::ipv4_to_string(notification->backhaul_params().bridge_ipv4);
    bool backhaul_manager = (bool)notification->backhaul_params().is_backhaul_manager;
    bool acs_enabled      = (notification->wlan_settings().channel == 0);

    std::string gw_name;
    if (is_gw_slave) {
        gw_name =
            "GW" +
            std::string(notification->platform_settings().local_master ? "_MASTER" : "_SLAVE_ONLY");
    }
    std::string slave_name =
        is_gw_slave
            ? gw_name
            : ("Prpl_" +
               (notification->platform_settings().local_master ? "MASTER_" : std::string()) +
               bridge_mac_str.substr(bridge_mac_str.size() - 5, bridge_mac_str.size() - 1));

    LOG(INFO) << "IRE Slave joined" << std::endl
              << "    slave_version=" << slave_version << std::endl
              << "    slave_name=" << slave_name << std::endl
              << "    parent_bssid_mac=" << parent_bssid_mac << std::endl
              << "    backhaul_mac=" << backhaul_mac << std::endl
              << "    backhaul_ipv4=" << backhaul_ipv4 << std::endl
              << "    bridge_mac=" << bridge_mac << std::endl
              << "    bridge_ipv4=" << bridge_ipv4 << std::endl
              << "    backhaul_manager=" << int(backhaul_manager) << std::endl
              << "    backhaul_type=" << beerocks::utils::get_iface_type_string(backhaul_iface_type)
              << std::endl
              << "    low_pass_filter_on = " << int(notification->low_pass_filter_on()) << std::endl
              << "    radio_mac = " << radio_mac << std::endl
              << "    channel = " << int(notification->wlan_settings().channel) << std::endl
              << "    is_gw_slave = " << int(is_gw_slave) << std::endl;

    if (!is_gw_slave) {

        // if not local GW but local master - then the parent bssid is remote AP (aka "far AP")
        // and is not yet in map
        if (!notification->platform_settings().local_master) {
            // rejecting join if gw haven't joined yet
            if ((parent_bssid_mac != beerocks::net::network_utils::ZERO_MAC) &&
                (!database.get_bss(parent_bssid_mac) ||
                 (!database.get_bss(parent_bssid_mac)->enabled))) {
                LOG(DEBUG) << "sending back join reject!";
                LOG(DEBUG) << "reject_debug: parent_bssid_has_node="
                           << (int)(database.get_bss(parent_bssid_mac) != nullptr);

                join_response->err_code() = beerocks::JOIN_RESP_REJECT;
                return son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);
            }
        }

        // sending to BML listeners, client disconnect notification on ire backhaul before changing it type from TYPE_CLIENT to bSTA
        std::shared_ptr<Station> bh_sta = database.get_station(tlvf::mac_from_string(backhaul_mac));
        if (bh_sta && !bh_sta->is_bSta() && bh_sta->state == beerocks::STATE_CONNECTED) {
            LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << backhaul_mac
                       << ", FORCING DISCONNECT NOTIFICATION!";
            bml_task::connection_change_event new_event;
            new_event.mac                     = backhaul_mac;
            new_event.force_client_disconnect = true;
            m_task_pool.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE,
                                   &new_event);
        }

        //TODO might need to handle bssids of VAP nodes as well in this case
        if (parent_bssid_mac != beerocks::net::network_utils::ZERO_MAC) {
            //add a placeholder
            LOG(DEBUG) << "add a placeholder backhaul_mac = " << backhaul_mac
                       << ", parent_bssid_mac = " << parent_bssid_mac
                       << ", bridge_mac = " << bridge_mac;
            database.add_backhaul_station(tlvf::mac_from_string(backhaul_mac), parent_bssid_mac,
                                          bridge_mac);
        } else if (database.get_sta_state(backhaul_mac) != beerocks::STATE_CONNECTED) {
            /* if the backhaul node doesn't exist, or is not already marked as connected,
            * we assume it is connected to the GW's LAN switch
            */
            LOG(DEBUG) << "connected to the GW's LAN switch ";
            auto gw = database.get_gw();
            if (!gw) {
                LOG(ERROR) << "can't get GW node!";
                return false;
            }

            if (gw->eth_switches.empty()) {
                LOG(ERROR) << "GW has no LAN SWITCH node!";
                return false;
            }

            LOG(DEBUG) << "add a placeholder backhaul_mac = " << backhaul_mac
                       << " gw_lan_switch = " << gw->eth_switches.begin()->first
                       << " STATE_CONNECTED";
            database.add_backhaul_station(tlvf::mac_from_string(backhaul_mac),
                                          gw->eth_switches.begin()->first);
            database.set_sta_state(backhaul_mac, beerocks::STATE_CONNECTED);
        }
    } else {
        backhaul_mac.clear();
    }

    //if the IRE connects via a different backhaul, mark previous backhaul as disconnected
    std::string previous_backhaul = tlvf::mac_to_string(database.get_agent_parent(bridge_mac));
    std::shared_ptr<Station> previous_bh_sta =
        database.get_station(tlvf::mac_from_string(previous_backhaul));
    if (!previous_backhaul.empty() && previous_backhaul != backhaul_mac && previous_bh_sta &&
        previous_bh_sta->is_bSta()) {
        LOG(DEBUG) << "marking previous backhaul " << previous_backhaul << " for IRE " << bridge_mac
                   << " as disconnected";
        database.set_sta_state(previous_backhaul, beerocks::STATE_DISCONNECTED);
    }

    // bridge_mac sta may have been created from DHCP/ARP event, if so delete it
    // this may only occur once
    if (database.has_station(bridge_mac)) {
        database.remove_sta(bridge_mac);
    }
    // add new GW/IRE bridge_mac
    LOG(DEBUG) << "adding node " << bridge_mac << " under " << backhaul_mac << ", and mark as type "
               << ire_type;
    if (is_gw_slave) {
        database.add_gateway(bridge_mac);
        agent->is_gateway = true;
    } else {
        database.add_agent(bridge_mac, tlvf::mac_from_string(backhaul_mac));
    }

    database.set_agent_state(bridge_mac_str, beerocks::STATE_CONNECTED);

    /*
    * Set IRE backhaul manager slave
    * keep in mind that the socket's peer mac will be the hostap mac
    */
    if (backhaul_manager) {
        /*
        * handle the IRE node itself, representing the backhaul
        */
        database.set_sta_backhaul_iface_type(tlvf::mac_from_string(backhaul_mac),
                                             backhaul_iface_type);

        database.set_sta_ipv4(backhaul_mac, bridge_ipv4);
        database.set_agent_ipv4(bridge_mac_str, bridge_ipv4);

        std::shared_ptr<Station> bh_sta = database.get_station(tlvf::mac_from_string(backhaul_mac));
        if (bh_sta) {
            bh_sta->set_bSta(true);
        }

        database.set_sta_name(backhaul_mac, slave_name + "_BH");
        database.set_agent_name(bridge_mac_str, slave_name);

        //TODO slave should include eth switch mac in the message
        //until then, generate eth address from bridge address
        auto eth_sw_mac_binary =
            beerocks::net::network_utils::get_eth_sw_mac_from_bridge_mac(bridge_mac);

        std::string eth_switch_mac = tlvf::mac_to_string(eth_sw_mac_binary);
        database.add_eth_switch(tlvf::mac_from_string(eth_switch_mac), bridge_mac);
        database.set_eth_switch_state(eth_switch_mac, beerocks::STATE_CONNECTED);
        database.set_eth_switch_name(eth_sw_mac_binary, slave_name + "_ETH");

        //run locating task on ire
        if (!database.is_sta_wireless(backhaul_mac)) {
            LOG(DEBUG) << "run_client_locating_task client_mac = " << bridge_mac;
            auto new_task = std::make_shared<client_locating_task>(database, cmdu_tx, m_task_pool,
                                                                   bridge_mac_str, true, 2000);
            m_task_pool.add_task(new_task);
        }

        //Run the client locating tasks for the previously located wired IRE. If cascaded IREs are connected with wire
        //the slave_join notification for the 2nd level IRE can come before 1st level IRE, causing the 2nd
        //level IRE to be placed at the same level as the 1st IRE in the DB
        auto agents = database.get_all_connected_agents();
        for (const auto &a : agents) {
            if (a->al_mac == bridge_mac || a->is_gateway) {
                LOG(INFO) << "client_locating_task is not run again for this ire: " << a->al_mac;
                continue;
            }
            std::shared_ptr<Station> agent_bh_sta = database.get_station(a->parent_mac);
            if (agent_bh_sta && !agent_bh_sta->get_bss()) {
                LOG(DEBUG) << "run_client_locating_task client_mac = " << a->al_mac;
                auto new_task = std::make_shared<client_locating_task>(
                    database, cmdu_tx, m_task_pool, tlvf::mac_to_string(a->al_mac), true, 2000);
                m_task_pool.add_task(new_task);
            }
        }
    }

    // Check Slave BeeRocks version //
    auto slave_version_s  = beerocks::version::version_from_string(slave_version);
    auto master_version_s = beerocks::version::version_from_string(BEEROCKS_VERSION);

    beerocks::string_utils::copy_string(join_response->master_version(), BEEROCKS_VERSION,
                                        beerocks::message::VERSION_LENGTH);

    // check if mismatch
    if (slave_version_s.major != master_version_s.major ||
        slave_version_s.minor != master_version_s.minor) {
        LOG(WARNING) << "IRE Slave joined, Mismatch version! slave_version="
                     << std::string(slave_version)
                     << " master_version=" << std::string(BEEROCKS_VERSION);
        LOG(WARNING) << " bridge_mac=" << bridge_mac << " bridge_ipv4=" << bridge_ipv4;
    }

    beerocks::eIfaceType hostap_iface_type =
        (beerocks::eIfaceType)notification->hostap().iface_type;

    LOG(INFO) << std::endl
              << "    hostap_iface_name=" << notification->hostap().iface_name << std::endl
              << "    hostap_iface_type="
              << beerocks::utils::get_iface_type_string(hostap_iface_type) << std::endl
              << "    ant_num=" << int(notification->hostap().ant_num)
              << " ant_gain=" << int(notification->hostap().ant_gain)
              << " channel=" << int(notification->cs_params().channel)
              << " conducted=" << int(notification->hostap().tx_power) << std::endl
              << "    radio_mac=" << radio_mac << std::endl;

    bool local_master = (bool)notification->platform_settings().local_master;
    if (local_master) {
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
        int prev_task_id = database.get_pre_association_steering_task_id();
        if (!m_task_pool.is_task_running(prev_task_id)) {
            LOG(DEBUG) << "starting Pre Association Steering task";
            auto new_pre_association_steering_task =
                std::make_shared<pre_association_steering_task>(database, cmdu_tx, m_task_pool);
            m_task_pool.add_task(new_pre_association_steering_task);
        }
#endif
    }

    /*
    * handle the HOSTAP node
    */
    if (database.get_radio_by_uid(radio_mac)) {
        auto hostap_iface_name = database.get_radio_iface_name(radio_mac);
        if (!hostap_iface_name.empty() &&
            hostap_iface_name.compare(notification->hostap().iface_name)) {
            LOG(ERROR) << "Mac duplication detected between "
                       << database.get_radio_iface_name(radio_mac) << " and "
                       << notification->hostap().iface_name;
            return false;
        }
    } else {
        database.add_radio(radio_mac, bridge_mac);
    }

    //reset/init radio stats when adding slave's radio node
    database.clear_radio_stats_info(bridge_mac, radio_mac);

    auto radio = database.get_radio(bridge_mac, radio_mac);
    if (!radio) {
        LOG(ERROR) << "Radio not found";
        return false;
    }

    radio->is_acs_enabled = acs_enabled;
    // Make sure AP is marked as not active. It will be set as active after setting all the
    // radio parameters on the database.
    son_actions::set_radio_active(database, m_task_pool, tlvf::mac_to_string(radio_mac), false);
    if (backhaul_manager) {
        agent->backhaul.wireless_backhaul_radio = radio;
    }

    database.set_radio_state(tlvf::mac_to_string(radio_mac), beerocks::STATE_CONNECTED);
    database.set_radio_iface_name(radio_mac, notification->hostap().iface_name);
    database.set_radio_iface_type(bridge_mac, radio_mac, hostap_iface_type);

    database.set_radio_ant_num(radio_mac, (beerocks::eWiFiAntNum)notification->hostap().ant_num);
    database.set_radio_ant_gain(radio_mac, notification->hostap().ant_gain);
    database.set_radio_tx_power(radio_mac, notification->hostap().tx_power);

    if (database.get_radio_5ghz_support(radio_mac)) {
        if (notification->low_pass_filter_on()) {
            database.set_radio_band_capability(radio_mac, beerocks::LOW_SUBBAND_ONLY);
        } else {
            database.set_radio_band_capability(radio_mac, beerocks::BOTH_SUBBAND);
        }
    } else {
        database.set_radio_band_capability(radio_mac, beerocks::SUBBAND_CAPABILITY_UNKNOWN);
    }
    autoconfig_wsc_parse_radio_caps(radio_mac, radio_caps);

    // send JOINED_RESPONSE with son config
    {

        beerocks::string_utils::copy_string(
            join_response->master_version(beerocks::message::VERSION_LENGTH), BEEROCKS_VERSION,
            beerocks::message::VERSION_LENGTH);
        join_response->config().monitor_total_ch_load_notification_hi_th_percent =
            database.config.monitor_total_ch_load_notification_hi_th_percent;
        join_response->config().monitor_total_ch_load_notification_lo_th_percent =
            database.config.monitor_total_ch_load_notification_lo_th_percent;
        join_response->config().monitor_total_ch_load_notification_delta_th_percent =
            database.config.monitor_total_ch_load_notification_delta_th_percent;
        join_response->config().monitor_min_active_clients =
            database.config.monitor_min_active_clients;
        join_response->config().monitor_active_client_th = database.config.monitor_active_client_th;
        join_response->config().monitor_client_load_notification_delta_th_percent =
            database.config.monitor_client_load_notification_delta_th_percent;
        join_response->config().monitor_rx_rssi_notification_threshold_dbm =
            database.config.monitor_rx_rssi_notification_threshold_dbm;
        join_response->config().monitor_rx_rssi_notification_delta_db =
            database.config.monitor_rx_rssi_notification_delta_db;
        join_response->config().monitor_ap_idle_threshold_B =
            database.config.monitor_ap_idle_threshold_B;
        join_response->config().monitor_ap_active_threshold_B =
            database.config.monitor_ap_active_threshold_B;
        join_response->config().monitor_ap_idle_stable_time_sec =
            database.config.monitor_ap_idle_stable_time_sec;
        join_response->config().monitor_disable_initiative_arp =
            database.config.monitor_disable_initiative_arp;
        join_response->config().ire_rssi_report_rate_sec = database.config.ire_rssi_report_rate_sec;

        LOG(DEBUG) << "send SLAVE_JOINED_RESPONSE";
        son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);
    }

    // calling this function to update arp monitor with new ip addr (bridge ip), which is diffrent from the ip received from, dhcp on the backhaul
    if (backhaul_manager && (!is_gw_slave) && database.is_sta_wireless(backhaul_mac)) {
        son_actions::handle_completed_connection(database, cmdu_tx, m_task_pool, backhaul_mac);
    }

    // update bml listeners
    bml_task::connection_change_event bml_new_event;
    bml_new_event.mac = bridge_mac_str;
    m_task_pool.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &bml_new_event);
    LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << bml_new_event.mac;

    // sending event to CS task
    LOG(DEBUG) << "CS_task,sending SLAVE_JOINED_EVENT for mac " << radio_mac;
    auto cs_new_event                  = new channel_selection_task::sSlaveJoined_event;
    cs_new_event->backhaul_is_wireless = beerocks::utils::is_device_wireless(backhaul_iface_type);
    cs_new_event->backhaul_channel     = backhaul_channel;
    cs_new_event->channel              = notification->cs_params().channel;
    cs_new_event->low_pass_filter_on   = notification->low_pass_filter_on();
    LOG(DEBUG) << "cs_new_event->low_pass_filter_on = " << int(cs_new_event->low_pass_filter_on)
               << " cs_new_event = " << intptr_t(cs_new_event);
    cs_new_event->hostap_mac = radio_mac;
    cs_new_event->cs_params  = notification->cs_params();

    m_task_pool.push_event(database.get_channel_selection_task_id(),
                           (int)channel_selection_task::eEvent::SLAVE_JOINED_EVENT,
                           (void *)cs_new_event);
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    // sending event to pre_association_steering_task
    LOG(DEBUG) << "pre_association_steering_task,sending STEERING_SLAVE_JOIN for mac " << radio_mac;
    pre_association_steering_task::sSteeringSlaveJoinEvent new_event{};
    new_event.radio_mac = tlvf::mac_to_string(radio_mac);
    m_task_pool.push_event(database.get_pre_association_steering_task_id(),
                           pre_association_steering_task::eEvents::STEERING_SLAVE_JOIN, &new_event);
#endif
    // In the case where wireless-BH is lost and agents reconnect to the controller
    // it is required to re-activate the AP in the nodes-map since it is set as not-active
    // when the topology-response not containing it is received by the controller.
    // When it joins the controller we need to activate it if not activated.
    database.set_radio_active(radio_mac, true);

    // Update all radios last seen timestamp
    database.update_radio_last_seen(radio_mac);

    return true;
}

/**
 * @brief Parse the radio basic capabilities TLV and store the operating class
 * in the database as supported channels.
 *
 * @param radio_mac radio mac address (RUID in non-Intel agent case)
 * @param radio_caps radio basic capabilities TLV received from the remote agent
 * @return true on success
 * @return false on failure
 */
bool Controller::autoconfig_wsc_parse_radio_caps(
    const sMacAddr &radio_mac, std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps)
{
    // read all operating class list
    auto operating_classes_list_length = radio_caps->operating_classes_info_list_length();
    if (operating_classes_list_length > beerocks::message::SUPPORTED_CHANNELS_LENGTH) {
        LOG(WARNING) << "operating class info list larger then maximum supported channels";
        operating_classes_list_length = beerocks::message::SUPPORTED_CHANNELS_LENGTH;
    }

    /*
    ** Here need to remove the OperatingClasses data element
    ** from the Controller Data Model because we are entering a new one
    */
    database.remove_hostap_supported_operating_classes(radio_mac);

    std::stringstream ss;
    for (int oc_idx = 0; oc_idx < operating_classes_list_length; oc_idx++) {
        auto operating_class_tuple = radio_caps->operating_classes_info_list(oc_idx);
        if (!std::get<0>(operating_class_tuple)) {
            LOG(ERROR) << "getting operating class entry has failed!";
            return false;
        }
        auto &op_class                  = std::get<1>(operating_class_tuple);
        auto operating_class            = op_class.operating_class();
        auto maximum_transmit_power_dbm = op_class.maximum_transmit_power_dbm();
        ss << "operating_class=" << int(operating_class) << std::endl;
        ss << "maximum_transmit_power_dbm=" << int(maximum_transmit_power_dbm) << std::endl;
        ss << "channel list={ ";
        auto channel_list = son::wireless_utils::operating_class_to_channel_set(operating_class);
        for (auto channel : channel_list) {
            ss << int(channel) << " ";
        }
        ss << "}" << std::endl;
        ss << "statically_non_operable_channel_list={ ";

        auto non_oper_channels_list_length =
            op_class.statically_non_operable_channels_list_length();
        std::vector<uint8_t> non_operable_channels;
        for (int ch_idx = 0; ch_idx < non_oper_channels_list_length; ch_idx++) {
            auto channel = op_class.statically_non_operable_channels_list(ch_idx);
            ss << int(*channel) << " ";
            non_operable_channels.push_back(*channel);
        }
        ss << " }" << std::endl;
        // store operating class in the DB for this hostap
        database.add_hostap_supported_operating_class(
            radio_mac, operating_class, maximum_transmit_power_dbm, non_operable_channels);
    }
    LOG(DEBUG) << "Radio basic capabilities:" << std::endl
               << ss.str() << std::endl
               << "Supported Channels:" << std::endl
               << database.get_hostap_supported_channels_string(radio_mac);

    return true;
}

bool Controller::handle_non_intel_slave_join(
    const sMacAddr &src_mac, std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
    const WSC::m1 &m1, const std::shared_ptr<Agent> &agent, const sMacAddr &radio_mac,
    ieee1905_1::CmduMessageTx &cmdu_tx)
{

    // Multi-AP Agent doesn't say anything about the backhaul, so simulate ethernet backhaul to satisfy
    // network map. MAC address is the bridge MAC with the fifth octet incremented by 1.
    // The mac address for the backhaul is the same since it is ethernet backhaul.
    sMacAddr bridge_mac = agent->al_mac;
    sMacAddr mac        = bridge_mac;
    mac.oct[4]++;
    //Set locally administered flag
    mac.oct[0] |= 1 << 1;
    std::string backhaul_mac = tlvf::mac_to_string(mac);
    // generate eth address from bridge address
    auto eth_switch_mac_binary = beerocks::net::network_utils::get_eth_sw_mac_from_bridge_mac(mac);
    std::string eth_switch_mac = tlvf::mac_to_string(eth_switch_mac_binary);
    LOG(INFO) << "IRE generic Slave joined" << std::endl
              << "    manufacturer=" << agent->device_info.manufacturer << std::endl
              << "    al_mac=" << bridge_mac << std::endl
              << "    eth_switch_mac=" << eth_switch_mac << std::endl
              << "    backhaul_mac=" << backhaul_mac << std::endl
              << "    radio_identifier = " << radio_mac << std::endl
              << "    radio band= " << m1.rf_bands() << std::endl;

    LOG(DEBUG) << "simulate backhaul connected to the GW's LAN switch ";
    auto gw = database.get_gw();
    if (!gw) {
        LOG(ERROR) << "can't get GW node!";
        return false;
    }

    if (gw->eth_switches.empty()) {
        LOG(ERROR) << "GW has no LAN SWITCH node!";
        return false;
    }

    LOG(DEBUG) << "add a placeholder backhaul_mac = " << backhaul_mac
               << " gw_lan_switch = " << gw->eth_switches.begin()->first << " STATE_CONNECTED";
    std::shared_ptr<Station> bh_sta = database.add_backhaul_station(
        tlvf::mac_from_string(backhaul_mac), gw->eth_switches.begin()->first);
    database.set_sta_state(backhaul_mac, beerocks::STATE_CONNECTED);

    // TODO bridge handling.
    // Assume repeater
    beerocks::eType ire_type = beerocks::TYPE_IRE;

    std::string bridge_mac_str = tlvf::mac_to_string(bridge_mac);

    // bridge_mac sta may have been created from DHCP/ARP event, if so delete it
    // this may only occur once
    if (database.has_station(bridge_mac)) {
        database.remove_sta(bridge_mac);
    }
    // add new GW/IRE bridge_mac
    LOG(DEBUG) << "adding node " << bridge_mac << " under " << backhaul_mac << ", and mark as type "
               << ire_type;

    database.add_agent(bridge_mac, tlvf::mac_from_string(backhaul_mac));

    database.set_agent_state(bridge_mac_str, beerocks::STATE_CONNECTED);
    database.set_sta_backhaul_iface_type(tlvf::mac_from_string(backhaul_mac),
                                         beerocks::eIfaceType::IFACE_TYPE_ETHERNET);
    bh_sta->set_bSta(true);
    database.set_sta_name(backhaul_mac, agent->device_info.manufacturer + "_BH");
    database.set_agent_name(bridge_mac_str, agent->device_info.manufacturer);
    database.add_eth_switch(tlvf::mac_from_string(eth_switch_mac), bridge_mac);
    database.set_eth_switch_state(eth_switch_mac, beerocks::STATE_CONNECTED);
    database.set_eth_switch_name(tlvf::mac_from_string(eth_switch_mac),
                                 agent->device_info.manufacturer + "_ETH");
    // An AP Capability Report will always be received after the M1, and thus
    // this OS Version value will be overriden by the more complete
    // SoftwareVersion value from the Device Inventory TLV if present
    database.set_software_version(agent, std::to_string(m1.os_version()));

    // Add radio if it doesn't already exist
    if (!database.get_radio_by_uid(radio_mac)) {
        database.add_radio(radio_mac, bridge_mac);
    }

    //reset/init radio stats when adding slave's radio node
    database.clear_radio_stats_info(bridge_mac, radio_mac);

    // TODO Assume no backhaul manager

    database.set_radio_state(tlvf::mac_to_string(radio_mac), beerocks::STATE_CONNECTED);
    database.set_radio_iface_name(radio_mac, "N/A");
    database.set_radio_iface_type(bridge_mac, radio_mac, beerocks::IFACE_TYPE_WIFI_UNSPECIFIED);

    // TODO number of antennas comes from HT/VHT capabilities (implicit from NxM)
    // TODO ant_gain and tx_power will not be set
    database.set_radio_ant_num(radio_mac, beerocks::eWiFiAntNum::ANT_NONE);
    database.set_radio_ant_gain(radio_mac, 0);
    database.set_radio_tx_power(radio_mac, 0);
    database.set_radio_active(radio_mac, true);
    // TODO ipv4 will not be set

    autoconfig_wsc_parse_radio_caps(radio_mac, radio_caps);
    // TODO assume SSIDs are not hidden

    // TODO
    //        if (database.get_radio_5ghz_support(radio_mac)) {
    //            if (notification->low_pass_filter_on()) {
    //                database.set_radio_band_capability(radio_mac, beerocks::LOW_SUBBAND_ONLY);
    //            } else {
    //                database.set_radio_band_capability(radio_mac, beerocks::BOTH_SUBBAND);
    //            }
    //        } else {
    database.set_radio_band_capability(radio_mac, beerocks::SUBBAND_CAPABILITY_UNKNOWN);
    //        }

    // update bml listeners
    bml_task::connection_change_event bml_new_event;
    bml_new_event.mac = bridge_mac_str;
    m_task_pool.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &bml_new_event);
    LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << bml_new_event.mac;

    LOG(DEBUG) << "send AP_AUTOCONFIG_WSC M2";
    return son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);
}

bool Controller::handle_cmdu_control_message(
    const sMacAddr &src_mac, std::shared_ptr<beerocks::beerocks_header> beerocks_header)
{
    sMacAddr radio_mac        = beerocks_header->actionhdr()->radio_mac();
    std::string radio_mac_str = tlvf::mac_to_string(radio_mac);

    // Sanity tests
    if (radio_mac == beerocks::net::network_utils::ZERO_MAC) {
        if (beerocks_header->action_op() !=
            beerocks_message::ACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION) {
            /*
                dhcp_complete_notification is sent to the controller
                without a radio mac. This is the expected behavior.
                Currently, the message is ignored which leads to the
                IP of the station not showing in the bml-connection-map.
                Don't ignore this message even though radio_mac is zero.
            */
            LOG(ERROR) << "CMDU received with id=" << int(beerocks_header->id())
                       << " op=" << int(beerocks_header->action_op()) << " with empty mac!";
            return false;
        }
    }

    if (beerocks_header->actionhdr()->direction() == beerocks::BEEROCKS_DIRECTION_AGENT) {
        return true;
    }

    //Update all radios last seen timestamp
    database.update_radio_last_seen(radio_mac);

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE: {
        LOG(DEBUG)
            << "received ACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE from "
            << radio_mac;
        auto response = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE>();

        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        auto new_event        = new channel_selection_task::sRestrictedChannelResponse_event;
        new_event->hostap_mac = beerocks_header->actionhdr()->radio_mac();
        new_event->success    = response->success();
        m_task_pool.push_event(
            database.get_channel_selection_task_id(),
            (int)channel_selection_task::eEvent::RESTRICTED_CHANNEL_RESPONSE_EVENT,
            (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION>();

        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        int vap_id = notification->vap_id();
        LOG(INFO) << "received ACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION from " << radio_mac
                  << " vap_id=" << vap_id;

        const auto disabled_bssid = database.get_radio_bss_mac(radio_mac, vap_id);
        if (disabled_bssid == beerocks::net::network_utils::ZERO_MAC) {
            LOG(INFO) << "AP Disabled on unknown vap, vap_id=" << vap_id;
            break;
        }
        auto bss = database.get_bss(disabled_bssid);
        if (!bss) {
            LOG(ERROR) << "Failed to get BSS with BSSID: " << disabled_bssid;
            break;
        }

        for (const auto &client : bss->connected_stations) {
            son_actions::handle_dead_station(tlvf::mac_to_string(client.first), true, database,
                                             m_task_pool);
        }

        // Update BSSes in the Agent
        auto radio = database.get_radio(src_mac, radio_mac);
        if (!radio) {
            LOG(ERROR) << "No radio found for radio_uid " << radio_mac << " on " << src_mac;
            break;
        }

        LOG(DEBUG) << "Removing vap from DB: " << bss->dm_path;
        database.disable_bss(*radio, *bss);

        if (radio->bsses.erase(disabled_bssid) != 1) {
            LOG(ERROR) << "No BSS " << disabled_bssid << " could be erased on " << radio_mac;
        }

        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION>();

        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        int vap_id = notification->vap_id();
        LOG(INFO) << "received ACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION from " << radio_mac
                  << " vap_id=" << vap_id;

        auto bssid = notification->vap_info().mac;
        auto ssid  = std::string((char *)notification->vap_info().ssid);

        // Update BSSes in the Agent
        auto radio = database.get_radio(src_mac, radio_mac);
        if (!radio) {
            LOG(ERROR) << "No radio found for radio_uid " << radio_mac << " on " << src_mac;
            break;
        }

        auto bss = database.add_bss(*radio, bssid, ssid, vap_id);
        // update BSS vap_id if still undefined
        bss->update_vap_id(vap_id);
        LOG_IF(bss->get_vap_id() != vap_id, ERROR)
            << "BSS " << bssid << " changed vap_id " << bss->get_vap_id() << " -> " << vap_id;
        bss->enabled   = true;
        bss->fronthaul = notification->vap_info().fronthaul_vap;
        bss->backhaul  = notification->vap_info().backhaul_vap;

        // update bml listeners
        bml_task::connection_change_event new_event;
        new_event.mac = tlvf::mac_to_string(database.get_radio_parent_agent(radio_mac));
        m_task_pool.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
        LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << new_event.mac;

        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION: {
        sMacAddr backhaul_mac = database.get_radio_parent_agent(radio_mac);

        LOG(ERROR) << "Hostap CSA ERROR for IRE " << backhaul_mac << " hostap mac=" << radio_mac;

        // TODO handle CSA error
        son_actions::set_radio_active(database, m_task_pool, radio_mac_str, false);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_CSA_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_CONTROL_HOSTAP_CSA_NOTIFICATION from " << radio_mac;

        auto notification =
            beerocks_header->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION failed";
            return false;
        }

        LOG(DEBUG) << "CS_task,sending CSA_EVENT for mac " << radio_mac;
        auto new_event        = new channel_selection_task::sCsa_event;
        new_event->hostap_mac = beerocks_header->actionhdr()->radio_mac();
        new_event->cs_params  = notification->cs_params();
        m_task_pool.push_event(database.get_channel_selection_task_id(),
                               (int)channel_selection_task::eEvent::CSA_EVENT, (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_ACS_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_CONTROL_HOSTAP_ACS_NOTIFICATION from " << radio_mac;

        auto notification =
            beerocks_header->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION failed";
            return false;
        }
        LOG(DEBUG) << "CS_task,sending ACS_RESPONSE_EVENT for mac " << radio_mac;

        auto new_event        = new channel_selection_task::sAcsResponse_event;
        new_event->hostap_mac = radio_mac;
        new_event->cs_params  = notification->cs_params();
        m_task_pool.push_event(database.get_channel_selection_task_id(),
                               (int)channel_selection_task::eEvent::ACS_RESPONSE_EVENT,
                               (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION failed";
            return false;
        }

        // Update BSSes in the Agent
        auto radio = database.get_radio(src_mac, radio_mac);
        if (!radio) {
            LOG(ERROR) << "No radio found for radio_uid " << radio_mac << " on " << src_mac;
            break;
        }

        radio->bsses.keep_new_prepare();
        typedef struct {
            std::string mac;
            std::string ssid;
        } sVapElement;
        std::unordered_map<int8_t, sVapElement> vaps_info;
        std::string vaps_list;
        for (int8_t vap_id = beerocks::IFACE_VAP_ID_MIN; vap_id <= beerocks::IFACE_VAP_ID_MAX;
             vap_id++) {
            auto vap_mac = tlvf::mac_to_string(notification->params().vaps[vap_id].mac);
            if (vap_mac != beerocks::net::network_utils::ZERO_MAC_STRING) {
                auto bss = database.add_bss(
                    *radio, notification->params().vaps[vap_id].mac,
                    std::string((char *)notification->params().vaps[vap_id].ssid), vap_id);
                // update BSS vap_id if still undefined
                bss->update_vap_id(vap_id);
                bss->fronthaul = notification->params().vaps[vap_id].fronthaul_vap;
                bss->backhaul  = notification->params().vaps[vap_id].backhaul_vap;

                vaps_info[vap_id].mac = vap_mac;
                vaps_info[vap_id].ssid =
                    std::string((char *)notification->params().vaps[vap_id].ssid);
                vaps_list += ("    vap_id=" + std::to_string(vap_id) +
                              ", vap_mac=" + (vaps_info[vap_id]).mac +
                              " , ssid=" + (vaps_info[vap_id]).ssid + std::string("\n"));
            }
        }

        LOG(INFO) << "sACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION from slave " << radio_mac
                  << std::endl
                  << "vaps_list:" << std::endl
                  << vaps_list;

        for (const auto &vap : vaps_info) {
            if (!database.update_bss(src_mac, radio_mac, tlvf::mac_from_string(vap.second.mac),
                                     vap.second.ssid)) {
                LOG(ERROR) << "Failed to update VAP for radio " << radio_mac << " BSS "
                           << vap.second.mac << " SSID " << vap.second.ssid;
            }
        }

        auto removed = radio->bsses.keep_new_remove_old();
        for (const auto &bss : removed) {

            // Remove all clients from that vap
            for (const auto &client : bss->connected_stations) {
                son_actions::handle_dead_station(tlvf::mac_to_string(client.first), true, database,
                                                 m_task_pool);
            }

            // Remove the vap from DB
            LOG(DEBUG) << "Removing BSS " << bss->bssid << " with path " << bss->dm_path
                       << " from DB";
            database.disable_bss(*radio, *bss);
        }

        // update bml listeners
        bml_task::connection_change_event new_event;
        new_event.mac = tlvf::mac_to_string(database.get_radio_parent_agent(radio_mac));
        m_task_pool.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
        LOG(DEBUG) << "BML, sending IRE connect CONNECTION_CHANGE for mac " << new_event.mac;

        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION: {

        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION failed";
            return false;
        }

        std::string client_mac = tlvf::mac_to_string(notification->params().mac);
        std::string client_ipv4 =
            beerocks::net::network_utils::ipv4_to_string(notification->params().ipv4);
        LOG(DEBUG) << "received arp monitor notification from slave mac " << radio_mac << ":"
                   << std::endl
                   << "   client_mac=" << client_mac << std::endl
                   << "   client_ipv4=" << client_ipv4 << std::endl
                   << "   state=" << int(notification->params().state)
                   << "   source=" << int(notification->params().source)
                   << "   type=" << int(notification->params().type);

        // IMPORTANT: Ignore RTM_DELNEIGH messages on the GRX350/IRE220 platforms.
        // Since the transport layer is accelerated, the OS may incorrectly decide
        // that a connected client has disconnected.
        //  if(notification->params.type == ARP_TYPE_DELNEIGH && !database.is_sta__wireless(client_mac)) {
        //     LOG(INFO) << "ARP type RTM_DELNEIGH received!! handle dead client mac = " << client_mac;
        //     son_actions::handle_dead_station(client_mac, true, database, tasks);
        //     break;
        //  }

        if (client_ipv4 == beerocks::net::network_utils::ZERO_IP_STRING) {
            LOG(DEBUG) << "arp ipv4 is 0.0.0.0, ignoring";
            break;
        }

        std::shared_ptr<Station> station = database.get_station(tlvf::mac_from_string(client_mac));
        std::shared_ptr<Agent> agent     = nullptr;
        if (!station) {
            agent = database.get_agent(tlvf::mac_from_string(client_mac));
        }
        bool new_node = !station && !agent;

        bool run_locating_task = false;
        // Since wireless clients are added to the DB on association, an ARP on non-existing node
        // can only be received for Ethernet clients
        if (new_node || !database.is_sta_wireless(client_mac)) {

            // Assume node is connected to the GW's LAN switch
            // client_locating_task will find the correct position
            if (new_node) {
                LOG(DEBUG) << "handle_control_message - calling add_node_to_gw_default_location "
                              "client_mac = "
                           << client_mac;
                if (!son_actions::add_station_to_default_location(database, client_mac)) {
                    LOG(ERROR)
                        << "handle_control_message - add_station_to_default_location failed!";
                    break;
                }
                station = database.get_station(tlvf::mac_from_string(client_mac));
            }

            // New IP
            if (new_node || database.get_sta_ipv4(client_mac) != client_ipv4) {
                LOG(DEBUG) << "Update node IP - mac: " << client_mac << " ipv4: " << client_ipv4;
                database.set_sta_ipv4(client_mac, client_ipv4);
                son_actions::handle_completed_connection(database, cmdu_tx, m_task_pool,
                                                         client_mac);
            }

            // Run locating task only on CLIENTs or IREs
            if (station || agent) {
                run_locating_task = true;
            }

            // Wireless Node
        } else {

            // Client NOT connected
            if (database.get_sta_state(client_mac) == beerocks::STATE_DISCONNECTED) {
                LOG(DEBUG) << "node_state = DISCONNECTED client_mac = " << client_mac
                           << " client_ipv4 =" << client_ipv4;

                // The IP has changed
            } else if (database.get_sta_ipv4(client_mac) != client_ipv4) {

                LOG(DEBUG) << "Update node IP - mac: " << client_mac << " ipv4: " << client_ipv4;
                database.set_sta_ipv4(client_mac, client_ipv4);
                son_actions::handle_completed_connection(database, cmdu_tx, m_task_pool,
                                                         client_mac);
            }
        }

        // Update the last-seen timestamp
        // Handled at this point to make sure the client was added to the DB
        database.update_sta_last_seen(client_mac);

        // Run client locating task for reachable or stale client/IRE nodes only if on ETH_FRONT port
        // or WIRELESS_FRONT (in case of eth devices connected to IREs and arp notf was send from GW)
        if (run_locating_task &&
            ((notification->params().source == beerocks::ARP_SRC_ETH_FRONT) ||
             (notification->params().source == beerocks::ARP_SRC_WIRELESS_FRONT))) {
            LOG(DEBUG) << "run_client_locating_task client_mac = " << client_mac;

            std::shared_ptr<Agent> parent_agent = database.get_agent_by_radio_uid(radio_mac);
            if (!parent_agent) {
                LOG(ERROR) << "Parent agent not found for radio " << radio_mac;
                break;
            }
            std::shared_ptr<Agent::sEthSwitch> eth_switch =
                database.get_eth_switch(parent_agent->al_mac);
            if (!eth_switch) {
                LOG(ERROR) << "Radio " << radio_mac << " does not have an Ethernet switch sibling!";
                break;
            }

            auto client = database.get_station(tlvf::mac_from_string(client_mac));
            if (!client) {
                LOG(ERROR) << "client " << client_mac << " not found";
                break;
            }

            int prev_task_id = client->get_client_locating_task_id(true /*reachable*/);

            if (m_task_pool.is_task_running(prev_task_id)) {
                LOG(DEBUG) << "client locating task already running for " << client_mac;
            } else {
                LOG(DEBUG) << "running client_locating_task on client = " << client_mac;
                auto new_task = std::make_shared<client_locating_task>(
                    database, cmdu_tx, m_task_pool, client_mac, true /*reachable*/, 2000,
                    tlvf::mac_to_string(eth_switch->mac));
                m_task_pool.add_task(new_task);
            }
        } else {
            LOG(INFO) << "Not running client_locating_task for client_mac " << client_mac
                      << " notification->params.source: " << (int)notification->params().source;
        }

        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass "
                          "ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION failed";
            return false;
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE failed";
            return false;
        }

        std::string client_mac = tlvf::mac_to_string(notification->params().result.mac);
        std::string ap_mac     = radio_mac_str;

        auto client = database.get_station(tlvf::mac_from_string(client_mac));
        if (!client) {
            LOG(ERROR) << "client " << client_mac << " not found";
            return false;
        }

        bool is_parent =
            (client->get_bss()->bssid == database.get_radio_bss_mac(tlvf::mac_from_string(ap_mac),
                                                                    notification->params().vap_id));

        auto agent = database.get_agent(src_mac);
        if (!agent) {
            LOG(ERROR) << "agent " << src_mac << " not found";
            break;
        }

        auto radio = database.get_radio(src_mac, radio_mac);
        if (!radio) {
            LOG(ERROR) << "No radio found with uid " << radio_mac << " on " << src_mac;
            break;
        }

        auto bh                  = agent->backhaul.wireless_backhaul_radio;
        bool is_backhaul_manager = (bh && (bh->radio_uid == radio->radio_uid));

        LOG_CLI(DEBUG,
                "rssi measurement response: "
                    << client_mac << " (sta) <-> (ap) " << ap_mac
                    << " rx_packets=" << int(notification->params().rx_packets)
                    << " rx_rssi=" << int(notification->params().rx_rssi)
                    << " phy_rate_100kb (RX|TX)=" << int(notification->params().rx_phy_rate_100kb)
                    << " | " << int(notification->params().tx_phy_rate_100kb)
                    << " is_parent=" << (is_parent ? "1" : "0")
                    << " is_backhaul_manager=" << (is_backhaul_manager ? "1" : "0")
                    << " src_module=" << int(notification->params().src_module)
                    << " id=" << int(beerocks_header->id()) << " bssid=" << client->get_bss()->bssid
                    << " vap_id=" << int(notification->params().vap_id));

        //response return from slave backhaul manager , updating the matching same band sibling.
        sMacAddr parent_agent_alid = database.get_radio_parent_agent(tlvf::mac_from_string(ap_mac));
        LOG(DEBUG) << "Parent agent alid " << parent_agent_alid;
        sMacAddr parent_agent_bh = database.get_agent_parent(parent_agent_alid);
        LOG(DEBUG) << "Parent agent BH " << parent_agent_bh;
        std::shared_ptr<Station> parent_backhaul = database.get_station(parent_agent_bh);
        if (is_backhaul_manager && parent_backhaul && parent_backhaul->get_bss() &&
            database.is_sta_5ghz(tlvf::mac_from_string(client_mac))) {
            auto priv_ap_mac = ap_mac;
            ap_mac           = database.get_5ghz_sibling_bss(ap_mac);
            LOG(DEBUG) << "update rssi measurement BH manager from ap_mac = " << priv_ap_mac
                       << " to = " << ap_mac;
        }
        if (ap_mac.empty()) {
            LOG(ERROR) << "update rssi measurement failed";
        }
        client->set_cross_rx_rssi(ap_mac, notification->params().rx_rssi,
                                  notification->params().rx_packets);
        if (is_parent) {
            client->cross_tx_phy_rate_100kb = notification->params().tx_phy_rate_100kb;
            client->cross_rx_phy_rate_100kb = notification->params().rx_phy_rate_100kb;
        }
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
        if ((beerocks_header->id() == database.get_pre_association_steering_task_id())) {
            beerocks_message::sSteeringEvSnr new_event;
            new_event.snr        = notification->params().rx_snr;
            new_event.client_mac = notification->params().result.mac;
            new_event.bssid      = database.get_radio_bss_mac(tlvf::mac_from_string(ap_mac),
                                                         notification->params().vap_id);
            m_task_pool.push_event(database.get_pre_association_steering_task_id(),
                                   pre_association_steering_task::eEvents::
                                       STEERING_EVENT_RSSI_MEASUREMENT_SNR_NOTIFICATION,
                                   &new_event);
        }
#endif
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION failed";
            return false;
        }
        std::string client_mac = tlvf::mac_to_string(notification->params().result.mac);

        auto client = database.get_station(tlvf::mac_from_string(client_mac));
        if (!client) {
            LOG(ERROR) << "client " << client_mac << " not found";
            return false;
        }

        std::string client_parent_mac = database.get_sta_parent(client_mac);
        sMacAddr bssid = database.get_radio_bss_mac(radio_mac, notification->params().vap_id);
        bool is_parent = (tlvf::mac_from_string(client_parent_mac) == bssid);

        int rx_rssi = int(notification->params().rx_rssi);

        LOG_CLI(DEBUG,
                "measurement change notification: "
                    << client_mac << " (sta) <-> (ap) " << radio_mac << " rx_rssi=" << rx_rssi
                    << " phy_rate_100kb (RX|TX)=" << int(notification->params().rx_phy_rate_100kb)
                    << " | " << int(notification->params().tx_phy_rate_100kb));

        if (!client->is_bSta() &&
            (database.get_sta_state(client_mac) == beerocks::STATE_CONNECTED) &&
            (!database.get_sta_handoff_flag(*client)) && is_parent) {

            client->set_cross_rx_rssi(radio_mac_str, notification->params().rx_rssi,
                                      notification->params().rx_packets);
            client->cross_tx_phy_rate_100kb = notification->params().tx_phy_rate_100kb;
            client->cross_rx_phy_rate_100kb = notification->params().rx_phy_rate_100kb;

            /*
                * when a notification arrives, it means a large change in rx_rssi occurred (above the defined thershold)
                * therefore, we need to create an optimal path task to relocate the node if needed
                */
            if (m_task_pool.is_task_running(client->roaming_task_id)) {
                LOG(DEBUG) << "roaming task already running for " << client_mac;
            } else {
                auto new_task = std::make_shared<optimal_path_task>(database, cmdu_tx, m_task_pool,
                                                                    client_mac, 0, "");
                m_task_pool.add_task(new_task);
            }
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION failed";
            return false;
        }
        std::string client_mac = tlvf::mac_to_string(notification->mac());

        LOG(DEBUG) << "ACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION, client_mac=" << client_mac
                   << " hostap mac=" << radio_mac;

        std::shared_ptr<Station> client = database.get_station(notification->mac());
        if (!client) {
            break;
        }
        if (client->is_bSta()) {
            LOG(INFO) << "IRE CLIENT_NO_RESPONSE_NOTIFICATION, client_mac=" << client_mac
                      << " hostap mac=" << radio_mac
                      << " closing socket and marking as disconnected";
            bool reported_by_parent = radio_mac_str == database.get_sta_parent(client_mac);
            son_actions::handle_dead_station(client_mac, reported_by_parent, database, m_task_pool);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION failed";
            return false;
        }

        std::string client_mac = tlvf::mac_to_string(notification_in->mac());
        std::string ipv4 = beerocks::net::network_utils::ipv4_to_string(notification_in->ipv4());
        LOG(DEBUG) << "dhcp complete for client " << client_mac << " new ip=" << ipv4
                   << " previous ip=" << database.get_sta_ipv4(client_mac);

        std::shared_ptr<Station> station = database.get_station(tlvf::mac_from_string(client_mac));
        if (!station) {
            LOG(DEBUG) << "client mac not in DB, add temp node " << client_mac;
            station = database.add_station(src_mac, tlvf::mac_from_string(client_mac));
            database.update_sta_last_seen(client_mac);
        }

        if (station->is_bSta()) {
            LOG(INFO) << "Ignoring DHCP notification for mac " << client_mac
                      << ", as it's not a client";
            return true;
        }

        database.set_sta_ipv4(client_mac, ipv4);
        database.set_sta_name(
            client_mac, std::string(notification_in->name(beerocks::message::NODE_NAME_LENGTH)));

        if (database.is_sta_wireless(client_mac)) {
            auto notification_out = beerocks::message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION>(cmdu_tx);

            if (!notification_out) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            notification_out->mac()  = notification_in->mac();
            notification_out->ipv4() = notification_in->ipv4();

            auto client_bssid = database.get_sta_parent(client_mac);
            if (client_bssid.empty()) {
                LOG(WARNING) << "Client does not have a valid parent hostap on the database";
                return true;
            }
            auto client_radio = database.get_bss_parent_radio(client_bssid);
            LOG(WARNING) << "Client " << client_mac
                         << " is connected wirelessly, Sending IP addr notification to radio="
                         << client_radio;

            auto agent_mac = database.get_radio_parent_agent(tlvf::mac_from_string(client_radio));
            son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, client_radio);

        } else {
            LOG(DEBUG) << "run_client_locating_task client_mac = " << client_mac;
            auto client = database.get_station(tlvf::mac_from_string(client_mac));
            if (!client) {
                LOG(ERROR) << "client " << client_mac << " not found";
                break;
            }
            int prev_task_id = client->get_client_locating_task_id(true);
            if (m_task_pool.is_task_running(prev_task_id)) {
                LOG(DEBUG) << "client locating task already running for " << client_mac;
            } else {
                auto new_task = std::make_shared<client_locating_task>(
                    database, cmdu_tx, m_task_pool, client_mac, true, 2000);
                m_task_pool.add_task(new_task);
            }
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION failed";
            return false;
        }
        LOG(DEBUG) << "received ACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION hostap_mac="
                   << radio_mac;

        auto new_event        = new channel_selection_task::sCacCompleted_event;
        new_event->hostap_mac = radio_mac;
        new_event->params     = notification->params();
        m_task_pool.push_event(database.get_channel_selection_task_id(),
                               (int)channel_selection_task::eEvent::CAC_COMPLETED_EVENT,
                               (void *)new_event);

        auto channel_ext_above =
            (notification->params().frequency < notification->params().center_frequency1) ? true
                                                                                          : false;

        beerocks::WifiChannel wifi_channel = beerocks::WifiChannel(
            notification->params().channel, notification->params().center_frequency1,
            static_cast<beerocks::eWiFiBandwidth>(notification->params().bandwidth),
            channel_ext_above);
        if (!database.set_radio_wifi_channel(radio_mac, wifi_channel)) {
            LOG(ERROR) << "set radio wifi channel failed, mac=" << radio_mac;
        }

        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION failed";
            return false;
        }
        LOG(DEBUG)
            << "received ACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION hostap_mac="
            << radio_mac;

        auto new_event        = new channel_selection_task::sDfsChannelAvailable_event;
        new_event->hostap_mac = radio_mac;
        new_event->params     = notification->params();
        m_task_pool.push_event(database.get_channel_selection_task_id(),
                               (int)channel_selection_task::eEvent::DFS_CHANNEL_AVAILABLE_EVENT,
                               (void *)new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE: {
        auto response =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE failed";
            return false;
        }

        for (auto i = 0; i < response->sta_stats_size(); i++) {
            auto sta_stats_tuple = response->sta_stats(i);
            if (!std::get<0>(sta_stats_tuple)) {
                LOG(ERROR) << "Couldn't access sta in location " << i;
                continue;
            }
            auto &sta_stats = std::get<1>(sta_stats_tuple);
            auto client_mac = tlvf::mac_to_string(sta_stats.mac);

            if (!database.has_station(tlvf::mac_from_string(client_mac))) {
                LOG(ERROR) << "sta " << client_mac << " is not in DB!";
                continue;
            } else if (database.get_sta_state(client_mac) != beerocks::STATE_CONNECTED) {
                LOG(DEBUG) << "sta " << client_mac << " is not connected to hostap " << radio_mac
                           << ", update is invalid!";
                continue;
            }

            //update station bandwidth from the current downlink bandwidth
            if ((sta_stats.dl_bandwidth != beerocks::BANDWIDTH_UNKNOWN) &&
                (sta_stats.dl_bandwidth < beerocks::BANDWIDTH_MAX)) {
                database.update_sta_wifi_channel_bw(
                    sta_stats.mac, static_cast<beerocks::eWiFiBandwidth>(sta_stats.dl_bandwidth));
            }

            // Note: The Database node stats and the Datamodels' stats are not the same.
            // Therefore, client information in data model and in node DB might differ.
            database.set_sta_stats_info(sta_stats.mac, &sta_stats);
        }

        if (response->ap_stats_size() == 0) {
            break;
        }

        auto ap_stats_tuple = response->ap_stats(response->ap_stats_size() - 1);
        if (!std::get<0>(ap_stats_tuple)) {
            LOG(ERROR) << "Couldn't access ap element";
            return false;
        }
        auto &ap_stats = std::get<1>(ap_stats_tuple);
        database.set_radio_stats_info(radio_mac, &ap_stats);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION: {
        std::shared_ptr<Agent> parent_agent = database.get_agent_by_radio_uid(radio_mac);
        if (!parent_agent) {
            LOG(ERROR) << "Parent agent not found for radio " << radio_mac;
            return false;
        }
        auto ire_mac      = tlvf::mac_to_string(parent_agent->al_mac);
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION>();
        if (!notification) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION failed";
            return false;
        }
        int active_client_count = notification->params().active_client_count;
        int client_load_percent = notification->params().client_tx_load_percent +
                                  notification->params().client_rx_load_percent;

        LOG(DEBUG) << "load notification from hostap " << radio_mac << " ire mac=" << ire_mac
                   << " active_client_count=" << active_client_count
                   << " client_load=" << client_load_percent;

        /*
            * start load balancing
            */
        if (active_client_count > database.config.monitor_min_active_clients &&
            client_load_percent >
                database.config.monitor_total_ch_load_notification_hi_th_percent &&
            database.settings_load_balancing() && database.is_radio_active(radio_mac) &&
            database.get_agent_state(parent_agent->al_mac) == beerocks::STATE_CONNECTED) {
            // On the above line, should we use get_sta_state()?
            /*
                * when a notification arrives, it means a large change in rx_rssi occurred (above the defined thershold)
                * therefore, we need to create a load balancing task to optimize the network
                */
            LOG(DEBUG) << "high load conditions, starting load balancer for ire " << ire_mac;
            int prev_task_id =
                database.get_agent_load_balancer_task_id(tlvf::mac_from_string(ire_mac));
            if (m_task_pool.is_task_running(prev_task_id)) {
                LOG(DEBUG) << "load balancer task already running for " << ire_mac;
            } else {
                auto new_task = std::make_shared<load_balancer_task>(
                    database, cmdu_tx, m_task_pool, ire_mac, "load notif (high)- load_balancer");
                m_task_pool.add_task(new_task);
            }
        } else if ((active_client_count < database.config.monitor_min_active_clients) &&
                   (client_load_percent <
                    database.config.monitor_total_ch_load_notification_lo_th_percent)) {
            LOG(DEBUG) << "low load conditions, removing confinements from STAs on ire " << ire_mac;
            /*
                * need to free and move previously confined sta
                * TODO
                * need to improve this logic and make it more robust
                */
            for (const auto &hostap : parent_agent->radios) {
                for (const auto &bss : hostap.second->bsses) {
                    for (const auto &station : bss.second->connected_stations) {
                        if (station.second->confined) {
                            LOG(DEBUG) << "removing confined flag from sta " << station.first;
                            station.second->confined = false;
                            /*
                                * launch optimal path task
                                */
                            if (database.get_sta_state(tlvf::mac_to_string(station.first)) ==
                                beerocks::STATE_CONNECTED) {
                                if (m_task_pool.is_task_running(station.second->roaming_task_id)) {
                                    LOG(DEBUG)
                                        << "roaming task already running for " << station.first;
                                } else {
                                    auto new_task = std::make_shared<optimal_path_task>(
                                        database, cmdu_tx, m_task_pool,
                                        tlvf::mac_to_string(station.first), 0,
                                        "load notif (low) - optimal_path");
                                    m_task_pool.add_task(new_task);
                                }
                            } else {
                                database.set_sta_handoff_flag(*station.second, false);
                            }
                        }
                    }
                }
            }
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE: {
        auto response =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE failed";
            return false;
        }
        LOG_CLI(
            DEBUG,
            "beacon response , ID: "
                << beerocks_header->id() << std::endl
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
                << "bssid: " << response->params().bssid << std::endl
                << "dialog token: " << response->params().dialog_token
            //<< std::endl << "ant_id: "               << (int)response->params.ant_id
            //<< std::endl << "tsf: "                  << (int)response->params.parent_tsf
            //<< std::endl << "new_ch_width: "                         << (int)response->params.new_ch_width
            //<< std::endl << "new_ch_center_freq_seg_0: "             << (int)response->params.new_ch_center_freq_seg_0
            //<< std::endl << "new_ch_center_freq_seg_1: "             << (int)response->params.new_ch_center_freq_seg_1
        );
        if (son_actions::validate_beacon_measurement_report(
                response->params(), tlvf::mac_to_string(response->params().sta_mac),
                tlvf::mac_to_string(response->params().bssid))) {
            database.dm_add_sta_beacon_measurement(response->params());
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE: {
        auto response = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE failed";
            return false;
        }
        std::string client_mac   = tlvf::mac_to_string(response->mac());
        auto client_wifi_channel = database.get_sta_wifi_channel(client_mac);
        if (client_wifi_channel.is_empty()) {
            LOG(WARNING) << "empty wifi channel of " << client_mac << " in DB";
        }
        LOG(DEBUG) << "ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE client_mac="
                   << client_mac << " received from hostap " << radio_mac
                   << " channel=" << int(client_wifi_channel.get_channel())
                   << " Frequency Type: " << client_wifi_channel.get_freq_type()
                   << " ïd = " << int(beerocks_header->id());
        //calculating response delay for associate client ap and cross ap's
        database.set_measurement_recv_delta(radio_mac_str);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION failed";
            return false;
        }
        std::string client_mac = tlvf::mac_to_string(notification->mac());
        LOG(INFO) << "CLIENT NO ACTIVITY MSG RX'ed for client" << client_mac;

        auto client = database.get_station(tlvf::mac_from_string(client_mac));
        if (!client) {
            LOG(ERROR) << "Client " << client_mac << " not found";
            return false;
        }

        if (m_task_pool.is_task_running(client->roaming_task_id)) {
            LOG(DEBUG) << "roaming task already running for " << client_mac;
        } else {
            LOG(INFO) << "Starting optimal path for client" << client_mac;
            auto new_task = std::make_shared<optimal_path_task>(database, cmdu_tx, m_task_pool,
                                                                client_mac, 0, "");
            m_task_pool.add_task(new_task);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION failed";
            return false;
        }

        database.set_radio_activity_mode(
            radio_mac, beerocks::eApActiveMode(notification->params().ap_activity_mode));
        if (notification->params().ap_activity_mode == beerocks::AP_IDLE_MODE) {
            LOG(DEBUG) << "CS_task,sending AP_ACTIVITY_IDLE_EVENT for mac " << radio_mac;
            auto new_event        = new channel_selection_task::sApActivityIdle_event;
            new_event->hostap_mac = radio_mac;
            m_task_pool.push_event(database.get_channel_selection_task_id(),
                                   (int)channel_selection_task::eEvent::AP_ACTIVITY_IDLE_EVENT,
                                   (void *)new_event);
        }

        break;
    }
    case beerocks_message::ACTION_CONTROL_ARP_QUERY_RESPONSE: {
        LOG(DEBUG) << "ACTION_CONTROL_ARP_QUERY_RESPONSE from "
                   << " id=" << beerocks_header->id();
        auto response =
            beerocks_header->addClass<beerocks_message::cACTION_CONTROL_ARP_QUERY_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_ARP_QUERY_RESPONSE failed";
            return false;
        }
        break;
    }
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    case beerocks_message::ACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass "
                          "cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION failed";
            return false;
        }

        beerocks_message::sSteeringEvActivity new_event;
        new_event = notification->params();
        m_task_pool.push_event(
            database.get_pre_association_steering_task_id(),
            pre_association_steering_task::eEvents::STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION,
            &new_event);

        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION failed";
            return false;
        }
        beerocks_message::sSteeringEvSnrXing new_event;
        new_event = notification->params();
        m_task_pool.push_event(
            database.get_pre_association_steering_task_id(),
            pre_association_steering_task::eEvents::STEERING_EVENT_SNR_XING_NOTIFICATION,
            &new_event);

        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION failed";
            return false;
        }

        beerocks_message::sSteeringEvProbeReq new_event;
        new_event = notification->params();
        m_task_pool.push_event(
            database.get_pre_association_steering_task_id(),
            pre_association_steering_task::eEvents::STEERING_EVENT_PROBE_REQ_NOTIFICATION,
            &new_event);

        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION: {
        auto notification = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION failed";
            return false;
        }
        beerocks_message::sSteeringEvAuthFail new_event;
        new_event = notification->params();
        m_task_pool.push_event(
            database.get_pre_association_steering_task_id(),
            pre_association_steering_task::eEvents::STEERING_EVENT_AUTH_FAIL_NOTIFICATION,
            &new_event);
        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST failed";
            return false;
        }
        pre_association_steering_task::sSteeringSetGroupResponseEvent new_event;
        new_event.ret_code = notification->params().error_code;
        m_task_pool.push_event(database.get_pre_association_steering_task_id(),
                               pre_association_steering_task::eEvents::STEERING_SET_GROUP_RESPONSE,
                               &new_event);

        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE failed";
            return false;
        }
        pre_association_steering_task::sSteeringClientSetResponseEvent new_event;
        new_event.ret_code = notification->params().error_code;
        m_task_pool.push_event(database.get_pre_association_steering_task_id(),
                               pre_association_steering_task::eEvents::STEERING_CLIENT_SET_RESPONSE,
                               &new_event);

        break;
    }
#endif // FEATURE_PRE_ASSOCIATION_STEERING
    case beerocks_message::ACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE failed";
            return false;
        }
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
        if (notification->params().src == eClient_Disconnect_Source_Pre_Association_Steering_Task) {
            //push event to pre association steering task
            pre_association_steering_task::sSteeringClientDisconnectResponseEvent new_event;
            new_event.ret_code = notification->params().error_code;
            m_task_pool.push_event(
                database.get_pre_association_steering_task_id(),
                pre_association_steering_task::eEvents::STEERING_CLIENT_DISCONNECT_RESPONSE,
                &new_event);
        }
#endif
        break;
    }
    case beerocks_message::ACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE: {
        break;
    }
    case beerocks_message::ACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION: {
    }
    case beerocks_message::ACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION: {
        break;
    }
    case beerocks_message::ACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION: {
        break;
    }
    case beerocks_message::ACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION: {
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE: {
        // handled in association handling task
        break;
    }
    default: {
        LOG_CLI(ERROR, "Unsupported CONTROL action_op: " << int(beerocks_header->action_op()));
        return false;
    }
    }

    // If this is a response message to a task (header->id() == task id), send it to it directly - cmdu_rx is owned by the task
    // e.g. only the task may call addClass
    if (beerocks_header->id()) {
        m_task_pool.response_received(radio_mac_str, beerocks_header);
        return true;
    }

    return true;
}

bool Controller::start_client_steering(const std::string &sta_mac, const std::string &target_bssid)
{
    std::string triggered_by{"NBAPI"};
    bool disassoc_imminent = false;

    LOG(DEBUG) << "NBAPI client steer request for " << sta_mac << " to hostap: " << target_bssid;
    son_actions::steer_sta(database, cmdu_tx, m_task_pool, sta_mac, target_bssid, triggered_by,
                           std::string(), disassoc_imminent,
                           database.config.steering_disassoc_timer_msec.count());
    return true;
}

#define BEACON_INTERVAL_MS_IN_BI 100
bool Controller::send_btm_request(const bool &disassoc_imminent,
                                  const uint32_t &disassoc_timer,    // beacon interval count
                                  const uint32_t &bss_term_duration, // minutes count
                                  const uint32_t &validity_interval, // beacon interval count
                                  const uint32_t &steering_timer,    // beacon interval count
                                  const std::string &sta_mac, const std::string &target_bssid)
{
    std::string triggered_by{"NBAPI BTMRequest"};

    int disassoc_timer_ms    = BEACON_INTERVAL_MS_IN_BI * disassoc_timer;
    int steering_timer_ms    = BEACON_INTERVAL_MS_IN_BI * steering_timer;
    int validity_interval_ms = BEACON_INTERVAL_MS_IN_BI * validity_interval;

    LOG(DEBUG) << "NBAPI BTMRequest to steer sta " << sta_mac << " to BSSID " << target_bssid;
    son_actions::start_btm_request_task(database, cmdu_tx, m_task_pool, disassoc_imminent,
                                        disassoc_timer_ms, bss_term_duration, validity_interval_ms,
                                        steering_timer_ms, sta_mac, target_bssid, triggered_by);

    return true;
}

bool Controller::trigger_scan(
    const sMacAddr &radio_mac,
    std::array<uint8_t, beerocks::message::SUPPORTED_CHANNELS_LENGTH> channel_pool,
    uint8_t pool_size, int dwell_time)
{
    auto channel_pool_set = std::unordered_set<uint8_t>(&channel_pool[0], &channel_pool[pool_size]);

    if (pool_size == 0) {
        LOG(DEBUG) << "Scan for all channels of radio " << radio_mac;
        if (!database.get_pool_of_all_supported_channels(channel_pool_set, radio_mac)) {
            return false;
        }
    }
    if (!database.set_channel_scan_pool(radio_mac, channel_pool_set, true)) {
        LOG(ERROR) << "set_channel_scan_pool failed";
        return false;
    }
    if (!database.set_channel_scan_dwell_time_msec(radio_mac, dwell_time, true)) {
        LOG(ERROR) << "set_channel_scan_dwell_time_msec failed";
        return false;
    }

    LOG(DEBUG) << "NBAPI trigger scan for radio " << radio_mac;

    dynamic_channel_selection_r2_task::sSingleScanRequestEvent new_event = {};
    new_event.radio_mac                                                  = radio_mac;
    m_task_pool.push_event(database.get_dynamic_channel_selection_r2_task_id(),
                           dynamic_channel_selection_r2_task::eEvent::TRIGGER_SINGLE_SCAN,
                           &new_event);
    return true;
}

bool Controller::trigger_set_spatial_reuse(
    const sMacAddr &ruid, uint32_t bss_color, const bool hesiga_spr_value15_allowed,
    const bool srg_information_valid, const bool non_srg_offset_valid, const bool psr_disallowed,
    uint32_t non_srg_obsspd_max_offset, uint32_t srg_obsspd_min_offset,
    uint32_t srg_obsspd_max_offset, uint64_t srg_bss_color_bitmap,
    uint64_t srg_partial_bssid_bitmap)
{
    LOG(DEBUG) << "Spatial reuse parameters. BSSColor: " << bss_color
               << ", HESIGASpatialReuseValue15Allowed: " << hesiga_spr_value15_allowed
               << ", SRGInformationValid: " << srg_information_valid
               << ", NonSRGOffsetValid: " << non_srg_offset_valid
               << ", PSRDisallowed: " << psr_disallowed
               << ", NonSRGOBSSPDMaxOffset: " << non_srg_obsspd_max_offset
               << ", SRGOBSSPDMinOffset: " << srg_obsspd_min_offset
               << ", SRGOBSSPDMaxOffset: " << srg_obsspd_max_offset
               << ", SRGBSSColorBitmap: " << srg_bss_color_bitmap
               << ", SRGPartialBSSIDBitmap: " << srg_partial_bssid_bitmap;

    // Need to create CHANNEL_SELECTION_REQUEST_MESSAGE with spatial reuse and send it to the agent.
    if (!cmdu_tx.create(0, ieee1905_1::eMessageType::CHANNEL_SELECTION_REQUEST_MESSAGE)) {
        LOG(ERROR) << "CMDU creation of type CHANNEL_SELECTION_REQUEST_MESSAGE, has failed";
        return false;
    }

    auto spatial_reuse_request_tlv = cmdu_tx.addClass<wfa_map::tlvSpatialReuseRequest>();
    if (!spatial_reuse_request_tlv) {
        LOG(ERROR) << "addClass wfa_map::tlvSpatialReuseRequest has failed";
        return false;
    }

    spatial_reuse_request_tlv->radio_uid()        = ruid;
    spatial_reuse_request_tlv->flags1().bss_color = bss_color;
    spatial_reuse_request_tlv->flags2().hesiga_spatial_reuse_value15_allowed =
        hesiga_spr_value15_allowed;
    spatial_reuse_request_tlv->flags2().srg_information_valid = srg_information_valid;
    spatial_reuse_request_tlv->flags2().non_srg_offset_valid  = non_srg_offset_valid;
    spatial_reuse_request_tlv->flags2().psr_disallowed        = psr_disallowed;
    if (non_srg_offset_valid) {
        spatial_reuse_request_tlv->non_srg_obsspd_max_offset() = non_srg_obsspd_max_offset;
    }
    if (srg_information_valid) {
        spatial_reuse_request_tlv->srg_obsspd_min_offset()    = srg_obsspd_min_offset;
        spatial_reuse_request_tlv->srg_obsspd_max_offset()    = srg_obsspd_max_offset;
        spatial_reuse_request_tlv->srg_bss_color_bitmap()     = srg_bss_color_bitmap;
        spatial_reuse_request_tlv->srg_partial_bssid_bitmap() = srg_partial_bssid_bitmap;
    }
    for (const auto &agent : database.get_all_connected_agents()) {
        son_actions::send_cmdu_to_agent(agent->al_mac, cmdu_tx, database);
        LOG(DEBUG) << "send CHANNEL_SELECTION_REQUEST_MESSAGE with spatial reuse to agent with "
                      "mac_address "
                   << tlvf::mac_to_string(agent->al_mac);
    }
    return true;
}

bool Controller::trigger_vbss_creation(const sMacAddr &dest_ruid, const sMacAddr &vbssid,
                                       const sMacAddr &client_mac, const std::string &new_bss_ssid,
                                       const std::string &new_bss_pass)
{
#ifdef ENABLE_VBSS
    vbss::sClientVBSS client_vbss = {};
    // Can assume client is not associated since this is not called during a move
    client_vbss.client_is_associated   = false;
    client_vbss.client_mac             = client_mac;
    client_vbss.current_connected_ruid = beerocks::net::network_utils::ZERO_MAC;
    client_vbss.vbssid                 = vbssid;

    return vbss::vbss_actions::create_vbss(client_vbss, dest_ruid, new_bss_ssid, new_bss_pass,
                                           nullptr, database);
#endif
    LOG(ERROR) << "Failed to trigger VBSS creation! VBSS is not enabled!";
    return false;
}

bool Controller::trigger_vbss_destruction(const sMacAddr &connected_ruid, const sMacAddr &vbssid,
                                          const sMacAddr &client_mac,
                                          const bool should_disassociate)
{
#ifdef ENABLE_VBSS
    vbss::sClientVBSS client_vbss = {};
    // Can assume client is associated since we are destroying a VBSS
    client_vbss.client_is_associated   = true;
    client_vbss.client_mac             = client_mac;
    client_vbss.current_connected_ruid = connected_ruid;
    client_vbss.vbssid                 = vbssid;

    return vbss::vbss_actions::destroy_vbss(client_vbss, should_disassociate, database);
#endif
    LOG(ERROR) << "Failed to trigger VBSS destruction! VBSS is not enabled!";
    return false;
}

bool Controller::update_agent_vbss_capabilities(const sMacAddr &agent_mac)
{
#ifdef ENABLE_VBSS
    return vbss::vbss_actions::request_ap_radio_vbss_caps(agent_mac, database);
#endif
    LOG(ERROR) << "Failed to update VBSS capabilities! VBSS is not enabled!";
    return false;
}

bool Controller::trigger_vbss_move(const sMacAddr &connected_ruid, const sMacAddr &dest_ruid,
                                   const sMacAddr &vbssid, const sMacAddr &client_mac,
                                   const std::string &new_bss_ssid, const std::string &new_bss_pass)
{
#ifdef ENABLE_VBSS
    int task_id = database.get_vbss_task_id();
    if (task_id == db::TASK_ID_NOT_FOUND) {
        LOG(ERROR) << "Could not trigger VBSS move! VBSS Task not found!";
        return false;
    }

    vbss::sClientVBSS client_vbss      = {};
    client_vbss.client_is_associated   = true; // Can assume client is associated in move req
    client_vbss.client_mac             = client_mac;
    client_vbss.current_connected_ruid = connected_ruid;
    client_vbss.vbssid                 = vbssid;

    vbss_task::sMoveEvent move_event = {};
    move_event.client_vbss           = client_vbss;
    move_event.dest_ruid             = dest_ruid;
    move_event.ssid                  = new_bss_ssid;
    move_event.password              = new_bss_pass;

    m_task_pool.push_event(task_id, vbss_task::eEventType::MOVE, &move_event);

    return true;
#endif
    LOG(ERROR) << "Failed to trigger VBSS move! VBSS is not enabled!";
    return false;
}

void Controller::trigger_prioritization_config()
{
    auto ev = agent_monitoring_task::CONFIGURE_QOS;
    m_task_pool.push_event(database.get_agent_monitoring_task_id(), ev);
}

bool Controller::handle_tlv_profile2_ap_capability(std::shared_ptr<Agent> agent,
                                                   ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto profile2_ap_capability_tlv = cmdu_rx.getClass<wfa_map::tlvProfile2ApCapability>();
    if (!profile2_ap_capability_tlv) {
        LOG(DEBUG) << "getClass wfa_map::tlvProfile2ApCapability has failed";
        return false;
    }

    agent->max_prioritization_rules = profile2_ap_capability_tlv->max_prioritization_rules();
    agent->byte_counter_units = static_cast<wfa_map::tlvProfile2ApCapability::eByteCounterUnits>(
        profile2_ap_capability_tlv->capabilities_bit_field().byte_counter_units);
    agent->prioritization_support =
        profile2_ap_capability_tlv->capabilities_bit_field().prioritization;
    agent->dpp_onboarding_support =
        profile2_ap_capability_tlv->capabilities_bit_field().dpp_onboarding;
    agent->traffic_separation_support =
        profile2_ap_capability_tlv->capabilities_bit_field().traffic_separation;
    agent->max_total_number_of_vids = profile2_ap_capability_tlv->max_total_number_of_vids();

    LOG(DEBUG) << "Profile-2 AP Capability TLV is received:" << std::endl
               << "The maximum total number of supported service priroritization rules: "
               << agent->max_prioritization_rules << std::endl
               << "The units used for byte counters: "
               << wfa_map::tlvProfile2ApCapability::eByteCounterUnits_str(agent->byte_counter_units)
               << std::endl
               << "Service Prioritization support: " << agent->prioritization_support << std::endl
               << "DPP Onboarding procedure support: " << agent->dpp_onboarding_support << std::endl
               << "Traffic Separation support: " << agent->traffic_separation_support << std::endl
               << "The maximum total number of supported unique VLAN IDs: "
               << agent->max_total_number_of_vids << std::endl;

    if (!database.dm_set_device_ap_capabilities(*agent)) {
        LOG(ERROR) << "Failed to set AP capability parameters in DM for Agent" << agent->al_mac;
        return false;
    }

    return true;
}

bool Controller::handle_tlv_profile2_cac_capabilities(Agent &agent,
                                                      ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto cac_capabilities_tlv = cmdu_rx.getClass<wfa_map::tlvProfile2CacCapabilities>();
    if (!cac_capabilities_tlv) {
        LOG(DEBUG) << "getClass wfa_map::tlvProfile2CacCapabilities has failed";
        return false;
    }

    LOG(DEBUG) << "Profile-2 CAC Capabilities TLV is received";

    std::stringstream ss;

    agent.device_info.country_code.clear();
    agent.device_info.country_code += static_cast<char>(*cac_capabilities_tlv->country_code(0));
    agent.device_info.country_code += static_cast<char>(*cac_capabilities_tlv->country_code(1));
    ss << "Country code: " << agent.device_info.country_code << std::endl;

    for (size_t radio_idx = 0; radio_idx < cac_capabilities_tlv->number_of_cac_radios();
         radio_idx++) {
        if (!std::get<0>(cac_capabilities_tlv->cac_radios(radio_idx))) {
            LOG(ERROR) << "Invalid CAC radio in tlvProfile2CacCapabilities";
            continue;
        }

        auto &cac_radio = std::get<1>(cac_capabilities_tlv->cac_radios(radio_idx));
        auto ruid       = cac_radio.radio_uid();

        auto radio = agent.radios.get(ruid);
        if (!radio) {
            LOG(ERROR) << "No radio found for ruid=" << ruid << " on " << agent.al_mac;
            continue;
        }

        database.dm_clear_radio_cac_capabilities(*radio);

        for (size_t type_idx = 0; type_idx < cac_radio.number_of_cac_type_supported(); type_idx++) {
            if (type_idx > 3) {
                LOG(ERROR) << "Invalid number of CAC types in tlvProfile2CacCapabilities";
                return false;
            }

            if (!std::get<0>(cac_radio.cac_types(type_idx))) {
                LOG(ERROR) << "Invalid CAC type in tlvProfile2CacCapabilities";
                continue;
            }
            auto &cac_type = std::get<1>(cac_radio.cac_types(type_idx));

            auto cac_method     = cac_type.cac_method();
            auto cac_method_str = wfa_map::eCacMethod_str(cac_method);
            auto cac_duration   = cac_type.duration();

            ss << "Radio " << ruid << ", supported CAC method " << cac_method_str
               << " with duration " << int(*cac_duration) << std::endl;

            if (!cac_type.number_of_operating_classes()) {
                LOG(ERROR) << "Invalid number of supported operating classes in "
                              "tlvProfile2CacCapabilities";
                continue;
            }

            // Key: operating class, value: vector with channel numbers
            std::unordered_map<uint8_t, std::vector<uint8_t>> oc_channels;

            ss << "Supported OC/channels:" << std::endl;
            for (size_t oc_idx = 0; oc_idx < cac_type.number_of_operating_classes(); oc_idx++) {
                if (!std::get<0>(cac_type.operating_classes(oc_idx))) {
                    LOG(ERROR) << "Invalid operating class in tlvProfile2CacCapabilities";
                    continue;
                }

                auto &operating_class = std::get<1>(cac_type.operating_classes(oc_idx));
                auto oc               = operating_class.operating_class();
                ss << "OC: " << int(oc) << ", channels: ";

                std::vector<uint8_t> channels;
                for (size_t ch_idx = 0; ch_idx < operating_class.number_of_channels(); ch_idx++) {
                    channels.push_back(*operating_class.channels(ch_idx));
                    ss << "#" << int(*operating_class.channels(ch_idx)) << " ";
                }
                oc_channels.emplace(oc, channels);
                ss << std::endl;
            }

            if (!database.dm_add_radio_cac_capabilities(*radio, cac_method, *cac_duration,
                                                        oc_channels)) {
                LOG(ERROR) << "Failed to add CAC capabilities for radio=" << radio->radio_uid;
                return false;
            }
        }
    }

    LOG(DEBUG) << ss.str();
    return true;
}

bool Controller::handle_tlv_profile2_channel_scan_capabilities(std::shared_ptr<Agent> &agent,
                                                               ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto channel_scan_capabilities_tlv = cmdu_rx.getClass<wfa_map::tlvChannelScanCapabilities>();
    if (!channel_scan_capabilities_tlv) {
        LOG(ERROR) << "getClass wfa_map::tlvChannelScanCapabilities has failed";
        return false;
    }

    auto radio_list_length = channel_scan_capabilities_tlv->radio_list_length();
    if (!radio_list_length) {
        LOG(WARNING) << "Received Channel Scan Capabilities TLV without any radios";
        return true;
    }

    for (int rc_idx = 0; rc_idx < radio_list_length; rc_idx++) {

        auto radio_capabilities_tuple = channel_scan_capabilities_tlv->radio_list(rc_idx);
        if (!std::get<0>(radio_capabilities_tuple)) {
            LOG(ERROR) << "Invalid radio in tlvChannelScanCapabilities";
            return false;
        }

        auto &radio_capabilities = std::get<1>(radio_capabilities_tuple);
        auto &ruid               = radio_capabilities.radio_uid();

        auto radio = agent->radios.get(ruid);
        if (!radio) {
            LOG(ERROR) << "No radio found for ruid=" << ruid << " on " << agent->al_mac;
            continue;
        }

        if (!database.set_radio_channel_scan_capabilites(*radio, radio_capabilities)) {
            LOG(ERROR) << "Failed to save channel scan capabilities for radio=" << ruid;
            return false;
        }

        if (!database.dm_add_radio_scan_capabilities(*radio)) {
            LOG(ERROR) << "Failed to add channel scan capabilities to DM for radio=" << ruid;
            return false;
        }
    }

    return true;
}

bool Controller::handle_tlv_profile3_1905_layer_security_capabilities(
    const Agent &agent, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto ieee_1905_security_tlv = cmdu_rx.getClass<wfa_map::tlv1905LayerSecurityCapability>();
    if (!ieee_1905_security_tlv) {
        LOG(DEBUG) << "getClass wfa_map::tlv1905LayerSecurityCapability has failed";
        return false;
    }

    LOG(DEBUG) << "Profile-3 1905 Layer Security Capability TLV is received";

    if (!database.dm_add_agent_1905_layer_security_capabilities(
            agent, ieee_1905_security_tlv->onboarding_protocol(),
            ieee_1905_security_tlv->mic_algorithm(),
            ieee_1905_security_tlv->encryption_algorithm())) {
        LOG(ERROR) << "Failed to add IEEE 1905 security capability";
        return false;
    }

    return true;
}

bool Controller::handle_cmdu_1905_bss_configuration_request_message(
    const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received BSS_CONFIGURATION_REQUEST_MESSAGE, mid=" << std::hex << mid;

    auto agent = database.m_agents.get(src_mac);
    if (!agent) {
        LOG(ERROR) << "Agent with mac is not found in database mac=" << src_mac;
        return false;
    }

    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_2) {
        auto tlvProfile2MultiApProfile = cmdu_rx.getClass<wfa_map::tlvProfile2MultiApProfile>();
        if (tlvProfile2MultiApProfile) {
            agent->profile = tlvProfile2MultiApProfile->profile();
            if (!database.dm_set_device_multi_ap_profile(*agent)) {
                LOG(ERROR) << "Failed to set Multi-AP profile in DM for Agent " << agent->al_mac;
                return false;
            }
        }
    }

    if (!handle_tlv_profile2_ap_capability(agent, cmdu_rx)) {
        LOG(ERROR) << "Couldn't handle Profile-2 AP Capability TLV from Agent " << src_mac;
    }

    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_2 &&
        !handle_tlv_profile3_akm_suite_capabilities(*agent, cmdu_rx)) {
        LOG(ERROR) << "Profile-3 AKM Suite Capabilities is not supplied for Agent " << agent->al_mac
                   << " with profile enum " << agent->profile;
    }

    if (!handle_tlv_profile2_ap_radio_advanced_capabilities(*agent, cmdu_rx)) {
        LOG(ERROR) << "Couldn't handle AP Radio Advanced Capabilities TLV from Agent "
                   << agent->al_mac;
    }

    // TODO: Implement parsing of unhandled TLVs (PPM-2325)

    auto cmdu_tx_header =
        cmdu_tx.create(mid, ieee1905_1::eMessageType::BSS_CONFIGURATION_RESPONSE_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type BSS_CONFIGURATION_RESPONSE_MESSAGE, has failed";
        return false;
    }

    agent_monitoring_task::add_profile_2default_802q_settings_tlv(database, cmdu_tx, src_mac);

    return son_actions::send_cmdu_to_agent(src_mac, cmdu_tx, database);
}

bool Controller::handle_tlv_profile3_akm_suite_capabilities(Agent &agent,
                                                            ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto akm_suite_capabilities_tlv = cmdu_rx.getClass<wfa_map::tlvAkmSuiteCapabilities>();
    if (!akm_suite_capabilities_tlv) {
        LOG(DEBUG) << "getClass wfa_map::tlvAkmSuiteCapabilities has failed";
        return false;
    }

    LOG(DEBUG) << "Profile-3 AKM Suite Capabilities TLV is received";

    std::vector<wfa_map::tlvAkmSuiteCapabilities::sBssAkmSuiteSelector> backhaul_bss_selectors;
    std::vector<wfa_map::tlvAkmSuiteCapabilities::sBssAkmSuiteSelector> fronthaul_bss_selectors;

    std::stringstream ss;
    for (size_t i = 0; i < akm_suite_capabilities_tlv->number_of_bh_bss_akm_suite_selectors();
         i++) {
        if (!std::get<0>(akm_suite_capabilities_tlv->backhaul_bss_akm_suite_selectors(i))) {
            LOG(ERROR) << "Invalid Backhaul BSS Selectors in tlvAkmSuiteCapabilities";
            continue;
        }

        auto &selector =
            std::get<1>(akm_suite_capabilities_tlv->backhaul_bss_akm_suite_selectors(i));

        ss << "Backhaul BSS, OUI: "
           << wfa_map::tlvAkmSuiteCapabilities::eAkmSuiteOUI_str(
                  wfa_map::tlvAkmSuiteCapabilities::eAkmSuiteOUI(uint32_t(selector.oui)))
           << ", suite type: " << (int)selector.akm_suite_type << std::endl;

        backhaul_bss_selectors.push_back(selector);
    }

    for (size_t i = 0; i < akm_suite_capabilities_tlv->number_of_fh_bss_akm_suite_selectors();
         i++) {
        if (!std::get<0>(akm_suite_capabilities_tlv->fronthaul_bss_akm_suite_selectors(i))) {
            LOG(ERROR) << "Invalid Fronthaul BSS Selectors in tlvAkmSuiteCapabilities";
            continue;
        }

        auto &selector =
            std::get<1>(akm_suite_capabilities_tlv->fronthaul_bss_akm_suite_selectors(i));

        ss << "Fronthaul BSS, OUI: "
           << wfa_map::tlvAkmSuiteCapabilities::eAkmSuiteOUI_str(
                  wfa_map::tlvAkmSuiteCapabilities::eAkmSuiteOUI(uint32_t(selector.oui)))
           << ", suite type: " << (int)selector.akm_suite_type << std::endl;

        fronthaul_bss_selectors.push_back(selector);
    }

    /* TODO: Establish a accordance between radio and given AKM Suite сapabilities (PPM-2332).
    if (!database.dm_add_radio_akm_suite_capabilities(radio, fronthaul_bss_selectors, backhaul_bss_selectors) {
        LOG(ERROR) << "Failed to add AKM Suite Capabilities for radio=" << radio->radio_uid;
        return false;
    }
    */

    return true;
}

bool Controller::handle_tlv_profile2_ap_radio_advanced_capabilities(
    Agent &agent, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto ap_radio_advanced_capabilities =
        cmdu_rx.getClass<wfa_map::tlvProfile2ApRadioAdvancedCapabilities>();
    if (!ap_radio_advanced_capabilities) {
        LOG(DEBUG) << "getClass wfa_map::tlvProfile2ApRadioAdvancedCapabilities has failed";
        return false;
    }

    LOG(DEBUG) << "Profile-2 AP Radio Advanced Capabilities TLV is received";

    std::stringstream ss;

    auto ruid = ap_radio_advanced_capabilities->radio_uid();

    auto radio = agent.radios.get(ruid);
    if (!radio) {
        LOG(ERROR) << "No radio found for ruid=" << ruid << " on " << agent.al_mac;
        return false;
    }

    auto advanced_radio_capabilities =
        ap_radio_advanced_capabilities->advanced_radio_capabilities();

    radio->advanced_capabilities.traffic_separation_combined_fronthaul =
        advanced_radio_capabilities.combined_front_back;
    radio->advanced_capabilities.traffic_separation_combined_backhaul =
        advanced_radio_capabilities.combined_profile1_and_profile2;
    radio->advanced_capabilities.mscs = advanced_radio_capabilities.mscs;
    radio->advanced_capabilities.scs  = advanced_radio_capabilities.scs;
    radio->advanced_capabilities.dscp_to_up_mapping =
        advanced_radio_capabilities.dscp_to_up_mapping;
    radio->advanced_capabilities.dscp_policy = advanced_radio_capabilities.dscp_policy;

    ss << "Radio " << ruid << ", traffic separation capabilities:" << std::endl
       << "Traffic separation on combined fronthaul and Profile-1 backhaul support = "
       << radio->advanced_capabilities.traffic_separation_combined_fronthaul << std::endl
       << "Traffic separation on combined Profile-1 backhaul and Profile-2 backhaul support = "
       << radio->advanced_capabilities.traffic_separation_combined_backhaul << std::endl
       << "MSCS support = " << radio->advanced_capabilities.mscs << std::endl
       << "SCS support = " << radio->advanced_capabilities.scs << std::endl
       << "DSCP-to-UP mapping support = " << radio->advanced_capabilities.dscp_to_up_mapping
       << std::endl
       << "DSCP Policy support = " << radio->advanced_capabilities.dscp_policy << std::endl;

    if (!database.dm_set_radio_advanced_capabilities(*radio)) {
        LOG(ERROR) << "Failed to set AP Radio Advanced Capabilities in DM for radio="
                   << radio->radio_uid;
        return false;
    }

    LOG(DEBUG) << ss.str();
    return true;
}

bool Controller::handle_tlv_profile3_device_inventory(Agent &agent,
                                                      ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto device_inventory = cmdu_rx.getClass<wfa_map::tlvDeviceInventory>();
    if (!device_inventory) {
        LOG(DEBUG) << "getClass wfa_map::tlvDeviceInventory has failed";
        return false;
    }

    LOG(DEBUG) << "Profile-3 Device Inventory TLV is received";

    std::stringstream ss;

    agent.device_info.serial_number    = device_inventory->serial_number_str();
    agent.device_info.software_version = device_inventory->software_version_str();
    agent.device_info.execution_env    = device_inventory->execution_environment_str();

    ss << "Device Inventory parameters: " << std::endl
       << "Serial number = " << agent.device_info.serial_number << std::endl
       << "Software version = " << agent.device_info.software_version << std::endl
       << "Execution environment = " << agent.device_info.execution_env << std::endl;

    for (size_t radio_idx = 0; radio_idx < device_inventory->number_of_radios(); radio_idx++) {
        if (!std::get<0>(device_inventory->radios_vendor_info(radio_idx))) {
            LOG(ERROR) << "Invalid Radio Vendor Info in tlvDeviceInventory";
            continue;
        }

        auto &radio_vendor_info = std::get<1>(device_inventory->radios_vendor_info(radio_idx));

        auto radio = agent.radios.get(radio_vendor_info.ruid());
        if (!radio) {
            LOG(ERROR) << "No radio found for ruid=" << radio_vendor_info.ruid() << " on "
                       << agent.al_mac;
            return false;
        }
        radio->chipset_vendor = radio_vendor_info.chipset_vendor_str();

        ss << "Radio UID = " << radio->radio_uid << ", chipset vendor = " << radio->chipset_vendor
           << std::endl;
    }

    if (!database.dm_set_profile3_device_info(agent)) {
        LOG(ERROR) << "Failed to set device info parameters for Agent" << agent.al_mac;
        return false;
    }

    LOG(DEBUG) << ss.str();
    return true;
}

bool Controller::handle_ap_capability_report(const sMacAddr &src_mac,
                                             ieee1905_1::CmduMessageRx &cmdu_rx, const bool early)
{
    auto agent = database.m_agents.get(src_mac);
    if (!agent) {
        LOG(ERROR) << "Agent with mac is not found in database mac=" << src_mac;
        return false;
    }

    agent->radios.keep_new_prepare();

    for (auto radio_tlv : cmdu_rx.getClassList<wfa_map::tlvApRadioBasicCapabilities>()) {

        LOG(DEBUG) << "Radio is reported in AP Capabilites with ruid=" << radio_tlv->radio_uid();
        database.add_radio(radio_tlv->radio_uid(), agent->al_mac);

        //TODO: We can decide to parse Radio CAPs here instead of WSC (autoconfig_wsc_parse_radio_caps)
        // to lower CPU usage on onboarding (PPM-1727)

        // Remove all previously set Capabilities of radio from data model
        database.clear_ap_capabilities(radio_tlv->radio_uid());
    }

    auto removed = agent->radios.keep_new_remove_old();
    for (const auto &removed_radio : removed) {

        LOG(INFO) << "Radio is not reported on AP_CAPABILITY_REPORT, remove radio object ruid="
                  << removed_radio->radio_uid;

        database.dm_remove_radio(*removed_radio);
        son_actions::handle_dead_radio(removed_radio->radio_uid, true, database, m_task_pool);
    }

    bool all_radio_capabilities_saved_successfully = true;
    if (!early &&
        agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1 &&
        !handle_tlv_profile2_channel_scan_capabilities(agent, cmdu_rx)) {
        LOG(ERROR) << "Profile2 Channel Scan Capabilities are not supplied for Agent " << src_mac
                   << " with profile enum " << agent->profile;
        all_radio_capabilities_saved_successfully = false;
    }

    // Reset Radios Capabilities
    for (const auto &radio : agent->radios) {
        radio.second->eht_supported = false;
    }
    if (!handle_tlv_ap_ht_capabilities(cmdu_rx)) {
        LOG(ERROR) << "Couldn't handle TLV AP HT Capabilities";
        return false;
    }
    if (!handle_tlv_ap_vht_capabilities(cmdu_rx)) {
        LOG(ERROR) << "Couldn't handle TLV AP VHT Capabilities";
        return false;
    }
    if (!handle_tlv_eht_operations(cmdu_rx, src_mac)) {
        LOG(DEBUG) << "Couldn't handle TLV AP EHT Operations";
        return false;
    }
    if (!handle_tlv_apCapability(cmdu_rx, agent, early)) {
        LOG(ERROR) << "Couldn't handle TLV tlvApCapability";
    }
    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_3 &&
        !handle_tlv_ap_wifi6_capabilities(cmdu_rx)) {
        LOG(ERROR) << "Couldn't handle TLV AP WIFI6Capabilities";
        return false;
    }

    if (!handle_tlv_wifi7_agent_capabilities(cmdu_rx, agent)) {
        LOG(DEBUG) << "Couldn't handle TLV WIFI7AgentCapabilities";
        return false;
    }

    // Profile-2 Multi AP profile is added for higher than Profile-1 agents.
    if (!handle_tlv_profile2_ap_capability(agent, cmdu_rx)) {
        LOG(ERROR) << "Profile2 AP Capability is not supplied for Agent " << src_mac
                   << " with profile enum " << agent->profile;
    }

    if (!handle_tlv_profile2_ap_radio_advanced_capabilities(*agent, cmdu_rx)) {
        LOG(ERROR) << "Couldn't handle AP Radio Advanced Capabilities TLV from Agent " << src_mac;
    }

    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1 &&
        !handle_tlv_profile2_cac_capabilities(*agent, cmdu_rx)) {
        LOG(ERROR) << "Profile2 CAC Capabilities are not supplied for Agent " << src_mac
                   << " with profile enum " << agent->profile;
    }

    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_2 &&
        !handle_tlv_profile3_1905_layer_security_capabilities(*agent, cmdu_rx)) {
        LOG(ERROR) << "Profile3 1905 Layer Security Capability is not supplied for Agent "
                   << src_mac << " with profile enum " << agent->profile;
    }

    if (agent->profile > wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_2 &&
        !handle_tlv_profile3_device_inventory(*agent, cmdu_rx)) {
        LOG(ERROR) << "Profile3 Device Inventory is not supplied for Agent " << src_mac
                   << " with profile enum " << agent->profile;
    }

    return all_radio_capabilities_saved_successfully;
}

bool Controller::handle_cmdu_1905_early_ap_capability_report_message(
    const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received EARLY_AP_CAPABILITY_REPORT_MESSAGE, mid=" << std::dec << int(mid);

    return handle_ap_capability_report(src_mac, cmdu_rx, true);
}

bool Controller::add_unassociated_station(const sMacAddr &station_mac_addr, uint8_t channel,
                                          uint8_t operating_class, const sMacAddr &agent_mac_addr,
                                          const sMacAddr &radio_mac_addr)
{
    if (!database.add_unassociated_station(station_mac_addr, channel, operating_class,
                                           agent_mac_addr, radio_mac_addr)) {
        return false;
    }
    return send_unassociated_sta_link_metrics_query_message(cmdu_tx, database);
}

bool Controller::remove_unassociated_station(const sMacAddr &station_mac_addr,
                                             const sMacAddr &agent_mac_addr,
                                             const sMacAddr &radio_mac_addr)
{
    if (!database.remove_unassociated_station(station_mac_addr, agent_mac_addr, radio_mac_addr)) {
        return false;
    }

    return send_unassociated_sta_link_metrics_query_message(cmdu_tx, database);
}

bool Controller::get_unassociated_stations_stats()
{
    return send_unassociated_sta_link_metrics_query_message(cmdu_tx, database);
}

bool Controller::send_unassociated_sta_link_metrics_query_message(
    ieee1905_1::CmduMessageTx &cmdu_tx, db &database)
{
    if (!cmdu_tx.create(0, ieee1905_1::eMessageType::UNASSOCIATED_STA_LINK_METRICS_QUERY_MESSAGE)) {
        LOG(ERROR) << "Failed building message UNASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE!";
        return false;
    }

    using map_stations_per_channel = std::unordered_map<uint8_t, std::list<sMacAddr>>;

    using map_operating_classes_per_agent = std::unordered_map<uint8_t, map_stations_per_channel>;

    using map_all_agents = std::unordered_map<
        sMacAddr,
        map_operating_classes_per_agent>; //mapping all opetating_classes to agents

    map_all_agents global_map;

    auto un_stations_db = database.get_unassociated_stations();
    if (un_stations_db.size()) {
        for (auto &station : un_stations_db) {
            for (const auto &agent : station.second->get_agents()) {

                if (global_map.find(agent.first) == global_map.end()) {
                    global_map.insert(
                        std::make_pair(agent.first, map_operating_classes_per_agent()));
                }
                auto &agents_operating_classes = global_map[agent.first];

                // now check if the operating_class exists
                uint8_t operating_class = station.second->get_operating_class();
                if (agents_operating_classes.find(operating_class) ==
                    agents_operating_classes.end()) {
                    agents_operating_classes.insert(
                        std::make_pair(operating_class, map_stations_per_channel()));
                }
                auto &stations_per_channel = agents_operating_classes[operating_class];

                // now check if the channel exist
                uint8_t channel = station.second->get_channel();
                if (stations_per_channel.find(channel) == stations_per_channel.end()) {
                    stations_per_channel.insert(std::make_pair(channel, std::list<sMacAddr>()));
                }

                auto &list_stations = stations_per_channel[channel];
                list_stations.emplace_back(station.first);
            }
        }
    } else { // this is a command to remove all monitored stations
        for (const auto &agent : database.get_all_connected_agents()) {
            cmdu_tx.addClass<wfa_map::tlvUnassociatedStaLinkMetricsQuery>();
            son_actions::send_cmdu_to_agent(agent->al_mac, cmdu_tx, database);
            LOG(DEBUG) << "removed  non_associated stations from  agent with mac_address "
                       << tlvf::mac_to_string(agent->al_mac);
        }
        return true;
    }

    //now loop over all agents  and send a specific telemtry for each operating_class
    for (auto &agent : global_map) {

        for (auto &operating_class : agent.second) {
            auto unassociated_sta_query =
                cmdu_tx.addClass<wfa_map::tlvUnassociatedStaLinkMetricsQuery>();
            if (operating_class.second.size() == 0) {
                continue; // no stations so nothing to do
            }
            for (auto &stations_per_channel : operating_class.second) {
                std::shared_ptr<wfa_map::cChannelParameters> channels =
                    unassociated_sta_query->create_channel_list();
                channels->channel_number() = stations_per_channel.first;
                channels->alloc_sta_list(stations_per_channel.second.size());
                size_t count(0);
                for (auto &station : stations_per_channel.second) {
                    std::get<1>(channels->sta_list(count)) =
                        station; // here we are setting the mac address into the telemtry
                    count++;
                }
                // Add channel_list object to TLV
                if (!unassociated_sta_query->add_channel_list(channels)) {
                    LOG(ERROR)
                        << "add_channel_list() failed  in tlvUnassociatedStaLinkMetricsQuery";
                    return false;
                }
            }

            LOG(DEBUG) << " sending a new tlvUnassociatedStaLinkMetricsQuery to agent: "
                       << tlvf::mac_to_string(agent.first)
                       << " with: operating_class: " << operating_class.first;
            unassociated_sta_query->operating_class_of_channel_list() = operating_class.first;
            unassociated_sta_query->channel_list_length() =
                operating_class.second.size(); //number of channels
            son_actions::send_cmdu_to_agent(agent.first, cmdu_tx, database);
        }
    }
    return true;
}

} // namespace son
