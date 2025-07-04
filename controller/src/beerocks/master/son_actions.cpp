/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "son_actions.h"

#include "db/db_algo.h"
#include "tasks/agent_monitoring_task.h"
#include "tasks/association_handling_task.h"
#include "tasks/bml_task.h"
#include "tasks/btm_request_task.h"
#include "tasks/client_steering_task.h"

#include <bcl/network/network_utils.h>
#include <bcl/network/sockets.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message_cli.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddress.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/wfa_map/tlvAgentApMldConfiguration.h>
#include <tlvf/wfa_map/tlvClientAssociationControlRequest.h>
#include <tlvf/wfa_map/tlvProfile2MultiApProfile.h>

#include "controller.h"

using namespace beerocks;
using namespace net;
using namespace son;

void son_actions::handle_completed_connection(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                              task_pool &tasks, std::string client_mac)
{
    LOG(INFO) << "handle_completed_connection client_mac=" << client_mac;
    std::shared_ptr<Station> client = database.get_station(tlvf::mac_from_string(client_mac));
    if (!client) {
        return;
    }
    if (!database.set_sta_state(client_mac, beerocks::STATE_CONNECTED)) {
        LOG(ERROR) << "set sta state failed";
    }
    // update bml listeners
    LOG(DEBUG) << "BML, sending connect CONNECTION_CHANGE for mac " << client_mac;
    bml_task::connection_change_event new_event;
    new_event.mac = client_mac;
    tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);

    auto new_hostap_mac      = database.get_sta_parent(client_mac);
    auto previous_hostap_mac = client->get_previous_bss()
                                   ? tlvf::mac_to_string(client->get_previous_bss()->bssid)
                                   : network_utils::ZERO_MAC_STRING;
    auto hostaps = database.get_active_radios();

    hostaps.erase(new_hostap_mac); //next operations will be done only on the other APs

    if (database.is_sta_wireless(client_mac)) {
        LOG(DEBUG) << "node " << client_mac << " is wireless";
        /*
         * send disassociate request to previous hostap to clear STA mac from its list
         */
        if ((!previous_hostap_mac.empty()) &&
            (previous_hostap_mac != network_utils::ZERO_MAC_STRING) &&
            (previous_hostap_mac != new_hostap_mac)) {
            disconnect_client(database, cmdu_tx, client_mac, previous_hostap_mac,
                              eDisconnect_Type_Disassoc, 0);
        }

        /*
         * launch association handling task for async actions
         * and further handling of the new connection
         */
        auto new_task =
            std::make_shared<association_handling_task>(database, cmdu_tx, tasks, client_mac);
        tasks.add_task(new_task);
    }
}

bool son_actions::add_station_to_default_location(db &database, std::string client_mac)
{
    sMacAddr gw_lan_switch;

    auto gw = database.get_gw();
    if (!gw) {
        LOG(WARNING)
            << "add_station_to_default_location - can't get GW node, adding to default location...";
    } else {
        if (gw->eth_switches.empty()) {
            LOG(ERROR) << "add_station_to_default_location - GW has no LAN SWITCH node!";
            return false;
        }
        gw_lan_switch = gw->eth_switches.begin()->first;
    }

    if (!database.add_station(network_utils::ZERO_MAC, tlvf::mac_from_string(client_mac),
                              gw_lan_switch)) {
        LOG(ERROR) << "add_station_to_default_location - add_station failed";
        return false;
    }

    if (!database.set_sta_state(client_mac, beerocks::STATE_CONNECTING)) {
        LOG(ERROR) << "add_station_to_default_location - set_sta_state failed.";
        return false;
    }

    return true;
}

void son_actions::unblock_sta(db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::string sta_mac)
{
    LOG(DEBUG) << "unblocking " << sta_mac << " from network";

    auto hostaps              = database.get_active_radios();
    const auto &current_bssid = database.get_sta_parent(sta_mac);
    const auto &ssid          = database.get_bss_ssid(tlvf::mac_from_string(current_bssid));

    std::unordered_set<sMacAddr> unblock_list{tlvf::mac_from_string(sta_mac)};

    for (auto &hostap : hostaps) {
        /*
         * unblock client from all hostaps to prevent it from getting locked out
         */
        std::shared_ptr<Agent::sRadio> radio =
            database.get_radio_by_uid(tlvf::mac_from_string(hostap));
        if (!radio) {
            continue;
        }

        for (const auto &bss : radio->bsses) {
            if (bss.second->ssid != ssid) {
                continue;
            }
            std::shared_ptr<Agent> agent = database.get_agent_by_radio_uid(radio->radio_uid);
            if (!agent) {
                continue;
            }

            son_actions::send_client_association_control(
                database, cmdu_tx, agent->al_mac, bss.second->bssid, unblock_list, 0,
                wfa_map::tlvClientAssociationControlRequest::UNBLOCK);
        }
    }
}

