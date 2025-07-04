/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2022 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "controller_ucc_listener.h"
#include "db/db.h"
#include "periodic/periodic_operation_pool.h"
#include "tasks/channel_selection_task.h"
#include "tasks/dynamic_channel_selection_r2_task.h"
#include "tasks/link_metrics_task.h"
#include "tasks/network_health_check_task.h"
#include "tasks/statistics_polling_task.h"
#include "tasks/task_pool.h"

#include "../../../common/beerocks/bwl/include/bwl/base_wlan_hal.h"
#include <bcl/beerocks_cmdu_server.h>
#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_event_loop.h>
#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_message_structs.h>
#include <bcl/beerocks_timer_manager.h>
#include <bcl/beerocks_ucc_server.h>
#include <bcl/network/file_descriptor.h>
#include <btl/broker_client_factory.h>

#include <mapf/common/encryption.h>
#include <tlvf/WSC/configData.h>
#include <tlvf/WSC/m1.h>
#include <tlvf/WSC/m2.h>
#include <tlvf/WSC/m8.h>
#include <tlvf/ieee_1905_1/tlvWsc.h>
#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>

#include <cstddef>
#include <ctime>
#include <stdint.h>

namespace son {
class Controller {

public:
    Controller(db &database_,
               std::unique_ptr<beerocks::btl::BrokerClientFactory> broker_client_factory,
               std::unique_ptr<beerocks::UccServer> ucc_server,
               std::unique_ptr<beerocks::CmduServer> cmdu_server,
               std::shared_ptr<beerocks::TimerManager> timer_manager,
               std::shared_ptr<beerocks::EventLoop> event_loop);
    ~Controller();

    /**
     * @brief Starts controller.
     *
     * @return true on success and false otherwise.
     */
    bool start();

    /**
     * @brief Stops controller.
     *
     * @return true on success and false otherwise.
     */
    bool stop();

    /**
     * @brief Sends given CMDU message through the specified socket connection.
     *
     * @param fd File descriptor of the connected socket.
     * @param cmdu_tx CMDU message to send.
     * @return true on success and false otherwise.
     */
    bool send_cmdu(int fd, ieee1905_1::CmduMessageTx &cmdu_tx);

    /**
     * @brief Sends given CMDU message to the broker server running in the transport process to be
     * forwarded to another device or multicast.
     *
     * @param cmdu_tx CMDU message to send.
     * @param dst_mac Destination MAC address (must not be empty).
     * @param src_mac Source MAC address (must not be empty).
     * @param iface_name Name of the network interface to use (set to empty string to send on all
     * available interfaces).
     * @return true on success and false otherwise.
     */
    bool send_cmdu_to_broker(ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &dst_mac,
                             const sMacAddr &src_mac, const std::string &iface_name = "");

    /**
     * @brief Start client steering initiated by NBAPI.
     *
     * @param sta_mac Mac address of client.
     * @param target_bssid Target BSSID.
     * @return True if client steering started successfully, false otherwise.
     */
    bool start_client_steering(const std::string &sta_mac, const std::string &target_bssid);

    /**
     * @brief Send BTM Request on NBAPI RPC.
     *
     * @param disassociation_imminent Flag for 802.11 BTM Request.
     * @param disassociation_timer Number of beacon transmission times until Disassoc Frame.
     * @param bss_termination_duration Minute count for which the BSS is not available.
     * @param validity_interval Beacon interval count for which the CandidateList is valid.
     * @param steering_timer Beacon interval count for which the STA is blacklisted.
     * @param sta_mac Mac address of the client
     * @param target_bssid Target BSSID.
     * @return True if client steering started successfully, false otherwise.
     */
    bool send_btm_request(const bool &disassociation_imminent, const uint32_t &disassociation_timer,
                          const uint32_t &bss_termination_duration,
                          const uint32_t &validity_interval, const uint32_t &steering_timer,
                          const std::string &sta_mac, const std::string &target_bssid);

    /**
     * @brief Trigger channel scan initiated by NBAPI.
     *
     * @param ruid ruid of radio for wich scan requested.
     * @param channel_pool List of channels.
     * @param pool_size Channel pool size.
     * @param dwell_time Channel scan dwell time in milliseconds.
     * @return True if channel scan tiggered, false otherwise.
     */
    bool
    trigger_scan(const sMacAddr &ruid,
                 std::array<uint8_t, beerocks::message::SUPPORTED_CHANNELS_LENGTH> channel_pool,
                 uint8_t pool_size, int dwell_time);

