/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_CLI_BML_H
#define _BEEROCKS_CLI_BML_H

#define PRINT_BUFFER_LENGTH 4096
#include "beerocks_cli.h"
#include "bml.h"

#include <bcl/beerocks_defines.h>
#include <bcl/network/network_utils.h>

#include <list>

namespace beerocks {
class cli_bml : public cli {

public:
    struct conn_map_node_t {
        uint8_t type;
        uint8_t state;
        uint8_t channel;
        uint8_t bw;
        uint8_t freq_type;
        uint8_t channel_ext_above_secondary;
        int8_t rx_rssi;
        bool isWiFiBH;
        std::string mac;
        bool status;
        std::string ip_v4;
        std::string name;

        struct gw_ire_t {

            std::string backhaul_mac;

            struct radio_t {
                uint8_t channel;
                uint8_t cac_completed;
                uint8_t bw;
                uint8_t freq_type;
                uint8_t channel_ext_above_secondary;
                uint8_t ch_load;
                std::string radio_identifier;
                std::string radio_mac;
                std::string ifname;

                struct vap_t {
                    std::string ssid;
                    std::string bssid;
                    int vap_id;
                    bool backhaul_vap;
                };

                std::list<std::shared_ptr<vap_t>> vap;
            };

            std::list<std::shared_ptr<radio_t>> radio;

        } gw_ire;
    };

    explicit cli_bml(const std::string &beerocks_conf_path_);
    virtual ~cli_bml();

    bool connect() override;
    void disconnect() override;
    bool is_connected() override;
    bool start() override { return true; };
    void stop() override{};
    void print_help(bool print_header = true) { help(print_header); }
    int get_onboarding_status();
    bool is_pending_response();

    int analyzer_init(std::string remote_pc_ip);

private:
    // Help functions
    void setFunctionsMapAndArray() override;
    void printBmlReturnVals(const std::string &func_name, int ret_val);

    static void map_query_cb(const struct BML_NODE_ITER *node_iter, bool to_console);
    static void connection_map_cb(const struct BML_NODE_ITER *node_iter, bool to_console);
    static void map_query_to_console_cb(const struct BML_NODE_ITER *node_iter);
    static void connection_map_to_console_cb(const struct BML_NODE_ITER *node_iter);
    static void map_query_to_socket_cb(const struct BML_NODE_ITER *node_iter);
    static void map_update_cb(const struct BML_NODE_ITER *node_iter, bool to_console);
    static void map_update_to_console_cb(const struct BML_NODE_ITER *node_iter);
    static void map_update_to_socket_cb(const struct BML_NODE_ITER *node_iter);
    static void stats_update_cb(const struct BML_STATS_ITER *stats_iter, bool to_console);
    static void stats_update_to_console_cb(const struct BML_STATS_ITER *stats_iter);
    static void stats_update_to_socket_cb(const struct BML_STATS_ITER *stats_iter);
    static void events_update_cb(const struct BML_EVENT *event, bool to_console);
    static void events_update_to_console_cb(const struct BML_EVENT *event);
    static void events_update_to_socket_cb(const struct BML_EVENT *event);

    // Caller functions
    int connect_caller(int numOfArgs);
    int onboard_status_caller(int numOfArgs);
    int ping_caller(int numOfArgs);
    int nw_map_register_update_cb_caller(int numOfArgs);
    int nw_map_query_caller(int numOfArgs);
    int bml_connection_map_caller(int numOfArgs);
    int bml_get_device_operational_radios_caller(int numOfArgs);
    int stat_register_cb_caller(int numOfArgs);
    int events_register_cb_caller(int numOfArgs);
    int set_wifi_credentials_caller(int numOfArgs);
    int clear_wifi_credentials_caller(int numOfArgs);
    int update_wifi_credentials_caller(int numOfArgs);
    int get_wifi_credentials_caller(int numOfArgs);
    int set_onboarding_state_caller(int numOfArgs);
    int get_onboarding_state_caller(int numOfArgs);
    int wps_onboarding_caller(int numOfArgs);