int son_actions::steer_sta(db &database, ieee1905_1::CmduMessageTx &cmdu_tx, task_pool &tasks,
                           std::string sta_mac, std::string chosen_hostap,
                           const std::string &triggered_by, const std::string &steering_type,
                           bool disassoc_imminent, int disassoc_timer_ms, bool steer_restricted)
{
    auto new_task = std::make_shared<client_steering_task>(
        database, cmdu_tx, tasks, sta_mac, chosen_hostap, triggered_by, steering_type,
        disassoc_imminent, disassoc_timer_ms, steer_restricted);

    tasks.add_task(new_task);
    return new_task->id;
}

int son_actions::start_btm_request_task(
    db &database, ieee1905_1::CmduMessageTx &cmdu_tx, task_pool &tasks,
    const bool &disassoc_imminent, const int &disassoc_timer_ms, const int &bss_term_duration_min,
    const int &validity_interval_ms, const int &steering_timer_ms, const std::string &sta_mac,
    const std::string &target_bssid, const std::string &event_source)
{

    auto new_task = std::make_shared<btm_request_task>(
        database, cmdu_tx, tasks, sta_mac, target_bssid, event_source, disassoc_imminent,
        validity_interval_ms, steering_timer_ms, disassoc_timer_ms);

    tasks.add_task(new_task);
    return new_task->id;
}
bool son_actions::set_radio_active(db &database, task_pool &tasks, std::string hostap_mac,
                                   const bool active)
{
    bool result = database.set_radio_active(tlvf::mac_from_string(hostap_mac), active);

    if (result) {
        bml_task::connection_change_event new_event;
        new_event.mac =
            tlvf::mac_to_string(database.get_radio_parent_agent(tlvf::mac_from_string(hostap_mac)));
        int bml_task_id = database.get_bml_task_id();
        tasks.push_event(bml_task_id, bml_task::CONNECTION_CHANGE, &new_event);
        LOG(TRACE) << "BML, sending hostap (" << hostap_mac
                   << ") active CONNECTION_CHANGE for IRE mac " << new_event.mac;
    }

    return result;
}

void son_actions::disconnect_client(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                    const std::string &client_mac, const std::string &bssid,
                                    eDisconnectType type, uint32_t reason,
                                    eClientDisconnectSource src)
{

    auto agent_mac = database.get_bss_parent_agent(tlvf::mac_from_string(bssid));

    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_DISCONNECT_REQUEST message!";
        return;
    }
    request->mac()    = tlvf::mac_from_string(client_mac);
    request->vap_id() = database.get_bss_vap_id(tlvf::mac_from_string(bssid));
    request->type()   = type;
    request->reason() = reason;
    request->src()    = src;

    const auto parent_radio = database.get_bss_parent_radio(bssid);
    son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database, parent_radio);
    LOG(DEBUG) << "sending DISASSOCIATE request, client " << client_mac << " bssid " << bssid;
}

void son_actions::send_cli_debug_message(db &database, ieee1905_1::CmduMessageTx &cmdu_tx,
                                         std::stringstream &ss)
{
    auto controller_ctx = database.get_controller_ctx();
    if (!controller_ctx) {
        LOG(ERROR) << "controller_ctx == nullptr";
        return;
    }

    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_RESPONSE_STR>(cmdu_tx);

    if (response == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_RESPONSE_STR message!";
        return;
    }

    //In case we don't have enough space for node length, reserve 1 byte for '\0'
    size_t reserved_size =
        (message_com::get_vs_cmdu_size_on_buffer<beerocks_message::cACTION_CLI_RESPONSE_STR>() - 1);
    size_t max_size = cmdu_tx.getMessageBuffLength() - reserved_size;
    size_t size     = (ss.tellp() > int(max_size)) ? max_size : size_t(ss.tellp());

    if (!response->alloc_buffer(size + 1)) {
        LOG(ERROR) << "Failed buffer allocation";
        return;
    }

    auto buf = response->buffer(0);
    if (!buf) {
        LOG(ERROR) << "Failed buffer allocation";
        return;
    }
    std::copy_n(ss.str().c_str(), size, buf);
    (buf)[size] = 0;

    for (int idx = 0;; idx++) {
        int fd = database.get_cli_socket_at(idx);
        if (beerocks::net::FileDescriptor::invalid_descriptor != fd) {
            controller_ctx->send_cmdu(fd, cmdu_tx);
        } else {
            break;
        }
    }
}

