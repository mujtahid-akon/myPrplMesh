/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _AP_MANAGER_H
#define _AP_MANAGER_H

// AP HAL
#include <bwl/ap_wlan_hal.h>

#include <bcl/beerocks_cmdu_client_factory.h>
#include <bcl/beerocks_event_loop.h>
#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_timer_manager.h>
#include <bcl/network/file_descriptor.h>
#include <beerocks/tlvf/beerocks_message_apmanager.h>

#include <atomic>
#include <list>

namespace son {
class ApManager {

public:
    ApManager(const std::string &iface, beerocks::logging &logger,
              std::shared_ptr<beerocks::CmduClientFactory> slave_cmdu_client_factory,
              std::shared_ptr<beerocks::TimerManager> timer_manager,
              std::shared_ptr<beerocks::EventLoop> event_loop);

    /**
     * @brief Starts AP manager.
     *
     * @return true on success and false otherwise.
     */
    bool start();

    /**
     * @brief Stops AP manager.
     *
     * @return true on success and false otherwise.
     */
    bool stop();

    enum class eApManagerState;

    /**
     * disallowed client parameters
     * Used to save clients mac, bssid that the client is disallowed from and 
     * validity period of time (blocking) 
     * so we could unblock it when the period expires
     */
    struct disallowed_client_t {
        sMacAddr mac;
        sMacAddr bssid;
        std::chrono::steady_clock::time_point timeout;
    };

    bool is_operational() const;

    /**
     * @brief Returns 'true' if the AP support ZWDFS.
     * 
     * @return true if the radio is ZWDFS radio, otherwise false. 
     */
    bool zwdfs_ap() const;

private:
    /**
     * @brief Sends given CMDU message to the slave.
     *
     * @param cmdu_tx CMDU message to send.
     * @return true on success and false otherwise.
     */
    bool send_cmdu(ieee1905_1::CmduMessageTx &cmdu_tx);

    /**
     * @brief Handles CMDU message received from slave.
     *
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles CMDU message received from dpp_onboarding_task.
     *
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_cmdu_ieee1905_1_message(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles DPP CCE Indication message.
     *
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_dpp_cce_indication_message(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Direct Encap DPP message.
     *
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_direct_encap_dpp_message(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Virtual BSS Request message.
     *
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_virtual_bss_request(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Virtual BSS Move Cancel Message
     *
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_virtual_bss_move_cancel_request(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles the request for the security context of a given vbss client
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_vbss_security_request(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Virtual BSS Move preparation Request message.
     *
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_virtual_bss_move_preparation_request(ieee1905_1::CmduMessageRx &cmdu_rx);

    /*
     * @brief Handles unassoc sta link metrics query
     *
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_unassoc_sta_link_metrics_query_message(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Handles Trigger Channel Switch Announcement requests
     * 
     * @param cmdu_rx Received CMDU to be handled.
     */
    void handle_trigger_channel_switch_announcement_request(ieee1905_1::CmduMessageRx &cmdu_rx);

    /**
     * @brief Runs the Finite State Machine of the AP manager.
     * 
     * @param[out] continue_processing Flag that means that state machine transitioned to a 
     * transient state and thus processing could and should continue immediately (no need to wait).
     * @return true on success and false otherwise.
     */
    bool ap_manager_fsm(bool &continue_processing);

    bool hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr);
    void handle_hostapd_attached();
    bool handle_ap_enabled(int vap_id);
    bool handle_aps_update_list();
    void fill_cs_params(beerocks_message::sApChannelSwitch &params);
    void fill_sr_params(beerocks_message::sSpatialReuseParams &params);
    bool create_ap_wlan_hal();
    void send_heartbeat();
    void send_steering_return_status(beerocks_message::eActionOp_APMANAGER ActionOp,
                                     int32_t status);
    void remove_client_from_disallowed_list(const sMacAddr &mac, const sMacAddr &bssid);
    void allow_expired_clients();
    bool register_ext_events_handlers(int fd);

    /**
     * @brief Create a new BSS and register handlers for it.
     *
     * @param ifname the interface name.
     * @param bss_conf the configuration for the new BSS.
     * @param bridge the bridge name.
     * @param vbss whether the new BSS is a VBSS or not.
     *
     * @return true on success, false otherwise.
     */
    bool add_bss(std::string &ifname, son::wireless_utils::sBssInfoConf &bss_conf,
                 std::string &bridge, bool vbss);

    /**
     * @brief Remove a BSS and its external event handlers.
     *
     * @param ifname the interface name.
     *
     * @return true on success, false otherwise.
     */
    bool remove_bss(std::string &ifname);

    /**
     * @brief Get the interface name to be used for a VBSS, based on its BSSID.
     *
     * @param bssid the BSSID of the VBSS.
     *
     * @return The interface name for the VBSS.
     */
    std::string get_vbss_interface_name(const sMacAddr &bssid);

    // Class constants
    static constexpr uint8_t BEACON_TRANSMIT_TIME_MS = 100;
    static constexpr uint8_t BSS_STEER_IMMINENT_VALID_INT_BTT =
        (beerocks::BSS_STEER_DISASSOC_TIMER_MS / BEACON_TRANSMIT_TIME_MS);
    static constexpr uint8_t BSS_STEER_VALID_INT_BTT = 2; // 200ms

