/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_INTERNAL_H_
#define _BML_INTERNAL_H_

#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_promise.h>
#include <bcl/beerocks_socket_thread.h>
#include <bcl/son/son_wireless_utils.h>

#include <beerocks/tlvf/beerocks_message_common.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_platform.h>

#include "bml_defs.h"

#include <list>
#include <map>
#include <mutex>

#ifdef FEATURE_PRE_ASSOCIATION_STEERING
#include "bml_pre_association_steering_defs.h"
#endif

class bml_internal : public beerocks::socket_thread {

public:
    bml_internal();
    ~bml_internal();

    // Initialize and connect to platform/master
    int connect(const std::string &beerocks_conf_path);

    // Ping the master
    int ping();

    // Register a callback for the network map query results
    void register_nw_map_query_cb(BML_NW_MAP_QUERY_CB pCB);

    // Register a callback for the network map update results
    int register_nw_map_update_cb(BML_NW_MAP_QUERY_CB pCB);

    // Query the beerocks master for the network map
    int nw_map_query();

    // Query the beerocks master for the network map
    int device_oper_radios_query(BML_DEVICE_DATA *device_data);

    // Register topology discovery query so bml listener could receive the response event.
    int register_topology_discovery_response();

    // Unregister topology discovery response events.
    int unregister_topology_discovery_response();

    // Register a callback for the statistcs results
    int register_stats_cb(BML_STATS_UPDATE_CB pCB);

    // Register a callback for events
    int register_event_cb(BML_EVENT_CB pCB);

    /**
    * @brief Add the Wi-Fi credentials for the beerocks network.
    * 
    * @param [in] al_mac The agent mac adress
    * @param [in] wifi_credentials Structure with credentials (ssid, network_key, etc)
    * 
    * @return BML_RET_OK on success.
    */
    int set_wifi_credentials(const sMacAddr &al_mac,
                             const son::wireless_utils::sBssInfoConf &wifi_credentials);

    /**
    * @brief Clear wifi credentials for specific AL-MAC
    *
    * @param al_mac al_mac client AL-MAC address
    *
    * @return BML_RET_OK if success, error code otherwise
    */
    int clear_wifi_credentials(const sMacAddr &al_mac);

    /**
    * @brief Update wifi credentials.
    *
    * @return BML_RET_OK if success, error code otherwise
    */
    int update_wifi_credentials();

    // Get wireless SSID and security
    int get_wifi_credentials(int vap_id, char *ssid, char *pass, int *sec);

    // get the platform onboarding state
    int get_onboarding_state(int *enable);

    // set the platform onboarding state
    int set_onboarding_state(int enable);

    // wps onboarding
    int bml_wps_onboarding(const char *iface);

    // Get administrator user credentials
    int get_administrator_credentials(char *user_password);

    // Enable/Disable client roaming
    int set_client_roaming(bool enable);

    // Return client roaming status (in res)
    int get_client_roaming(int &res);

    // Enable/Disable 11k feature support
    int set_client_roaming_11k_support(bool enable);

    // Return client 11k feature support (in res)
    int get_client_roaming_11k_support(int &res);
    //
    // Enable/Disable legacy client roaming
    int set_legacy_client_roaming(bool enable);

    // Return legacy client roaming status (in res)
    int get_legacy_client_roaming(int &res);

    // Enable/Disable client roaming prefer by signal_strength
    int set_client_roaming_prefer_signal_strength(bool enable);

    // Return client roaming prefer signal strength status (in res)
    int get_client_roaming_prefer_signal_strength(int &res);

    // Enable/Disable band steering
    int set_client_band_steering(bool enable);

    // Return band steering status (in res)
    int get_client_band_steering(int &res);

    // Enable/Disable ire roaming
    int set_ire_roaming(bool enable);

    // Return ire roaming status (in res)
    int get_ire_roaming(int &res);

    // Enable/Disable load_balancer
    int set_load_balancer(bool enable);

    // Return load_balancer status (in res)
    int get_load_balancer(int &res);

    // Enable/Disable service_fairness
    int set_service_fairness(bool enable);

