/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_autoconfiguration_task.h"
#include "link_metrics_collection_task.h"

#include "../agent_db.h"
#include "../son_slave_thread.h"
#include "../tlvf_utils.h"
#include "../traffic_separation.h"
#include "multi_vendor.h"
#include <bcl/beerocks_config_file.h>

#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_version.h>
#include <bcl/beerocks_wifi_channel.h>
#include <mapf/common/utils.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_apmanager.h>
#include <beerocks/tlvf/beerocks_message_control.h>
#include <beerocks/tlvf/beerocks_message_monitor.h>
#include <beerocks/tlvf/beerocks_message_platform.h>

#include <tlvf/WSC/m1.h>
#include <tlvf/airties/eAirtiesTLVId.h>
#include <tlvf/airties/tlvAirtiesMsgType.h>
#include <tlvf/airties/tlvAirtiesServiceStatus.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddress.h>
#include <tlvf/ieee_1905_1/tlvAutoconfigFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSearchedRole.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/wfa_map/tlvAgentApMldConfiguration.h>
#include <tlvf/wfa_map/tlvApRadioIdentifier.h>
#include <tlvf/wfa_map/tlvChannelScanReportingPolicy.h>
#include <tlvf/wfa_map/tlvControllerCapability.h>
#include <tlvf/wfa_map/tlvMetricReportingPolicy.h>
#include <tlvf/wfa_map/tlvProfile2ApRadioAdvancedCapabilities.h>
#include <tlvf/wfa_map/tlvProfile2Default802dotQSettings.h>
#include <tlvf/wfa_map/tlvProfile2MultiApProfile.h>
#include <tlvf/wfa_map/tlvProfile2TrafficSeparationPolicy.h>
#include <tlvf/wfa_map/tlvProfile2UnsuccessfulAssociationPolicy.h>
#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/wfa_map/tlvSteeringPolicy.h>
#include <tlvf/wfa_map/tlvSupportedService.h>

#include <beerocks/tlvf/beerocks_message_control.h>

#include <bpl/bpl_board.h>
#include <bpl/bpl_cfg.h>

#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace multi_vendor;

static constexpr uint8_t AUTOCONFIG_DISCOVERY_TIMEOUT_SECONDS = 3;
#define HANDLE_THIRD_PARTY_ENABLE "1"
#define VENDOR_HIDE_SSID 0x80
#define VENDOR_BSS_CFG 0x02

#define FSM_MOVE_STATE(radio_iface, new_state)                                                     \
    ({                                                                                             \
        LOG(TRACE) << "AP_AUTOCONFIGURATION " << radio_iface                                       \
                   << " FSM: " << fsm_state_to_string(m_radios_conf_params[radio_iface].state)     \
                   << " --> " << fsm_state_to_string(new_state);                                   \
        m_radios_conf_params[radio_iface].state = new_state;                                       \
    })

const std::string ApAutoConfigurationTask::fsm_state_to_string(eState status)
{
    switch (status) {
    case eState::UNCONFIGURED:
        return "UNCONFIGURED";
    case eState::CONTROLLER_DISCOVERY:
        return "CONTROLLER_DISCOVERY";
    case eState::WAIT_FOR_CONTROLLER_DISCOVERY_COMPLETE:
        return "WAIT_FOR_CONTROLLER_DISCOVERY_COMPLETE";
    case eState::SEND_AP_AUTOCONFIGURATION_WSC_M1:
        return "SEND_AP_AUTOCONFIGURATION_WSC_M1";
    case eState::WAIT_AP_AUTOCONFIGURATION_WSC_M2:
        return "WAIT_AP_AUTOCONFIGURATION_WSC_M2";
    case eState::WAIT_AP_CONFIGURATION_COMPLETE:
        return "WAIT_AP_CONFIGURATION_COMPLETE";
    case eState::CONFIGURED:
        return "CONFIGURED";
    case eState::SKIPPED:
        return "SKIPPED";
    default:
        LOG(ERROR) << "state argument doesn't have an enum";
        break;
    }
    return std::string();
}

ApAutoConfigurationTask::ApAutoConfigurationTask(slave_thread &btl_ctx,
                                                 ieee1905_1::CmduMessageTx &cmdu_tx)
    : Task(eTaskType::AP_AUTOCONFIGURATION), m_btl_ctx(btl_ctx), m_cmdu_tx(cmdu_tx)
{
}

void ApAutoConfigurationTask::work()
{
    if (!m_task_is_active) {
        return;
    }

    uint8_t configured_aps_count = 0;
    for (auto &radios_conf_param_kv : m_radios_conf_params) {
        const auto &radio_iface = radios_conf_param_kv.first;
        auto &conf_params       = radios_conf_param_kv.second;
        switch (conf_params.state) {
        case eState::UNCONFIGURED: {
            break;
        }
        case eState::CONTROLLER_DISCOVERY: {
            auto db = AgentDB::get();

            auto radio = db->radio(radio_iface);
            if (!radio) {
                continue;
            }

            // If another radio with same band already finished the discovery phase, we can skip
            // directly to next phase (AP_CONFIGURATION).
            if (m_discovery_status[radio->wifi_channel.get_freq_type()].completed) {
                FSM_MOVE_STATE(radio_iface, eState::SEND_AP_AUTOCONFIGURATION_WSC_M1);
            }

            // If another radio with same band already have sent the
            // AP_AUTOCONFIGURATION_SEARCH_MESSAGE, we can skip and let it handle it.
            if (m_discovery_status[radio->wifi_channel.get_freq_type()].msg_sent) {
                continue;
            }

            if (send_ap_autoconfiguration_search_message(radio_iface)) {
                m_discovery_status[radio->wifi_channel.get_freq_type()].msg_sent = true;
            }

            if (m_discovery_status[radio->wifi_channel.get_freq_type()].skipped) {
                FSM_MOVE_STATE(radio_iface, eState::SKIPPED);
                break;
            }

            conf_params.timeout = std::chrono::steady_clock::now() +
                                  std::chrono::seconds(AUTOCONFIG_DISCOVERY_TIMEOUT_SECONDS);

            FSM_MOVE_STATE(radio_iface, eState::WAIT_FOR_CONTROLLER_DISCOVERY_COMPLETE);
            break;
        }
        case eState::WAIT_FOR_CONTROLLER_DISCOVERY_COMPLETE: {
            auto db    = AgentDB::get();
            auto radio = db->radio(radio_iface);
            if (!radio) {
                continue;
            }
            if (m_discovery_status[radio->wifi_channel.get_freq_type()].completed) {
                FSM_MOVE_STATE(radio_iface, eState::SEND_AP_AUTOCONFIGURATION_WSC_M1);
                break;
            }

            if (std::chrono::steady_clock::now() > conf_params.timeout) {
                FSM_MOVE_STATE(radio_iface, eState::CONTROLLER_DISCOVERY);
                m_discovery_status[radio->wifi_channel.get_freq_type()].msg_sent = false;
            }
            break;
        }
        case eState::SEND_AP_AUTOCONFIGURATION_WSC_M1: {
            send_ap_autoconfiguration_wsc_m1_message(radio_iface);
            conf_params.timeout =
                std::chrono::steady_clock::now() +
                std::chrono::seconds(beerocks::ieee1905_1_consts::AUTOCONFIG_M2_TIMEOUT_SECONDS);
            FSM_MOVE_STATE(radio_iface, eState::WAIT_AP_AUTOCONFIGURATION_WSC_M2);
            break;
        }
        case eState::WAIT_AP_AUTOCONFIGURATION_WSC_M2: {
            if (std::chrono::steady_clock::now() > conf_params.timeout) {
                FSM_MOVE_STATE(radio_iface, eState::SEND_AP_AUTOCONFIGURATION_WSC_M1);
            }
            break;
        }
        case eState::WAIT_AP_CONFIGURATION_COMPLETE: {
            configuration_complete_wait_action(radio_iface);
            break;
        }
        case eState::CONFIGURED: {
            configured_aps_count++;
            break;
        }
        case eState::SKIPPED: {
            configured_aps_count++;
            break;
        }
        default:
            break;
        }
    }

    // Update status on the database.
    auto db = AgentDB::get();
    if (configured_aps_count > 0 && configured_aps_count == m_radios_conf_params.size()) {
        db->statuses.ap_autoconfiguration_completed = true;
        m_task_is_active                            = false;
        LOG(DEBUG) << "Link to the controller is established";

        // Send pre-associated sta notification request to all radio
        for (const auto &radios_conf_param_kv : m_radios_conf_params) {
            const auto &radio_iface = radios_conf_param_kv.first;
            if (radios_conf_param_kv.second.state == eState::SKIPPED) {
                LOG(ERROR) << "Skipping send_ap_connected_sta_notifications_request for "
                           << radio_iface;
                continue;
            }
            if (!send_ap_connected_sta_notifications_request(radio_iface)) {
                LOG(ERROR) << "send_ap_connected_sta_notifications_request failed for "
                           << radio_iface;
                continue;
            }
            LOG(DEBUG) << "Pre-associated station notification request send done for radio iface "
                       << radio_iface;
        }
    }
}

void ApAutoConfigurationTask::handle_event(uint8_t event_enum_value, const void *event_obj)
{
    switch (eEvent(event_enum_value)) {
    case INIT_TASK: {
        auto db = AgentDB::get();

        db->statuses.ap_autoconfiguration_completed = false;

        if (!m_traffic_separation_configurator) {
            m_traffic_separation_configurator =
                std::make_unique<TrafficSeparation>(m_btl_ctx.m_broker_client);
        }

        // Reset the traffic separation configuration as they will be reconfigured on
        // autoconfiguration.
        m_traffic_separation_configurator->clear_configuration();

        // Reset the discovery statuses.
        for (auto &discovery_status : m_discovery_status) {
            discovery_status.second = {};
        }
        m_task_is_active = false;
        break;
    }
    case START_AP_AUTOCONFIGURATION: {
        auto db = AgentDB::get();
        for (const auto radio : db->get_radios_list()) {
            if (!radio) {
                continue;
            }

            if (event_obj) {
                auto specific_iface_ptr = reinterpret_cast<const std::string *>(event_obj);
                if (*specific_iface_ptr != radio->front.iface_name) {
                    continue;
                }
            }

            LOG(DEBUG) << "starting discovery sequence on radio_iface=" << radio->front.iface_name;
            FSM_MOVE_STATE(radio->front.iface_name, eState::CONTROLLER_DISCOVERY);
            m_task_is_active = true;
        }

        /* Workaround: Send Gratuitous ARP to get autoconfig response
           from Controller which is available after hops greater than 1.
        */
        std::string str_iface_ip;
        std::string str_iface_mac;
        int arp_socket                = -1;
        int count                     = 5;
        sMacAddr dst_mac              = {.oct = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
        std::string bridge_iface_name = db->bridge.iface_name;

        /* Get the IP address and MAC Address of the bridge */
        if (!network_utils::linux_iface_get_ip(bridge_iface_name, str_iface_ip)) {
            LOG(ERROR) << "Failed reading '" << bridge_iface_name << "' IP!";
            return;
        }
        if (!network_utils::linux_iface_get_mac(bridge_iface_name, str_iface_mac)) {
            LOG(ERROR) << "Failed reading '" << bridge_iface_name << "' MAC!";
            return;
        }
        /* Send Gratuitous ARP*/
        LOG(DEBUG) << "Sending Gratuitous ARP from "
                   << " Bridge: " << bridge_iface_name << " IP: " << str_iface_ip
                   << " Src MAC: " << str_iface_mac << " Dst MAC: " << tlvf::mac_to_string(dst_mac)
                   << " Count: " << count;

        network_utils::arp_send(bridge_iface_name, str_iface_ip, str_iface_ip, dst_mac,
                                tlvf::mac_from_string(str_iface_mac), count, arp_socket);
        // Call work() to not waste time, and send_ap_autoconfiguration_search_message immediately.
        work();
        break;
    }
    case APPLY_CONFIG_FOR_NEW_IFACE: {
        auto db = AgentDB::get();
        for (const auto radio : db->get_radios_list()) {
            if (!radio) {
                continue;
            }
            for (auto &bss : radio->front.bssids) {
                if (bss.backhaul_bss) {
                    m_traffic_separation_configurator->apply_policy_for_new_interface(
                        bss.iface_name);
                }
            }
        }
        break;
    }
    default: {
        LOG(DEBUG) << "Message handler doesn't exists for event type " << event_enum_value;
        break;
    }
    }
}

bool ApAutoConfigurationTask::handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx, uint32_t iface_index,
                                          const sMacAddr &dst_mac, const sMacAddr &src_mac, int fd,
                                          std::shared_ptr<beerocks_header> beerocks_header)
{
    switch (cmdu_rx.getMessageType()) {
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RESPONSE_MESSAGE: {
        handle_ap_autoconfiguration_response(cmdu_rx, src_mac);
        return true;
    }
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE: {
        handle_ap_autoconfiguration_wsc(cmdu_rx);
        return true;
    }
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RENEW_MESSAGE: {
        handle_ap_autoconfiguration_wsc_renew(cmdu_rx);
        return true;
    }
    case ieee1905_1::eMessageType::MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE: {
        handle_multi_ap_policy_config_request(cmdu_rx);
        return true;
    }
    case ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE: {
        // Internally, the 'handle_vendor_specific' might not really handle
        // the CMDU, thus we need to return the real return value and not 'true'.
        return handle_vendor_specific(cmdu_rx, src_mac, fd, beerocks_header);
    }
    default: {
        // Message was not handled, therefore return false.
        return false;
    }
    }
}