void son_actions::handle_dead_radio(const sMacAddr &mac, bool reported_by_parent, db &database,
                                    task_pool &tasks)
{
    LOG(DEBUG) << "NOTICE: handling dead radio " << mac << " reported by parent "
               << reported_by_parent;

    std::shared_ptr<Agent::sRadio> radio = database.get_radio_by_uid(mac);
    if (!radio) {
        return;
    }

    std::string mac_str = tlvf::mac_to_string(mac);
    if (reported_by_parent) {
        database.set_radio_state(mac_str, beerocks::STATE_DISCONNECTED);
        set_radio_active(database, tasks, mac_str, false);

        /*
         * set all stations in the subtree as disconnected
         */
        int agent_monitoring_task_id = database.get_agent_monitoring_task_id();
        for (const auto &bss : radio->bsses) {
            for (const auto &client : bss.second->connected_stations) {
                // kill old roaming task
                int prev_task_id = client.second->roaming_task_id;
                if (tasks.is_task_running(prev_task_id)) {
                    tasks.kill_task(prev_task_id);
                }

                tasks.push_event(agent_monitoring_task_id, STATE_DISCONNECTED, &mac_str);
                bml_task::connection_change_event new_event;
                new_event.mac = tlvf::mac_to_string(client.first);
                tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE,
                                 &new_event);
                LOG(DEBUG) << "BML, sending client disconnect CONNECTION_CHANGE for mac "
                           << new_event.mac;
            }
        }
    }
    LOG(DEBUG) << "handling dead radio, done for mac " << mac;
}

void son_actions::handle_dead_station(std::string mac, bool reported_by_parent, db &database,
                                      task_pool &tasks)
{
    LOG(DEBUG) << "NOTICE: handling dead station " << mac << " reported by parent "
               << reported_by_parent;

    std::shared_ptr<Station> station = database.get_station(tlvf::mac_from_string(mac));
    if (!station) {
        return;
    }

    if (database.is_sta_wireless(mac)) {
        // If there is running association handling task already, terminate it.
        int prev_task_id = station->association_handling_task_id;
        if (tasks.is_task_running(prev_task_id)) {
            tasks.kill_task(prev_task_id);
        }
    }

    if (reported_by_parent) {
        database.set_sta_state(mac, beerocks::STATE_DISCONNECTED);

        // Clear node ipv4
        database.set_sta_ipv4(mac, std::string());

        // Notify steering task, if any, of disconnect.
        int steering_task = station->steering_task_id;
        if (tasks.is_task_running(steering_task))
            tasks.push_event(steering_task, client_steering_task::STA_DISCONNECTED);

        // Notify btm_request task, if any, of disconnect.
        int btm_request_task = station->btm_request_task_id;
        if (tasks.is_task_running(btm_request_task))
            tasks.push_event(btm_request_task, btm_request_task::STA_DISCONNECTED);

        if (database.get_sta_handoff_flag(*station)) {
            LOG(DEBUG) << "handoff_flag == true, mac " << mac;
            // We're in the middle of steering, don't mark as disconnected (yet).
            return;
        } else {
            LOG(DEBUG) << "handoff_flag == false, mac " << mac;

            // If we're not in the middle of steering, kill roaming task
            int prev_task_id = station->roaming_task_id;
            if (tasks.is_task_running(prev_task_id)) {
                tasks.kill_task(prev_task_id);
            }
        }

        // If there is an instance of association handling task, kill it
        int association_handling_task_id = station->association_handling_task_id;
        if (tasks.is_task_running(association_handling_task_id)) {
            tasks.kill_task(association_handling_task_id);
        }
    }

    // update bml listeners
    if (!station->is_bSta()) {
        bml_task::connection_change_event new_event;
        new_event.mac = mac;
        tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
        LOG(DEBUG) << "BML, sending client disconnect CONNECTION_CHANGE for mac " << new_event.mac;
    } else {
        auto backhaul_bridge = database.m_agents.get(station->al_mac);
        if (!backhaul_bridge) {
            LOG(ERROR) << "Station: " << mac << "does not have a bridge under it!";
        } else {
            bml_task::connection_change_event new_event;
            new_event.mac = tlvf::mac_to_string(backhaul_bridge->al_mac);
            LOG(DEBUG) << "BML, sending IRE disconnect CONNECTION_CHANGE for mac " << new_event.mac;
            tasks.push_event(database.get_bml_task_id(), bml_task::CONNECTION_CHANGE, &new_event);
        }
    }

    LOG(DEBUG) << "handling dead station, done for mac " << mac;
}

