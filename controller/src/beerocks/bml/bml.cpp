/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml.h"
#include "internal/bml_internal.h"

#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_string_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>

#include <easylogging++.h>

using namespace beerocks::net;

int bml_configure_external_logging(const char *beerocks_conf_dir_path,
                                   const unsigned int path_length, const char *module_name,
                                   const unsigned int module_name_length)
{
    if (!beerocks_conf_dir_path || (path_length == 0) || !module_name ||
        (module_name_length == 0)) {
        std::cout << "ERROR! Failed to configure BML logging - Invalid input!" << std::endl;
        return (-BML_RET_INVALID_ARGS);
    }

    // read controller config file
    std::string controller_config_file_path =
        std::string(beerocks_conf_dir_path) + "/" + std::string(BEEROCKS_CONTROLLER) +
        ".conf"; //search first in platform-specific default directory
    beerocks::config_file::sConfigMaster beerocks_controller_conf;
    if (!beerocks::config_file::read_master_config_file(controller_config_file_path,
                                                        beerocks_controller_conf)) {
        return (-BML_RET_OP_FAILED);
    }

    beerocks::logging external_app_logger(module_name, beerocks_controller_conf.sLog);
    // Init logger
    external_app_logger.apply_settings();

    LOG(DEBUG) << "BML logger configuration completed successfully";

    return (BML_RET_OK);
}

int bml_connect(BML_CTX *ctx, const char *beerocks_conf_path, void *user_data)
{
    if (!ctx) {
        LOG(ERROR) << "bml_connect - ctx is null!";
        return (-BML_RET_INVALID_ARGS);
    }

    // Clear context pointer
    *ctx = nullptr;

    // Create a new internal BML class instance
    bml_internal *pBML = new bml_internal();
    if (pBML == nullptr) {
        LOG(ERROR) << "bml_connect - bml_internal creation failed";
        return (-BML_RET_MEM_FAIL);
    }

    pBML->set_user_data(user_data);

    // Start the BML thread
    if (pBML->start("BML") == false) {
        LOG(ERROR) << "bml_connect - pBML->start failed";
        delete pBML;
        return (-BML_RET_INIT_FAIL);
    }

    // Connect to the platform
    int iRet;
    if ((iRet = pBML->connect(beerocks_conf_path)) != BML_RET_OK) {

        LOG(ERROR) << "bml_connect - pBML->connect failed";

        // Stop the BML thread (and wait for it to stop...)
        pBML->stop(true);

        delete pBML;
        return (iRet);
    }

    // Store the context
    (*ctx) = pBML;

    return (BML_RET_OK);
}

int bml_disconnect(BML_CTX ctx)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    // Stop the BML thread (and wait for it to stop...)
    pBML->stop(true);

    // Delete the instance
    delete pBML;
    pBML = nullptr;

    return (BML_RET_OK);
}

int bml_onboard_status(BML_CTX ctx)
{
    if (!ctx)
        return (-1);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->is_onboarding() ? 1 : 0);
}

int bml_local_master_enabled(BML_CTX ctx)
{
    if (!ctx)
        return (-1);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->is_local_master() ? 1 : 0);
}

void *bml_get_user_data(BML_CTX ctx)
{
    if (!ctx)
        return (NULL);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->get_user_data());
}

int bml_set_vap_list_credentials(BML_CTX ctx, BML_VAP_INFO *vap_list, const uint8_t vaps_num)
{
    // Validate input parameters
    if (!ctx || !vap_list)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->bml_set_vap_list_credentials(vap_list, vaps_num));
}

int bml_get_vap_list_credentials(BML_CTX ctx, BML_VAP_INFO *vap_list, uint8_t *vaps_num)
{
    if (!ctx || !vap_list || !vaps_num)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->bml_get_vap_list_credentials(vap_list, *vaps_num));
}

int bml_ping(BML_CTX ctx)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->ping());
}

int bml_nw_map_register_query_cb(BML_CTX ctx, BML_NW_MAP_QUERY_CB cb)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    pBML->register_nw_map_query_cb(cb);

    return (BML_RET_OK);
}

int bml_nw_map_register_update_cb(BML_CTX ctx, BML_NW_MAP_QUERY_CB cb)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    pBML->register_nw_map_update_cb(cb);

    return (BML_RET_OK);
}

int bml_nw_map_query(BML_CTX ctx)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->nw_map_query());
}