bool ApAutoConfigurationTask::handle_vendor_specific(
    ieee1905_1::CmduMessageRx &cmdu_rx, const sMacAddr &src_mac, int sd,
    std::shared_ptr<beerocks_header> beerocks_header)
{
    if (!beerocks_header) {
        LOG(ERROR) << "beerocks_header is nullptr";
        return false;
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE: {
        handle_vs_wifi_credentials_update_response(cmdu_rx, sd, beerocks_header);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION: {
        handle_vs_ap_enabled_notification(cmdu_rx, sd, beerocks_header);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION: {
        handle_vs_vaps_list_update_notification(cmdu_rx, sd, beerocks_header);
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_APPLY_VLAN_POLICY_REQUEST: {
        handle_vs_apply_vlan_policy_request(cmdu_rx, sd, beerocks_header);
        break;
    }

    default: {
        // Message was not handled, therefore return false.
        return false;
    }
    }
    return true;
}

void ApAutoConfigurationTask::configuration_complete_wait_action(const std::string &radio_iface)
{
    auto &radio_conf_params = m_radios_conf_params[radio_iface];
#if !USE_PRPLMESH_WHM
    // counting BSSes that are actually active is a faux-pas for whm; see PPM-2531 for discussion
    // if after onboarding the agent is not operating correctly, debug by reading the datamodel

    // num_of_bss_available updates on ACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE handler
    if (!radio_conf_params.num_of_bss_available) {
        return;
    }

    // Ap AutoConf starts to be applied once radio is up, i.e at least one BSS is enabled
    // So no need to require for all BBSs to be enabled, (or even timeouting here)
    // to follow up with auto conf completion
    if (radio_conf_params.enabled_bssids.size() < 1) {

        // Wait until WAIT_AP_ENABLED_TIMEOUT_SECONDS timeout is expired.
        // If expired and we did not receive AP_ENABLE on the radios BSSs, assume it was already
        // configured, and the AP_ENABLE will never come.
        // The timer is set on "handle_vs_wifi_credentials_update_response()""
        if (std::chrono::steady_clock::now() < radio_conf_params.timeout) {
            return;
        }
    }
#endif

    LOG(INFO) << "num_of_bss_available " << radio_conf_params.num_of_bss_available
              << " enabled_bssids " << radio_conf_params.enabled_bssids.size() << " radio_iface "
              << radio_iface;

    // Arbitrary value
    constexpr uint8_t WAIT_IFACES_INSIDE_THE_BRIDGE_TIMEOUT_SECONDS = 30;

    if (!radio_conf_params.sent_vaps_list_update) {
        if (!send_ap_bss_info_update_request(radio_iface)) {
            LOG(ERROR) << "send_ap_bss_info_update_request has failed";
            return;
        } else {
            LOG(INFO) << "send ACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST " << radio_iface;
        }
        radio_conf_params.sent_vaps_list_update = true;

        radio_conf_params.timeout =
            std::chrono::steady_clock::now() +
            std::chrono::seconds(WAIT_IFACES_INSIDE_THE_BRIDGE_TIMEOUT_SECONDS);
    }

    if (!radio_conf_params.received_vaps_list_update) {
        return;
    }
    LOG(INFO) << "received_vaps_list_update is true";

#if !USE_PRPLMESH_WHM

    // Check if all reported interfaces are in the bridge
    auto db               = AgentDB::get();
    auto ifaces_in_bridge = network_utils::linux_get_iface_list_from_bridge(db->bridge.iface_name);

    auto radio = db->radio(radio_iface);
    if (!radio) {
        return;
    }
    for (const auto &bssid : radio->front.bssids) {

        if (bssid.iface_name.empty()) {
            continue;
        }

        if (!bssid.active) {
            LOG(INFO) << "skip bssid " << bssid.iface_name << " since it is disabled";
            continue;
        }
        auto found = std::find(ifaces_in_bridge.begin(), ifaces_in_bridge.end(), bssid.iface_name);

        // Return if not all bssids are in the bridge, and print error.
        if (found == ifaces_in_bridge.end()) {
            auto host_bridge = network_utils::linux_iface_get_host_bridge(bssid.iface_name);
            if (!host_bridge.empty()) {
                // skip secondary bss, already belonging to different bridge
                continue;
            }
            if (std::chrono::steady_clock::now() > radio_conf_params.timeout) {
                auto timeout_sec =
                    std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::steady_clock::now() - radio_conf_params.timeout +
                        std::chrono::seconds(WAIT_IFACES_INSIDE_THE_BRIDGE_TIMEOUT_SECONDS))
                        .count();
                LOG_EVERY_N(10, ERROR)
                    << "Some of or all " << radio_iface << " BSSIDs are not in the bridge after "
                    << timeout_sec << " seconds after AP configuration!";
            }
            return;
        }
    }
    /* these checks do not make any sense when using bwl::whm;
    if after onboarding the agent is not operating correctly,
    debug by reading the datamodel */
#endif

    FSM_MOVE_STATE(radio_iface, eState::CONFIGURED);

    LOG(TRACE) << "Finished configuration on " << radio_iface;
    m_traffic_separation_configurator->apply_policy(radio_iface);

    return;
}

bool ApAutoConfigurationTask::send_ap_autoconfiguration_search_message(
    const std::string &radio_iface)
{
    auto db = AgentDB::get();

    auto numberOfSupportedService = 2;
    if (db->device_conf.local_controller) {
        numberOfSupportedService += 2;
    }
    if (db->device_conf.certification_mode) {
        // The R4 QCA WFA controller won't answer Autoconfig Searches that specify EM_AP_AGENT as a supported service. See PPM-3287.
        numberOfSupportedService = 1;
    }

    ieee1905_1::tlvAutoconfigFreqBand::eValue freq_band =
        ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ;
    /*
     * TODO
     * this is a workaround, need to find a better way to know each slave's band
     */
    auto radio = db->radio(radio_iface);
    if (!radio) {
        LOG(DEBUG) << "Radio of iface " << radio_iface << " does not exist on the db";
        return false;
    }
    if (radio->wifi_channel.get_freq_type() == beerocks::eFreqType::FREQ_24G) {
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ;
    } else if (radio->wifi_channel.get_freq_type() == beerocks::eFreqType::FREQ_5G) {
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_5_GHZ;
    } else if (radio->wifi_channel.get_freq_type() == beerocks::eFreqType::FREQ_6G) {
        if (db->device_conf.certification_mode) {
            LOG(INFO) << "Certification mode: skipping AP-Autoconfiguration Search on 6GHz iface: "
                      << radio_iface;
            m_discovery_status[beerocks::eFreqType::FREQ_6G].skipped = true;
            return false;
        }
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_6_GHZ;
    } else {
        LOG(ERROR) << "unsupported freq_type=" << int(radio->wifi_channel.get_freq_type())
                   << ", iface=" << radio_iface;
        return false;
    }

    auto create_autoconfig_search = [&]() -> bool {
        auto cmdu_header =
            m_cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE);
        if (!cmdu_header) {
            LOG(ERROR) << "cmdu creation of type AP_AUTOCONFIGURATION_SEARCH_MESSAGE, has failed";
            return false;
        }

        auto tlvAlMacAddress = m_cmdu_tx.addClass<ieee1905_1::tlvAlMacAddress>();
        if (!tlvAlMacAddress) {
            LOG(ERROR) << "addClass ieee1905_1::tlvAlMacAddress failed";
            return false;
        }
        tlvAlMacAddress->mac() = db->bridge.mac;

        auto tlvSearchedRole = m_cmdu_tx.addClass<ieee1905_1::tlvSearchedRole>();
        if (!tlvSearchedRole) {
            LOG(ERROR) << "addClass ieee1905_1::tlvSearchedRole failed";
            return false;
        }
        tlvSearchedRole->value() = ieee1905_1::tlvSearchedRole::REGISTRAR;

        auto tlvAutoconfigFreqBand = m_cmdu_tx.addClass<ieee1905_1::tlvAutoconfigFreqBand>();
        if (!tlvAutoconfigFreqBand) {
            LOG(ERROR) << "addClass ieee1905_1::tlvAutoconfigFreqBand failed";
            return false;
        }
        tlvAutoconfigFreqBand->value() = freq_band;

        auto tlvSupportedService = m_cmdu_tx.addClass<wfa_map::tlvSupportedService>();
        if (!tlvSupportedService) {
            LOG(ERROR) << "addClass wfa_map::tlvSupportedService failed";
            return false;
        }
        if (!tlvSupportedService->alloc_supported_service_list(numberOfSupportedService)) {
            LOG(ERROR) << "alloc_supported_service_list failed";
            return false;
        }
        for (int serviceID = 0; serviceID < tlvSupportedService->supported_service_list_length();
             serviceID++) {
            auto supportedServiceTuple = tlvSupportedService->supported_service_list(serviceID);
            if (!std::get<0>(supportedServiceTuple)) {
                LOG(ERROR) << "Invalid tlvSupportedService";
                return false;
            }
            if (serviceID == 0) {
                std::get<1>(supportedServiceTuple) =
                    wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_AGENT;
            } else if (serviceID == 1) {
                std::get<1>(supportedServiceTuple) =
                    wfa_map::tlvSupportedService::eSupportedService::EM_AP_AGENT;
            } else if (serviceID == 2) {
                std::get<1>(supportedServiceTuple) =
                    wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_CONTROLLER;
            } else if (serviceID == 3) {
                std::get<1>(supportedServiceTuple) =
                    wfa_map::tlvSupportedService::eSupportedService::EM_AP_CONTROLLER;
            }
        }

        auto tlvSearchedService = m_cmdu_tx.addClass<wfa_map::tlvSearchedService>();
        if (!tlvSearchedService) {
            LOG(ERROR) << "addClass wfa_map::tlvSearchedService failed";
            return false;
        }
        if (!tlvSearchedService->alloc_searched_service_list()) {
            LOG(ERROR) << "alloc_searched_service_list failed";
            return false;
        }
        auto searchedServiceTuple = tlvSearchedService->searched_service_list(0);
        if (!std::get<0>(searchedServiceTuple)) {
            LOG(ERROR) << "Failed accessing searched_service_list";
            return false;
        }
        std::get<1>(searchedServiceTuple) =
            wfa_map::tlvSearchedService::eSearchedService::MULTI_AP_CONTROLLER;

        // Add prplMesh handshake in a vendor specific TLV.
        // If the controller is prplMesh, it will reply to the autoconfig search with
        // handshake response.
        auto request =
            message_com::add_vs_tlv<beerocks_message::cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST>(
                m_cmdu_tx);
        if (!request) {
            LOG(ERROR) << "Failed adding cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST";
            return false;
        }
        auto beerocks_header                      = message_com::get_beerocks_header(m_cmdu_tx);
        beerocks_header->actionhdr()->direction() = beerocks::BEEROCKS_DIRECTION_CONTROLLER;

        // The add_vs_tlv method invokes the handler to add Vendor specific TLVs to the
        // WSC search message.
        if (!multi_vendor::tlvf_handler::add_vs_tlv(
                m_cmdu_tx, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE)) {
            LOG(ERROR) << "Failed adding few TLVs in AP Search Message";
        }
        return true;
    };

    if (!create_autoconfig_search()) {
        LOG(ERROR) << "Failed creating search message";
        return false;
    }
    if (db->controller_info.profile_support ==
        wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1) {
        LOG(DEBUG) << "sending autoconfig search message, bridge_mac=" << db->bridge.mac;
        return m_btl_ctx.send_cmdu_to_controller({}, m_cmdu_tx);
    } else if (db->controller_info.profile_support ==
               wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::PRPLMESH_PROFILE_UNKNOWN) {
        // If we still don't know which profile the controller supports, send 2 autoconfig search
        // messages: one without the MultiAp profile TLV and one with it.
        // We do this since we came across certified agents that don't respond to a search message
        // that contains the newly added TLV. So to make sure we will get a response send both
        // options.
        m_btl_ctx.send_cmdu_to_controller({}, m_cmdu_tx);
        if (!create_autoconfig_search()) {
            LOG(ERROR) << "Failed creating search message";
            return false;
        }
    }

    auto tlvProfile2MultiApProfile = m_cmdu_tx.addClass<wfa_map::tlvProfile2MultiApProfile>();
    if (!tlvProfile2MultiApProfile) {
        LOG(ERROR) << "addClass wfa_map::tlvProfile2MultiApProfile failed";
        return false;
    }

    if (db->device_conf.certification_mode && db->device_conf.certification_profile) {
        // If certification is enabled, override the MultiApProfile in Autoconfiguration
        // Search Message according to the certification program.
        tlvProfile2MultiApProfile->profile() = db->device_conf.certification_profile;
    }

    LOG(DEBUG) << "sending autoconfig search message, bridge_mac=" << db->bridge.mac
               << " with Profile TLV";
    return m_btl_ctx.send_cmdu_to_controller({}, m_cmdu_tx);
}

bool ApAutoConfigurationTask::send_ap_autoconfiguration_wsc_m1_message(
    const std::string &radio_iface)
{
    LOG(DEBUG) << "Sending AP_AUTOCONFIGURATION_WSC_MESSAGE with M1 " << radio_iface;
    if (!m_cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE)) {
        LOG(ERROR) << "Failed creating AP_AUTOCONFIGURATION_WSC_MESSAGE";
        return false;
    }

    auto db    = AgentDB::get();
    auto radio = db->radio(radio_iface);
    if (!radio) {
        return false;
    }

    if (!tlvf_utils::add_ap_radio_basic_capabilities(m_cmdu_tx, radio->front.iface_mac)) {
        LOG(ERROR) << "Failed adding AP Radio Basic Capabilities TLV";
        return false;
    }

    if (!add_wsc_m1_tlv(radio_iface)) {
        LOG(ERROR) << "Failed adding WSC M1 TLV";
        return false;
    }

    // If the Multi-AP Agent onboards to a Multi-AP Controller that implements Profile-1, the
    // Multi-AP Agent shall set the Byte Counter Units field to 0x00 (bytes) and report the
    // values of the BytesSent and BytesReceived fields in the Associated STA Traffic Stats TLV
    // in bytes. Section 9.1 of the spec.
    db->device_conf.byte_counter_units = wfa_map::tlvProfile2ApCapability::eByteCounterUnits::BYTES;

    if (db->controller_info.profile_support >=
        wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_2) {
        /* One Profile-2 AP Capability TLV */
        auto profile2_ap_capability_tlv = m_cmdu_tx.addClass<wfa_map::tlvProfile2ApCapability>();
        if (!profile2_ap_capability_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvProfile2ApCapability failed";
            return false;
        }

        // If a Multi-AP Agent that implements Profile-2 sends a Profile-2 AP Capability TLV
        // shall set the Byte Counter Units field to 0x01 (KiB (kibibytes)). Section 9.1 of
        // the spec.
        db->device_conf.byte_counter_units =
            wfa_map::tlvProfile2ApCapability::eByteCounterUnits::KIBIBYTES;
        profile2_ap_capability_tlv->capabilities_bit_field().byte_counter_units =
            db->device_conf.byte_counter_units;

        // Calculate max total number of VLANs which can be configured on the Agent, and
        // save it on on the AgentDB.
        db->traffic_separation.max_number_of_vlans_ids =
            db->get_radios_list().size() * eBeeRocksIfaceIds::IFACE_TOTAL_VAPS;

        profile2_ap_capability_tlv->max_total_number_of_vids() =
            db->traffic_separation.max_number_of_vlans_ids;

        /* One AP Radio Advanced Capabilities TLV */
        auto ap_radio_advanced_capabilities_tlv =
            m_cmdu_tx.addClass<wfa_map::tlvProfile2ApRadioAdvancedCapabilities>();
        if (!ap_radio_advanced_capabilities_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvProfile2ApRadioAdvancedCapabilities failed";
            return false;
        }

        ap_radio_advanced_capabilities_tlv->radio_uid() = radio->front.iface_mac;

        // Currently Set the flag as we don't support traffic separation.
        ap_radio_advanced_capabilities_tlv->advanced_radio_capabilities().combined_front_back =
            radio->front.hybrid_mode_supported;
        ap_radio_advanced_capabilities_tlv->advanced_radio_capabilities()
            .combined_profile1_and_profile2 = 0;

        // TODO: Fill in the missing fields (related to R4 specification, PPM-2327).
    }

    // The add_vs_tlv method invokes the handler to add Vendor specific TLVs to the
    // AP_AUTOCONFIGURATION_WSC_MESSAGE.
    if (!db->controller_info.prplmesh_controller) {
        if (!multi_vendor::tlvf_handler::add_vs_tlv(
                m_cmdu_tx, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE)) {
            LOG(ERROR) << "Failed adding few TLVs in AP_AUTOCONFIGURATION_WSC_MESSAGE";
        }

        LOG(INFO) << "Configured as non-prplMesh, not sending SLAVE_JOINED_NOTIFICATION";
        m_btl_ctx.send_cmdu_to_controller(radio_iface, m_cmdu_tx);
        LOG(DEBUG) << "sending WSC M1 Size=" << m_cmdu_tx.getMessageLength();
        return true;
    }

    auto notification =
        message_com::add_vs_tlv<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION>(
            m_cmdu_tx);

    if (!notification) {
        LOG(ERROR) << "Failed building cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION!";
        return false;
    }

    // Version
    string_utils::copy_string(notification->slave_version(message::VERSION_LENGTH),
                              BEEROCKS_VERSION, message::VERSION_LENGTH);

    // Platform Configuration
    auto &config = m_btl_ctx.get_agent_conf();
    notification->low_pass_filter_on() =
        config.radios.at(radio_iface).backhaul_wireless_iface_filter_low;
    notification->enable_repeater_mode() = config.radios.at(radio_iface).enable_repeater_mode;

    // Backhaul Params
    bool wireless_bh_manager =
        db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wireless &&
        radio->back.iface_name == db->backhaul.selected_iface_name;

    std::string first_radio_iface;
    auto first_radio_it = db->get_radios_list().begin();
    if (first_radio_it != db->get_radios_list().end()) {
        first_radio_iface = (*first_radio_it)->front.iface_name;
    }
    bool wired_bh_manager =
        db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wired &&
        first_radio_iface == radio_iface;

    if (db->device_conf.local_gw) {
        if (radio_iface == first_radio_iface) {
            notification->backhaul_params().is_backhaul_manager = true;
        }
    } else {
        notification->backhaul_params().is_backhaul_manager =
            wireless_bh_manager || wired_bh_manager;
    }

    if (db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wireless) {
        auto wireless_bh_radio                       = db->radio(db->backhaul.selected_iface_name);
        notification->backhaul_params().backhaul_mac = wireless_bh_radio->back.iface_mac;
        notification->backhaul_params().backhaul_channel =
            wireless_bh_radio->wifi_channel.get_channel();
        notification->backhaul_params().backhaul_iface_type = eIfaceType::IFACE_TYPE_WIFI_INTEL;
    } else if (db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wired) {
        notification->backhaul_params().backhaul_mac        = db->ethernet.wan.mac;
        notification->backhaul_params().backhaul_channel    = 0;
        notification->backhaul_params().backhaul_iface_type = eIfaceType::IFACE_TYPE_ETHERNET;
    } else { // Local GW
        notification->backhaul_params().backhaul_mac        = {};
        notification->backhaul_params().backhaul_channel    = 0;
        notification->backhaul_params().backhaul_iface_type = eIfaceType::IFACE_TYPE_GW_BRIDGE;
    }

    notification->backhaul_params().backhaul_bssid = db->backhaul.backhaul_bssid;
    notification->backhaul_params().backhaul_is_wireless =
        db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wireless;

    // Read the IP addresses of the bridge interface
    network_utils::iface_info bridge_info;
    if (beerocks::net::network_utils::get_iface_info(bridge_info, db->bridge.iface_name) != 0) {
        LOG(ERROR) << "Failed reading addresses for: " << db->bridge.iface_name;
        return false;
    }
    notification->backhaul_params().bridge_ipv4   = network_utils::ipv4_from_string(bridge_info.ip);
    notification->backhaul_params().backhaul_ipv4 = notification->backhaul_params().bridge_ipv4;

    // Platform Settings
    notification->platform_settings().client_band_steering_enabled =
        db->device_conf.client_band_steering_enabled;
    notification->platform_settings().client_optimal_path_roaming_enabled =
        db->device_conf.client_optimal_path_roaming_enabled;
    notification->platform_settings().client_optimal_path_roaming_prefer_signal_strength_enabled =
        db->device_conf.client_optimal_path_roaming_prefer_signal_strength_enabled;
    notification->platform_settings().client_11k_roaming_enabled =
        db->device_conf.client_11k_roaming_enabled;
    notification->platform_settings().load_balancing_enabled =
        db->device_conf.load_balancing_enabled;
    notification->platform_settings().service_fairness_enabled =
        db->device_conf.service_fairness_enabled;

    notification->platform_settings().local_master = db->device_conf.local_controller;

    // Wlan Settings
    notification->wlan_settings().band_enabled =
        db->device_conf.front_radio.config.at(radio_iface).band_enabled;
    notification->wlan_settings().channel =
        db->device_conf.front_radio.config.at(radio_iface).configured_channel;
    // Hostap Params
    string_utils::copy_string(notification->hostap().iface_name, radio->front.iface_name.c_str(),
                              beerocks::message::IFACE_NAME_LENGTH);
    notification->hostap().iface_mac      = radio->front.iface_mac;
    notification->hostap().ant_num        = radio->number_of_antennas;
    notification->hostap().tx_power       = radio->tx_power_dB;
    notification->hostap().frequency_band = radio->wifi_channel.get_freq_type();
    notification->hostap().max_bandwidth  = radio->max_supported_bw;
    notification->hostap().ht_supported   = radio->ht_supported;
    notification->hostap().ht_capability  = radio->ht_capability;
    std::copy_n(radio->ht_mcs_set.begin(), beerocks::message::HT_MCS_SET_SIZE,
                notification->hostap().ht_mcs_set);
    notification->hostap().vht_supported  = radio->vht_supported;
    notification->hostap().vht_capability = radio->vht_capability;
    std::copy_n(radio->vht_mcs_set.begin(), beerocks::message::VHT_MCS_SET_SIZE,
                notification->hostap().vht_mcs_set);
    notification->hostap().he_supported     = radio->he_supported;
    notification->hostap().he_capability    = radio->he_capability;
    notification->hostap().wifi6_capability = radio->wifi6_capability;
    std::copy_n(radio->he_mcs_set.begin(), beerocks::message::HE_MCS_SET_SIZE,
                notification->hostap().he_mcs_set);

    notification->hostap().ant_gain = config.radios.at(radio_iface).hostap_ant_gain;

    // Channel Selection Params
    notification->cs_params().channel   = radio->wifi_channel.get_channel();
    notification->cs_params().bandwidth = radio->wifi_channel.get_bandwidth();
    notification->cs_params().channel_ext_above_primary =
        radio->wifi_channel.get_ext_above_primary();
    if (radio->wifi_channel.get_freq_type() == eFreqType::FREQ_6G &&
        radio->wifi_channel.get_bandwidth() == eWiFiBandwidth::BANDWIDTH_160) {
        notification->cs_params().vht_center_frequency =
            radio->wifi_channel.get_center_frequency_2();
    } else {
        notification->cs_params().vht_center_frequency = radio->wifi_channel.get_center_frequency();
    }
    notification->cs_params().tx_power = radio->tx_power_dB;

    // The add_vs_tlv method invokes the handler to add Vendor specific TLVs to the
    // AP_AUTOCONFIGURATION_WSC_MESSAGE.
    if (!multi_vendor::tlvf_handler::add_vs_tlv(
            m_cmdu_tx, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE)) {
        LOG(ERROR) << "Failed adding few TLVs in AP_AUTOCONFIGURATION_WSC_MESSAGE";
    }
    m_btl_ctx.send_cmdu_to_controller(radio_iface, m_cmdu_tx);
    LOG(DEBUG) << "sending WSC M1 Size=" << m_cmdu_tx.getMessageLength();
    return true;
}