bool son_actions::validate_beacon_measurement_report(beerocks_message::sBeaconResponse11k report,
                                                     const std::string &sta_mac,
                                                     const std::string &bssid)
{
    if (report.rcpi > RCPI_MAX) {
        LOG(WARNING) << "RCPI Measurement is in reserved value range rcpi=" << report.rcpi;
    }

    return (report.rep_mode == 0) &&
           //      (report.rsni                                  >  0          ) &&
           (report.rcpi != RCPI_INVALID) &&
           //      (report.start_time                            >  0          ) &&
           //      (report.duration                              >  0          ) &&
           (report.channel > 0) && (tlvf::mac_to_string(report.sta_mac) == sta_mac) &&
           (tlvf::mac_to_string(report.bssid) == bssid);
}

/**
 * @brief Check if the operating classes of @a radio_basic_caps matches any of the operating classes
 *        in @a bss_info_conf
 *
 * @param radio_basic_caps The AP Radio Basic Capabilities TLV of the radio
 * @param bss_info_conf The BSS Info we try to configure
 * @return true if one of the operating classes overlaps, false if they are disjoint
 */
bool son_actions::has_matching_operating_class(
    wfa_map::tlvApRadioBasicCapabilities &radio_basic_caps,
    const wireless_utils::sBssInfoConf &bss_info_conf)
{
    for (uint8_t i = 0; i < radio_basic_caps.operating_classes_info_list_length(); i++) {
        auto operating_class_info = std::get<1>(radio_basic_caps.operating_classes_info_list(i));
        for (auto operating_class : bss_info_conf.operating_class) {
            if (operating_class == operating_class_info.operating_class()) {
                return true;
            }
        }
    }
    return false;
}

bool son_actions::send_cmdu_to_agent(const sMacAddr &dest_mac, ieee1905_1::CmduMessageTx &cmdu_tx,
                                     db &database, const std::string &radio_mac)
{
    if (cmdu_tx.getMessageType() == ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE) {
        if (!database.is_prplmesh(dest_mac)) {
            // skip non-prplmesh agents
            return false;
        }
        auto beerocks_header = message_com::get_beerocks_header(cmdu_tx);
        if (!beerocks_header) {
            LOG(ERROR) << "Failed getting beerocks_header!";
            return false;
        }

        beerocks_header->actionhdr()->radio_mac() = tlvf::mac_from_string(radio_mac);
        beerocks_header->actionhdr()->direction() = beerocks::BEEROCKS_DIRECTION_AGENT;
    }

    auto controller_ctx = database.get_controller_ctx();
    if (controller_ctx == nullptr) {
        LOG(ERROR) << "controller_ctx == nullptr";
        return false;
    }

    return controller_ctx->send_cmdu_to_broker(cmdu_tx, dest_mac, database.get_local_bridge_mac());
}

