/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CHANNEL_SELECTION_TASK_H_
#define _CHANNEL_SELECTION_TASK_H_

#include "../agent_db.h"
#include "task.h"
#include <map>
#include <tlvf/CmduMessageTx.h>
#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/wfa_map/tlvChannelSelectionResponse.h>
#include <tlvf/wfa_map/tlvSpatialReuseConfigResponse.h>
#include <tlvf/wfa_map/tlvSpatialReuseRequest.h>
#include <tlvf/wfa_map/tlvTransmitPowerLimit.h>
#include <unordered_map>

#include <beerocks/tlvf/enums/eDfsState.h>

namespace beerocks {

// Forward declaration for BackhaulManager context saving
class BackhaulManager;

class ChannelSelectionTask : public Task {
public:
    ChannelSelectionTask(BackhaulManager &btl_ctx, ieee1905_1::CmduMessageTx &cmdu_tx);

    void work() override;

    enum eEvent : uint8_t { AP_DISABLED, AP_ENABLED };

    void handle_event(uint8_t event_enum_value, const void *event_obj) override;

    bool handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx, uint32_t iface_index,
                     const sMacAddr &dst_mac, const sMacAddr &src_mac, int fd,
                     std::shared_ptr<beerocks_header> beerocks_header) override;

private:
    /**
     * @brief Contain the current channel selection which has chosen by the task.
     *
     * From the channel and the bandwidth, a center channel can be evaluated by a look-up on
     * 'son::wireless_utils::channels_table_5g'
     *
     * @param channel Chosen beacon channel.
     * @param secondary_channel Chosen secondary beacon channel. Relevant only if the bandwidth is
     *  80+80.
     * @param bw Bandwidth of the channel.
     * @param dfs_state DFS state for knowing if the channel is DFS channel or not.
     */
    struct sSelectedChannel {
        uint8_t channel                       = 0;
        uint8_t operating_class               = 0;
        beerocks::eWiFiBandwidth bw           = beerocks::eWiFiBandwidth::BANDWIDTH_UNKNOWN;
        beerocks_message::eDfsState dfs_state = beerocks_message::eDfsState::UNAVAILABLE;
        int rank                              = -1;
        uint8_t preference_score              = 0;
    } m_selected_channel;

    enum eRadioChanSwitchState : uint8_t {
        INVALID,
        NO_CHANNEL_SWITCH,
        ACS_SENT,
        CSA_NOTIF_RECEIVED
    };
    // task sends ACS_START but reply from base_bwl_hal is CSA_NOTIFICATION

    struct sIncomingChannelSelectionRequest {
        // Assume response will be successful
        wfa_map::tlvChannelSelectionResponse::eResponseCode response_code =
            wfa_map::tlvChannelSelectionResponse::eResponseCode::ACCEPT;

        AgentDB::sRadio::channel_preferences_map controller_preferences;

        eRadioChanSwitchState chan_sel_state = INVALID;
        struct sChannelSelectionParams {
            uint8_t channel                    = 0;
            beerocks::eFreqType freq_type      = beerocks::eFreqType::FREQ_UNKNOWN;
            beerocks::eWiFiBandwidth bandwidth = beerocks::eWiFiBandwidth::BANDWIDTH_UNKNOWN;
            int8_t tx_limit                    = 0;
            bool tx_limit_valid                = false;
            uint8_t CSA_count                  = 5;
        } outgoing_request;
        son::wireless_utils::sSpatialReuseParams spatial_reuse_request;
        sSelectedChannel selected_channel;
        bool power_switch_received          = false;
        bool channel_switch_needed          = false;
        bool spatial_reuse_request_received = false;
        bool is_zwdfs_needed                = false;
        bool manually_send_operating_report = false;
    };

    struct sPendingChannelPreferenceReport {
        uint16_t mid = 0;
        std::unordered_map<sMacAddr, bool> preference_ready{};
    };

    struct sPendingChannelSelection {
        uint16_t mid = 0;
        std::chrono::steady_clock::time_point last_chan_sel;
        std::unordered_map<sMacAddr, sIncomingChannelSelectionRequest> requests;
    };

