/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_CONFIG_FILE_H_
#define _BEEROCKS_CONFIG_FILE_H_

#include "beerocks_defines.h"

#include <string>
#include <vector>

namespace beerocks {

class config_file {
public:
    typedef std::vector<std::tuple<std::string, std::string *, int>> tConfig;

    struct SConfigLog {
        //[log]
        std::string global_levels;
        std::string syslog_levels;
        std::string global_size;
        std::string files_enabled;
        std::string files_path;
        std::string files_auto_roll;
        std::string stdout_enabled;
        std::string syslog_enabled;
    };

    // config file parameters master / slave
    typedef struct { // master
        //[master]
        std::string temp_path;
        std::string vendor;
        std::string model;
        std::string ucc_listener_port;
        std::string load_legacy_client_roaming;
        std::string load_service_fairness;
        std::string load_backhaul_measurements;
        std::string load_front_measurements;
        std::string load_monitor_on_vaps;
        std::string global_restricted_channels;
        std::string ire_rssi_report_rate_sec;
        std::string roaming_unconnected_client_rssi_compensation_db;
        std::string roaming_hop_percent_penalty;
        std::string roaming_band_pathloss_delta_db;
        std::string roaming_6ghz_failed_attemps_threshold;
        std::string roaming_5ghz_failed_attemps_threshold;
        std::string roaming_24ghz_failed_attemps_threshold;
        std::string roaming_11v_failed_attemps_threshold;
        std::string roaming_rssi_cutoff_db;
        std::string monitor_total_channel_load_notification_lo_th_percent;
        std::string monitor_total_channel_load_notification_hi_th_percent;
        std::string monitor_total_channel_load_notification_delta_th_percent;
        std::string monitor_min_active_clients;
        std::string monitor_active_client_th;
        std::string monitor_client_load_notification_delta_th_percent;
        std::string monitor_rx_rssi_notification_threshold_dbm;
        std::string monitor_rx_rssi_notification_delta_db;
        std::string monitor_ap_idle_threshold_B;
        std::string monitor_ap_active_threshold_B;
        std::string monitor_ap_idle_stable_time_sec;
        std::string monitor_disable_initiative_arp;
        std::string channel_selection_random_delay;
        std::string fail_safe_5G_frequency;
        std::string fail_safe_5G_bw;
        std::string fail_safe_5G_vht_frequency;
        std::string channel_selection_long_delay;
        std::string roaming_sticky_client_rssi_threshold;
        std::string credentials_change_timeout_sec;
        std::string use_dataelements_vap_configs;

        //[log]
        SConfigLog sLog;
    } sConfigMaster;

    typedef struct { // slave
        //[global]
        std::string platform;
        std::string temp_path;
        std::string vendor;
        std::string model;
        std::string ucc_listener_port;
        std::string enable_arp_monitor;
        std::string bridge_iface;
        std::string backhaul_preferred_bssid;
        std::string enable_system_hang_test;
        std::string monitor_polling_rate_msec;
        std::string monitor_measurement_window_poll_count;
        std::string profile_x_disallow_override_unsupported_configuration;
        std::string on_boot_scan;
        //[slaveX]
        std::string enable_repeater_mode[MAX_RADIOS_PER_AGENT];
        std::string hostap_iface_type[MAX_RADIOS_PER_AGENT];
        std::string sta_iface[MAX_RADIOS_PER_AGENT];
        std::string sta_iface_filter_low[MAX_RADIOS_PER_AGENT];
        std::string hostap_ant_gain[MAX_RADIOS_PER_AGENT];
        std::string em_handle_third_party;
        //[log]
        SConfigLog sLog;
    } sConfigSlave;

    static bool read_config_file(std::string config_file_path, tConfig &conf,
                                 const std::string &config_type);
    static bool read_master_config_file(const std::string &config_file_path, sConfigMaster &conf);
    static bool read_slave_config_file(const std::string &config_file_path, sConfigSlave &conf);
};

} //  namespace beerocks

#endif