    int get_bml_version_caller(int numOfArgs);
    int get_master_slave_versions_caller(int numOfArgs);

    int get_unassociated_station_stats_caller(int numOfArgs);
    int add_unassociated_station_stats_caller(int numOfArgs);
    int remove_unassociated_station_stats_caller(int numOfArgs);

    int enable_legacy_client_roaming_caller(int numOfArgs);
    int enable_client_roaming_caller(int numOfArgs);
    int enable_client_roaming_11k_support_caller(int numOfArgs);
    int enable_client_roaming_prefer_signal_strength_caller(int numOfArgs);
    int enable_client_band_steering_caller(int numOfArgs);
    int enable_ire_roaming_caller(int numOfArgs);
    int enable_load_balancer_caller(int numOfArgs);
    int enable_service_fairness_caller(int numOfArgs);
    int enable_dfs_reentry_caller(int numOfArgs);
    int enable_certification_mode_caller(int numOfArgs);
    int set_log_level_caller(int numOfArgs);
    int set_global_restricted_channels_caller(int numOfArgs);
    int get_global_restricted_channels_caller(int numOfArgs);
    int set_slave_restricted_channels_caller(int numOfArgs);
    int get_slave_restricted_channels_caller(int numOfArgs);
    int bml_trigger_topology_discovery_caller(int numOfArgs);
    int bml_channel_selection_caller(int numOfArgs);
    int bml_set_selection_channel_pool_caller(int numOfArgs);
    int bml_get_selection_channel_pool_caller(int numOfArgs);
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    int bml_pre_association_steering_set_group_caller(int numOfArgs);
    int bml_pre_association_steering_client_set_caller(int numOfArgs);
    int bml_pre_association_steering_event_register_caller(int numOfArgs);
    int bml_pre_association_steering_client_measure_caller(int numOfArgs);
    int bml_pre_association_steering_client_disconnect_caller(int numOfArgs);
#endif
    int set_dcs_continuous_scan_enable_caller(int numOfArgs);
    int get_dcs_continuous_scan_enable_caller(int numOfArgs);
    int set_dcs_continuous_scan_params_caller(int numOfArgs);
    int get_dcs_continuous_scan_params_caller(int numOfArgs);
    int start_dcs_single_scan_caller(int numOfArgs);
    int get_dcs_scan_results_caller(int numOfArgs);
    int client_get_client_list_caller(int numOfArgs);
    int client_set_client_caller(int numOfArgs);
    int client_get_client_caller(int numOfArgs);
    int client_clear_client_caller(int numOfArgs);
    int send_unassoc_sta_rcpi_query_caller(int numOfArgs);
    int get_unassoc_sta_rcpi_result_caller(int numOfArgs);
    // Functions
    int onboard_status();
    int ping();
    int nw_map_register_update_cb(const std::string &optional = std::string());
    int nw_map_query();
    int connection_map();
    int get_device_operational_radios(const std::string &al_mac);
    int stat_register_cb(const std::string &optional = std::string());
    int events_register_cb(const std::string &optional = std::string());
    int set_wifi_credentials(const std::string &al_mac, const std::string &ssid,
                             const std::string &network_key = "",
                             const std::string &bands       = "24g-5g",
                             const std::string &bss_type = "fronthaul", bool add_sae = false);
    int clear_wifi_credentials(const std::string &al_mac);
    int update_wifi_credentials();
    int get_wifi_credentials(int vap_id = 0);
    int set_onboarding_state(int enable);
    int get_onboarding_state();
    int wps_onboarding(const std::string &iface = std::string());

    int get_bml_version();
    int get_master_slave_versions();

    int client_allow(const std::string &client_mac, const std::string &hostap_mac);
    int client_disallow(const std::string &client_mac, const std::string &hostap_mac);