    /**
     * @brief Trigger the set spatial reuse parameters by NBAPI.
     *
     * @param ruid ruid of radio for wich scan requested.
     * @param bss_color The value of the BSS Color subfield of the HEOperations.BSSColorInformation field being transmitted by BSSs operating on this radio
     * @param hesiga_spr_value15_allowed Indicates if the Agent is allowed to set HESIGA.SpatialReuse field to value 15 (PSR_AND_NON_SRG_OBSS_PD_PROHIBITED) in HE PPDU transmissions of this radio
     * @param srg_information_valid This field indicates whether the SRG Information fields (SRG OBSS PD Min Offset, SRG OBSS PD Max Offset, SRG BSS Color Bitmap and SRG Partial BSSID Bitmap) in this command are valid.
     * @param non_srg_offset_valid This field indicates whether the Non-SRG OBSSPD Max Offset field in this command is valid.
     * @param psr_disallowed Indicates if the Agent is disallowed to use Parameterized Spatial Reuse (PSR)-based Spatial Reuse for transmissions by the specified radio.
     * @param non_srg_obsspd_max_offset The value of dot11NonSRGAPOBSSPDMaxOffset (i.e the Non-SRG OBSSPD Max Offset value being used to control the transmissions of the specified radio)
     * @param srg_obsspd_min_offset The value of dot11SRGAPOBSSPDMinOffset (i.e. the SRG OBSSPD Min Offset value being used to control the transmissions of the specified radio)
     * @param srg_obsspd_max_offset The value of dot11SRGAPOBSSPDMaxOffset (i.e. the SRG OBSSPD Max Offset value being used to control the transmissions of the specified radio)
     * @param srg_bss_color_bitmap The value of dot11SRGAPBSSColorBitmap (i.e. the SRG BSS Color Bitmap being used to control the tranmissions of the specified radio)
     * @param srg_partial_bssid_bitmap The value of dot11SRGAPBSSIDBitmap (i.e. the SRG Partial BSSID Color Bitmap being used to control the transmissions of the specified radio)
     * @return True if set spatial reuse tiggered, false otherwise.
     */
    bool trigger_set_spatial_reuse(const sMacAddr &ruid, uint32_t bss_color,
                                   const bool hesiga_spr_value15_allowed,
                                   const bool srg_information_valid,
                                   const bool non_srg_offset_valid, const bool psr_disallowed,
                                   uint32_t non_srg_obsspd_max_offset,
                                   uint32_t srg_obsspd_min_offset, uint32_t srg_obsspd_max_offset,
                                   uint64_t srg_bss_color_bitmap,
                                   uint64_t srg_partial_bssid_bitmap);

    /**
     * @brief Triggers VBSS creation for the given VBSSID on the given radio/agent
     * 
     * @param dest_ruid The UID of the radio to create the VBSS on
     * @param vbssid The VBSSID to create for the client
     * @param client_mac The MAC Address of the client to create the VBSS for
     * @param new_bss_ssid The SSID to set for the BSS on the new agent
     * @param new_bss_pass The password to set for the BSS on the new agent
     * @return True if the creation operation was triggered, false otherwise
     */
    bool trigger_vbss_creation(const sMacAddr &dest_ruid, const sMacAddr &vbssid,
                               const sMacAddr &client_mac, const std::string &new_bss_ssid,
                               const std::string &new_bss_pass);

    /**
     * @brief Triggers VBSS destruction for the given VBSSID on the given radio/agent
     * 
     * @param connected_ruid The UID of the radio that the client is currently connected to
     * @param vbssid The BSSID of the VBSS to destroy
     * @param client_mac The MAC address of the VBSS client
     * @param should_disassociate Wether the client should disassociate from the network after destruction
     * @return True if the destruction operation was triggered, false otherwise 
     */
    bool trigger_vbss_destruction(const sMacAddr &connected_ruid, const sMacAddr &vbssid,
                                  const sMacAddr &client_mac,
                                  const bool should_disassociate = true);

    /**
     * @brief Requests the VBSS capabilities from the specified agent. 
     * Result which will be reflected in the NB API
     * 
     * @param agent_mac The MAC address of the agent to send the request to
     * @return True if the request was sent successfully, false otherwise
     */
    bool update_agent_vbss_capabilities(const sMacAddr &agent_mac);

