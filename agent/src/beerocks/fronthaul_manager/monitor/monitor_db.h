/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MONITOR_DB_H
#define MONITOR_DB_H

#include <bcl/beerocks_defines.h>
#include <bwl/mon_wlan_hal_types.h>

#include <chrono>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

namespace son {
////////////////////////////////////////////
////////////////////////////////////////////
class monitor_sta_node {
public:
    monitor_sta_node(const int8_t vap_id_, const std::string &mac_)
        : vap_id(vap_id_), mac(mac_), m_sta_stats()
    {
    }

    enum eArpState {
        IDLE = 0,
        SEND_ARP,
        WAIT_FIRST_REPLY,
        WAIT_REPLY,
        SEND_RESPONSE,
    };

    void set_last_change_time() { last_change_time = std::chrono::steady_clock::now(); }
    std::chrono::steady_clock::time_point get_last_change_time() const { return last_change_time; }

    int8_t get_vap_id() const { return vap_id; }

    void set_ipv4(const std::string &ip) { ipv4 = ip; }
    std::string get_ipv4() const { return ipv4; }

    void set_bridge_4addr_mac(const std::string &bridge_mac_4addr_)
    {
        bridge_mac_4addr = bridge_mac_4addr_;
    }
    std::string get_bridge_4addr_mac() const { return bridge_mac_4addr; }

    std::string get_mac() const { return mac; }

    void set_arp_state(eArpState state) { arp_state = state; }
    eArpState get_arp_state() const { return arp_state; }

    void set_arp_burst(bool en) { arp_burst = en; }
    bool get_arp_burst() const { return arp_burst; }

    void set_rx_rssi_ready(bool en) { rx_rssi_ready = en; }
    bool get_rx_rssi_ready() const { return rx_rssi_ready; }

    void set_rx_snr_ready(bool en) { rx_snr_ready = en; }
    bool get_rx_snr_ready() const { return rx_snr_ready; }

    void push_rx_rssi_request_id(uint16_t id) { pending_rx_rssi_requests_id.push_back(id); }
    std::list<uint16_t> &get_rx_rssi_request_id_list() { return pending_rx_rssi_requests_id; }
    void clear_rx_rssi_request_id_list() { pending_rx_rssi_requests_id.clear(); }

    void arp_set_start_time() { arp_time = std::chrono::steady_clock::now(); }
    std::chrono::steady_clock::time_point arp_get_start_time() const { return arp_time; }

    void arp_recv_count_clear() { arp_recv_count = 0; }
    void arp_recv_count_inc() { arp_recv_count++; }
    uint8_t arp_recv_count_get() const { return arp_recv_count; }

    void arp_retry_count_clear() { arp_retry_count = 0; }
    void arp_retry_count_inc() { arp_retry_count++; }
    uint8_t arp_retry_count_get() const { return arp_retry_count; }

    double get_load_rx_phy_rate() const;
    double get_load_tx_phy_rate() const;
    double get_load_rx_bit_rate() const;
    double get_load_tx_bit_rate() const;
    double get_load_rx_percentage() const;
    double get_load_tx_percentage() const;
    uint32_t get_tx_packets() const;
    uint32_t get_rx_packets() const;
    void reset_poll_data();

    void set_measure_sta_enable(bool en) { m_measure_sta_enable = en; }
    bool get_measure_sta_enable() { return m_measure_sta_enable; }

    friend std::ostream &operator<<(std::ostream &os, const monitor_sta_node &sta_node);
    friend std::ostream &operator<<(std::ostream &os, const monitor_sta_node *sta_node);

    // Statistics //
    struct SStaStats {
        uint8_t poll_cnt                                       = 0;
        uint16_t delta_ms                                      = 0;
        std::chrono::steady_clock::time_point last_update_time = std::chrono::steady_clock::now();