bool son_actions::send_ap_config_renew_msg(ieee1905_1::CmduMessageTx &cmdu_tx, db &database)
{
    // Create AP-Configuration renew message
    auto cmdu_header =
        cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RENEW_MESSAGE);
    if (!cmdu_header) {
        LOG(ERROR) << "Failed building IEEE1905 AP_AUTOCONFIGURATION_RENEW_MESSAGE";
        return false;
    }

    // Add MAC address TLV
    auto tlvAlMac = cmdu_tx.addClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMac) {
        LOG(ERROR) << "Failed addClass ieee1905_1::tlvAlMacAddress";
        return false;
    }
    tlvAlMac->mac() = database.get_local_bridge_mac();

    // Add Supported-Role TLV
    auto tlvSupportedRole = cmdu_tx.addClass<ieee1905_1::tlvSupportedRole>();
    if (!tlvSupportedRole) {
        LOG(ERROR) << "Failed addClass ieee1905_1::tlvSupportedRole";
        return false;
    }
    tlvSupportedRole->value() = ieee1905_1::tlvSupportedRole::REGISTRAR;

    // Add Supported-Frequency-Band TLV
    auto tlvSupportedFreqBand = cmdu_tx.addClass<ieee1905_1::tlvSupportedFreqBand>();
    if (!tlvSupportedFreqBand) {
        LOG(ERROR) << "Failed addClass ieee1905_1::tlvSupportedFreqBand";
        return false;
    }
    // According to the Multi-AP Specification Version 2.0 section 7.1
    // Ragardless of what is sent here, the Agent will handle the Renew eitherway
    tlvSupportedFreqBand->value() = ieee1905_1::tlvSupportedFreqBand::eValue(0);

    LOG(INFO) << "Send AP_AUTOCONFIGURATION_RENEW_MESSAGE";
    return son_actions::send_cmdu_to_agent(network_utils::MULTICAST_1905_MAC_ADDR, cmdu_tx,
                                           database);
}

bool son_actions::send_topology_query_msg(const sMacAddr &dest_mac,
                                          ieee1905_1::CmduMessageTx &cmdu_tx, db &database)
{
    if (!cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_QUERY_MESSAGE)) {
        LOG(ERROR) << "Failed building TOPOLOGY_QUERY_MESSAGE message!";
        return false;
    }
    auto tlvProfile2MultiApProfile = cmdu_tx.addClass<wfa_map::tlvProfile2MultiApProfile>();
    if (!tlvProfile2MultiApProfile) {
        LOG(ERROR) << "addClass wfa_map::tlvProfile2MultiApProfile failed";
        return false;
    }
    return send_cmdu_to_agent(dest_mac, cmdu_tx, database);
}

bool son_actions::send_client_association_control(
    db &database, ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &agent_mac,
    const sMacAddr &agent_bssid, const std::unordered_set<sMacAddr> &station_list,
    const int &duration_sec,
    wfa_map::tlvClientAssociationControlRequest::eAssociationControl association_flag)
{
    if (!cmdu_tx.create(0, ieee1905_1::eMessageType::CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE)) {
        LOG(ERROR) << "Failed building CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE message";
        return false;
    }

    auto association_control_request_tlv =
        cmdu_tx.addClass<wfa_map::tlvClientAssociationControlRequest>();

    if (!association_control_request_tlv) {
        LOG(ERROR) << "addClass wfa_map::tlvClientAssociationControlRequest failed";
        return false;
    }

    association_control_request_tlv->bssid_to_block_client() = agent_bssid;
    association_control_request_tlv->association_control()   = association_flag;

    if (association_flag == wfa_map::tlvClientAssociationControlRequest::BLOCK ||
        association_flag == wfa_map::tlvClientAssociationControlRequest::TIMED_BLOCK) {
        association_control_request_tlv->validity_period_sec() = duration_sec;
    } else {
        association_control_request_tlv->validity_period_sec() = 0;
    }

    int index = 0;
    std::stringstream sta_list_str;
    for (auto station : station_list) {
        if (!association_control_request_tlv->alloc_sta_list()) {
            LOG(ERROR)
                << "can't alloc new station for client association control, currently allocd "
                << index << " stations";

            if (association_control_request_tlv->sta_list_length() > index) {
                association_control_request_tlv->sta_list_length() = index;
                // alloc_sta_list() may fail after incrementing the sta_list_lenght;
            }
            break;
        }
        auto sta_list_unblock         = association_control_request_tlv->sta_list(index);
        std::get<1>(sta_list_unblock) = station;
        index++;
        sta_list_str << tlvf::mac_to_string(station) << ", ";
    }

    std::string action_str =
        (association_flag == wfa_map::tlvClientAssociationControlRequest::BLOCK ||
         association_flag == wfa_map::tlvClientAssociationControlRequest::TIMED_BLOCK)
            ? "block"
            : "unblock";

    std::string duration_str =
        duration_sec != 0 ? "for " + std::to_string(duration_sec) + " seconds" : "";

    LOG(DEBUG) << "sending " << action_str << " request for " << sta_list_str.str() << " to agent "
               << tlvf::mac_to_string(agent_mac) << " for bssid "
               << association_control_request_tlv->bssid_to_block_client() << duration_str;
    return son_actions::send_cmdu_to_agent(agent_mac, cmdu_tx, database);
}