int bml_device_oper_radios_query(BML_CTX ctx, struct BML_DEVICE_DATA *device_data)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->device_oper_radios_query(device_data));
}

int bml_stat_register_cb(BML_CTX ctx, BML_STATS_UPDATE_CB cb)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return pBML->register_stats_cb(cb);
}

int bml_event_register_cb(BML_CTX ctx, BML_EVENT_CB cb)
{
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return pBML->register_event_cb(cb);
}

int bml_set_wifi_credentials(BML_CTX ctx, const char *al_mac, const char *ssid,
                             const char *network_key, const char *bands, const char *bss_type,
                             bool add_sae)
{
    sMacAddr al_mac_addr;
    son::wireless_utils::sBssInfoConf wifi_credentials;

    // Validate input parameters
    if (!ctx || !al_mac || !ssid)
        return (-BML_RET_INVALID_ARGS);

    al_mac_addr           = tlvf::mac_from_string(al_mac);
    wifi_credentials.ssid = ssid;

    if (network_key) {
        wifi_credentials.network_key = network_key;
        wifi_credentials.authentication_type =
            add_sae ? WSC::eWscAuth(WSC::eWscAuth::WSC_AUTH_WPA2PSK | WSC::eWscAuth::WSC_AUTH_SAE)
                    : WSC::eWscAuth::WSC_AUTH_WPA2PSK;
        wifi_credentials.encryption_type = WSC::eWscEncr::WSC_ENCR_AES;
    } else {
        wifi_credentials.authentication_type = WSC::eWscAuth::WSC_AUTH_OPEN;
        wifi_credentials.encryption_type     = WSC::eWscEncr::WSC_ENCR_NONE;
    }

    if (!bands) {
        bands = "24g-5g";
    }

    wifi_credentials.operating_class = son::wireless_utils::string_to_wsc_oper_class(bands);
    if (wifi_credentials.operating_class.empty()) {
        LOG(ERROR) << "Wrong operating class value.";
        return (-BML_RET_INVALID_ARGS);
    }

    if (!bss_type) {
        bss_type = "fronthaul";
    }

    std::string bss_type_str = std::string(bss_type);
    wifi_credentials.fronthaul =
        (bss_type_str == "fronthaul" || bss_type_str == "fronthaul-backhaul");
    wifi_credentials.backhaul =
        (bss_type_str == "backhaul" || bss_type_str == "fronthaul-backhaul");

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->set_wifi_credentials(al_mac_addr, wifi_credentials));
}

int bml_clear_wifi_credentials(BML_CTX ctx, const char *al_mac)
{

    if (!al_mac)
        return (-BML_RET_INVALID_ARGS);

    auto al_mac_addr   = tlvf::mac_from_string(al_mac);
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->clear_wifi_credentials(al_mac_addr));
}

int bml_update_wifi_credentials(BML_CTX ctx)
{
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->update_wifi_credentials());
}

int bml_get_wifi_credentials(BML_CTX ctx, int vap_id, char *ssid, char *pass, int *sec)
{
    // Validate input parameters
    if (!ctx || !ssid || !sec)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->get_wifi_credentials(vap_id, ssid, pass, sec));
}

int bml_get_onboarding_state(BML_CTX ctx, int *enable)
{
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return pBML->get_onboarding_state(enable);
}

int bml_set_onboarding_state(BML_CTX ctx, int enable)
{
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return pBML->set_onboarding_state(enable);
}

int bml_wps_onboarding(BML_CTX ctx, const char *iface)
{
    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return pBML->bml_wps_onboarding(iface);
}

int bml_get_administrator_credentials(BML_CTX ctx, char *user_password)
{
    // Validate input parameters
    if (!ctx || !user_password)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->get_administrator_credentials(user_password));
}

int bml_set_client_roaming(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    // Set the client roaming configuration
    // TODO: Propogate error code from bml_internal...
    return (pBML->set_client_roaming(enable));
}

int bml_get_client_roaming(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_client_roaming(*res));
}

int bml_set_client_roaming_11k_support(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    // Set the client roaming configuration
    // TODO: Propogate error code from bml_internal...
    return (pBML->set_client_roaming_11k_support(enable));
}

int bml_get_client_roaming_11k_support(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_client_roaming_11k_support(*res));
}