        int8_t rx_rssi_prev            = beerocks::RSSI_INVALID;
        int8_t rx_rssi_curr            = beerocks::RSSI_INVALID;
        int8_t rx_snr_curr             = beerocks::SNR_INVALID;
        uint16_t tx_phy_rate_100kb_avg = 0;
        uint16_t tx_phy_rate_100kb_min = 0;
        uint16_t tx_phy_rate_100kb_acc = 0;
        uint16_t rx_phy_rate_100kb_avg = 0;
        uint16_t rx_phy_rate_100kb_min = 0;
        uint16_t rx_phy_rate_100kb_acc = 0;

        bwl::SStaStats hal_stats = {};

        uint8_t tx_load_percent_curr = 0;
        uint8_t tx_load_percent_prev = 0;
        uint8_t rx_load_percent_curr = 0;
        uint8_t rx_load_percent_prev = 0;
    };

    bwl::SStaQosCtrlParams hal_qos_ctrl_params = {};
    bwl::SStaQosCtrlParams &get_qos_ctrl_params() { return m_sta_qos_ctrl_params; }
    const bwl::SStaQosCtrlParams &get_qos_ctrl_params() const { return m_sta_qos_ctrl_params; }

    SStaStats &get_stats() { return m_sta_stats; }
    const SStaStats &get_stats() const { return m_sta_stats; }

    // Idle stations //
    bool idle_detected       = false;
    bool enable_idle_monitor = false;
    std::chrono::steady_clock::time_point idle_detected_start_time;

private:
    int8_t vap_id = beerocks::IFACE_ID_INVALID;
    std::string ipv4;
    /*
             * IREs working in 4address mode operate from behind their bridge
             * we have to know it in order to send and receive ARP messages
             */
    std::string mac;
    std::string bridge_mac_4addr;
    eArpState arp_state     = eArpState::IDLE;
    bool rx_rssi_ready      = false;
    bool rx_snr_ready       = false;
    bool arp_burst          = false;
    uint8_t arp_recv_count  = 0;
    uint8_t arp_retry_count = 0;
    std::list<uint16_t> pending_rx_rssi_requests_id;
    std::chrono::steady_clock::time_point last_change_time;
    std::chrono::steady_clock::time_point arp_time = std::chrono::steady_clock::now();
    SStaStats m_sta_stats;
    bwl::SStaQosCtrlParams m_sta_qos_ctrl_params;
    bool m_measure_sta_enable = false;
};

////////////////////////////////////////////
////////////////////////////////////////////
class monitor_vap_node {
public:
    monitor_vap_node(const std::string &iface_, const int8_t vap_id_)
        : vap_id(vap_id_), iface(iface_), m_vap_stats()
    {
        // LOG(DEBUG) << "new vap_node=" << uint32_t(this);
        // LOG(DEBUG) << "new vap_stats=" << uint32_t(p_stats);
    }
    ~monitor_vap_node() {}

    std::string get_iface() const { return iface; }
    int8_t get_vap_id() const { return vap_id; }

    void set_mac(const std::string &ap_mac_) { mac = ap_mac_; }
    std::string get_mac() const { return mac; }

    std::string get_ipv4();

    void set_bridge_iface(const std::string &bridge_iface_) { bridge_iface = bridge_iface_; }
    std::string get_bridge_iface() { return bridge_iface; }

    void set_bridge_mac(const std::string &bridge_mac_) { bridge_mac = bridge_mac_; }
    std::string get_bridge_mac() { return bridge_mac; }

    void set_bridge_ipv4(const std::string &bridge_ipv4_) { bridge_ipv4 = bridge_ipv4_; }
    std::string get_bridge_ipv4() { return bridge_ipv4; }

    void sta_count_inc() { sta_count += 1; }
    void sta_count_dec()
    {
        if (sta_count > 0)
            sta_count -= 1;
    }
    int sta_get_count() const { return sta_count; }

    double get_rx_bit_rate();
    double get_tx_bit_rate();

    friend std::ostream &operator<<(std::ostream &os, const monitor_vap_node &vap_node);
    friend std::ostream &operator<<(std::ostream &os, const monitor_vap_node *vap_node);