    /**
     * Buffer to hold CMDU to be transmitted.
     */
    uint8_t m_tx_buffer[beerocks::message::MESSAGE_BUFFER_LENGTH];

    /**
     * CMDU to be transmitted.
     */
    ieee1905_1::CmduMessageTx cmdu_tx;

    std::string m_iface;
    beerocks::logging &m_logger;
    bool acs_enabled;
    bool m_ap_support_zwdfs;
    wfa_map::tlvProfile2MultiApProfile::eMultiApProfile m_multiap_controller_profile =
        wfa_map::tlvProfile2MultiApProfile::eMultiApProfile::PRPLMESH_PROFILE_UNKNOWN;

    int bss_steer_valid_int          = BSS_STEER_VALID_INT_BTT;
    int bss_steer_imminent_valid_int = BSS_STEER_IMMINENT_VALID_INT_BTT;
    struct sApManagerState {
        ApManager *parent;
        eApManagerState cur;
        eApManagerState max;

        eApManagerState operator=(eApManagerState state);

        operator eApManagerState() const { return cur; }
    } m_state;
    std::chrono::steady_clock::time_point m_state_timeout;
    std::vector<disallowed_client_t> m_disallowed_clients;

    struct pending_disable_vap_t {
        int8_t vap_id;
        std::chrono::steady_clock::time_point timeout;
    };

    std::list<pending_disable_vap_t> pending_disable_vaps;

    /**
     * File descriptor to the external events queue.
     */
    std::vector<int> m_ap_hal_ext_events = {beerocks::net::FileDescriptor::invalid_descriptor};

    /**
     * Maps dynamically created interfaces to their external events
     * queue file descriptor.
     */
    std::unordered_map<std::string, int> m_dynamic_bss_ext_events_fds;

    /**
     * File descriptor to the internal events queue.
     */
    int m_ap_hal_int_events = beerocks::net::FileDescriptor::invalid_descriptor;

    int sta_unassociated_rssi_measurement_header_id = -1;

    std::shared_ptr<bwl::ap_wlan_hal> ap_wlan_hal;

    std::chrono::steady_clock::time_point next_heartbeat_notification_timestamp;

    const uint8_t HEARTBEAT_NOTIFICATION_DELAY_SEC = 1;

    bool acs_completed_vap_update = false;

    const uint8_t GENERATE_CONNECTED_EVENTS_WORK_TIME_LIMIT_MSEC = 5;
    const uint16_t GENERATE_CONNECTED_EVENTS_DELAY_MSEC          = 500;
    bool m_generate_connected_clients_events                     = false;
    std::chrono::steady_clock::time_point m_next_generate_connected_events_time =
        std::chrono::steady_clock::time_point::min();

    //Timer for triggering a CSA notification
    void start_csa_notification_timer(
        std::shared_ptr<beerocks_message::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START>
            request);
    void csa_notification_timer_elapsed(
        std::shared_ptr<beerocks_message::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START>
            request);
    int csa_notification_timer = beerocks::net::FileDescriptor::invalid_descriptor;
    bool csa_notification_timer_on() const
    {
        return csa_notification_timer != beerocks::net::FileDescriptor::invalid_descriptor;
    }

    /**
     * Factory to create CMDU client instances connected to CMDU server running in slave.
     */
    std::shared_ptr<beerocks::CmduClientFactory> m_slave_cmdu_client_factory;

    /**
     * Timer manager to help using application timers.
     */
    std::shared_ptr<beerocks::TimerManager> m_timer_manager;

    /**
     * Application event loop used by the process to wait for I/O events.
     */
    std::shared_ptr<beerocks::EventLoop> m_event_loop;

    /**
     * File descriptor of the timer to run the Finite State Machine.
     */
    int m_fsm_timer = beerocks::net::FileDescriptor::invalid_descriptor;

    /**
     * File descriptor of the timer to resume deauthenticating unknown stations.
     */
    int m_vbss_deauth_unknown_stas_timer = beerocks::net::FileDescriptor::invalid_descriptor;

    /**
     * CMDU client connected to the the CMDU server running in slave.
     * This object is dynamically created using the CMDU client factory for the slave provided in 
     * class constructor.
     */
    std::unique_ptr<beerocks::CmduClient> m_slave_client;

    bool certification_mode = false;
    bool radio_state_lock   = false;

    struct sBeaconMetricsResponse {
        sMacAddr sta_mac;
        struct sBeaconReport {
            uint8_t length;
            beerocks_message::sBeaconResponse11k params;
        };
        std::vector<sBeaconReport> beacon_report;
        int resp_timer = beerocks::net::FileDescriptor::invalid_descriptor;
    };

    /* To store scheduled Beacon Metrics Responses */
    std::vector<sBeaconMetricsResponse> m_beacon_metrics_response;

    /* Timer callback for scheduled Beacon Metrics Responses */
    void beacon_metrics_response_cb(int fd);
};

} // namespace son

#endif