int bml_set_legacy_client_roaming(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    // Set the client roaming configuration
    // TODO: Propogate error code from bml_internal...
    return (pBML->set_legacy_client_roaming(enable));
}

int bml_get_legacy_client_roaming(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_legacy_client_roaming(*res));
}

int bml_set_client_roaming_prefer_signal_strength(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    // Set the client roaming configuration
    // TODO: Propogate error code from bml_internal...
    return (pBML->set_client_roaming_prefer_signal_strength(enable));
}

int bml_get_client_roaming_prefer_signal_strength(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_client_roaming_prefer_signal_strength(*res));
}

int bml_set_client_band_steering(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->set_client_band_steering(enable));
}

int bml_get_client_band_steering(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_client_band_steering(*res));
}

int bml_set_ire_roaming(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->set_ire_roaming(enable));
}

int bml_get_ire_roaming(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_ire_roaming(*res));
}

int bml_set_load_balancer(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->set_load_balancer(enable));
}

int bml_get_load_balancer(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_load_balancer(*res));
}

int bml_set_service_fairness(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->set_service_fairness(enable));
}

int bml_get_service_fairness(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_service_fairness(*res));
}

int bml_set_dfs_reentry(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->set_dfs_reentry(enable));
}

int bml_get_dfs_reentry(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_dfs_reentry(*res));
}

int bml_set_certification_mode(BML_CTX ctx, int enable)
{
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->set_certification_mode(enable));
}

int bml_get_certification_mode(BML_CTX ctx, int *res)
{
    // Validate input parameters
    if (!ctx || !res)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->get_certification_mode(*res));
}

int bml_set_log_level(BML_CTX ctx, const char *module_name, const char *log_level, uint8_t on,
                      const char *mac)
{
    // Validate input parameters
    if (!ctx || !module_name || !log_level || !mac)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    return (pBML->set_log_level(module_name, log_level, on, mac));
}

int bml_get_master_slave_versions(BML_CTX ctx, char *master_version, char *slave_version)
{
    // Validate input parameters
    if (!ctx || !master_version || !slave_version)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    if (pBML->is_local_master()) {
        beerocks::string_utils::copy_string(master_version, bml_get_bml_version(), BML_VERSION_LEN);
        beerocks::string_utils::copy_string(slave_version, bml_get_bml_version(), BML_VERSION_LEN);
        return BML_RET_OK;
    }

    return (pBML->get_master_slave_versions(master_version, slave_version));
}

int bml_set_local_log_context(void *log_ctx) { return (bml_internal::set_log_context(log_ctx)); }

const char *bml_get_bml_version() { return (BEEROCKS_VERSION); }

int bml_set_global_restricted_channels(BML_CTX ctx, const uint8_t *restricted_channels,
                                       uint8_t size)
{
    // Validate input parameters
    if (!ctx || !restricted_channels || !size)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    LOG(WARNING) << "bml_set_global_restricted_channels entry";

    return (pBML->set_restricted_channels(restricted_channels, network_utils::ZERO_MAC_STRING, 1,
                                          size));
}

int bml_get_global_restricted_channels(BML_CTX ctx, uint8_t *restricted_channels)
{
    // Validate input parameters
    if (!ctx || !restricted_channels)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->get_restricted_channels(restricted_channels, network_utils::ZERO_MAC_STRING, 1));
}

int bml_set_slave_restricted_channels(BML_CTX ctx, const uint8_t *restricted_channels,
                                      const char *mac, uint8_t size)
{
    // Validate input parameters
    if (!ctx || !restricted_channels || !mac || !size)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);
    LOG(WARNING) << "bml_set_slave_restricted_channels entry";

    std::string temp_mac(mac);
    LOG(WARNING) << "temp_mac = " << temp_mac;
    return (pBML->set_restricted_channels(restricted_channels, mac, 0, size));
}

int bml_get_slave_restricted_channels(BML_CTX ctx, uint8_t *restricted_channels, const char *mac)
{
    // Validate input parameters
    if (!ctx || !restricted_channels || !mac)
        return (-BML_RET_INVALID_ARGS);

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->get_restricted_channels(restricted_channels, mac, 0));
}

int bml_trigger_topology_discovery(BML_CTX ctx, const char *al_mac)
{
    // Validate input parameter
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->trigger_topology_discovery_query(al_mac));
}