    // Return service_fairness status (in res)
    int get_service_fairness(int &res);

    // Enable/Disable service_fairness
    int set_dfs_reentry(bool enable);

    // Return service_fairness status (in res)
    int get_dfs_reentry(int &res);

    // Enable/Disable certification mode
    int set_certification_mode(bool enable);

    // Return certification mode enable (in res)
    int get_certification_mode(int &res);

    // Set log level
    int set_log_level(const std::string &module_name, const std::string &log_level, uint8_t on,
                      const std::string &mac);

    // Return master & slave version
    int get_master_slave_versions(char *master_version, char *slave_version);

    // set global/slave restricted channel
    int set_restricted_channels(const uint8_t *restricted_channels, const std::string &mac,
                                uint8_t is_global, uint8_t size);

    // get global/slave restricted channel
    int get_restricted_channels(uint8_t *restricted_channels, const std::string &mac,
                                uint8_t is_global);

    // triggers topology discovery
    int trigger_topology_discovery_query(const char *al_mac);

    // triggers channel selection on specific Agent
    int channel_selection(const sMacAddr &radio_mac, uint8_t channel, uint8_t bandwidth,
                          uint8_t csa_count = 5);

    // Set the channel pool for the Auto Channel Selection
    int set_selection_channel_pool(const sMacAddr &radio_mac, const unsigned int *channel_pool,
                                   const int channel_pool_size);
    // Get the channel pool for the Auto Channel Selection
    int get_selection_channel_pool(const sMacAddr &radio_mac, unsigned int *channel_pool,
                                   int *channel_pool_size);

    //set and get vaps list
    int bml_set_vap_list_credentials(const BML_VAP_INFO *vaps, const uint8_t vaps_num);
    int bml_get_vap_list_credentials(BML_VAP_INFO *vaps, uint8_t &vaps_num);

    /**
    * @brief Enables or disables beerocks DCS continuous scans.
    *
    * @param [in] radio_mac Radio MAC of selected radio
    * @param [in] enable    Value of 1 to enable or 0 to disable.
    *
    * @return BML_RET_OK on success.
    */
    int set_dcs_continuous_scan_enable(const sMacAddr &mac, int enable);

    /**
    * @brief Sends unassoc rcpi query to capable agents.
    *
    * @param [in] sta_mac Unassociated sta MAC.
    * @param [in] opclass    operating class.
    * @param [in] channel    channel from operating class.
    *
    * @return BML_RET_OK on success.
    */
    int send_unassoc_sta_rcpi_query(const sMacAddr &mac, int16_t opclass, int16_t channel);

    /**
    * @brief fetches the unassoc sta link metrics from db
    *
    * @param [in] sta_mac Unassociated sta MAC.
    *
    * @return BML_RET_OK on success.
    */
    int get_unassoc_sta_rcpi_query_result(const sMacAddr &mac,
                                          struct BML_UNASSOC_STA_LINK_METRIC *sta_info);

    /**
    * @brief Get DCS continuous scans param.
    *
    * @param [in] mac     Radio MAC of selected radio
    * @param [out] enable A reference for the result to be stored in.
    *
    * @return BML_RET_OK on success.
    */
    int get_dcs_continuous_scan_enable(const sMacAddr &mac, int &enable);

    /**
    * @brief Set DCS continuous scan params.
    *
    * @param [in] mac               Radio MAC of selected radio
    * @param [in] dwell_time        Set the dwell time in milliseconds.
    * @param [in] interval_time     Set the interval time in seconds.
    * @param [in] channel_pool      Set the channel pool for the DCS.
    * @param [in] channel_pool_size Set the DCS channel pool size.
    *
    * @return BML_RET_OK on success.
    */
    int set_dcs_continuous_scan_params(const sMacAddr &mac, int dwell_time, int interval_time,
                                       unsigned int *channel_pool, int channel_pool_size);