    /**
     * @brief Triggers the move operation of a client between two agents on the vbss system
     * 
     * @param connected_ruid The UID of the currently connected radio
     * @param dest_ruid The UID of the radio to move to
     * @param vbssid The VBSSID to move between agents
     * @param client_mac The MAC Address of the client to move between agents
     * @param new_bss_ssid The SSID to set for the BSS on the new agent
     * @param new_bss_pass The password to set for the BSS on the new agent
     * @return True if move operation was triggered, false otherwise.
     */
    bool trigger_vbss_move(const sMacAddr &connected_ruid, const sMacAddr &dest_ruid,
                           const sMacAddr &vbssid, const sMacAddr &client_mac,
                           const std::string &new_bss_ssid, const std::string &new_bss_pass);

    /**
     * @brief Triggers the sending of QoS configuration to agents
     *
     */
    void trigger_prioritization_config();

    /**
     * @brief Function that starts all mandatory periodic tasks on controller start-up
     * Mandatory task list is
     * bml_task
     * topology_task
     * client_association_task
     * agent_monitoring_task
     * vbss_task (ifdef ENABLE_VBSS)
     * DhcpTask
     *
     * @return void.
     */
    void start_mandatory_tasks();

    /**
     * @brief Function that starts/stops configurable periodic tasks based on database settings
     * Optional tasks can be enabled/disabled via NbAPI flags.
     * The following tasks are in this category:
     * statistics_polling_task
     * LinkMetricTask
     * channel_selection_task
     * dynamic_channel_selection_task_r2
     * health_check_task
     *
     * @return void.
     */
    void start_optional_tasks();

    /**
     * @brief add an unassociated station to be monitored
     * 
     * @param station_mac_addr The MAC address of the unassociated station
     * @param channel used by the measurement. Note that the agent may ignore this value and use his active channel. 
     * @param preferred operating_class for the measurement.
     * @param agent_mac_addr The MAC address of the agent, if null, all agents are concerned.
     * @param radio_mac_addrress of the agent which supports the channel to be used for monitoring, if not given, it will be deduced automatically.
     * 
     * @return true if station added successfully, false if it exists already or any other issue.
     */
    bool add_unassociated_station(
        const sMacAddr &station_mac_addr, uint8_t channel, uint8_t operating_class,
        const sMacAddr &agent_mac_addr,
        const sMacAddr &radio_mac_addr = beerocks::net::network_utils::ZERO_MAC);

    /**
     * @brief remove an unassociated station to be monitored
     * 
     * @param station_mac_addr The MAC address of the unassociated station
     * @param agent_mac_addr The MAC address of the agent, if null, all agents are concerned.
     * 
     * @return true if station removed successfully, false if it does not exists  or any other issue.
     */
    bool remove_unassociated_station(
        const sMacAddr &station_mac_addr, const sMacAddr &agent_mac_addr,
        const sMacAddr &radio_mac_addr = beerocks::net::network_utils::ZERO_MAC);

    /**
     * @brief get all stats from all connected agents and update the DM.
     * 
     * @return true ifsuccess, false for any issue.
     */
    bool get_unassociated_stations_stats();

private:
    /**
     * @brief Handles the client-connected event in the CMDU server.
     *
     * @param fd File descriptor of the socket that got connected.
     */
    void handle_connected(int fd);

    /**
     * @brief Handles the client-disconnected event in the CMDU server.
     *
     * @param fd File descriptor of the socket that got disconnected.
     */
    void handle_disconnected(int fd);