    int enable_legacy_client_roaming(int8_t isEnable = -1);
    int enable_client_roaming(int8_t isEnable = -1);
    int enable_client_roaming_11k_support(int8_t isEnable = -1);
    int enable_client_roaming_prefer_signal_strength(int8_t isEnable = -1);
    int enable_client_band_steering(int8_t isEnable = -1);
    int enable_ire_roaming(int8_t isEnable = -1);
    int enable_load_balancer(int8_t isEnable = -1);
    int enable_service_fairness(int8_t isEnable = -1);
    int enable_dfs_reentry(int8_t isEnable = -1);
    int enable_certification_mode(int8_t isEnable = -1);
    int set_log_level(const std::string &module_name, const std::string &log_level, uint8_t on,
                      const std::string &mac = net::network_utils::WILD_MAC_STRING);
    int set_global_restricted_channels(const std::string &restricted_channels);
    int get_global_restricted_channels();
    int set_slave_restricted_channels(const std::string &restricted_channels,
                                      const std::string &hostap_mac);
    int get_slave_restricted_channels(const std::string &hostap_mac);
    int topology_discovery(const std::string &al_mac);
    int channel_selection(const std::string &radio_mac, uint8_t channel, uint8_t bw,
                          uint8_t csa_count = 5);
    int set_selection_pool(const std::string &radio_mac, const std::string &channel_pool);
    int get_selection_pool(const std::string &radio_mac);
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    int steering_set_group(uint32_t steeringGroupIndex,
                           const std::vector<std::string> &str_ap_cfgs);
    int steering_client_set(uint32_t steeringGroupIndex, const std::string &str_bssid,
                            const std::string &str_client_mac,
                            const std::string &str_config = std::string());
    int steering_event_register(const std::string &optional = std::string());
    int steering_client_measure(uint32_t steeringGroupIndex, const std::string &str_bssid,
                                const std::string &str_client_mac);
    int steering_client_disconnect(uint32_t steeringGroupIndex, const std::string &str_bssid,
                                   const std::string &str_client_mac, uint32_t type,
                                   uint32_t reason);
#endif /* FEATURE_PRE_ASSOCIATION_STEERING */
    int set_dcs_continuous_scan_enable(const std::string &radio_mac, int8_t enable);
    int get_dcs_continuous_scan_enable(const std::string &radio_mac);
    int set_dcs_continuous_scan_params(const std::string &radio_mac, int32_t dwell_time,
                                       int32_t interval_time, const std::string &channel_pool);
    int get_dcs_continuous_scan_params(const std::string &radio_mac);
    int start_dcs_single_scan(const std::string &radio_mac, int32_t dwell_time,
                              const std::string &channel_pool);
    int get_dcs_scan_results(const std::string &radio_mac, uint32_t max_results_size,
                             bool is_single_scan = false);
    int client_get_client_list();
    int client_set_client(const std::string &sta_mac, int8_t selected_bands,
                          int8_t stay_on_initial_radio, int32_t time_life_delay_minutes);
    int client_get_client(const std::string &sta_mac);
    int client_clear_client(const std::string &sta_mac);
    int send_unassoc_sta_rcpi_query(const std::string &sta_mac, int16_t opclass, int16_t channel);
    int get_unassoc_sta_rcpi_query_result(const std::string &sta_mac);
    template <typename T> const std::string string_from_int_array(T *arr, size_t arr_max_size);
    // Variable
    std::string beerocks_conf_path;
    BML_CTX ctx = nullptr;
    char print_buffer[PRINT_BUFFER_LENGTH];
    int is_onboarding;
    bool pending_response = false;

    bool is_analyzer = false;
    static SocketClient *m_analyzer_socket;
    SocketSelect select;
    std::unordered_multimap<std::string, std::shared_ptr<conn_map_node_t>>
        conn_map_nodes; // key=parent_mac, val=conn_map_node_t

    uint8_t rx_buffer[message::MESSAGE_BUFFER_LENGTH];

    //unassociated stations stats
    int get_unassociated_stations_stats();
    int add_unassociated_station_stats(const std::string &mac_address, const std::string &channel,
                                       const std::string &operating_class,
                                       const std::string &agent_mac_addr);
    int remove_unassociated_station_stats(std::string &mac_address,
                                          const std::string &agent_mac_addr);
};
} // namespace beerocks

#endif // _BEEROCKS_CLI_BML_H