bool ApAutoConfigurationTask::add_wsc_m1_tlv(const std::string &radio_iface)
{
    auto tlv = m_cmdu_tx.addClass<ieee1905_1::tlvWsc>();
    if (tlv == nullptr) {
        LOG(ERROR) << "Error creating tlvWsc";
        return false;
    }

    // Allocate maximum allowed length for the payload, so it can accommodate variable length
    // data inside the internal TLV list.
    // On finalize(), the buffer is shrunk back to its real size.
    size_t payload_length =
        tlv->getBuffRemainingBytes() - ieee1905_1::tlvEndOfMessage::get_initial_size();
    tlv->alloc_payload(payload_length);

    WSC::m1::config cfg;
    auto db    = AgentDB::get();
    auto radio = db->radio(radio_iface);
    if (!radio) {
        LOG(ERROR) << "Cannot find radio for " << radio_iface;
        return false;
    }

    auto &radio_conf_params = m_radios_conf_params[radio_iface];

    cfg.msg_type = WSC::eWscMessageType::WSC_MSG_TYPE_M1;
    cfg.mac      = db->bridge.mac;

    radio_conf_params.dh = std::make_unique<mapf::encryption::diffie_hellman>();

    std::copy(radio_conf_params.dh->nonce(),
              radio_conf_params.dh->nonce() + radio_conf_params.dh->nonce_length(),
              cfg.enrollee_nonce);
    copy_pubkey(*radio_conf_params.dh, cfg.pub_key);
    cfg.auth_type_flags =
        WSC::eWscAuth(WSC::eWscAuth::WSC_AUTH_OPEN | WSC::eWscAuth::WSC_AUTH_WPA2PSK |
                      WSC::eWscAuth::WSC_AUTH_SAE);
    cfg.encr_type_flags = uint16_t(WSC::eWscEncr::WSC_ENCR_AES) |
                          uint16_t(WSC::eWscEncr::WSC_ENCR_TKIP) |
                          uint16_t(WSC::eWscEncr::WSC_ENCR_NONE);

    bpl::sBoardInfo board_info;
    bpl::get_board_info(board_info);

    cfg.manufacturer        = board_info.manufacturer;
    cfg.model_name          = board_info.manufacturer_model;
    cfg.serial_number       = db->device_conf.device_serial_number;
    cfg.model_number        = "18.04";
    cfg.primary_dev_type_id = WSC::WSC_DEV_NETWORK_INFRA_AP;
    cfg.device_name         = "prplmesh-agent";
    switch (radio->wifi_channel.get_freq_type()) {
    case beerocks::FREQ_24G:
        cfg.bands = WSC::WSC_RF_BAND_2GHZ;
        break;
    case beerocks::FREQ_5G:
        cfg.bands = WSC::WSC_RF_BAND_5GHZ;
        break;
    case beerocks::FREQ_24G_5G:
        cfg.bands = WSC::WSC_RF_BAND_2GHZ_5GHZ;
        break;
    case beerocks::FREQ_6G:
        cfg.bands = WSC::WSC_RF_BAND_6GHZ;
        break;
    default:
        LOG(ERROR) << "The frequency type of " << radio_iface
                   << " must be 2.4GHz, 5GHz or 6GHz. Frequency:"
                   << beerocks::utils::convert_frequency_type_to_string(
                          radio->wifi_channel.get_freq_type());
        return false;
    }
    auto attributes = WSC::m1::create(*tlv, cfg);
    if (!attributes)
        return false;

    // Authentication support - store swapped M1 for later M1 || M2* authentication
    // This is the content of M1, without the type and length.
    if (radio_conf_params.m1_auth_buf) {
        delete[] radio_conf_params.m1_auth_buf;
    }
    radio_conf_params.m1_auth_buf_len = attributes->len();
    radio_conf_params.m1_auth_buf     = new uint8_t[radio_conf_params.m1_auth_buf_len];
    std::copy_n(attributes->buffer(), radio_conf_params.m1_auth_buf_len,
                radio_conf_params.m1_auth_buf);
    return true;
}