    // Statistics //
    struct SVapStats {
        uint16_t delta_ms                                      = 0;
        std::chrono::steady_clock::time_point last_update_time = std::chrono::steady_clock::now();

        bwl::SVapStats hal_stats = {};

        uint8_t client_tx_load_tot_prev      = 0;
        uint8_t client_rx_load_tot_prev      = 0;
        uint8_t client_tx_load_tot_curr      = 0;
        uint8_t client_rx_load_tot_curr      = 0;
        bool active_client_count_is_above_th = false;
        int active_client_count_prev         = 0;
        int active_client_count_curr         = 0;
    };

    SVapStats &get_stats() { return m_vap_stats; }

    void clear_stats();

private:
    int8_t vap_id;
    std::string iface;
    int sta_count = 0;
    std::string mac;
    std::string ipv4;
    std::string bridge_iface;
    std::string bridge_mac;
    std::string bridge_ipv4;

    SVapStats m_vap_stats;
};

////////////////////////////////////////////
////////////////////////////////////////////
class monitor_radio_node {
public:
    monitor_radio_node() : m_radio_stats() {}
    ~monitor_radio_node() {}

    void set_iface(const std::string &iface_) { iface = iface_; }
    std::string get_iface() const { return iface; }

    void set_channel(const uint8_t channel_) { channel = channel_; }
    uint8_t get_channel() const { return channel; }

    double get_rx_bit_rate();
    double get_tx_bit_rate();

    friend std::ostream &operator<<(std::ostream &os, const monitor_radio_node &radio_node);
    friend std::ostream &operator<<(std::ostream &os, const monitor_radio_node *radio_node);

    void set_first_threshold_enabled(bool threshold_enabled_flag_)
    {
        m_send_first_ap_metrics_response_after_threshold_enable = threshold_enabled_flag_;
    }
    bool get_first_threshold_enabled()
    {
        return m_send_first_ap_metrics_response_after_threshold_enable;
    }

    /**
     * AP Metrics Reporting configuration and status type.
     */
    struct sApMetricsReportingInfo {
        /**
         * STA Metrics Reporting RCPI Threshold.
         * 0: Do not report STA Metrics based on RCPI threshold.
         * 1 – 220: RCPI threshold (encoded per Table 9-154 of [1]).
         * 221 – 255: Reserved
         * (Value is obtained from Metric Reporting Policy TLV)
         */
        uint8_t sta_metrics_reporting_rcpi_threshold = 0;

        /**
         * STA Metrics Reporting RCPI Hysteresis Margin Override.
         * 0: Use Agent's implementation-specific default RCPI Hysteresis margin.
         * >0: RCPI hysteresis margin value
         * (Value is obtained from Metric Reporting Policy TLV)
         */
        uint8_t sta_metrics_reporting_rcpi_hysteresis_margin_override = 0;

        /**
         * AP Metrics Channel Utilization Reporting Threshold.
         * 0: Do not report AP Metrics based on Channel utilization threshold.
         * >0: AP Metrics Channel Utilization Reporting Threshold (similar to channel utilization
         * measurement in 9.4.2.28 of [1]=[IEEE Std 802.11™-2016.pdf])
         * (Value is obtained from Metric Reporting Policy TLV)
         */
        uint8_t ap_channel_utilization_reporting_threshold = 0;

        /**
         * Associated STA Traffic Stats Inclusion Policy.
         * 0: Do not include Associated STA Traffic Stats TLV in AP Metrics Response
         * 1: Include Associated STA Traffic Stats TLV in AP Metrics Response
         * (Value is obtained from Metric Reporting Policy TLV)
         */
        bool include_associated_sta_link_metrics_tlv_in_ap_metrics_response = false;

        /**
         * Associated STA Link Metrics Inclusion Policy.
         * 0: Do not include Associated STA Link Metrics TLV in AP Metrics Response
         * 1: Include Associated STA Link Metrics TLV in AP Metrics Response
         * (Value is obtained from Metric Reporting Policy TLV)
         */
        bool include_associated_sta_traffic_stats_tlv_in_ap_metrics_response = false;