    /**
    * @brief Get DCS continuous scan params.
    *
    * @param [in] mac                Radio MAC of selected radio
    * @param [out] dwell_time        Get the dwell time in milliseconds.
    * @param [out] interval_time     Get the interval time in seconds.
    * @param [out] channel_pool      Get the channel pool for the DCS.
    * @param [out] channel_pool_size Get the DCS channel pool size.
    *
    * @return BML_RET_OK on success.
    */
    int get_dcs_continuous_scan_params(const sMacAddr &mac, int *dwell_time, int *interval_time,
                                       unsigned int *channel_pool, int *channel_pool_size);

    /**
    * @brief Get DCS channel scan results.
    *
    * @param [in] mac              Radio MAC of selected radio
    * @param [out] results         Returning results.
    * @param [out] results_size    Returning results size.
    * @param [in] max_results_size Max requested results
    * @param [out] result_status   Returning status of results
    * @param [in] is_single_scan   Flag, if the results should be from a single scan or continuous
    * 
    * @return BML_RET_OK on success.
    */
    int get_dcs_scan_results(const sMacAddr &mac, BML_NEIGHBOR_AP *results,
                             unsigned int &results_size, const unsigned int max_results_size,
                             uint8_t &result_status, bool is_single_scan);

    /**
    * Start a single DCS scan with parameters.
    *
    * @param [in] mac                  Radio MAC of selected radio
    * @param [in] dwell_time_ms        Set the dwell time in milliseconds.
    * @param [in] channel_pool         Set the channel pool for the DCS.
    * @param [in] channel_pool_size    Set the DCS channel pool size.
    *
    * @return BML_RET_OK on success.
    */
    int start_dcs_single_scan(const sMacAddr &mac, int dwell_time_ms, unsigned int *channel_pool,
                              int channel_pool_size);

    /**
     * Get client list.
     *
     * @param [in,out] client_list List of MAC addresses sepereted by a comma.
     * @param [in,out] client_list_size Size of client list.
     * @return BML_RET_OK on success.
     */
    int client_get_client_list(char *client_list, unsigned int *client_list_size);

    /**
     * Add a station to the unassociated stations 
     * @param [in] mac_address address of the station
     * @param [in] desired channel, Access point  might  still decide to use its active channel instead
     * @param [in] desired operating_class.Access point  might  still decide to use its active operating_class.
     * @param [in] mac_add of the agent that will be monitoring the station. IF empty, all connected agents will be selected.
     * @return BML_RET_OK on success.
     */
    int add_unassociated_station_stats(const char *mac_address, const char *channel_str,
                                       const char *operating_class, const char *agent_mac_address);

    /**
     * Remove a station from the unassociated stations 
     * @param [in] mac_address address of the station
     * @param [in] mac_add of monitoring station. IF empty, all agents will be selected.
     * @return BML_RET_OK on success,BML_RET_OP_FAILED is station does not exist or any other issue.
     */
    int remove_unassociated_station_stats(const char *mac_address, const char *agent_mac_address);
    /**
     * Get unassociated stations stats.
     *
     * @param [out] pointer where to write the status 
     * @param [out] stats_results_size Size number of char being written to the buffer stats_results
     * @return BML_RET_OK on success.
     */
    int get_un_stations_stats(char *stats_results, unsigned int *stats_results_size);

    /**
     * Set client configuration.
     *
     * @param [in] sta_mac MAC address of a station.
     * @param [in] client_config Client configuration to be set.
     * @return BML_RET_OK on success.
     */
    int client_set_client(const sMacAddr &sta_mac, const BML_CLIENT_CONFIG &client_config);

    /**
     * Get client info.
     *
     * @param [in] sta_mac MAC address of a station.
     * @param [in,out] client Client information.
     * @return BML_RET_OK on success.
     */
    int client_get_client(const sMacAddr &sta_mac, BML_CLIENT *client);