void ApAutoConfigurationTask::handle_ap_autoconfiguration_response(
    ieee1905_1::CmduMessageRx &cmdu_rx, const sMacAddr &src_mac)
{
    auto db = AgentDB::get();
    if (db->device_conf.local_controller && src_mac != db->bridge.mac) {
        LOG(ERROR) << "[Multiple Controllers Detected] This agent has a local controller with mac="
                   << db->bridge.mac << " but response came from src_mac=" << src_mac
                   << ", ignoring";
        return;
    }
    if (db->controller_info.bridge_mac != network_utils::ZERO_MAC &&
        src_mac != db->controller_info.bridge_mac) {
        LOG(ERROR) << "[Multiple Controllers Detected] current controller_bridge_mac="
                   << db->controller_info.bridge_mac
                   << " but response came from src_mac=" << src_mac << ", ignoring";
        return;
    }

    auto tlvSupportedRole = cmdu_rx.getClass<ieee1905_1::tlvSupportedRole>();
    if (!tlvSupportedRole) {
        LOG(ERROR) << "getClass tlvSupportedRole failed";
        return;
    }

    if (tlvSupportedRole->value() != ieee1905_1::tlvSupportedRole::REGISTRAR) {
        LOG(ERROR) << "invalid tlvSupportedRole value";
        return;
    }

    auto tlvSupportedFreqBand = cmdu_rx.getClass<ieee1905_1::tlvSupportedFreqBand>();
    if (!tlvSupportedFreqBand) {
        LOG(ERROR) << "getClass tlvSupportedFreqBand failed";
        return;
    }

    std::string band_name;
    beerocks::eFreqType freq_type = beerocks::eFreqType::FREQ_UNKNOWN;
    switch (tlvSupportedFreqBand->value()) {
    case ieee1905_1::tlvSupportedFreqBand::BAND_2_4G:
        band_name = "2.4GHz";
        freq_type = beerocks::eFreqType::FREQ_24G;
        break;
    case ieee1905_1::tlvSupportedFreqBand::BAND_5G:
        band_name = "5GHz";
        freq_type = beerocks::eFreqType::FREQ_5G;
        break;
    case ieee1905_1::tlvSupportedFreqBand::BAND_6G:
        band_name = "6GHz";
        freq_type = beerocks::eFreqType::FREQ_6G;
        break;
    case ieee1905_1::tlvSupportedFreqBand::BAND_60G:
        LOG(DEBUG) << "received autoconfiguration response for 60GHz band, unsupported";
        return;
    default:
        LOG(ERROR) << "invalid tlvSupportedFreqBand value";
        return;
    }

    // We are sending the search message multiple times to support diffrent Profiles Controllers.
    // As a result we might get several responses. If we already completed the discovery return
    // right away to prevent redundant processing and log printing.
    auto discovery_status_it = m_discovery_status.find(freq_type);
    if (discovery_status_it != m_discovery_status.end() && discovery_status_it->second.completed) {
        return;
    }
    m_discovery_status[freq_type].completed = true;

    LOG(DEBUG) << "received ap_autoconfiguration response for " << band_name << " band";

    // Set prplmesh_controller to false by default. If "SLAVE_HANDSHAKE_RESPONSE" is received, mark
    // it to 'true'.
    bool prplmesh_controller = false;

    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header &&
        beerocks_header->action_op() == beerocks_message::ACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE) {
        // Mark controller as prplMesh.
        LOG(DEBUG) << "prplMesh controller: received ACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE from "
                   << src_mac;
        prplmesh_controller = true;
    } else {
        LOG(DEBUG) << "Not prplMesh controller " << src_mac;
    }

    auto tlvProfile2MultiApProfile = cmdu_rx.getClass<wfa_map::tlvProfile2MultiApProfile>();
    if (tlvProfile2MultiApProfile) {
        db->controller_info.profile_support = tlvProfile2MultiApProfile->profile();
        if (db->controller_info.profile_support ==
            wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1) {
            db->controller_info.profile_support =
                wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1_AS_OF_R4;
        }
    }

    auto controller_capability_tlv = cmdu_rx.getClass<wfa_map::tlvControllerCapability>();
    if (controller_capability_tlv) {
        if (controller_capability_tlv->flags().early_ap_capability) {
            db->controller_info.early_ap_capability = true;
        }
    }

    auto tlvSupportedService = cmdu_rx.getClass<wfa_map::tlvSupportedService>();
    if (!tlvSupportedService) {
        LOG(ERROR) << "getClass tlvSupportedService failed";
        return;
    }
    bool controller_found          = false;
    bool multi_ap_controller_found = false;
    for (int i = 0; i < tlvSupportedService->supported_service_list_length(); i++) {
        auto supportedServiceTuple = tlvSupportedService->supported_service_list(i);
        if (!std::get<0>(supportedServiceTuple)) {
            LOG(ERROR) << "Invalid tlvSupportedService";
            return;
        }
        if (std::get<1>(supportedServiceTuple) ==
            wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_CONTROLLER) {
            multi_ap_controller_found = true;
        }
        if (std::get<1>(supportedServiceTuple) ==
            wfa_map::tlvSupportedService::eSupportedService::EM_AP_CONTROLLER) {
            db->em_ap_controller_found = true;
        }
    }
    if (db->em_handle_third_party == HANDLE_THIRD_PARTY_ENABLE) {

        if (multi_ap_controller_found && (db->em_ap_controller_found)) {
            controller_found = true;
        } else {
            LOG(DEBUG) << "Detected third party vendor controller. Ignore the message.";
        }
    } else {
        controller_found = multi_ap_controller_found;
    }

    if (!controller_found) {
        LOG(WARNING)
            << "Invalid tlvSupportedService - supported service is not MULTI_AP_CONTROLLER";
        return;
    } else {
        m_btl_ctx.send_event(slave_thread::eEvent::CONTROLLER_DISCOVERED);
    }

    // Mark discovery status completed on band mentioned on the response and fill AgentDB fields.
    db->controller_info.prplmesh_controller = prplmesh_controller;
    db->controller_info.bridge_mac          = src_mac;
    m_discovery_status[freq_type].completed = true;
    LOG(DEBUG) << "controller_discovered on " << band_name
               << " band, controller bridge_mac=" << src_mac
               << ", prplmesh_controller=" << prplmesh_controller;

    // Send Early AP Capability
    if (db->controller_info.early_ap_capability &&
        !db->controller_info.early_ap_capability_report_sent) {
        db->controller_info.early_ap_capability_report_sent = true;
        m_btl_ctx.send_event(slave_thread::eEvent::CONTROLLER_EARLY_AP_CAPABILITY);
    }

    // Update the AP Manager with the Multi-AP Controller Profile
    m_btl_ctx.m_radio_managers.do_on_each_radio_manager(
        [&](slave_thread::sManagedRadio &radio_manager,
            const std::string &fronthaul_iface) -> bool {
            auto msg = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE>(m_cmdu_tx);
            if (!msg) {
                LOG(ERROR)
                    << "Failed building cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE message!";
                return false;
            }
            msg->profile() = db->controller_info.profile_support;
            m_btl_ctx.send_cmdu(radio_manager.ap_manager_fd, m_cmdu_tx);
            return true;
        });
}

void ApAutoConfigurationTask::handle_ap_autoconfiguration_wsc(ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto ruid = cmdu_rx.getClass<wfa_map::tlvApRadioIdentifier>();
    if (!ruid) {
        LOG(INFO) << "tlvApRadioIdentifier is part of M2 CMDU sent by the controller, Ignore M1 "
                     "CMDU sent by the agent";
        return;
    }

    auto db = AgentDB::get();

    auto radio = db->get_radio_by_mac(ruid->radio_uid(), AgentDB::eMacType::RADIO);
    if (!radio) {
        LOG(ERROR) << "Failed to find ruid " << ruid->radio_uid() << " in the Agent";
        return;
    }
    LOG(DEBUG) << "Received AP_AUTOCONFIGURATION_WSC_MESSAGE for iface " << radio->front.iface_name;

    if (db->em_ap_controller_found) {
        LOG(DEBUG) << "EM+ controller is found. Check for Service Status";
        if (!airties_vs_ap_autoconfiguration_wsc_parse_service_status(cmdu_rx,
                                                                      radio->front.iface_name)) {
            LOG(INFO) << "Service Status is not found in Vendor Specific TLV";
        }
    }

    std::vector<WSC::m2> m2_list;
    std::shared_ptr<WSC::m8> m8 = nullptr;
    for (auto tlv : cmdu_rx.getClassList<ieee1905_1::tlvWsc>()) {
        std::shared_ptr<WSC::m2> m2 = nullptr;
        // Manually check the TLV subtype (M2 or M8) to apply the adequate parsing
        WSC::WscAttrList::wsc_header *tlv_header =
            reinterpret_cast<WSC::WscAttrList::wsc_header *>(tlv->payload());
        if (tlv_header == nullptr) {
            LOG(ERROR) << "Can't extract WSC header";
            continue;
        }
        if (tlv_header->msg_type_value == WSC::eWscMessageType::WSC_MSG_TYPE_M2) {
            m2 = WSC::m2::parse(*tlv);
        } else if (!m8) {
            m8 = WSC::m8::parse(*tlv);
        } else {
            LOG(INFO) << "Not a valid Registration Protocol messages - Ignoring WSC CMDU";
        }
        if (!m2) {
            LOG(INFO) << "Not a valid M2 - Ignoring WSC CMDU";
            continue;
        }
        m2_list.push_back(*m2);
    }
    if (m2_list.empty()) {
        LOG(ERROR) << "No M2s present";
        return;
    }

    if (!handle_profile2_default_802dotq_settings_tlv(cmdu_rx)) {
        LOG(ERROR) << "handle_profile2_default_802dotq_settings_tlv has failed!";
        return;
    }

    std::unordered_set<std::string> misconfigured_ssids;
    // tlvProfile2TrafficSeparationPolicy is not mandatory.
    if (!cmdu_rx.getClass<wfa_map::tlvProfile2TrafficSeparationPolicy>()) {
        LOG(INFO) << "tlvProfile2TrafficSeparationPolicy not found";
    } else if (!handle_profile2_traffic_separation_policy_tlv(cmdu_rx, misconfigured_ssids)) {
        LOG(ERROR) << "handle_profile2_traffic_separation_policy_tlv has failed!";
        return;
    }

    std::vector<WSC::configData::config> configs;
    if (!handle_wsc_m2_tlv(cmdu_rx, radio->front.iface_name, m2_list, configs,
                           misconfigured_ssids)) {
        LOG(ERROR) << "handle_wsc_m2_tlv has failed!";
        return;
    }
    if (m8 && !handle_wsc_m8_tlv(radio->front.iface_name, m8, configs)) {
        LOG(ERROR) << "handle_wsc_m8_tlv has failed!";
        return;
    }
    if (!handle_agent_ap_mld_configuration_tlv(cmdu_rx, configs)) {
        LOG(ERROR) << "handle_agent_ap_mld_configuration_tlv has failed!";
        return;
    }

    if (db->device_conf.management_mode != BPL_MGMT_MODE_NOT_MULTIAP) {
        validate_reconfiguration(radio->front.iface_name, configs);
        if (!configs.empty()) {
            // Update the BSS credentials if a backhaul link for this radio already exists
            // or add a new one otherwise.
            auto it =
                find_if(db->backhaul.backhaul_links.begin(), db->backhaul.backhaul_links.end(),
                        [&](const AgentDB::sBackhaul::sBackhaulLink &c) {
                            return c.iface_name == radio->front.iface_name;
                        });
            if (it != db->backhaul.backhaul_links.end()) {
                LOG(DEBUG) << "Updating credentials for backhaul interface with type="
                           << int(it->connection_type) << ", iface_name=" << it->iface_name
                           << ", iface_mac=" << it->iface_mac;
                it->credentials = configs;
            } else {
                LOG(DEBUG) << "Storing backhaul credentials for new interface: "
                           << radio->front.iface_name << ", iface_mac=" << radio->front.iface_mac;
                db->backhaul.backhaul_links.emplace_back(
                    AgentDB::sBackhaul::eConnectionType::Wireless, radio->front.iface_name,
                    radio->front.iface_mac, configs);
            }

            if (m8) {
                auto bSTA_it = std::find_if(
                    configs.begin(), configs.end(), [&](const WSC::configData::config &config) {
                        return ((config.bss_type ==
                                 static_cast<uint8_t>(
                                     WSC::eWscVendorExtSubelementBssType::BACKHAUL_STA)));
                    });

                if (bSTA_it == configs.end()) {
                    return;
                }
                send_bsta_configuration(radio->front.iface_mac, *bSTA_it);
                configs.erase(bSTA_it);
            } else if (db->controller_info.early_ap_capability) {
                send_enable_disable_endpoint(radio->front.iface_mac, false, true);
            }
            send_ap_bss_configuration_message(radio->front.iface_name, configs);
        } else {
            LOG(INFO) << "Reconfiguration is not needed";
        }
    } else {
        LOG(WARNING) << "non-EasyMesh mode - skip updating VAP credentials";
    }

    if (!handle_ap_autoconfiguration_wsc_vs_extension_tlv(cmdu_rx, radio->front.iface_name)) {
        LOG(ERROR) << "handle_ap_autoconfiguration_wsc_vs_extension_tlv has failed";
        return;
    }

    // Initialize for next state
    auto &radio_conf_params = m_radios_conf_params[radio->front.iface_name];

    radio_conf_params.num_of_bss_available = 0;
    radio_conf_params.enabled_bssids.clear();
    radio_conf_params.sent_vaps_list_update     = false;
    radio_conf_params.received_vaps_list_update = false;

    if (db->device_conf.management_mode != BPL_MGMT_MODE_NOT_MULTIAP) {
        FSM_MOVE_STATE(radio->front.iface_name, eState::WAIT_AP_CONFIGURATION_COMPLETE);
        return;
    }

    // MODE is NOT_MULTIAP
    FSM_MOVE_STATE(radio->front.iface_name, eState::CONFIGURED);
    return;
}

void ApAutoConfigurationTask::handle_ap_autoconfiguration_wsc_renew(
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    LOG(INFO) << "received autoconfig renew message";

    if (!m_btl_ctx.link_to_controller()) {
        LOG(INFO) << "No link to Multi-AP Controller, ignoring renew.";
        return;
    }

    auto tlvAlMac = cmdu_rx.getClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMac) {
        LOG(ERROR) << "tlvAlMac missing - ignoring autconfig renew message";
        return;
    }

    const auto &src_mac = tlvAlMac->mac();
    LOG(DEBUG) << "AP-Autoconfiguration Renew Message from Controller " << src_mac;
    auto db = AgentDB::get();
    if (src_mac != db->controller_info.bridge_mac) {
        LOG(ERROR) << "[Multiple Controllers Detected] Ignoring AP-Autoconfiguration Renew Message "
                      "from an unknown Controller: "
                   << src_mac;
        return;
    }

    auto tlvSupportedRole = cmdu_rx.getClass<ieee1905_1::tlvSupportedRole>();
    if (!tlvSupportedRole) {
        LOG(ERROR) << "tlvSupportedRole missing - ignoring autconfig renew message";
        return;
    }

    LOG(DEBUG) << "tlvSupportedRole->value()=" << int(tlvSupportedRole->value());
    if (tlvSupportedRole->value() != ieee1905_1::tlvSupportedRole::REGISTRAR) {
        LOG(ERROR) << "invalid tlvSupportedRole value, supporting only REGISTRAR controllers";
        return;
    }

    // Not reading tlvSupportedFreqBand since the Multi-AP Specification Version 2.0 section 7.1
    // defines that should reply with AP-Autoconfiguration WSC message "for each of its radios,
    // irrespective of the value specified in the SupportedFreqBand TLV.
    auto tlvSupportedFreqBand = cmdu_rx.getClass<ieee1905_1::tlvSupportedFreqBand>();
    if (!tlvSupportedFreqBand) {
        LOG(ERROR) << "tlvSupportedFreqBand missing - ignoring autoconfig renew message";
        return;
    }

    for (const auto radio : db->get_radios_list()) {
        if (!radio) {
            return;
        }

        m_task_is_active = true;
        FSM_MOVE_STATE(radio->front.iface_name, eState::SEND_AP_AUTOCONFIGURATION_WSC_M1);
    }
}