        /**
         * Associated WIFI 6 Sta Status Report Inclusion Policy.
         * 0: Do not include Associated WIFI 6 Sta Status Report TLV in AP Metrics Response
         * 1: Include Associated WIFI 6 Sta Status Report TLV in AP Metrics Response
         * (Value is obtained from Metric Reporting Policy TLV)
         */
        bool include_associated_wifi_6_sta_status_report_tlv_in_ap_metrics_response = false;

        /**
         * Last value reported for STA Metrics Reporting RCPI.
         * Must be compared with threshold value and hysteresis to decide if current value has to
         * be reported.
         */
        uint8_t sta_metrics_reporting_rcpi_value = 0;

        /**
         * Last value reported for AP Metrics Channel Utilization Reporting.
         * Must be compared with threshold value to decide if current value has to be reported.
         */
        uint8_t ap_metrics_channel_utilization_reporting_value = 0;

        /**
         *  TBD. Specifications:
         *  An indicator of the average radio noise plus interference
         *  power measured for the primary operating channel.
         *  Encoding as defined for ANPI in section 11.11.9.4 of [1]. Reserved: 221-224.
         */
        uint8_t ap_metrics_radio_noise = 0;

        /**
         *  TBD. Specification:
         *  The percentage of time (linearly scaled with 255 representing 100%)
         *  the radio has spent on individually or group addressed transmissions by the AP.
         *  When more than one channel is in use by BSS operating on the radio,
         *  then the Transmit value is calculated only for the primary channel.
         */
        uint8_t ap_metrics_radio_transmit = 0;

        /**
         *  TBD. Specification:
         *  The percentage of time (linearly scaled with 255 representing 100%)
         *  the radio has spent on receiving individually or group addressed transmissions
         *  from any STA associated with any BSS operating on this radio.
         *  When more than one channel is in use by BSS operating on the radio,
         *  then the ReceiveSelf value is calculated only for the primary channel.
         */
        uint8_t ap_metrics_radio_receive_self = 0;

        /**
         *  TBD. Specification: The percentage of time (linearly scaled with 255
         *  representing 100%) the radio has spent on receiving valid IEEE
         *  802.11 PPDUs that are not associated with any BSS operating on this
         *  radio. When more than one channel is in use by BSS operating on the
         *  radio, then the ReceiveOther value is calculated only for the
         *  primary channel.
        */
        uint8_t ap_metrics_radio_receive_other = 0;

        /**
         * Time point at which channel utilization was reported for the last time.
         */
        std::chrono::steady_clock::time_point
            ap_metrics_channel_utilization_last_reporting_time_point;
    };

    sApMetricsReportingInfo &ap_metrics_reporting_info() { return m_ap_metrics_reporting_info; }
    const sApMetricsReportingInfo &ap_metrics_reporting_info() const
    {
        return m_ap_metrics_reporting_info;
    }

    uint8_t get_channel_utilization() const
    {
        return m_ap_metrics_reporting_info.ap_metrics_channel_utilization_reporting_value;
    }

    // Statistics //
    struct SRadioStats {
        uint16_t delta_ms                                      = 0;
        std::chrono::steady_clock::time_point last_update_time = std::chrono::steady_clock::now();

        bwl::SRadioStats hal_stats = {};

        uint32_t total_retrans_count = 0;

        uint8_t channel_load_tot_prev        = 0;
        uint8_t channel_load_tot_curr        = 0;
        uint8_t channel_load_others          = 0;
        uint8_t channel_load_idle            = 0;
        bool channel_load_tot_is_above_hi_th = false;

        uint8_t client_tx_load_tot_prev      = 0;
        uint8_t client_rx_load_tot_prev      = 0;
        uint8_t client_tx_load_tot_curr      = 0;
        uint8_t client_rx_load_tot_curr      = 0;
        bool active_client_count_is_above_th = false;
        int active_client_count_prev         = 0;
        int active_client_count_curr         = 0;
        uint32_t sta_count                   = 0;