    void handle_channel_preference_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                         const sMacAddr &src_mac);

    void handle_channel_selection_request(ieee1905_1::CmduMessageRx &cmdu_rx,
                                          const sMacAddr &src_mac);

    /**
     * @brief Handles Vendor Specific messages.
     *
     * @param[in] cmdu_rx Received CMDU.
     * @param[in] src_mac MAC address of the message sender.
     * @param[in] fd File descriptor of the socket connection with the slave that sent the message.
     * @param[in] beerocks_header Shared pointer to beerocks header.
     * @return true, if the message has been handled, otherwise false.
     */
    bool handle_vendor_specific(ieee1905_1::CmduMessageRx &cmdu_rx, const sMacAddr &src_mac, int fd,
                                std::shared_ptr<beerocks_header> beerocks_header);

    /* Vendor specific message handlers: */

    void handle_vs_csa_notification(ieee1905_1::CmduMessageRx &cmdu_rx, int fd,
                                    std::shared_ptr<beerocks_header> beerocks_header);

    void handle_vs_csa_error_notification(ieee1905_1::CmduMessageRx &cmdu_rx, int fd,
                                          std::shared_ptr<beerocks_header> beerocks_header);

    void handle_vs_cac_started_notification(ieee1905_1::CmduMessageRx &cmdu_rx, int fd,
                                            std::shared_ptr<beerocks_header> beerocks_header);

    void handle_vs_dfs_cac_completed_notification(ieee1905_1::CmduMessageRx &cmdu_rx, int fd,
                                                  std::shared_ptr<beerocks_header> beerocks_header);

    void handle_vs_channels_list_response(ieee1905_1::CmduMessageRx &cmdu_rx, int fd,
                                          std::shared_ptr<beerocks_header> beerocks_header);

    void
    handle_vs_zwdfs_ant_channel_switch_response(ieee1905_1::CmduMessageRx &cmdu_rx, int fd,
                                                std::shared_ptr<beerocks_header> beerocks_header);

    void handle_ap_disabled_event(const std::string &iface);

    void handle_ap_enable_event(const std::string &iface);

    bool build_channel_preference_report(const sMacAddr &radio_mac);

    bool channel_preference_report_ready();

    bool send_channel_preference_report();

    bool create_channel_preference_tlv(const sMacAddr &radio_mac);

    bool create_cac_completion_report_tlv();

    bool create_cac_status_report_tlv();

    bool create_radio_operation_restriction_tlv(const sMacAddr &radio_mac);

    bool create_cac_status_tlv();

    bool handle_transmit_power_limit(
        const std::shared_ptr<wfa_map::tlvTransmitPowerLimit> tx_power_limit_tlv);

    bool store_controller_preference(
        const std::shared_ptr<wfa_map::tlvChannelPreference> channel_preference_tlv);

    bool check_received_preferences_contain_violation(const sMacAddr &radio_mac);

    bool check_is_there_better_channel_than_current(const sMacAddr &radio_mac);

    bool handle_spatial_reuse_tlv(wfa_map::tlvSpatialReuseRequest &spatial_reuse_tlv);

    ChannelSelectionTask::sSelectedChannel
    select_next_channel(const sMacAddr &radio_mac,
                        const AgentDB::sRadio::channel_preferences_map &controller_preferences);

    bool handle_on_demand_selection_request_extension_tlv(ieee1905_1::CmduMessageRx &cmdu_rx);

    bool send_channel_switch_request(const sMacAddr &radio_mac,
                                     const sIncomingChannelSelectionRequest &request);

    /* ZWDFS */
    static constexpr int8_t ZWDFS_FLOW_MAX_RETRIES                 = 5;
    static constexpr int16_t ZWDFS_FLOW_DELAY_BETWEEN_RETRIES_MSEC = 1000;

    bool zwdfs_in_process() { return m_zwdfs_state != eZwdfsState::NOT_RUNNING; }

    enum eZwdfsState : uint8_t {
        NOT_RUNNING,
        INIT_ZWDFS_FLOW,
        REQUEST_CHANNELS_LIST,
        WAIT_FOR_CHANNELS_LIST,
        CHOOSE_NEXT_BEST_CHANNEL,
        ZWDFS_SWITCH_ANT_SET_CHANNEL_REQUEST,
        WAIT_FOR_ZWDFS_CAC_STARTED,
        WAIT_FOR_ZWDFS_CAC_COMPLETED,
        SWITCH_CHANNEL_PRIMARY_RADIO,
        WAIT_FOR_PRIMARY_RADIO_CSA_NOTIFICATION,
        ZWDFS_SWITCH_ANT_OFF_REQUEST,
        WAIT_FOR_ZWDFS_SWITCH_ANT_OFF_RESPONSE,
    } m_zwdfs_state = eZwdfsState::ZWDFS_SWITCH_ANT_OFF_REQUEST;

    // clang-format off
    const std::unordered_map<eZwdfsState, std::string, std::hash<int>> m_zwdfs_states_string = {
      { eZwdfsState::NOT_RUNNING,                             "NOT_RUNNING"                             },
      { eZwdfsState::INIT_ZWDFS_FLOW,                          "INIT_ZWDFS_FLOW"                         },
      { eZwdfsState::REQUEST_CHANNELS_LIST,                   "REQUEST_CHANNELS_LIST"                   },
      { eZwdfsState::WAIT_FOR_CHANNELS_LIST,                  "WAIT_FOR_CHANNELS_LIST"                  },
      { eZwdfsState::CHOOSE_NEXT_BEST_CHANNEL,                "CHOOSE_NEXT_BEST_CHANNEL"                },
      { eZwdfsState::ZWDFS_SWITCH_ANT_SET_CHANNEL_REQUEST,    "ZWDFS_SWITCH_ANT_SET_CHANNEL_REQUEST"    },
      { eZwdfsState::WAIT_FOR_ZWDFS_CAC_STARTED,              "WAIT_FOR_ZWDFS_CAC_STARTED"              },
      { eZwdfsState::WAIT_FOR_ZWDFS_CAC_COMPLETED,            "WAIT_FOR_ZWDFS_CAC_COMPLETED"            },
      { eZwdfsState::SWITCH_CHANNEL_PRIMARY_RADIO,            "SWITCH_CHANNEL_PRIMARY_RADIO"            },
      { eZwdfsState::WAIT_FOR_PRIMARY_RADIO_CSA_NOTIFICATION, "WAIT_FOR_PRIMARY_RADIO_CSA_NOTIFICATION" },
      { eZwdfsState::ZWDFS_SWITCH_ANT_OFF_REQUEST,            "ZWDFS_SWITCH_ANT_OFF_REQUEST"            },
      { eZwdfsState::WAIT_FOR_ZWDFS_SWITCH_ANT_OFF_RESPONSE,  "WAIT_FOR_ZWDFS_SWITCH_ANT_OFF_RESPONSE"  },
    };
    // clang-format on

    void zwdfs_fsm();

    /**
     * @brief The function initialize the class members 'm_zwdfs_iface' to the zwdfs radio
     * interface name.
     *
     * @return true on success, otherwise false.
     */
    bool initialize_zwdfs_interface_name();

    /**
     * @brief Check if a Radio on a given band, or all band is doing background scan.
     * 
     * @param [in] band If set, check on the specific band, otherwise check on all bands.
     * @return true if scan is being performed, otherwise false. 
     */
    bool radio_scan_in_progress(eFreqType band = eFreqType::FREQ_UNKNOWN);

    sSelectedChannel select_best_usable_channel(const std::string &front_radio_iface);

    /**
     * @brief Abort ZWDFS flow in progress
     *
     * @param [in] external_channel_switch true if external channel switch detected, false if end of zwdfs flow
     */
    void abort_zwdfs_flow(bool external_channel_switch = true);

    std::string m_zwdfs_iface;
    std::string m_zwdfs_primary_radio_iface;
    std::chrono::steady_clock::time_point m_zwdfs_fsm_timeout;

    /** @brief Indicator if the ZW-DFS antenna is in use.
    *
    * The ZW-DFS antenna is assumed to be in use at task start to enable ZW-DFS antenna release
    * in case the agent got restarted while the antenna is still owned by the ZW-DFS antenna hostapd
    * (due to hostapd crash or agent process crash).
    */
    bool m_zwdfs_ant_in_use = true;

    bool m_zwdfs_ap_enabled = false;

    // channel_selection_task propagates the CAC_COMPLETED_NOTIFICATION event to the
    // coordinated_cac_task which results in sending ACTION_BACKHAUL_CHANNELS_LIST_REQUEST.
    // Set this flag to true to send preference report when handling the response for the
    // above request in handle_vs_channels_list_response()
    bool m_send_preference_report_after_cac_started_event    = false;
    bool m_send_preference_report_after_cac_completion_event = false;

    /**
     * CSA event from the radio will trigger SWITCH_CHANNEL_NOTIFICATION_EVENT to the
     * switch_channel task which results in sending ACTION_BACKHAUL_CHANNELS_LIST_REQUEST.
     * Set this flag to true to send preference report when handling the response for the
     * above request in handle_vs_channels_list_response()
     */
    bool m_send_preference_report_after_csa_finished_event = false;

    /* Class members */

    sPendingChannelSelection m_pending_selection;
    bool pending_operating_channel_report() { return m_pending_selection.mid != 0; }
    void send_operating_channel_report_if_ready();
    bool send_operating_channel_report(const sMacAddr &radio_mac);

    /**
     * @brief if channel selection context is waiting for a channel selection notification,
     * consume the event and return true;
     * else return false
     */
    bool is_waiting_for_csa_notification(const sMacAddr radio_mac);

    sPendingChannelPreferenceReport m_pending_preference;

    BackhaulManager &m_btl_ctx;
    ieee1905_1::CmduMessageTx &m_cmdu_tx;

    uint8_t m_retry_counter = 0;
    std::chrono::steady_clock::time_point m_next_retry_time =
        std::chrono::steady_clock::time_point::min(); // way in the past;
};

} // namespace beerocks

#endif // _CHANNEL_SELECTION_TASK_H_