void ApAutoConfigurationTask::handle_multi_ap_policy_config_request(
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE, mid=" << std::hex << int(mid);

    // send ACK_MESSAGE back to the controller
    if (!m_cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return;
    }
    LOG(DEBUG) << "Sending ACK message to the originator, mid=" << std::hex << mid;
    m_btl_ctx.send_cmdu_to_controller({}, m_cmdu_tx);

    /** Traffic Separation Policy **/
    if (!handle_profile2_default_802dotq_settings_tlv(cmdu_rx)) {
        LOG(ERROR) << "handle_profile2_default_802dotq_settings_tlv has failed!";
        return;
    }

    std::unordered_set<std::string> misconfigured_ssids;
    auto db = AgentDB::get();
    // tlvProfile2TrafficSeparationPolicy is not mandatory. But if it does not exist, need to clear
    // traffic separation settings.
    if (!cmdu_rx.getClass<wfa_map::tlvProfile2TrafficSeparationPolicy>()) {
        LOG(INFO) << "tlvProfile2TrafficSeparationPolicy not found";
        db->traffic_separation.ssid_vid_mapping.clear();
    } else if (!handle_profile2_traffic_separation_policy_tlv(cmdu_rx, misconfigured_ssids)) {
        LOG(ERROR) << "handle_profile2_traffic_separation_policy_tlv has failed!";
        return;
    }

    if (db->traffic_separation.ssid_vid_mapping.empty()) {
        // If SSID VID map is empty, need to clear traffic separation policy.
        db->traffic_separation.primary_vlan_id = 0;
        db->traffic_separation.default_pcp     = 0;
    }

    std::vector<std::pair<wfa_map::tlvProfile2ErrorCode::eReasonCode, sMacAddr>> bss_errors;
    if (!misconfigured_ssids.empty()) {
        bss_errors.push_back({wfa_map::tlvProfile2ErrorCode::eReasonCode::
                                  NUMBER_OF_UNIQUE_VLAN_ID_EXCEEDS_MAXIMUM_SUPPORTED,
                              sMacAddr()});
    }

    if (bss_errors.size()) {
        if (!send_error_response_message(bss_errors)) {
            LOG(ERROR) << "send_error_response_message has failed";
            return;
        }
        return;
    }

    /** Steering Policy **/
    auto steering_policy_tlv = cmdu_rx.getClass<wfa_map::tlvSteeringPolicy>();
    if (steering_policy_tlv) {
        //BTM Steering Disallowed list
        std::unordered_set<sMacAddr> new_disallowed_stas;
        for (size_t i = 0; i < steering_policy_tlv->btm_steering_disallowed_sta_list_length();
             i++) {
            auto tuple = steering_policy_tlv->btm_steering_disallowed_sta_list(i);
            if (!std::get<0>(tuple)) {
                LOG(ERROR) << "Failed to get btm_steering_disallowed_sta[" << i
                           << "] from TLV_STEERING_POLICY";
                return;
            }
            new_disallowed_stas.insert(std::get<1>(tuple));
        }
        db->steering_policy.btm_steering_disallowed = std::move(new_disallowed_stas);
    }

    /** Link Metrics Policy **/
    auto metric_reporting_policy_tlv = cmdu_rx.getClass<wfa_map::tlvMetricReportingPolicy>();
    if (metric_reporting_policy_tlv) {
        for (size_t i = 0; i < metric_reporting_policy_tlv->metrics_reporting_conf_list_length();
             i++) {
            auto tuple = metric_reporting_policy_tlv->metrics_reporting_conf_list(i);
            if (!std::get<0>(tuple)) {
                LOG(ERROR) << "Failed to get metrics_reporting_conf[" << i
                           << "] from TLV_METRIC_REPORTING_POLICY";
                return;
            }

            auto metrics_reporting_conf = std::get<1>(tuple);

            auto radio =
                db->get_radio_by_mac(metrics_reporting_conf.radio_uid, AgentDB::eMacType::RADIO);
            if (!radio) {
                LOG(ERROR) << "RUID not found " << metrics_reporting_conf.radio_uid;
                continue;
            }

            auto monitor_fd = m_btl_ctx.get_monitor_fd(radio->front.iface_name);

            /**
             * The Agent forwards the request message again "as is" to the monitor thread.
             */
            if (monitor_fd == beerocks::net::FileDescriptor::invalid_descriptor) {
                LOG(ERROR) << "monitor_socket is null";
                return;
            }

            if (!m_btl_ctx.forward_cmdu_to_uds(monitor_fd, cmdu_rx)) {
                LOG(ERROR) << "Failed to forward message to monitor";
                return;
            }
        }

        /**
         * The AP Metrics Reporting Interval field indicates if periodic AP metrics reporting is
         * to be enabled, and if so the cadence.
         */
        db->link_metrics_policy.reporting_interval_sec =
            metric_reporting_policy_tlv->metrics_reporting_interval_sec();

        /**
         * Notify the link metrics collection task that the metric reporting policy has been
         * updated. It will start or update a timer for periodic reporting if necessary.
         */
        m_btl_ctx.task_pool_send_event(
            eTaskType::LINK_METRICS_COLLECTION,
            LinkMetricsCollectionTask::eEvent::METRIC_REPORTING_POLICY_UPDATED);
    }

    /** Channel Scan Policy **/
    auto channel_scan_reporting_policy = cmdu_rx.getClass<wfa_map::tlvChannelScanReportingPolicy>();
    if (channel_scan_reporting_policy) {
        db->link_metrics_policy.report_independent_channel_scans =
            (channel_scan_reporting_policy->report_independent_channel_scans() ==
             channel_scan_reporting_policy->REPORT_INDEPENDENT_CHANNEL_SCANS);
    }

    /** Unsuccessful Association Policy **/
    const auto unsuccessful_association_policy_tlv =
        cmdu_rx.getClass<wfa_map::tlvProfile2UnsuccessfulAssociationPolicy>();
    if (unsuccessful_association_policy_tlv) {
        // Set enable/disable
        db->link_metrics_policy.report_unsuccessful_associations =
            unsuccessful_association_policy_tlv->report_unsuccessful_associations().report;

        // Set reporting rate
        db->link_metrics_policy.failed_associations_maximum_reporting_rate =
            unsuccessful_association_policy_tlv->maximum_reporting_rate();

        // Reset internal counters
        db->link_metrics_policy.number_of_reports_in_last_minute = 0;
        db->link_metrics_policy.failed_association_last_reporting_time_point =
            std::chrono::steady_clock::time_point::min();

        LOG(DEBUG) << "Unsuccessful Association Policy tlv found, mid: " << mid
                   << "\n Report: " << db->link_metrics_policy.report_unsuccessful_associations
                   << "; maximum reporting rate: "
                   << db->link_metrics_policy.failed_associations_maximum_reporting_rate;
    }

    for (const auto &radios_conf_param_kv : m_radios_conf_params) {
        const auto &radio_iface = radios_conf_param_kv.first;
        const auto &conf_params = radios_conf_param_kv.second;

        if (conf_params.state == eState::CONFIGURED) {
            m_traffic_separation_configurator->apply_policy(radio_iface);
        } else {
            LOG(WARNING) << "autoconfiguration procedure is not completed yet, traffic separation "
                         << "policy cannot be applied";
        }
    }

    return;
}

bool ApAutoConfigurationTask::handle_profile2_default_802dotq_settings_tlv(
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto db = AgentDB::get();

    auto dot1q_settings = cmdu_rx.getClass<wfa_map::tlvProfile2Default802dotQSettings>();
    // tlvProfile2Default802dotQSettings is not mandatory.
    if (!dot1q_settings) {
        LOG(INFO) << "No tlvProfile2Default802dotQSettings";
        return true;
    }

    LOG(DEBUG) << "Primary VLAN ID: " << dot1q_settings->primary_vlan_id()
               << ", PCP: " << dot1q_settings->default_pcp();

    db->traffic_separation.primary_vlan_id = dot1q_settings->primary_vlan_id();
    db->traffic_separation.default_pcp     = dot1q_settings->default_pcp();

    for (auto radio : db->get_radios_list()) {
        auto pvid_set_request = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST>(m_cmdu_tx);
        if (!pvid_set_request) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        pvid_set_request->primary_vlan_id() = dot1q_settings->primary_vlan_id();

        // Send ACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST.
        auto ap_manager_fd = m_btl_ctx.get_ap_manager_fd(radio->front.iface_name);
        m_btl_ctx.send_cmdu(ap_manager_fd, m_cmdu_tx);
    }

    return true;
}

bool ApAutoConfigurationTask::handle_profile2_traffic_separation_policy_tlv(
    ieee1905_1::CmduMessageRx &cmdu_rx, std::unordered_set<std::string> &misconfigured_ssids)
{
    auto traffic_seperation_policy =
        cmdu_rx.getClass<wfa_map::tlvProfile2TrafficSeparationPolicy>();

    if (!traffic_seperation_policy) {
        LOG(ERROR) << "tlvProfile2TrafficSeparationPolicy not found!";
        return false;
    }

    auto db = AgentDB::get();

    std::unordered_map<std::string, uint16_t> tmp_ssid_vid_mapping;
    for (int i = 0; i < traffic_seperation_policy->ssids_vlan_id_list_length(); i++) {
        auto ssid_vid_tuple = traffic_seperation_policy->ssids_vlan_id_list(i);
        if (!std::get<0>(ssid_vid_tuple)) {
            LOG(ERROR) << "Failed to get ssid_vid mapping, idx=" << i;
            return false;
        }
        auto &ssid_vid_mapping = std::get<1>(ssid_vid_tuple);

        tmp_ssid_vid_mapping[ssid_vid_mapping.ssid_name_str()] = ssid_vid_mapping.vlan_id();
        LOG(DEBUG) << "SSID: " << ssid_vid_mapping.ssid_name_str()
                   << ", VID: " << ssid_vid_mapping.vlan_id();
    }

    // Overwriting the whole container instead of pushing one by one, since we need to remove
    // old configuration from previous configurations messages.
    db->traffic_separation.ssid_vid_mapping = tmp_ssid_vid_mapping;

    // Fill secondary VLANs IDs to the database.
    for (const auto &ssid_vid_pair : db->traffic_separation.ssid_vid_mapping) {
        auto vlan_id = ssid_vid_pair.second;
        if (vlan_id != db->traffic_separation.primary_vlan_id) {
            db->traffic_separation.secondary_vlans_ids.insert(vlan_id);
        }
    }

    // Erase excessive secondary VIDs.
    if (db->traffic_separation.ssid_vid_mapping.size() >
        db->traffic_separation.max_number_of_vlans_ids) {

        for (auto it = std::next(db->traffic_separation.ssid_vid_mapping.begin(),
                                 db->traffic_separation.max_number_of_vlans_ids);
             it != db->traffic_separation.ssid_vid_mapping.end();) {

            auto &ssid = it->first;
            misconfigured_ssids.insert(ssid);
            it = db->traffic_separation.ssid_vid_mapping.erase(it);
        }
    }
    return true;
}

bool ApAutoConfigurationTask::handle_wsc_m2_tlv(
    ieee1905_1::CmduMessageRx &cmdu_rx, const std::string &radio_iface,
    const std::vector<WSC::m2> &m2_list, std::vector<WSC::configData::config> &configs,
    std::unordered_set<std::string> &misconfigured_ssids)
{
    auto db    = AgentDB::get();
    auto radio = db->radio(radio_iface);
    std::vector<std::pair<wfa_map::tlvProfile2ErrorCode::eReasonCode, sMacAddr>> bss_errors;
    for (auto m2 : m2_list) {
        LOG(DEBUG) << "M2 Parse " << m2.manufacturer()
                   << " Controller configuration (WSC M2 Encrypted Settings)";
        uint8_t authkey[32];
        uint8_t keywrapkey[16];
        LOG(DEBUG) << "M2 Parse: calculate keys";
        if (!ap_autoconfiguration_wsc_calculate_keys(radio->front.iface_name, m2.public_key(),
                                                     m2.registrar_nonce(), authkey, keywrapkey))
            return false;

        if (!ap_autoconfiguration_wsc_authenticate(radio->front.iface_name, m2, authkey,
                                                   reinterpret_cast<uint8_t *>(m2.authenticator())))
            return false;

        WSC::configData::config config;
        if (!ap_autoconfiguration_wsc_parse_encrypted_settings(m2.encrypted_settings(), authkey,
                                                               keywrapkey, config)) {
            LOG(ERROR) << "Invalid config data, skip it";
            return false;
        }
        if (db->em_ap_controller_found) {
            LOG(DEBUG) << "EM+ controller is found. Check for Hidden SSID parameters";
            if (!airties_vs_ap_autoconfiguration_wsc_parse_hidden_ssid(m2, config)) {
                LOG(INFO) << "Hidden SSID parameter not found in Vendor Extension";
            }
        }

        bool bSTA = bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_STA);
        bool fBSS = bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS);
        bool bBSS = bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS);
        bool bBSS_p1_disallowed =
            bool(config.bss_type &
                 WSC::eWscVendorExtSubelementBssType::PROFILE1_BACKHAUL_STA_ASSOCIATION_DISALLOWED);
        bool bBSS_p2_disallowed =
            bool(config.bss_type &
                 WSC::eWscVendorExtSubelementBssType::PROFILE2_BACKHAUL_STA_ASSOCIATION_DISALLOWED);
        bool teardown = bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN);

        std::stringstream ss;
        ss << "Parsed config data: " << std::endl;
        ss << "bssid: " << config.bssid << std::endl;
        ss << "ssid: " << config.ssid << std::endl;
        ss << "fBSS: " << fBSS << std::endl;
        ss << "bBSS: " << bBSS << std::endl;
        ss << "teardown: " << teardown << std::endl;
        ss << "hidden_ssid " << config.hidden_ssid << std::endl;
        if (bBSS) {
            ss << "profile1_backhaul_sta_association_disallowed: " << bBSS_p1_disallowed;
            ss << "profile2_backhaul_sta_association_disallowed: " << bBSS_p2_disallowed;
        }

        LOG(DEBUG) << m2.manufacturer() << " " << ss.str();

        if (teardown) {
            LOG(DEBUG) << "BSSID: " << config.bssid << " is flagged for teardown!";
            configs.push_back(config);
            continue;
        }

        // TODO - revisit this in the future
        // In practice, some controllers simply send an empty config data when asked for tear down,
        // so tear down the radio if the SSID is empty.
        if (config.ssid.empty()) {
            LOG(INFO) << "Empty config data, tear down radio";
            config.bss_type = WSC::eWscVendorExtSubelementBssType::TEARDOWN;
            configs.push_back(config);
            continue;
        }

        // BACKHAUL_STA bit is not expected to be set
        if (bSTA) {
            LOG(WARNING) << "Unexpected backhaul STA bit";
        }

        if (misconfigured_ssids.find(config.ssid) != misconfigured_ssids.end()) {
            LOG(WARNING) << "Controller configured VLANs more than maximum supported";
            bss_errors.push_back({wfa_map::tlvProfile2ErrorCode::eReasonCode::
                                      NUMBER_OF_UNIQUE_VLAN_ID_EXCEEDS_MAXIMUM_SUPPORTED,
                                  config.bssid});

            // Multi-AP standard requires to tear down any misconfigured BSS.
            config.bss_type = WSC::eWscVendorExtSubelementBssType::TEARDOWN;
        } else if (fBSS && bBSS && !radio->front.hybrid_mode_supported) {
            LOG(WARNING) << "Controller configured hybrid mode, but it is not supported!";
            bss_errors.push_back(
                {wfa_map::tlvProfile2ErrorCode::eReasonCode::
                     TRAFFIC_SEPARATION_ON_COMBINED_FRONTHAUL_AND_PROFILE1_BACKHAUL_UNSUPPORTED,
                 config.bssid});

            // Multi-AP standard requires to tear down any misconfigured BSS.
            config.bss_type = WSC::eWscVendorExtSubelementBssType::TEARDOWN;
        } else if (db->controller_info.profile_support !=
                       wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::MULTIAP_PROFILE_1 &&
                   bBSS && !bBSS_p1_disallowed && !bBSS_p2_disallowed) {

            LOG(WARNING) << "Controller configured Backhaul BSS for combined Profile1 and "
                         << "Profile2, but it is not supported!";
            /**
             * We currently do not support bBSS with both profile 1/2 disallow flags set to false
             * (Combined Profile bBSS mode).
             * When we are configured in a way we don't support, we should tear down the BSS, and
             * send an error response on that BSS.
             * Currently R2 certified controllers (Mediatek/Marvel) have a bug (PPM-1389) that ends
             * up sending M2 with both profile 1/2 disallow flags set to false although we report 
             * combined_profile1_and_profile2 = 0 in ap_radio_advanced_capabilities_tlv.
             * To deal with it, we using
             * TrafficSeparation::m_profile_x_disallow_override_unsupported_configuration integer
             * originated in the beerocks_agent.conf to resolve the conflict with predefined
             * value. PPM-1389.
             */
            if (TrafficSeparation::m_profile_x_disallow_override_unsupported_configuration == 0) {
                LOG(WARNING) << "Sending error and Tearing down BSS that controller configured to "
                             << config.ssid;
                bss_errors.push_back(
                    {wfa_map::tlvProfile2ErrorCode::eReasonCode::
                         TRAFFIC_SEPARATION_ON_COMBINED_PROFILE1_BACKHAUL_AND_PROFILE2_BACKHAUL_UNSUPPORTED,
                     config.bssid});

                // Multi-AP standard requires to tear down any misconfigured BSS.
                config.bss_type = WSC::eWscVendorExtSubelementBssType::TEARDOWN;
                continue;
            } else if (TrafficSeparation::m_profile_x_disallow_override_unsupported_configuration ==
                       1) {
                config.bss_type |= WSC::eWscVendorExtSubelementBssType::
                    PROFILE1_BACKHAUL_STA_ASSOCIATION_DISALLOWED;
            }
            // TrafficSeparation::m_profile_x_disallow_override_unsupported_configuration == 2
            else {
                config.bss_type |= WSC::eWscVendorExtSubelementBssType::
                    PROFILE2_BACKHAUL_STA_ASSOCIATION_DISALLOWED;
            }
            LOG(DEBUG)
                << "Override unsupported 'profile disallow' configuration and disallow profile "
                << TrafficSeparation::m_profile_x_disallow_override_unsupported_configuration;
        }

        configs.push_back(config);
    }

    LOG(INFO) << "Finished M2 parsing with " << configs.size() << " vaps and " << bss_errors.size()
              << " errors.";

    if (bss_errors.size()) {
        if (!send_error_response_message(bss_errors)) {
            LOG(ERROR) << "send_error_response_message has failed";
            return false;
        }
    }

    return true;
}