        /**
         * Flag that signals if last VAP statistics read contain meaningful data for at least one
         * VAP. This flag is updated whenever VAP statistics are updated.
         * The term "available" in this context means that both transmitted and received bytes are
         * set to a value greater than 0.
         */
        bool vap_stats_available = false;
    };

    SRadioStats &get_stats() { return m_radio_stats; }

    void clear_stats();

private:
    std::string iface;
    uint8_t channel = 0;
    SRadioStats m_radio_stats;

    /**
     * AP Metrics Reporting configuration and status value.
     * Configuration fields in this struct are initialized to default values during initialization
     * and from then on overwritten each time a Multi-AP Policy Config Request message is received.
     * Status fields contain last reported values for different AP metrics reported and the time
     * point they were reported for the last time. Difference between last reported value and
     * current value is compared against a threshold value to decide if AP metrics have changed
     * enough to be reported.
     */
    sApMetricsReportingInfo m_ap_metrics_reporting_info;

    /**
     * Send AP Metrics Response if channel utilization threshold enabled for the first time
     * 0: ap_channel_utilization_reporting_threshold field in sApMetricsReportingInfo is 0
     * 1: ap_channel_utilization_reporting_threshold field in sApMetricsReportingInfo changed
     * from 0 to 1
     */
    bool m_send_first_ap_metrics_response_after_threshold_enable = false;
};

////////////////////////////////////////////
////////////////////////////////////////////
class monitor_db {
public:
    enum class eClientsMeasurementMode : uint8_t {
        // equivalent to bpl::eClientsMeasurementMode
        DISABLE_ALL = 0,
        ENABLE_ALL,
        ONLY_CLIENTS_SELECTED_FOR_STEERING
    };

    monitor_db();
    ~monitor_db();

    void clear();

    // Radio //
    monitor_radio_node *get_radio_node() { return &radio_node; }
    int8_t get_hostapd_enabled() { return ap_hostapd_enabled; }
    void set_hostapd_enabled(int8_t enabled) { ap_hostapd_enabled = enabled; }
    int8_t get_ap_tx_enabled() { return ap_tx_enabled; }
    void set_ap_tx_enabled(int8_t enabled) { ap_tx_enabled = enabled; }

    // VAP //
    std::shared_ptr<monitor_vap_node> vap_add(const std::string &iface, int8_t vap_id);
    std::shared_ptr<monitor_vap_node> vap_get_by_id(int vap_id);
    int get_vap_id(const std::string &bssid);
    std::shared_ptr<monitor_vap_node> get_vap_node(const std::string &bssid);
    bool vap_remove(int vap_id);
    int get_vap_count() { return vap_nodes.size(); }
    void vap_erase_all();

    /**
     * @brief Gets a list containing the BSSID of all VAPs.
     *
     * @param[out] bssid_list list of BSSID to fill in.
     */
    void get_bssid_list(std::vector<sMacAddr> &bssid_list) const;

    // STA's //
    monitor_sta_node *sta_add(const std::string &sta_mac, const int8_t vap_id);
    void sta_erase(const std::string &sta_mac);
    void sta_erase_all();
    monitor_sta_node *sta_find(const std::string &mac);
    monitor_sta_node *sta_find_by_ipv4(const std::string &ipv4);
    std::unordered_map<std::string, monitor_sta_node *>::iterator sta_begin()
    {
        return sta_nodes.begin();
    }
    std::unordered_map<std::string, monitor_sta_node *>::iterator sta_end()
    {
        return sta_nodes.end();
    }

    size_t get_sta_count() const { return sta_nodes.size(); }

    // Monitor parameters //
    void set_arp_burst_delay(int arp_burst_delay_) { arp_burst_delay = arp_burst_delay_; }
    uint8_t get_arp_burst_delay() { return arp_burst_delay; }
    void set_arp_burst_pkt_num(int arp_burst_pkt_num_) { arp_burst_pkt_num = arp_burst_pkt_num_; }
    uint8_t get_arp_burst_pkt_num() { return arp_burst_pkt_num; }