    /**
     * @brief Delete client persistent DB info.
     *
     * @param [in] sta_mac MAC address of a station.
     * @return BML_RET_OK on success, BML_RET_OP_FAILED on failure.
     */
    int client_clear_client(const sMacAddr &sta_mac);

#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    /*
    * A steering group defines a group of apIndex's which can have steering done
    * between them.
    * To remove a group configuration call with NULL as ap_cfgs, and length as 0.
    * @param[in] steeringGroupIndex  Wifi Steering Group index
    * @param[in] ap_cfgs               Array of AP Configurations.
    * @param[in] length                The number of AP Configurations in the array. Cannot be above 3.
    *
    * @return BML_RET_OK on success.
    *
    * @warning All apIndex's provided within a group must have the same SSID,
    * encryption, and passphrase configured for steering to function properly.
    *
    */
    int steering_set_group(uint32_t steeringGroupIndex, BML_STEERING_AP_CONFIG *ap_cfgs,
                           size_t length);
    /**
     * Call this function to add/modify per-client configuration config of client_mac.
     * To remove a client configuration call with NULL as config.
     * @param[in] steeringGroupIndex   Wifi Steering Group index
     * @param[in] bssid                AP bssid.
     * @param[in] client_mac           The Client's MAC address.
     * @param[in] config               The client configuration
     * 
     * @return RETURN_OK on success.
     */
    int steering_client_set(uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                            const BML_MAC_ADDR client_mac, BML_STEERING_CLIENT_CONFIG *config);
    /**
     * Call this function to register/unregister the callback function.
     *
     * @param[in] pCB  a callback function pointer or NULL to unregister.
     * 
     * @return RETURN_OK on success.
     */
    int steering_event_register(BML_EVENT_CB pCB);
    /** 
    *
    * @param[in] steeringGroupIndex  Wifi Steering Group index
    * @param[in] bssid               AP bssid.   
    * @param[in] client_mac           The Client's MAC address.
    *
    * @return BML_RET_OK on success.
    *
    */
    int steering_client_measure(uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                                const BML_MAC_ADDR client_mac);
    /**Initiate a Client Disconnect.
     *
     * This is used to kick off a client, for steering purposes.
     *
     * @param[in]  steeringgroupIndex  Wifi Steering Group index
     * @param[in]  bssid               AP bssid.
     * @param[in]  client_mac          The Client's MAC address
     * @param[in]  type                Disconnect Type
     * @param[in]  reason              Reason code to provide in deauth/disassoc frame.
     *
     * @return BML_RET_OK on success.
     */
    int steering_client_disconnect(uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                                   const BML_MAC_ADDR client_mac, BML_DISCONNECT_TYPE type,
                                   uint32_t reason);

#endif /* FEATURE_PRE_ASSOCIATION_STEERING */

    /*
 * Public static methods:
 */
public:
    // Set easylogging context
    static int set_log_context(void *log_ctx);

    /*
 * Public getter/setter methods:
 */
public:
    void set_user_data(void *pUserData) { m_pUserData = pUserData; }

    void *get_user_data() const { return (m_pUserData); }

    bool is_onboarding() const { return (m_fOnboarding); }

    bool is_local_master() const { return (m_fLocal_Master); }

protected:
    virtual bool init() override;
    virtual void on_thread_stop() override;
    virtual bool socket_disconnected(Socket *sd) override;
    virtual std::string print_cmdu_types(const beerocks::message::sUdsHeader *cmdu_header) override;
    bool wake_up(uint8_t action_opcode, int value);
    bool connect_to_master();
    virtual int process_cmdu_header(std::shared_ptr<beerocks::beerocks_header> beerocks_header);

    SocketClient *m_sockMaster = nullptr;

private:
    bool initialize(const std::string &beerocks_conf_path);
    bool connect_to_platform();

    bool handle_nw_map_query_update(int elements_num, int last_node, void *data_buffer,
                                    bool is_query);
    bool handle_stats_update(int elements_num, void *data_buffer);
    bool handle_event_update(uint8_t *data_buffer);
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    // Send message contained in cmdu to m_sockMaster,
    int send_bml_cmdu(int &result, uint8_t action_op);

private:
    std::string m_strBeerocksConfPath;
    beerocks::config_file::sConfigSlave m_sConfig;
    bool m_fOnboarding   = false;
    bool m_fLocal_Master = false;