// Zero or one WSC TLV (containing M8)
bool ApAutoConfigurationTask::handle_wsc_m8_tlv(const std::string &radio_iface,
                                                std::shared_ptr<WSC::m8> m8,
                                                std::vector<WSC::configData::config> &configs)
{
    auto db    = AgentDB::get();
    auto radio = db->radio(radio_iface);
    uint8_t authkey[32];
    uint8_t keywrapkey[16];
    LOG(DEBUG) << "M8 Parse: calculate keys";
    if (!ap_autoconfiguration_wsc_calculate_keys(radio->front.iface_name, m8->public_key(),
                                                 m8->registrar_nonce(), authkey, keywrapkey))
        return false;

    if (!ap_autoconfiguration_wsc_authenticate(radio->front.iface_name, *m8, authkey,
                                               reinterpret_cast<uint8_t *>(m8->authenticator())))
        return false;

    WSC::configData::config config;
    if (!ap_autoconfiguration_wsc_parse_encrypted_settings(m8->encrypted_settings(), authkey,
                                                           keywrapkey, config)) {
        LOG(ERROR) << "Invalid config data, skip it";
        return false;
    }

    if (!(config.bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_STA)) {
        LOG(ERROR) << "Invalid config data, not a bSTA one";
        return false;
    }

    std::stringstream ss;
    ss << "Parsed bSTA config data: " << std::endl;
    ss << "bssid: " << config.bssid << std::endl;
    ss << "ssid: " << config.ssid << std::endl;
    LOG(INFO) << ss.str();
    configs.push_back(config);

    LOG(INFO) << "Finished M8 parsing";

    return true;
}

bool ApAutoConfigurationTask::handle_agent_ap_mld_configuration_tlv(
    ieee1905_1::CmduMessageRx &cmdu_rx, std::vector<WSC::configData::config> &configs)
{
    auto db(AgentDB::get());
    db->ap_mld_configurations.clear();

    auto agent_ap_mld_configuration(cmdu_rx.getClass<wfa_map::tlvAgentApMldConfiguration>());
    if (!agent_ap_mld_configuration) {
        LOG(DEBUG) << "No tlvAgentApMldConfiguration TLV received";
        return true;
    }

    for (uint8_t ap_mld_it = 0; ap_mld_it < agent_ap_mld_configuration->num_ap_mld(); ++ap_mld_it) {

        std::tuple<bool, wfa_map::cApMld &> ap_mld_tuple(
            agent_ap_mld_configuration->ap_mld(ap_mld_it));
        if (!std::get<0>(ap_mld_tuple)) {
            LOG(ERROR) << "Couldn't get AP MLD from tlvAgentApMldConfiguration";
            return false;
        }
        wfa_map::cApMld &ap_mld = std::get<1>(ap_mld_tuple);

        std::string ssid(ap_mld.ssid_str());
        if (ssid.empty()) {
            LOG(ERROR) << "SSID is empty in tlvAgentApMldConfiguration";
            return false;
        }

        // Find existing MLD Config
        AgentDB::sAPMLDConfiguration *current_ap_mld_conf = nullptr;
        for (auto &ap_mld_conf : db->ap_mld_configurations) {
            if (ssid == ap_mld_conf.mld_config.mld_ssid) {
                LOG(DEBUG) << "AP MLD configuration already exists for SSID " << ssid;
                current_ap_mld_conf = &ap_mld_conf;
                break;
            }
        }
        // Insert new MLD Config
        if (!current_ap_mld_conf) {
            db->ap_mld_configurations.push_back(AgentDB::sAPMLDConfiguration());
            current_ap_mld_conf                      = &(db->ap_mld_configurations.back());
            current_ap_mld_conf->mld_config.mld_ssid = ssid;
        }
        // Find new MLD Unit
        if (current_ap_mld_conf->mld_config.mld_unit == -1) {
            std::unordered_set<int8_t> used_mld_units;
            if (db->bsta_mld_configuration) {
                used_mld_units.insert(db->bsta_mld_configuration->mld_config.mld_unit);
            }
            for (auto ap_mld_conf : db->ap_mld_configurations) {
                used_mld_units.insert(ap_mld_conf.mld_config.mld_unit);
            }
            for (int8_t mld_unit = 0; mld_unit < db->max_mlds; ++mld_unit) {
                if (used_mld_units.find(mld_unit) == used_mld_units.end()) {
                    current_ap_mld_conf->mld_config.mld_unit = mld_unit;
                    LOG(DEBUG) << "MLD Unit " << mld_unit << " has been assigned to AP MLD "
                               << ssid;
                    break;
                }
            }
        }

        if (ap_mld.modes().str) {
            current_ap_mld_conf->mld_config.mld_mode = AgentDB::sMLDConfiguration::mode(
                current_ap_mld_conf->mld_config.mld_mode | AgentDB::sMLDConfiguration::mode::STR);
        }
        if (ap_mld.modes().nstr) {
            current_ap_mld_conf->mld_config.mld_mode = AgentDB::sMLDConfiguration::mode(
                current_ap_mld_conf->mld_config.mld_mode | AgentDB::sMLDConfiguration::mode::NSTR);
        }
        if (ap_mld.modes().emlsr) {
            current_ap_mld_conf->mld_config.mld_mode = AgentDB::sMLDConfiguration::mode(
                current_ap_mld_conf->mld_config.mld_mode | AgentDB::sMLDConfiguration::mode::EMLSR);
        }
        if (ap_mld.modes().emlmr) {
            current_ap_mld_conf->mld_config.mld_mode = AgentDB::sMLDConfiguration::mode(
                current_ap_mld_conf->mld_config.mld_mode | AgentDB::sMLDConfiguration::mode::EMLMR);
        }

        LOG(DEBUG) << "Storing MLD configuration for BSta MLD " << ssid
                   << ": [MLD_Unit=" << current_ap_mld_conf->mld_config.mld_unit
                   << ", MLD_Mode=" << std::hex << current_ap_mld_conf->mld_config.mld_mode << "]";

        current_ap_mld_conf->affiliated_aps.clear();
        for (uint8_t affiliated_ap_it = 0; affiliated_ap_it < ap_mld.num_affiliated_ap();
             ++affiliated_ap_it) {
            std::tuple<bool, wfa_map::cAffiliatedAp &> affiliated_ap_tuple(
                ap_mld.affiliated_ap(affiliated_ap_it));
            if (!std::get<0>(affiliated_ap_tuple)) {
                LOG(ERROR) << "Couldn't get Affiliated AP from APMLD SSID : " << ssid;
                return false;
            }

            AgentDB::sAPMLDConfiguration::sAffiliatedAP affiliated_conf = {};
            affiliated_conf.ruid = std::get<1>(affiliated_ap_tuple).ruid();
            current_ap_mld_conf->affiliated_aps.push_back(affiliated_conf);
        }

        for (auto &config : configs) {
            if (config.ssid == ssid) {
                config.mld_id = current_ap_mld_conf->mld_config.mld_unit;
            }
        }
    }

    return true;
}

bool ApAutoConfigurationTask::send_bsta_configuration(const sMacAddr &radio_mac,
                                                      const WSC::configData::config &config)
{
    // Place holder
    return true;
}

bool ApAutoConfigurationTask::send_enable_disable_endpoint(const sMacAddr &radio_mac,
                                                           const bool enable, const bool force)
{
    // Place holder
    return true;
}

void ApAutoConfigurationTask::handle_vs_wifi_credentials_update_response(
    ieee1905_1::CmduMessageRx &cmdu_rx, int fd, std::shared_ptr<beerocks_header> beerocks_header)
{
    auto response =
        beerocks_header
            ->addClass<beerocks_message::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE>();
    if (!response) {
        LOG(ERROR) << "addClass cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE failed";
        return;
    }

    auto radio_iface        = m_btl_ctx.m_radio_managers.get_radio_iface_from_fd(fd);
    auto &radio_conf_params = m_radios_conf_params[radio_iface];

    radio_conf_params.num_of_bss_available = response->number_of_bss_available();

    auto db    = AgentDB::get();
    auto radio = db->radio(radio_iface);
    if (!radio) {
        return;
    }

    LOG(TRACE) << "received ACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE from "
               << radio->front.iface_name;

    // This value have an arbitrary value based on observation on real platform behavior.
    constexpr uint8_t WAIT_AP_ENABLED_TIMEOUT_SECONDS = 20;
    radio_conf_params.timeout =
        std::chrono::steady_clock::now() + std::chrono::seconds(WAIT_AP_ENABLED_TIMEOUT_SECONDS);
}

void ApAutoConfigurationTask::handle_vs_ap_enabled_notification(
    ieee1905_1::CmduMessageRx &cmdu_rx, int fd, std::shared_ptr<beerocks_header> beerocks_header)
{
    auto notification_in =
        beerocks_header
            ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION>();
    if (!notification_in) {
        LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION failed";
        return;
    }

    auto radio_iface = m_btl_ctx.m_radio_managers.get_radio_iface_from_fd(fd);

    auto db    = AgentDB::get();
    auto radio = db->radio(radio_iface);
    if (!radio) {
        return;
    }

    LOG(INFO) << "received ACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION "
              << notification_in->vap_info().iface_name;

    const auto &vap_info = notification_in->vap_info();
    auto bssid           = std::find_if(radio->front.bssids.begin(), radio->front.bssids.end(),
                              [&vap_info](const beerocks::AgentDB::sRadio::sFront::sBssid &bssid) {
                                  return bssid.mac == vap_info.mac;
                              });
    if (bssid == radio->front.bssids.end()) {
        LOG(ERROR) << "Radio does not contain BSSID: " << vap_info.mac;
        return;
    }

    // Update VAP info (BSSID) in the AgentDB
    bssid->ssid          = vap_info.ssid;
    bssid->fronthaul_bss = vap_info.fronthaul_vap;
    bssid->backhaul_bss  = vap_info.backhaul_vap;
    if (vap_info.backhaul_vap) {
        bssid->backhaul_bss_disallow_profile1_agent_association =
            vap_info.profile1_backhaul_sta_association_disallowed;
        bssid->backhaul_bss_disallow_profile2_agent_association =
            vap_info.profile2_backhaul_sta_association_disallowed;
    }

    auto notification_out = message_com::create_vs_message<
        beerocks_message::cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION>(m_cmdu_tx);
    if (!notification_out) {
        LOG(ERROR) << "Failed building message!";
        return;
    }

    notification_out->vap_id()   = notification_in->vap_id();
    notification_out->vap_info() = notification_in->vap_info();
    m_btl_ctx.send_cmdu_to_controller(radio->front.iface_name, m_cmdu_tx);

    // Marked BSSID as "enabled".
    auto &radio_conf_params = m_radios_conf_params[radio_iface];
    radio_conf_params.enabled_bssids.insert(bssid->mac);
}

void ApAutoConfigurationTask::handle_vs_vaps_list_update_notification(
    ieee1905_1::CmduMessageRx &cmdu_rx, int fd, std::shared_ptr<beerocks_header> beerocks_header)
{
    auto notification_in =
        beerocks_header
            ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>();
    if (!notification_in) {
        LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION failed";
        return;
    }

    auto radio_iface = m_btl_ctx.m_radio_managers.get_radio_iface_from_fd(fd);
    auto db          = AgentDB::get();
    auto radio       = db->radio(radio_iface);
    if (!radio) {
        return;
    }

    const auto &fronthaul_iface = radio->front.iface_name;

    auto &radio_conf_params = m_radios_conf_params[radio_iface];
    LOG(INFO) << "received ACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION "
              << fronthaul_iface << " waiting for it? " << radio_conf_params.sent_vaps_list_update;

    m_btl_ctx.update_vaps_info(fronthaul_iface, notification_in->params().vaps);

    auto notification_out = message_com::create_vs_message<
        beerocks_message::cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>(m_cmdu_tx);
    if (!notification_out) {
        LOG(ERROR) << "Failed building message!";
        return;
    }

    notification_out->params() = notification_in->params();
    LOG(TRACE) << "handle ACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION " << fronthaul_iface;
    m_btl_ctx.send_cmdu_to_controller(fronthaul_iface, m_cmdu_tx);

    // This probably changed the "AP Operational BSS" list in topology, so send a notification
    if (!m_cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
        return;
    }

    auto tlvAlMacAddress = m_cmdu_tx.addClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMacAddress) {
        LOG(ERROR) << "addClass ieee1905_1::tlvAlMacAddress failed";
        return;
    }

    tlvAlMacAddress->mac() = db->bridge.mac;
    m_btl_ctx.send_cmdu_to_controller(fronthaul_iface, m_cmdu_tx);

    radio_conf_params.received_vaps_list_update = true;

    radio_conf_params.num_of_bss_available =
        std::count_if(radio->front.bssids.begin(), radio->front.bssids.end(),
                      [](beerocks::AgentDB::sRadio::sFront::sBssid b) {
                          return b.mac != net::network_utils::ZERO_MAC;
                      });
}