int bml_channel_selection(BML_CTX ctx, const char *radio_mac, uint8_t channel, uint8_t bandwidth,
                          uint8_t csa_count)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return pBML->channel_selection(tlvf::mac_from_string(std::string(radio_mac)), channel,
                                   bandwidth, csa_count);
}

int bml_set_selection_channel_pool(BML_CTX ctx, const char *radio_mac, unsigned int *channel_pool,
                                   int channel_pool_size)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return pBML->set_selection_channel_pool(tlvf::mac_from_string(std::string(radio_mac)),
                                            channel_pool, channel_pool_size);
}

int bml_get_selection_channel_pool(BML_CTX ctx, const char *radio_mac, unsigned int *channel_pool,
                                   int *channel_pool_size)
{
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    bml_internal *pBML = static_cast<bml_internal *>(ctx);

    return pBML->get_selection_channel_pool(tlvf::mac_from_string(std::string(radio_mac)),
                                            channel_pool, channel_pool_size);
}

int bml_set_dcs_continuous_scan_enable(BML_CTX ctx, const char *radio_mac, int enable)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->set_dcs_continuous_scan_enable(tlvf::mac_from_string(std::string(radio_mac)),
                                                enable);
}

int bml_send_unassoc_sta_rcpi_query(BML_CTX ctx, const char *sta_mac, int16_t opclass,
                                    int16_t channel)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->send_unassoc_sta_rcpi_query(tlvf::mac_from_string(std::string(sta_mac)), opclass,
                                             channel);
}

int bml_get_unassoc_sta_rcpi_query_result(BML_CTX ctx, const char *sta_mac,
                                          struct BML_UNASSOC_STA_LINK_METRIC *sta_info)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->get_unassoc_sta_rcpi_query_result(tlvf::mac_from_string(std::string(sta_mac)),
                                                   sta_info);
}

int bml_get_dcs_continuous_scan_enable(BML_CTX ctx, const char *radio_mac, int *enable)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->get_dcs_continuous_scan_enable(tlvf::mac_from_string(std::string(radio_mac)),
                                                *enable);
}

int bml_set_dcs_continuous_scan_params(BML_CTX ctx, const char *radio_mac, int dwell_time,
                                       int interval_time, unsigned int *channel_pool,
                                       int channel_pool_size)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->set_dcs_continuous_scan_params(tlvf::mac_from_string(std::string(radio_mac)),
                                                dwell_time, interval_time, channel_pool,
                                                channel_pool_size);
}

int bml_get_dcs_continuous_scan_params(BML_CTX ctx, const char *radio_mac, int *output_dwell_time,
                                       int *output_interval_time, unsigned int *output_channel_pool,
                                       int *output_channel_pool_size)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->get_dcs_continuous_scan_params(tlvf::mac_from_string(std::string(radio_mac)),
                                                output_dwell_time, output_interval_time,
                                                output_channel_pool, output_channel_pool_size);
}

int bml_get_dcs_scan_results(BML_CTX ctx, const char *radio_mac,
                             struct BML_NEIGHBOR_AP *output_results,
                             unsigned int *output_results_size, unsigned char *output_result_status,
                             bool is_single_scan)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->get_dcs_scan_results(tlvf::mac_from_string(std::string(radio_mac)), output_results,
                                      *output_results_size, *output_results_size,
                                      *output_result_status, is_single_scan);
}

int bml_start_dcs_single_scan(BML_CTX ctx, const char *radio_mac, int dwell_time,
                              int channel_pool_size, unsigned int *channel_pool)
{
    // Validate input parameters
    if (!ctx) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->start_dcs_single_scan(tlvf::mac_from_string(std::string(radio_mac)), dwell_time,
                                       channel_pool, channel_pool_size);
}

int bml_client_get_client_list(BML_CTX ctx, char *client_list, unsigned int *client_list_size)
{
    // Validate input parameters
    if (!ctx || !client_list || !client_list_size) {
        return (-BML_RET_INVALID_ARGS);
    }

    if (*client_list_size == 0) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->client_get_client_list(client_list, client_list_size);
}

int bml_add_unassociated_station_stats(BML_CTX ctx, const char *mac_address,
                                       const char *channel_str, const char *operating_class_str,
                                       const char *agent_mac_address)
{
    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->add_unassociated_station_stats(mac_address, channel_str, operating_class_str,
                                                agent_mac_address);
}
int bml_remove_unassociated_station_stats(BML_CTX ctx, const char *mac_address,
                                          const char *agent_mac_address)
{
    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->remove_unassociated_station_stats(mac_address, agent_mac_address);
}