bool son_actions::handle_agent_ap_mld_configuration_tlv(db &database, const sMacAddr &al_mac,
                                                        ieee1905_1::CmduMessageRx &cmdu_rx)
{

    // Handle AgentApMldConfiguration TLV
    auto agent = database.m_agents.get(al_mac);
    if (!agent) {
        LOG(ERROR) << "Agent with mac is not found in database mac=" << al_mac;
        return false;
    }

    auto agent_ap_mld_configuration = cmdu_rx.getClass<wfa_map::tlvAgentApMldConfiguration>();
    if (!agent_ap_mld_configuration) {
        LOG(DEBUG) << "No tlvAgentApMldConfiguration TLV received";
    } else {
        // Update APMLD Database and Data Model based on received AgentApMldConfiguration TLV
        for (uint8_t ap_mld_it = 0; ap_mld_it < agent_ap_mld_configuration->num_ap_mld();
             ++ap_mld_it) {

            // Get AgentApMld configuration from TLV
            std::tuple<bool, wfa_map::cApMld &> ap_mld_tuple(
                agent_ap_mld_configuration->ap_mld(ap_mld_it));
            if (!std::get<0>(ap_mld_tuple)) {
                LOG(ERROR) << "Couldn't get AP MLD from tlvAgentApMldConfiguration";
                return false;
            }
            wfa_map::cApMld &ap_mld = std::get<1>(ap_mld_tuple);

            // SSID
            if (ap_mld.ssid_str().empty()) {
                // Dropping this MLD from updation, as SSID is our key for ApMld
                // Hence, SSID shall not be empty
                LOG(ERROR) << "SSID is empty in tlvAgentApMldConfiguration";
                continue;
            }

            // Get or Allocate ApMld from DB
            auto ssid            = ap_mld.ssid_str();
            Agent::sAPMLD *apmld = database.get_or_allocate_ap_mld(al_mac, ssid);

            // SSID
            // Update SSID for both existing and new ApMld
            apmld->mld_info.mld_ssid = ssid;

            // MAC
            if (ap_mld.ap_mld_mac_addr_valid().is_valid) {
                apmld->mld_info.mld_mac = ap_mld.ap_mld_mac_addr();
            } else {
                apmld->mld_info.mld_mac = beerocks::net::network_utils::ZERO_MAC;
                LOG(WARNING) << "AP MLD MAC is not valid in tlvAgentApMldConfiguration";
            }

            // MLD MODE FLAGS - str, nstr, emlsr, emlmr
            if (ap_mld.modes().str) {
                apmld->mld_info.mld_mode =
                    Agent::sMLDInfo::mode(apmld->mld_info.mld_mode | Agent::sMLDInfo::mode::STR);
            }

            if (ap_mld.modes().nstr) {
                apmld->mld_info.mld_mode =
                    Agent::sMLDInfo::mode(apmld->mld_info.mld_mode | Agent::sMLDInfo::mode::NSTR);
            }

            if (ap_mld.modes().emlsr) {
                apmld->mld_info.mld_mode =
                    Agent::sMLDInfo::mode(apmld->mld_info.mld_mode | Agent::sMLDInfo::mode::EMLSR);
            }

            if (ap_mld.modes().emlmr) {
                apmld->mld_info.mld_mode =
                    Agent::sMLDInfo::mode(apmld->mld_info.mld_mode | Agent::sMLDInfo::mode::EMLMR);
            }

            for (uint8_t affiliated_ap_it = 0; affiliated_ap_it < ap_mld.num_affiliated_ap();
                 ++affiliated_ap_it) {
                std::tuple<bool, wfa_map::cAffiliatedAp &> affiliated_ap_tuple(
                    ap_mld.affiliated_ap(affiliated_ap_it));
                if (!std::get<0>(affiliated_ap_tuple)) {
                    LOG(ERROR) << "Couldn't get Affiliated AP from APMLD SSID : "
                               << apmld->mld_info.mld_ssid;
                    return false;
                }

                // Get or Allocate Affiliated AP from DB
                auto ruid = std::get<1>(affiliated_ap_tuple).ruid();
                Agent::sAPMLD::sAffiliatedAP *affiliated_ap =
                    database.get_or_allocate_affiliated_ap(*apmld, ruid);

                // RUID
                // Update RUID for both existing and new Affiliated AP
                affiliated_ap->ruid = ruid;

                // BSSID
                if (std::get<1>(affiliated_ap_tuple)
                        .affiliated_ap_fields_valid()
                        .affiliated_ap_mac_addr_valid) {
                    affiliated_ap->bssid =
                        std::get<1>(affiliated_ap_tuple).affiliated_ap_mac_addr();
                } else {
                    affiliated_ap->bssid = beerocks::net::network_utils::ZERO_MAC;
                }

                // LinkID
                // Check existing TLVs while polulating valid
                if (std::get<1>(affiliated_ap_tuple).affiliated_ap_fields_valid().linkid_valid) {
                    affiliated_ap->link_id = std::get<1>(affiliated_ap_tuple).linkid();
                } else {
                    affiliated_ap->link_id = -1;
                }
            }
            // Add AP MLD Info in Data Model
            database.dm_add_ap_mld(al_mac, *apmld);
        }

        // Remove redundant APMLD/AffiliatedAP

        /* Use local copy of ap_mlds for looping
           and remove actual ap_mlds from db */
        auto ap_mlds = agent->ap_mlds;

        for (auto db_apmld_it : ap_mlds) {
            bool apmld_found     = 0;
            uint8_t tlv_apmld_it = 0;
            auto mld_ssid        = db_apmld_it.first;
            for (tlv_apmld_it = 0; tlv_apmld_it < agent_ap_mld_configuration->num_ap_mld();
                 ++tlv_apmld_it) {
                // Get AgentApMld config from TLV
                std::tuple<bool, wfa_map::cApMld &> ap_mld_tuple(
                    agent_ap_mld_configuration->ap_mld(tlv_apmld_it));
                if (std::get<0>(ap_mld_tuple)) {
                    wfa_map::cApMld &ap_mld = std::get<1>(ap_mld_tuple);
                    // Check for SSID match
                    if (mld_ssid == ap_mld.ssid_str()) {
                        apmld_found = 1;
                        break;
                    }
                }
            }

            // Remove redundant ApMld
            if (!apmld_found) {
                // Remove Database only if Data Model is removed
                if (database.dm_remove_ap_mld(agent->al_mac, mld_ssid)) {
                    // Remove Database
                    agent->ap_mlds.erase(mld_ssid);
                    LOG(DEBUG) << "Removed ApMld with ssid: " << mld_ssid
                               << " from Agent: " << al_mac;
                }
            } else {
                // Logic to remove redundant Affiliated APs (added earlier, but not present now)
                for (auto db_affl_ap_it : db_apmld_it.second.affiliated_aps) {
                    auto ruid = db_affl_ap_it.first;
                    std::tuple<bool, wfa_map::cApMld &> ap_mld_tuple(
                        agent_ap_mld_configuration->ap_mld(tlv_apmld_it));
                    if (std::get<0>(ap_mld_tuple)) {
                        wfa_map::cApMld &ap_mld = std::get<1>(ap_mld_tuple);
                        bool afflap_found       = 0;
                        for (uint8_t tlv_affl_ap_it = 0;
                             tlv_affl_ap_it < ap_mld.num_affiliated_ap(); ++tlv_affl_ap_it) {
                            // Get AffiliatedAp config from TLV
                            std::tuple<bool, wfa_map::cAffiliatedAp &> affiliated_ap_tuple(
                                ap_mld.affiliated_ap(tlv_affl_ap_it));
                            if (std::get<0>(affiliated_ap_tuple)) {
                                // Check RUID match
                                if (ruid == std::get<1>(affiliated_ap_tuple).ruid()) {
                                    afflap_found = 1;
                                    break;
                                }
                            }
                        }
                        // Remove redundant Affiliated AP
                        if (!afflap_found) {
                            // Remove Database only if Data Model is removed
                            if (database.dm_remove_affiliated_ap(agent->al_mac, mld_ssid, ruid)) {
                                // Remove Database
                                agent->ap_mlds[mld_ssid].affiliated_aps.erase(ruid);
                                LOG(DEBUG) << "Removed Affiliated AP with ruid: " << ruid
                                           << " from ApMld with ssid: " << mld_ssid
                                           << " of Agent: " << al_mac;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}