void ApAutoConfigurationTask::handle_vs_apply_vlan_policy_request(
    ieee1905_1::CmduMessageRx &cmdu_rx, int fd, std::shared_ptr<beerocks_header> beerocks_header)
{
    m_traffic_separation_configurator->apply_policy();
}

bool ApAutoConfigurationTask::airties_vs_ap_autoconfiguration_wsc_parse_hidden_ssid(
    WSC::m2 &m2, WSC::configData::config &config)
{
    bool retval = false;
    for (auto &vendor_ext_attr : m2.getAttrList<WSC::cWscAttrVendorExtension>()) {
        if ((WSC::eWscVendorId::WSC_VENDOR_ID_AIRTIES_1 != vendor_ext_attr->vendor_id_0()) ||
            (WSC::eWscVendorId::WSC_VENDOR_ID_AIRTIES_2 != vendor_ext_attr->vendor_id_1()) ||
            (WSC::eWscVendorId::WSC_VENDOR_ID_AIRTIES_3 != vendor_ext_attr->vendor_id_2())) {
            continue;
        }

        LOG(INFO) << "Vendor OUI: " << vendor_ext_attr->vendor_id_0()
                  << vendor_ext_attr->vendor_id_1() << vendor_ext_attr->vendor_id_2()
                  << "is received";
        auto vendor_data = vendor_ext_attr->vendor_data();

        if (vendor_data[0] == VENDOR_BSS_CFG) {
            //Hidden BSS attribute is set
            config.hidden_ssid = (vendor_data[1] == VENDOR_HIDE_SSID) ? true : false;
            retval             = true;
            break;
        }
    }

    LOG(INFO) << "Hidden SSID is set to " << config.hidden_ssid;
    return retval;
}

bool ApAutoConfigurationTask::ap_autoconfiguration_wsc_calculate_keys(
    const std::string &fronthaul_iface, const uint8_t *remote_pubkey, const uint8_t *nonce,
    uint8_t authkey[32], uint8_t keywrapkey[16])
{
    const auto &radio_conf_params = m_radios_conf_params[fronthaul_iface];
    if (!radio_conf_params.dh) {
        LOG(ERROR) << "diffie hellman member not initialized";
        return false;
    }

    auto db = AgentDB::get();
    mapf::encryption::wps_calculate_keys(
        *radio_conf_params.dh, remote_pubkey, WSC::eWscLengths::WSC_PUBLIC_KEY_LENGTH,
        radio_conf_params.dh->nonce(), db->bridge.mac.oct, nonce, authkey, keywrapkey);

    return true;
}

bool ApAutoConfigurationTask::ap_autoconfiguration_wsc_authenticate(
    const std::string &fronthaul_iface, WSC::WscAttrList &wsc, uint8_t authkey[32],
    uint8_t authenticator[WSC::WSC_AUTHENTICATOR_LENGTH])
{
    const auto &radio_conf_params = m_radios_conf_params[fronthaul_iface];
    if (!radio_conf_params.m1_auth_buf) {
        LOG(ERROR) << "Invalid M1";
        return false;
    }

    // This is the content of M1 and M2/M8, without the type and length.
    uint8_t buf[radio_conf_params.m1_auth_buf_len + wsc.getMessageLength() -
                WSC::cWscAttrAuthenticator::get_initial_size()];
    auto next = std::copy_n(radio_conf_params.m1_auth_buf, radio_conf_params.m1_auth_buf_len, buf);
    wsc.swap(); // swap to get network byte order
    std::copy_n(wsc.getMessageBuff(),
                wsc.getMessageLength() - WSC::cWscAttrAuthenticator::get_initial_size(), next);
    wsc.swap(); // swap back

    uint8_t kwa[WSC::WSC_AUTHENTICATOR_LENGTH];
    // Add KWA which is the 1st 64 bits of HMAC of config_data using AuthKey
    if (!mapf::encryption::kwa_compute(authkey, buf, sizeof(buf), kwa)) {
        LOG(ERROR) << "kwa_compute failure";
        return false;
    }

    if (!std::equal(kwa, kwa + sizeof(kwa), authenticator)) {
        LOG(ERROR) << "WSC Global authentication failed";
        LOG(DEBUG) << "authenticator: "
                   << utils::dump_buffer(authenticator, WSC::WSC_AUTHENTICATOR_LENGTH);
        LOG(DEBUG) << "calculated:    " << utils::dump_buffer(kwa, WSC::WSC_AUTHENTICATOR_LENGTH);
        LOG(DEBUG) << "authenticator key: " << utils::dump_buffer(authkey, 32);
        LOG(DEBUG) << "authenticator buf:" << std::endl << utils::dump_buffer(buf, sizeof(buf));
        return false;
    }

    LOG(DEBUG) << "WSC Global authentication success";
    return true;
}

bool ApAutoConfigurationTask::ap_autoconfiguration_wsc_parse_encrypted_settings(
    WSC::cWscAttrEncryptedSettings encrypted_settings, uint8_t authkey[32], uint8_t keywrapkey[16],
    WSC::configData::config &config)
{
    uint8_t *iv     = reinterpret_cast<uint8_t *>(encrypted_settings.iv());
    auto ciphertext = reinterpret_cast<uint8_t *>(encrypted_settings.encrypted_settings());
    int cipherlen   = encrypted_settings.encrypted_settings_length();
    // leave room for up to 16 bytes internal padding length - see aes_decrypt()
    int datalen = cipherlen + 16;
    uint8_t decrypted[datalen];

    // LOG(DEBUG) << "WSC Message received encrypted settings with length " << cipherlen;

    LOG(DEBUG) << "WSC Message Parse: aes decrypt";
    if (!mapf::encryption::aes_decrypt(keywrapkey, iv, ciphertext, cipherlen, decrypted, datalen)) {
        LOG(ERROR) << "aes decrypt failure";
        return false;
    }

    LOG(DEBUG) << "WSC Message Parse: parse config_data, len = " << datalen;
    // LOG(DEBUG) << "decrypted config_data buffer: " << std::endl
    //            << utils::dump_buffer(decrypted, datalen);

    // Parsing failure means that the config data is invalid,
    // in which case it is unclear what we should do.
    // In practice, some controllers simply send an empty config data
    // when the radio should be tore down, so let the caller handle this
    // by returning true with a warning for now.
    auto config_data = WSC::configData::parse(decrypted, datalen);
    if (!config_data) {
        LOG(WARNING) << "Invalid config data, skip it";
        return true;
    }

    // get length of config_data for KWA authentication
    size_t len = config_data->getMessageLength();
    // Protect against M2|M8 buffer overflow attacks
    if (len > size_t(datalen)) {
        LOG(ERROR) << "invalid config data length";
        return false;
    }
    // Update VAP configuration
    config.auth_type   = config_data->auth_type();
    config.encr_type   = config_data->encr_type();
    config.bssid       = config_data->bssid();
    config.network_key = config_data->network_key();
    config.ssid        = config_data->ssid();
    config.bss_type    = config_data->bss_type();

    // Get the Key Wrap Authenticator data
    auto kwa_data = config_data->key_wrap_authenticator();
    if (!kwa_data) {
        LOG(ERROR) << "No KeyWrapAuthenticator in config_data";
        return false;
    }

    // The keywrap authenticator is part of the config_data (last member of the
    // config_data to be precise).
    // However, since we need to calculate it over the part of config_data without the keywrap
    // authenticator, substruct it's size from the computation length
    size_t config_data_len_for_kwa = len - config_data->key_wrap_authenticator_size();

    // Swap to network byte order for KWA HMAC calculation
    // from this point config data is not readable!
    config_data->swap();
    uint8_t kwa[WSC::WSC_AUTHENTICATOR_LENGTH];
    // Compute KWA based on decrypted settings
    if (!mapf::encryption::kwa_compute(authkey, decrypted, config_data_len_for_kwa, kwa)) {
        LOG(ERROR) << "kwa compute";
        return false;
    }

    if (!std::equal(kwa, kwa + sizeof(kwa), kwa_data)) {
        LOG(ERROR) << "WSC KWA (Key Wrap Auth) failure";
        return false;
    }
    LOG(DEBUG) << "KWA (Key Wrap Auth) success";

    return true;
}

bool ApAutoConfigurationTask::send_error_response_message(
    const std::vector<std::pair<wfa_map::tlvProfile2ErrorCode::eReasonCode, sMacAddr>> &bss_errors)
{
    if (!m_cmdu_tx.create(0, ieee1905_1::eMessageType::ERROR_RESPONSE_MESSAGE)) {
        LOG(ERROR) << "cmdu creation has failed";
        return false;
    }

    LOG(INFO) << "Sending ERROR_RESPONSE_MESSAGE to the controller on:";
    for (const auto &bss_error : bss_errors) {
        auto &reason = bss_error.first;
        auto &bssid  = bss_error.second;
        LOG(INFO) << "reason : " << reason << ", bssid: " << bssid;

        auto profile2_error_code_tlv = m_cmdu_tx.addClass<wfa_map::tlvProfile2ErrorCode>();
        if (!profile2_error_code_tlv) {
            LOG(ERROR) << "addClass has failed";
            return false;
        }

        profile2_error_code_tlv->reason_code() = reason;
        profile2_error_code_tlv->set_bssid(bssid);

        m_btl_ctx.send_cmdu_to_controller({}, m_cmdu_tx);
    }
    return true;
}

bool ApAutoConfigurationTask::validate_reconfiguration(
    const std::string &radio_iface, std::vector<WSC::configData::config> &configs)
{
    auto db    = AgentDB::get();
    auto radio = db->radio(radio_iface);
    if (!radio) {
        LOG(ERROR) << "Radio not found " << radio_iface;
        return false;
    }
    std::stringstream config_prints;
    config_prints << "-- Current BSS config data:" << std::endl;
    for (const auto &bssid : radio->front.bssids) {
        if (!bssid.active) {
            continue;
        }
        config_prints << " bssid: " << bssid.mac << ", ssid: " << bssid.ssid
                      << ", fBSS: " << bssid.fronthaul_bss << ", bBSS: " << bssid.backhaul_bss
                      << (bssid.active ? ", is active." : ", isn't active.") << std::endl;
    }

    config_prints << "-- Incoming BSS config data:" << std::endl;
    for (const auto &config : configs) {
        config_prints << " bssid: " << config.bssid << ", ssid: " << config.ssid
                      << ", network_key: " << config.network_key
                      << ", authentication_type: " << std::hex << int(config.auth_type)
                      << ", encryption_type: " << std::hex << int(config.encr_type)
                      << ", bss_type: " << std::hex << int(config.bss_type) << std::endl;
    }
    config_prints << "--" << std::endl;

    // Using a nested lambda to return a predicate function.
    // The named lambda "find_by_similarity" receives a AgentDB BSS element.
    // The anonymous returning predicate lambda, finds a "matching" WSC config element.
    const auto find_by_similarity = [&db](const AgentDB::sRadio::sFront::sBssid &bss) {
        return [&db, &bss](const WSC::configData::config &config) {
            // Check if config's BSSID is valid
            if (config.bssid != db->bridge.mac) {
                // Config BSSID is valid, can check BSSID only.
                return (config.bssid == bss.mac);
            }
            // Need to expand the comparison to create a stricter match
            // TODO: PPM-2296
            // The issue here is that the Agent DB's sBssid does not contain the following
            //  - Authentication type
            //  - Encryption type
            //  - Network Key
            // For now validate against the values we do have.
            constexpr int minimal_similarity = 1;
            int matching_fields              = 0;

            // SSID
            if (bss.ssid == config.ssid) {
                matching_fields++;
            }

            // BSS Type
            if (bss.active &&
                !bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN)) {
                matching_fields++;
            }
            if (bss.backhaul_bss &&
                bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS)) {
                matching_fields++;
            }
            if (bss.fronthaul_bss &&
                bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS)) {
                matching_fields++;
            }
            if (bss.hidden_ssid && bool(config.hidden_ssid)) {
                matching_fields++;
            }

            return (matching_fields >= minimal_similarity);
        };
    };
    const auto bss_needs_reconfiguration = [](const WSC::configData::config &config,
                                              const AgentDB::sRadio::sFront::sBssid &bss) {
        // Need to read compare the incoming config (WSC::configData::config)
        // and compare it to the existing bss (AgentDB::sRadio::sFront::sBssid)
        // TODO: PPM-2296
        // The issue here is that the Agent DB's sBssid does not contain the following
        //  - Authentication type
        //  - Encryption type
        //  - Network Key
        // For now validate against the values we do have.

        // SSID
        if (bss.ssid != config.ssid) {
            LOG(DEBUG) << "SSID needs reconfiguration: " << bss.ssid << " != " << config.ssid;
            return true;
        }

        // BSS Type
        if (bss.active && bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN)) {
            LOG(DEBUG) << "BSS type needs reconfiguration: bss.active: " << bss.active
                       << " bss_type: " << config.bss_type;
            return true;
        }
        if (bss.backhaul_bss &&
            !bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS)) {
            LOG(DEBUG) << "BSS type needs reconfiguration: bss.backhaul_bss: " << bss.backhaul_bss
                       << " bss_type: " << config.bss_type;
            return true;
        }
        if (bss.fronthaul_bss &&
            !bool(config.bss_type & WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS)) {
            LOG(DEBUG) << "BSS type needs reconfiguration: bss.fronthaul_bss: " << bss.fronthaul_bss
                       << " bss_type: " << config.bss_type;
            return true;
        }
        if (bss.hidden_ssid && !bool(config.hidden_ssid)) {
            return true;
        }

        // Structures match
        return true;
    };

    const auto bss_pending_teardown = [](const WSC::configData::config &config) {
        return ((config.bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN) != 0);
    };

    const auto &bssids = radio->front.bssids;
    // Create a copy of the existing configuration.
    // Create a container for the final configuration.

    // We create these two containers, so we could remove and insert into the configuration
    std::vector<WSC::configData::config> config_copy = configs;
    std::vector<WSC::configData::config> final_config;

    // Iterate over existing configuration.
    for (const auto &bssid : bssids) {
        if (!bssid.active) {
            continue;
        }
        auto iter = std::find_if(config_copy.begin(), config_copy.end(), find_by_similarity(bssid));
        if (iter != config_copy.end()) {
            // Found a configuration that is similar to current bssid
            if (bss_needs_reconfiguration(*iter, bssid)) {

                // BSS needs reconfiguration
                LOG(DEBUG) << "BSS " << bssid.mac << " needs reconfiguration.";

                // Set the BSSID of the BSS since the controller does not send this information.
                iter->bssid = bssid.mac;

                // Add to the final configuration.
                final_config.emplace_back(std::move(*iter));

                // Remove from incoming configuration so we would not match with it again by mistake.
                config_copy.erase(iter);
            } else {
                LOG(DEBUG) << "BSS " << bssid.mac << " does not need reconfiguration.";
            }
        } else {
            // Did not find vap in configuration, need to teardown
            WSC::configData::config vap_to_teardown;
            vap_to_teardown.bssid    = bssid.mac;
            vap_to_teardown.bss_type = WSC::eWscVendorExtSubelementBssType::TEARDOWN;
            final_config.emplace_back(std::move(vap_to_teardown));
        }
    }

    // Now that all the existing BSSs were updated, we need to iterate over the remaining incoming configuration.
    // Any BSS that is flagged for teardown in the final configuration will instead be updated with the incoming configuration.

    for (auto &remaining_bss : config_copy) {
        // Find if there are any BSSs that are pending for teardown
        auto iter = std::find_if(final_config.begin(), final_config.end(), bss_pending_teardown);
        if (iter != final_config.end()) {
            // Override BSSs parameters
            LOG(DEBUG) << "BSS " << iter->bssid
                       << " will be reconfigured instead of being torn down.";
            iter->ssid        = remaining_bss.ssid;
            iter->auth_type   = remaining_bss.auth_type;
            iter->encr_type   = remaining_bss.encr_type;
            iter->network_key = remaining_bss.network_key;
            iter->bss_type    = remaining_bss.bss_type;
            iter->hidden_ssid = remaining_bss.hidden_ssid;

        } else if (final_config.size() < radio->front.radio_max_bss) {
            LOG(DEBUG) << "SSID " << remaining_bss.ssid
                       << " will be marked for a new instance of AccessPoint";
            // use wildcard mac for 'new' vaps
            tlvf::mac_from_string(remaining_bss.bssid.oct, network_utils::WILD_MAC_STRING);

            final_config.emplace_back(std::move(remaining_bss));
        } else {
            LOG(ERROR) << "Cannot add more VAPs then what are currently configured";
        }
    }

    config_prints << "-- New BSS config data:" << std::endl;
    for (const auto &config : final_config) {
        config_prints << " bssid: " << config.bssid << ", ssid: " << config.ssid
                      << ", network_key: " << config.network_key
                      << ", authentication_type: " << std::hex << int(config.auth_type)
                      << ", encryption_type: " << std::hex << int(config.encr_type)
                      << ", bss_type: " << std::hex << int(config.bss_type) << std::endl
                      << ", Hidden SSID: " << config.hidden_ssid << ", bss_type: " << std::hex
                      << int(config.bss_type) << std::endl;
    }

    // Set final configuration.
    configs = final_config;

    // This log is very large and spamming, can be used for debugging purposes if needed.
    // LOG(INFO) << "Config Prints: " << std::endl
    //           << std::endl
    //           << config_prints.str() << std::endl
    //           << std::endl;
    return true;
}