    void *m_pUserData            = nullptr;
    SocketClient *m_sockPlatform = nullptr;

    std::mutex m_mtxLock;
    beerocks::promise<bool> *m_prmPing                  = nullptr;
    beerocks::promise<bool> *m_prmGetVapListCreds       = nullptr;
    beerocks::promise<bool> *m_prmSetVapListCreds       = nullptr;
    beerocks::promise<bool> *m_prmOnboard               = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsSet    = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsUpdate = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsClear  = nullptr;
    beerocks::promise<bool> *m_prmWiFiCredentialsGet    = nullptr;
    beerocks::promise<bool> *m_prmAdminCredentialsGet   = nullptr;
    beerocks::promise<bool> *m_prmDeviceDataGet         = nullptr;
    beerocks::promise<bool> *m_prmMasterSlaveVersions   = nullptr;
    beerocks::promise<bool> *m_prmLocalMasterGet        = nullptr;
    beerocks::promise<bool> *m_prmRestrictedChannelsGet = nullptr;
    //Promise used to indicate the GetParams response was received
    beerocks::promise<bool> *m_prmChannelScanParamsGet = nullptr;
    //Promise used to indicate the GetResults response was received
    beerocks::promise<int> *m_prmChannelScanResultsGet   = nullptr;
    beerocks::promise<bool> *m_prmClientListGet          = nullptr;
    beerocks::promise<bool> *m_prmUnStationsStatsGet     = nullptr;
    beerocks::promise<bool> *m_prmClientGet              = nullptr;
    beerocks::promise<bool> *m_prmSelectionPoolGet       = nullptr;
    beerocks::promise<int> *m_prmUnAssocStaLinkMetricGet = nullptr;

    std::map<uint8_t, beerocks::promise<int> *> m_prmCliResponses;

    // Callback functions
    BML_NW_MAP_QUERY_CB m_cbNetMapQuery  = nullptr;
    BML_NW_MAP_QUERY_CB m_cbNetMapUpdate = nullptr;
    BML_STATS_UPDATE_CB m_cbStatsUpdate  = nullptr;
    BML_EVENT_CB m_cbEvent               = nullptr;

    beerocks_message::sDeviceData *m_device_data                 = nullptr;
    beerocks_message::sWifiCredentials *m_wifi_credentials       = nullptr;
    beerocks_message::sAdminCredentials *m_admin_credentials     = nullptr;
    beerocks_message::sVersions *m_master_slave_versions         = nullptr;
    beerocks_message::sRestrictedChannels *m_Restricted_channels = nullptr;
    //m_scan_params is used when receiving the channel scan parameters
    beerocks_message::sChannelScanRequestParams *m_scan_params = nullptr;
    //m_scan_results is used when receiving channel scan results
    std::list<beerocks_message::sChannelScanResults> *m_scan_results = nullptr;
    //m_scan_results_status is used to store the results' latest status
    uint8_t *m_scan_results_status = nullptr;
    //m_scan_results_maxsize is used to indicate the maximum capacity of the requested results
    uint32_t *m_scan_results_maxsize       = nullptr;
    std::list<sMacAddr> *m_client_list     = nullptr;
    uint32_t *m_client_list_size           = nullptr;
    BML_CLIENT *m_client                   = nullptr;
    BML_VAP_INFO *m_vaps                   = nullptr;
    uint8_t *m_pvaps_list_size             = nullptr;
    std::vector<uint8_t> *m_selection_pool = nullptr;
    uint32_t *m_selection_pool_size        = nullptr;
    std::string m_un_stations_stats;
    uint16_t id = 0;
    static bool s_fExtLogContext;
#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    bool handle_steering_event_update(uint8_t *data_buffer);
    beerocks::promise<int> *m_prmPreAssociationSteering = nullptr;
    BML_EVENT_CB m_cbSteeringEvent                      = nullptr;
#endif /* FEATURE_PRE_ASSOCIATION_STEERING */
    struct BML_UNASSOC_STA_LINK_METRIC *m_unassoc_sta_link_metric = nullptr;
};

#endif /* _BML_INTERNAL_H_ */