    /**
     * @brief Handles received CMDU message.
     *
     * @param fd File descriptor of the socket connection the CMDU was received through.
     * @param iface_index Index of the network interface that the CMDU message was received on.
     * @param dst_mac Destination MAC address.
     * @param src_mac Source MAC address.
     * @param cmdu_rx Received CMDU to be handled.
     * @return true on success and false otherwise.
     */
    bool handle_cmdu(int fd, uint32_t iface_index, const sMacAddr &dst_mac, const sMacAddr &src_mac,
                     ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles CMDU message received from broker.
     *
     * This handler is slightly different than the handler for CMDU messages received from other
     * processes as it checks the source and destination MAC addresses set by the original sender.
     * It also filters out messages that are not addressed to the controller.
     *
     * @param iface_index Index of the network interface that the CMDU message was received on.
     * @param dst_mac Destination MAC address.
     * @param src_mac Source MAC address.
     * @param cmdu_rx Received CMDU to be handled.
     * @return true on success and false otherwise.
     */
    bool handle_cmdu_from_broker(uint32_t iface_index, const sMacAddr &dst_mac,
                                 const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx);

    bool handle_cmdu_1905_1_message(const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_control_message(const sMacAddr &src_mac,
                                     std::shared_ptr<beerocks::beerocks_header> beerocks_header);
    void handle_cmdu_control_ieee1905_1_message(const std::string &src_mac,
                                                ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_intel_slave_join(const sMacAddr &src_mac,
                                 std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
                                 beerocks::beerocks_header &beerocks_header,
                                 ieee1905_1::CmduMessageTx &cmdu_tx,
                                 const std::shared_ptr<Agent> &agent);
    bool
    handle_non_intel_slave_join(const sMacAddr &src_mac,
                                std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps,
                                const WSC::m1 &m1, const std::shared_ptr<Agent> &agent,
                                const sMacAddr &radio_mac, ieee1905_1::CmduMessageTx &cmdu_tx);

    // 1905 messages handlers
    bool handle_cmdu_1905_autoconfiguration_search(const sMacAddr &src_mac,
                                                   ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_autoconfiguration_WSC(const sMacAddr &src_mac,
                                                ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_ap_metric_response(const sMacAddr &src_mac,
                                             ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_ap_capability_report(const sMacAddr &src_mac,
                                               ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_channel_selection_response(const sMacAddr &src_mac,
                                                     ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_channel_scan_report(const sMacAddr &src_mac,
                                              ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_operating_channel_report(const sMacAddr &src_mac,
                                                   ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_ack_message(const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_higher_layer_data_message(const sMacAddr &src_mac,
                                                    ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_steering_completed_message(const sMacAddr &src_mac,
                                                     ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_client_steering_btm_report_message(const sMacAddr &src_mac,
                                                             ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_beacon_response(const sMacAddr &src_mac,
                                          ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_backhaul_sta_steering_response(const sMacAddr &src_mac,
                                                         ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_tunnelled_message(const sMacAddr &src_mac,
                                            ieee1905_1::CmduMessageRx &cmdu_rx);
    bool
    handle_cmdu_1905_backhaul_sta_capability_report_message(const sMacAddr &src_mac,
                                                            ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_failed_connection_message(const sMacAddr &src_mac,
                                                    ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_associated_sta_link_metrics_response_message(
        const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx);
    bool handle_cmdu_1905_bss_configuration_request_message(const sMacAddr &src_mac,
                                                            ieee1905_1::CmduMessageRx &cmdu_rx);

    bool handle_ap_capability_report(const sMacAddr &src_mac, ieee1905_1::CmduMessageRx &cmdu_rx,
                                     const bool early);
    bool handle_cmdu_1905_early_ap_capability_report_message(const sMacAddr &src_mac,
                                                             ieee1905_1::CmduMessageRx &cmdu_rx);

    bool autoconfig_wsc_parse_radio_caps(
        const sMacAddr &radio_mac,
        std::shared_ptr<wfa_map::tlvApRadioBasicCapabilities> radio_caps);

    /**
     * @brief Get info from 'AP HT Capabilities' TLV,
     * set data to AP HTCapabilities data element from Controller Data Model.
     *
     * @param cmdu_rx AP Capability Report message
     * @return true on success, false otherwise
    */
    bool handle_tlv_ap_ht_capabilities(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Get info from 'AP HE Capabilities' TLV,
     * set data to AP WiFi6Capabilities data element.
     *
     * @param cmdu_rx AP Capability Report message.
     * @param agent shared pointer
     * @return True on success, false otherwise.
    */
    bool handle_tlv_apCapability(ieee1905_1::CmduMessageRx &cmdu_rx, std::shared_ptr<Agent> agent,
                                 bool early);

    /**
     * @brief Get info from 'AP WIFI6 Capabilities' TLV,
     * set data to AP WiFi6Capabilities data element.
     *
     * @param cmdu_rx AP Capability Report message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_ap_wifi6_capabilities(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Get info from 'WIFI7 Agent Capabilities' TLV,
     * set data to WiFi7 Agent Capabilities data element.
     *
     * @param cmdu_rx AP Capability Report message.
     * @param agent shared ptr of agent sending the message
     * @return True on success, false otherwise.
    */
    bool handle_tlv_wifi7_agent_capabilities(ieee1905_1::CmduMessageRx &cmdu_rx,
                                             std::shared_ptr<Agent> agent);

    /**
     * @brief Get info from 'EHT Operations' TLV,
     * set data to EHT Operatons data element.
     *
     * @param cmdu_rx AP Capability Report message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_eht_operations(ieee1905_1::CmduMessageRx &cmdu_rx, const sMacAddr &al_mac);

    /**
     * @brief Get info from 'Agent AP MLD Configuration' TLV,
     * set data to Agent AP MLD Configuration data element.
     *
     * @param cmdu_rx AP Capability Report message.
     * @param src_mac MAC address of the sender of the message
     * @return True on success, false otherwise.
    */
    bool handle_tlv_agent_ap_mld_configuration(ieee1905_1::CmduMessageRx &cmdu_rx,
                                               const sMacAddr &src_mac);

    /**
     * @brief Get info from 'AP VHT Capabilities' TLV,
     * set data to AP VHTCapabilities data element.
     *
     * @param cmdu_rx AP Capability Report message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_ap_vht_capabilities(ieee1905_1::CmduMessageRx &cmdu_rx);

    // Autoconfig encryption support
    bool autoconfig_wsc_add_m2(WSC::m1 &m1, const wireless_utils::sBssInfoConf *bss_info_conf);
    bool autoconfig_wsc_add_m8(WSC::m1 &m1, const wireless_utils::sBssInfoConf &bss_info_conf);
    bool autoconfig_wsc_add_encrypted_settings(uint8_t &iv,
                                               std::vector<uint8_t> &encrypted_settings,
                                               WSC::configData &config_data, uint8_t authkey[32],
                                               uint8_t keywrapkey[16]);

    /**
     * @brief autoconfig global authenticator attribute calculation
     *
     * Calculate authentication on the Full M1 || M2* whereas M2* = M2 without the authenticator
     * attribute.
     *
     * @param m1 WSC M1 attribute list
     * @param wsc WSC M2 or WSC M8
     * @param authkey authentication key
     * @return true on success
     * @return false on failure
     */
    template <class C> bool autoconfig_wsc_authentication(WSC::m1 &m1, C &wsc, uint8_t authkey[32])
    {
        // Authentication on Full M1 || M2*/M8* (without the authenticator attribute)
        // This is the content of M1 and M2/M8, without the type and length.
        // Authentication is done on swapped data.
        // Since m1 is parsed, it is in host byte order, and needs to be swapped.
        // m2/m8 is created, and already finalized so its in network byte order, so no
        // need to swap it.
        m1.swap();
        uint8_t buf[m1.getMessageLength() + wsc->getMessageLength() -
                    WSC::cWscAttrAuthenticator::get_initial_size()];
        auto next = std::copy_n(m1.getMessageBuff(), m1.getMessageLength(), buf);
        std::copy_n(wsc->getMessageBuff(),
                    wsc->getMessageLength() - WSC::cWscAttrAuthenticator::get_initial_size(), next);
        // swap back
        m1.swap();
        uint8_t *kwa = reinterpret_cast<uint8_t *>(wsc->authenticator());
        // Add KWA which is the 1st 64 bits of HMAC of config_data using AuthKey
        if (!mapf::encryption::kwa_compute(authkey, buf, sizeof(buf), kwa)) {
            LOG(ERROR) << "kwa_compute failure";
            return false;
        }
        return true;
    }
    void autoconfig_wsc_calculate_keys(WSC::m1 &m1, uint8_t &enrollee_nonce,
                                       uint8_t &registrar_nonce, uint8_t &pub_key,
                                       const mapf::encryption::diffie_hellman &dh,
                                       uint8_t authkey[32], uint8_t keywrapkey[16]);

    /**
     * @brief Handles Tlv of AP Extended Metrics (tlvApExtendedMetrics).
     *
     * @param agent agent shared object.
     * @param cmdu_rx  AP Extended Metrics Response message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_ap_extended_metrics(std::shared_ptr<Agent> agent,
                                        ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Tlv of STA Link Metrics (tlvAssociatedStaLinkMetrics).
     *
     * @param src_mac Source MAC address.
     * @param cmdu_rx  AP Metrics Response or Associated STA Link Metrics Response message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_associated_sta_link_metrics(const sMacAddr &src_mac,
                                                ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Tlv of STA Extended Link Metrics (tlvAssociatedStaExtendedLinkMetrics).
     *
     * @param src_mac Source MAC address.
     * @param cmdu_rx  AP Metrics Response or Associated STA Link Metrics Response message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_associated_sta_extended_link_metrics(const sMacAddr &src_mac,
                                                         ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Tlv of STA Traffic Stats (tlvAssociatedStaTrafficStats).
     *
     * @param src_mac Source MAC address.
     * @param cmdu_rx  AP Metrics Response message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_associated_sta_traffic_stats(const sMacAddr &src_mac,
                                                 ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles TLV of Associated Wi-Fi 6 STA Status Report (tlvAssociatedWiFi6StaStatusReport).
     *
     * @param src_mac Source MAC address.
     * @param cmdu_rx  AP Metrics Response message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_associated_wifi6_sta_status_report(const sMacAddr &src_mac,
                                                       ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Tlv of Profile-2 AP Capability (tlvProfile2ApCapability).
     *
     * @param agent agent db shared object.
     * @param cmdu_rx Received CMDU as AP Capability Report message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_profile2_ap_capability(std::shared_ptr<Agent> agent,
                                           ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles TLV of CAC Capabilities (tlvProfile2CacCapabilities).
     *
     * @param agent Agent DB object.
     * @param cmdu_rx Received CMDU as Profile2 CAC Capabilities Report message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_profile2_cac_capabilities(Agent &agent, ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles TLV of Channel Scan Capabilities (tlvChannelScanCapabilities).
     *
     * @param agent Agent DB shared object.
     * @param cmdu_rx Received CMDU as Channel Scan Capabilities message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_profile2_channel_scan_capabilities(std::shared_ptr<Agent> &agent,
                                                       ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles TLV of 1905 Layer Security (tlv1905LayerSecurityCapability).
     *
     * @param agent agent db object.
     * @param cmdu_rx Received CMDU as Profile3 1905 Layer Security Capabilities Report message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_profile3_1905_layer_security_capabilities(const Agent &agent,
                                                              ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles TLV of AKM Suite Capabilities (tlvAkmSuiteCapabilities).
     *
     * @param agent Agent DB object.
     * @param cmdu_rx Received CMDU as Profile3 AKM Suite Capabilities message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_profile3_akm_suite_capabilities(Agent &agent,
                                                    ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Extracts ESP value from est_service_info_field and set it to specified
     * with @param_name NBAPI EstServiceParameter.
     *
     * @param param_name Name of NBAPI EstServiceParameter parameter to be set.
     * @param reporting_agent_bssid BSSID of BSS for which parameter will be set.
     * @param est_service_info_field Array with ESP values.
     *
    */

    /**
     * @brief Handles TLV of AP Radio Advanced Capabilities (tlvProfile2ApRadioAdvancedCapabilities).
     *
     * @param agent Agent DB object.
     * @param cmdu_rx Received CMDU as Profile-2 AP Radio Advanced Capabilities message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_profile2_ap_radio_advanced_capabilities(Agent &agent,
                                                            ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles TLV of Device Inventory (tlvDeviceInventory).
     *
     * @param agent Agent DB object.
     * @param cmdu_rx Received CMDU as Profile-3 Device Inventory message.
     * @return True on success, false otherwise.
    */
    bool handle_tlv_profile3_device_inventory(Agent &agent, ieee1905_1::CmduMessageRx &cmdu_rx);

    void set_esp(const std::string &param_name, const sMacAddr &reporting_agent_bssid,
                 uint8_t *est_service_info_field);

    /**
     * Buffer to hold CMDU to be transmitted.
     */
    uint8_t m_tx_buffer[beerocks::message::MESSAGE_BUFFER_LENGTH];

    /**
     * CMDU to be transmitted.
     */
    ieee1905_1::CmduMessageTx cmdu_tx;

    /**
     * Buffer to hold CMDU to be transmitted by the UCC listener (in certification mode).
     */
    uint8_t m_cert_tx_buffer[beerocks::message::MESSAGE_BUFFER_LENGTH];

    /**
     * CMDU to be transmitted by the UCC listener (in certification mode).
     */
    ieee1905_1::CmduMessageTx cert_cmdu_tx;

    db &database;
    task_pool m_task_pool;
    periodic_operation_pool operations;
    beerocks::controller_ucc_listener m_controller_ucc_listener;

    // It is used only in handle_cmdu_1905_ap_metric_response() to call construct_combined_infra_metric().
    // TODO It can be removed after cert_cmdu_tx usage is removed (PPM-1130).
    std::shared_ptr<LinkMetricsTask> m_link_metrics_task;

    /**
     * Task_id used to stop/start the channel_selection_task without restarting the controller
     */
    int m_channel_selection_task_id = -1;

    /**
     * Task_id used to stop/start the dynamic_channel_selection_r2_task without restarting the controller
     */
    int m_dynamic_channel_selection_task_id = -1;

    /**
     * Task_id used to start/stop the network_health_check_task without restarting the controller
     */
    int m_network_health_check_task_id = -1;

    /**
     * Task_id used to stop/start the statistics_polling_task without restarting the controller
     */
    int m_statistics_polling_task_id = -1;

    /**
     * @brief Checks if the current status of the task (running/not running) corresponds with
     * the configuration (enabled/disabled). Updates the status in accordance with the configuration
     *
     * @param[in] task_enabled should the task be running, as configured in the controller database
     * @param[out] current_task_id may be -1 for a not running task or a positive integer for a running task
     * @param[in] task_name string used to construct debug messages
     *
    */
    template <class Task>
    void task_status_helper(bool task_enabled, int &current_task_id, const std::string &task_name)
    {

        int current_status = ((current_task_id > 0) * 2) + (task_enabled * 1);
        // (current_task_id > 0) : is the task running ?
        // task_enabled : should it be running ?

        // using a switch case instead of a if/else construction
        // as it arranges the four possible cases linearly, and should be more readable

        switch (current_status) {
        case 0: // task disabled and not running: do nothing
            LOG(DEBUG) << task_name << ": already disabled";
            break;
        case 1: // task enabled but not running: start task
        {
            auto new_task = std::make_shared<Task>(database, cmdu_tx, m_task_pool);
            LOG(DEBUG) << task_name << ": starting with task_id " << new_task->id;
            current_task_id = new_task->id;
            LOG_IF(!m_task_pool.add_task(new_task), FATAL) << "Failed adding task!" << task_name;
            break;
        }
        case 2: // task disabled but running : kill task
        {
            LOG(DEBUG) << task_name << ": running with " << current_task_id << ", disabling";
            m_task_pool.kill_task(current_task_id);
            current_task_id = -1;
            break;
        }
        case 3: // task enabled and running : do nothing
        {
            if (m_task_pool.is_task_running(current_task_id)) {
                LOG(DEBUG) << task_name << " already running with task_id " << current_task_id;
            } else {
                LOG(ERROR) << task_name << " should be running but is not";
            }
            break;
        }
        }
    }

    /**
     * Factory to create broker client instances connected to broker server.
     * Broker client instances are used to exchange CMDU messages with remote processes running in
     * other devices in the network via the broker server running in the transport process.
     */
    std::unique_ptr<beerocks::btl::BrokerClientFactory> m_broker_client_factory;

    /**
     * CMDU server to exchange CMDU messages with clients through socket connections.
     */
    std::unique_ptr<beerocks::CmduServer> m_cmdu_server;

    /**
     * Timer manager to help using application timers.
     */
    std::shared_ptr<beerocks::TimerManager> m_timer_manager;

    /**
     * Application event loop used by the process to wait for I/O events.
     */
    std::shared_ptr<beerocks::EventLoop> m_event_loop;

    /**
     * File descriptor of the timer to run internal tasks periodically.
     */
    int m_tasks_timer = beerocks::net::FileDescriptor::invalid_descriptor;

    /**
     * File descriptor of the timer to run periodic operations.
     */
    int m_operations_timer = beerocks::net::FileDescriptor::invalid_descriptor;

    /**
     * Broker client to exchange CMDU messages with broker server running in transport process.
     */
    std::shared_ptr<beerocks::btl::BrokerClient> m_broker_client;

    /**
     * @brief sends a message of type UNASSOCIATED_STA_LINK_METRICS_QUERY_MESSAGE, it contains list/ddata of unassociated stations 
     *
     * @param cmdu_tx CMDU message to send.
     * @param database 
     *
     *  @return true on success and false otherwise.
    */
    bool send_unassociated_sta_link_metrics_query_message(ieee1905_1::CmduMessageTx &cmdu_tx,
                                                          db &database);
};

} // namespace son

#endif