bool ApAutoConfigurationTask::send_ap_bss_configuration_message(
    const std::string &radio_iface, const std::vector<WSC::configData::config> &configs)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST>(m_cmdu_tx);
    if (!request) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }

    auto db = AgentDB::get();
    request->set_bridge_ifname(db->bridge.iface_name);

    std::stringstream ss;
    for (const auto &config : configs) {
        auto c = request->create_wifi_credentials();
        if (!c) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        ss << "VAP: " << config.bssid << std::endl
           << "- BSS type: " << std::hex << int(config.bss_type) << std::endl;

        c->bssid_attr().data = config.bssid;
        c->bss_type()        = config.bss_type;

        if ((config.bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN) != 0) {
            ss << "- - BSS flagged for teardown." << std::endl;
            // BSS needs teardown, can skip setting the rest of the values.
            c->set_ssid("");
            c->set_network_key("");
            c->authentication_type_attr().data = WSC::eWscAuth::WSC_AUTH_INVALID;
            c->encryption_type_attr().data     = WSC::eWscEncr::WSC_ENCR_INVALID;
            request->add_wifi_credentials(c);
            continue;
        }

        ss << "- SSID: " << config.ssid << std::endl
           << "- Key: " << config.network_key << std::endl
           << "- Auth: " << std::hex << int(config.auth_type) << std::endl
           << "- Encr: " << std::hex << int(config.encr_type) << std::endl
           << "- Hidden_SSID: " << config.hidden_ssid << std::endl;

        c->set_ssid(config.ssid);
        c->set_network_key(config.network_key);
        c->authentication_type_attr().data = config.auth_type;
        c->encryption_type_attr().data     = config.encr_type;
        c->mld_id()                        = config.mld_id;
        c->hidden_ssid()                   = config.hidden_ssid;
        request->add_wifi_credentials(c);
    }
    LOG(INFO) << "Sending reconfiguration: " << std::endl << ss.str();

    auto ap_manager_fd = m_btl_ctx.get_ap_manager_fd(radio_iface);
    m_btl_ctx.send_cmdu(ap_manager_fd, m_cmdu_tx);
    return true;
}

bool ApAutoConfigurationTask::send_ap_bss_info_update_request(const std::string &radio_iface)
{
    // request the current vap list from ap_manager
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST>(m_cmdu_tx);
    if (!request) {
        LOG(ERROR) << "Failed building cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST message!";
        return false;
    }
    auto ap_manager_fd = m_btl_ctx.get_ap_manager_fd(radio_iface);
    m_btl_ctx.send_cmdu(ap_manager_fd, m_cmdu_tx);
    return true;
}

bool ApAutoConfigurationTask::send_ap_connected_sta_notifications_request(
    const std::string &radio_iface)
{
    // request the current vap list from ap_manager
    auto client_notifications_request = message_com::create_vs_message<
        beerocks_message::
            cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST>(m_cmdu_tx);
    if (!client_notifications_request) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }
    auto ap_manager_fd = m_btl_ctx.get_ap_manager_fd(radio_iface);
    m_btl_ctx.send_cmdu(ap_manager_fd, m_cmdu_tx);
    return true;
}

bool ApAutoConfigurationTask::airties_vs_ap_autoconfiguration_wsc_parse_service_status(
    ieee1905_1::CmduMessageRx &cmdu_rx, const std::string &radio_iface)
{
    if (!cmdu_rx.getMessageLength()) {
        LOG(ERROR) << "cmdu is not initialized";
        return false;
    }

    auto tlv = cmdu_rx.getClass<ieee1905_1::tlvVendorSpecific>();
    if (!tlv) {
        LOG(ERROR) << "Error creating tlvVendorSpecific";
        return false;
    }

    if (tlv->vendor_oui() != airties::tlvAirtiesMsgType::airtiesVendorOUI::OUI_AIRTIES) {
        LOG(ERROR) << "Mismatch OUI!";
        return false;
    }

    auto vendor_tlv =
        std::make_shared<airties::tlvAirtiesServiceStatus>(tlv->payload(), tlv->payload_length());
    if (!vendor_tlv) {
        LOG(ERROR) << "Error creating tlvAirtiesServiceStatus";
        return false;
    }

    vendor_tlv->class_swap(); // swap to get network byte order
    if (vendor_tlv->tlv_id() != static_cast<int>(airties::eAirtiesTlVId::AIRTIES_SERVICE_STATUS)) {
        LOG(ERROR) << "Mismatch tlv_id!";
        return false;
    }

    auto radio_state = (static_cast<int>(vendor_tlv->payload().reason) == 1 ? false : true);
    vendor_tlv->class_swap(); // swap back

    auto change_radio_state = [this](bool enable, const std::string &radio_iface) -> bool {
        if (enable) {
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_RADIO_ENABLE_REQUEST>(m_cmdu_tx);
            if (!request) {
                LOG(ERROR) << "Failed building cACTION_APMANAGER_RADIO_ENABLE_REQUEST message!";
                return false;
            }
        } else {
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_RADIO_DISABLE_REQUEST>(m_cmdu_tx);
            if (!request) {
                LOG(ERROR) << "Failed building cACTION_APMANAGER_RADIO_DISABLE_REQUEST message!";
                return false;
            }
        }
        auto ap_manager_fd = m_btl_ctx.get_ap_manager_fd(radio_iface);
        m_btl_ctx.send_cmdu(ap_manager_fd, m_cmdu_tx);
        return true;
    };

    if (!change_radio_state(radio_state, radio_iface)) {
        LOG(ERROR) << "Error changing radio state!";
        return false;
    }

    return true;
}

bool ApAutoConfigurationTask::handle_ap_autoconfiguration_wsc_vs_extension_tlv(
    ieee1905_1::CmduMessageRx &cmdu_rx, const std::string &radio_iface)
{
    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (!beerocks_header) {
        LOG(INFO) << "No MaxLinear Controller join response";
        return true;
    }

    LOG(INFO) << "MaxLinear Controller join response";
    LOG(DEBUG) << "ACTION_CONTROL_SLAVE_JOINED_RESPONSE " << radio_iface;

    if (beerocks_header->action_op() != beerocks_message::ACTION_CONTROL_SLAVE_JOINED_RESPONSE) {
        LOG(ERROR) << "Unexpected Intel action op " << beerocks_header->action_op();
        return false;
    }

    auto joined_response =
        beerocks_header->addClass<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_RESPONSE>();
    if (joined_response == nullptr) {
        LOG(ERROR) << "addClass cACTION_CONTROL_SLAVE_JOINED_RESPONSE failed";
        return false;
    }

    std::string controller_version(joined_response->master_version(message::VERSION_LENGTH));

    LOG(DEBUG) << "Version (Controller/Agent): " << controller_version << "/" << BEEROCKS_VERSION;
    auto agent_version_s      = version::version_from_string(BEEROCKS_VERSION);
    auto controller_version_s = version::version_from_string(controller_version);

    // check for mismatch
    if (controller_version_s.major != agent_version_s.major ||
        controller_version_s.minor != agent_version_s.minor ||
        controller_version_s.build_number != agent_version_s.build_number) {
        LOG(WARNING) << "controller_version != agent_version";
        LOG(WARNING) << "Version (Controller/Agent): " << controller_version << "/"
                     << BEEROCKS_VERSION;
    }

    // check if fatal mismatch
    if (joined_response->err_code() == beerocks::JOIN_RESP_VERSION_MISMATCH) {
        LOG(ERROR) << "Mismatch version! slave_version=" << std::string(BEEROCKS_VERSION)
                   << " controller_version=" << controller_version;
        LOG(DEBUG) << "goto STATE_STOPPED";
        m_btl_ctx.fsm_stop();
        return false;
    }
    if (joined_response->err_code() == beerocks::JOIN_RESP_SSID_MISMATCH) {
        LOG(ERROR) << "Mismatch SSID!";
        LOG(DEBUG) << "goto STATE_STOPPED";
        m_btl_ctx.fsm_stop();
        return false;
    }

    if (!send_platform_version_notification(radio_iface, controller_version)) {
        LOG(ERROR) << "send_platform_version_notification failed";
        return false;
    }

    if (!send_monitor_son_config(radio_iface, joined_response->config())) {
        LOG(ERROR) << "send_monitor_son_config failed";
        return false;
    }

    return true;
}

bool ApAutoConfigurationTask::send_platform_version_notification(
    const std::string &radio_iface, const std::string &controller_version)
{
    // Send Controller version + Agent version to platform manager
    auto notification = message_com::create_vs_message<
        beerocks_message::cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION>(m_cmdu_tx);
    if (!notification) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }
    string_utils::copy_string(notification->versions().master_version, controller_version.c_str(),
                              sizeof(beerocks_message::sVersions::master_version));
    string_utils::copy_string(notification->versions().slave_version, BEEROCKS_VERSION,
                              sizeof(beerocks_message::sVersions::slave_version));

    auto platform_manager_cmdu_client = m_btl_ctx.get_platform_manager_cmdu_client();
    if (!platform_manager_cmdu_client) {
        LOG(ERROR) << "Failed to get platform manager cmdu client";
        return false;
    }
    platform_manager_cmdu_client->send_cmdu(m_cmdu_tx);

    LOG(DEBUG) << "send ACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION " << radio_iface;
    return true;
}

bool ApAutoConfigurationTask::send_monitor_son_config(
    const std::string &radio_iface, const beerocks_message::sSonConfig &son_config)
{
    LOG(INFO) << "sending ACTION_MONITOR_SON_CONFIG_UPDATE " << radio_iface;

    auto update =
        message_com::create_vs_message<beerocks_message::cACTION_MONITOR_SON_CONFIG_UPDATE>(
            m_cmdu_tx);
    if (!update) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }

    update->config() = son_config;

    auto monitor_fd = m_btl_ctx.get_monitor_fd(radio_iface);
    m_btl_ctx.send_cmdu(monitor_fd, m_cmdu_tx);

    LOG(DEBUG) << "SON_CONFIG_UPDATE " << radio_iface << ":" << std::endl
               << "monitor_total_ch_load_notification_th_hi_percent="
               << son_config.monitor_total_ch_load_notification_lo_th_percent << std::endl
               << "monitor_total_ch_load_notification_th_lo_percent="
               << son_config.monitor_total_ch_load_notification_hi_th_percent << std::endl
               << "monitor_total_ch_load_notification_delta_th_percent="
               << son_config.monitor_total_ch_load_notification_delta_th_percent << std::endl
               << "monitor_min_active_clients=" << son_config.monitor_min_active_clients
               << std::endl
               << "monitor_active_client_th=" << son_config.monitor_active_client_th << std::endl
               << "monitor_client_load_notification_delta_th_percent="
               << son_config.monitor_client_load_notification_delta_th_percent << std::endl
               << "monitor_rx_rssi_notification_threshold_dbm="
               << son_config.monitor_rx_rssi_notification_threshold_dbm << std::endl
               << "monitor_rx_rssi_notification_delta_db="
               << son_config.monitor_rx_rssi_notification_delta_db << std::endl
               << "monitor_ap_idle_threshold_B=" << son_config.monitor_ap_idle_threshold_B
               << std::endl
               << "monitor_ap_active_threshold_B=" << son_config.monitor_ap_active_threshold_B
               << std::endl
               << "monitor_ap_idle_stable_time_sec=" << son_config.monitor_ap_idle_stable_time_sec
               << std::endl
               << "monitor_disable_initiative_arp=" << son_config.monitor_disable_initiative_arp
               << std::endl;

    return true;
}