    // Polling functions //
    std::chrono::steady_clock::time_point get_poll_next_time();
    void set_poll_next_time(std::chrono::steady_clock::time_point pt, bool reset_poll = false);
    bool poll_done();
    uint8_t get_poll_cnt();
    bool is_last_poll();

    void set_polling_rate_msec(uint32_t poll_rate_msec) { m_poll_rate_msec = poll_rate_msec; }
    uint32_t get_polling_rate_msec() { return m_poll_rate_msec; }
    void set_measurement_window_msec(uint32_t measurement_window_msec)
    {
        m_measurement_window_msec = measurement_window_msec;
    }
    uint32_t get_measurement_window_msec() { return m_measurement_window_msec; }

    std::chrono::steady_clock::time_point get_last_stats_update_time();
    void set_last_stats_update_time(std::chrono::steady_clock::time_point pt,
                                    bool reset_poll = false);

    std::chrono::steady_clock::time_point get_ap_poll_next_time();
    void set_ap_poll_next_time(std::chrono::steady_clock::time_point pt, bool reset_poll = false);

    void set_clients_measuremet_mode(eClientsMeasurementMode mode)
    {
        m_clients_measurement_mode = mode;
    }
    eClientsMeasurementMode get_clients_measuremet_mode() { return m_clients_measurement_mode; }

    void set_radio_stats_enable(bool enable) { m_radio_stats_enable = enable; }
    bool get_radio_stats_enable() { return m_radio_stats_enable; }

    void set_clients_unicast_measurements(bool enable) { m_clients_unicast_measurements = enable; }
    bool get_clients_unicast_measurements() { return m_clients_unicast_measurements; }

    const int MONITOR_LAST_CHANGE_TIMEOUT_MSEC                            = 30000;
    const int MONITOR_DB_AP_POLLING_RATE_SEC                              = 60;
    static constexpr int MONITOR_DB_DEFAULT_POLLING_RATE_MSEC             = 250;
    static constexpr int MONITOR_DB_DEFAULT_MEASUREMENT_WINDOW_POLL_COUNT = 4;

    const int MONITOR_ARP_TIMEOUT_MSEC = 550;
    const int MONITOR_ARP_PKT_NUM      = 6;

    const int MONITOR_ARP_COUNT_OK      = 4;
    const int MONITOR_ARP_WAKE_COUNT_OK = 1;
    const int MONITOR_ARP_RETRY_COUNT   = 10;

private:
    int8_t poll_cnt           = 0;
    uint32_t m_poll_rate_msec = MONITOR_DB_DEFAULT_POLLING_RATE_MSEC;
    uint32_t m_measurement_window_msec =
        MONITOR_DB_DEFAULT_MEASUREMENT_WINDOW_POLL_COUNT * MONITOR_DB_DEFAULT_POLLING_RATE_MSEC;
    std::chrono::steady_clock::time_point last_stats_update_time;
    std::chrono::steady_clock::time_point poll_next_time;
    std::chrono::steady_clock::time_point ap_poll_next_time;
    monitor_radio_node radio_node;
    std::unordered_map<int8_t, std::shared_ptr<monitor_vap_node>> vap_nodes;
    std::unordered_map<std::string, monitor_sta_node *> sta_nodes;

    int8_t ap_tx_enabled      = 0;
    int8_t ap_hostapd_enabled = 2;

    int arp_burst_pkt_num = 0;
    int arp_burst_delay   = 0;

    /**
     * The mode is read once from BPL as part of the monitor start and used to determine if
     * measurements for clients are: disable, enabled or enabled-only-for-selected-clients.
     * The default value is set to ENABLE_ALL.
    */
    eClientsMeasurementMode m_clients_measurement_mode = eClientsMeasurementMode::ENABLE_ALL;

    bool m_radio_stats_enable           = true;
    bool m_clients_unicast_measurements = false;
};

} // namespace son

#endif