int bml_get_unassociated_station_stats(BML_CTX ctx, char *stats_results,
                                       unsigned int *stats_results_size)
{
    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->get_un_stations_stats(stats_results, stats_results_size);
}

int bml_client_set_client(BML_CTX ctx, const char *sta_mac,
                          const struct BML_CLIENT_CONFIG *client_config)
{
    // Validate input parameters
    if (!ctx || !sta_mac || !client_config) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->client_set_client(tlvf::mac_from_string(std::string(sta_mac)), *client_config);
}

int bml_client_get_client(BML_CTX ctx, const char *sta_mac, struct BML_CLIENT *client)
{
    // Validate input parameters
    if (!ctx || !sta_mac || !client) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->client_get_client(tlvf::mac_from_string(std::string(sta_mac)), client);
}

int bml_client_clear_client(BML_CTX ctx, const char *sta_mac)
{
    // Validate input parameters
    if (!ctx || !sta_mac) {
        return (-BML_RET_INVALID_ARGS);
    }

    auto pBML = static_cast<bml_internal *>(ctx);
    return pBML->client_clear_client(tlvf::mac_from_string(std::string(sta_mac)));
}

#ifdef FEATURE_PRE_ASSOCIATION_STEERING

int bml_pre_association_steering_set_group(BML_CTX ctx, uint32_t steeringGroupIndex,
                                           struct BML_STEERING_AP_CONFIG *ap_cfgs,
                                           unsigned int length)
{
    LOG(DEBUG) << "bml_pre_association_steering_set_ap_set_config is called";
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    auto *pBML = static_cast<bml_internal *>(ctx);

    if (length >= 4) {
        LOG(ERROR) << "The length of AP Configurations cannot be above 3";
        return (-BML_RET_INVALID_ARGS);
    }
    if (!ap_cfgs && length != 0) {
        LOG(ERROR) << "AP Configurations is NULL, but the length is not 0. The length must be 0.";
        return (-BML_RET_INVALID_ARGS);
    }
    if (ap_cfgs && length == 0) {
        LOG(ERROR)
            << "AP Configurations is no NULL, but length is zero. The length must be above 0.";
        return (-BML_RET_INVALID_ARGS);
    }
    return (pBML->steering_set_group(steeringGroupIndex, ap_cfgs, length));
}

int bml_pre_association_steering_client_set(BML_CTX ctx, uint32_t steeringGroupIndex,
                                            const BML_MAC_ADDR bssid, const BML_MAC_ADDR client_mac,
                                            BML_STEERING_CLIENT_CONFIG *config)
{
    LOG(DEBUG) << "bml_pre_association_steering_client_set is called";
    // Validate input parameters
    if (!ctx || !client_mac)
        return (-BML_RET_INVALID_ARGS);

    auto *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->steering_client_set(steeringGroupIndex, bssid, client_mac, config));
}

int bml_pre_association_steering_event_register(BML_CTX ctx, BML_EVENT_CB pCB)
{
    LOG(DEBUG) << "bml_pre_association_steering_event_register entry";
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    auto *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->steering_event_register(pCB));
}

int bml_pre_association_steering_client_measure(BML_CTX ctx, unsigned int steeringGroupIndex,
                                                const BML_MAC_ADDR bssid,
                                                const BML_MAC_ADDR client_mac)
{
    LOG(DEBUG) << "bml_pre_association_steering_client_measure entry";
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);

    auto *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->steering_client_measure(steeringGroupIndex, bssid, client_mac));
}

int bml_pre_association_steering_client_disconnect(BML_CTX ctx, unsigned int steeringGroupIndex,
                                                   const BML_MAC_ADDR bssid,
                                                   const BML_MAC_ADDR client_mac,
                                                   BML_DISCONNECT_TYPE type, unsigned int reason)
{
    LOG(DEBUG) << "bml_pre_association_steering_client_disconnect entry";
    // Validate input parameters
    if (!ctx)
        return (-BML_RET_INVALID_ARGS);
    auto *pBML = static_cast<bml_internal *>(ctx);

    return (pBML->steering_client_disconnect(steeringGroupIndex, bssid, client_mac, type, reason));
}

#endif /* FEATURE_PRE_ASSOCIATION_STEERING */
