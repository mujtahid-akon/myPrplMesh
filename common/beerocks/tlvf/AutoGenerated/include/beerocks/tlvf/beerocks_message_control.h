///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_CONTROL_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_CONTROL_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include <tuple>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {


class cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
};

class cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
};

class cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_SLAVE_JOINED_NOTIFICATION);
        }
        std::string slave_version_str();
        char* slave_version(size_t length = 0);
        bool set_slave_version(const std::string& str);
        bool set_slave_version(const char buffer[], size_t size);
        sPlatformSettings& platform_settings();
        sWlanSettings& wlan_settings();
        sBackhaulParams& backhaul_params();
        sNodeHostapVendorSpec& hostap();
        sApChannelSwitch& cs_params();
        uint8_t& low_pass_filter_on();
        uint8_t& enable_repeater_mode();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        char* m_slave_version = nullptr;
        size_t m_slave_version_idx__ = 0;
        int m_lock_order_counter__ = 0;
        sPlatformSettings* m_platform_settings = nullptr;
        sWlanSettings* m_wlan_settings = nullptr;
        sBackhaulParams* m_backhaul_params = nullptr;
        sNodeHostapVendorSpec* m_hostap = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
        uint8_t* m_low_pass_filter_on = nullptr;
        uint8_t* m_enable_repeater_mode = nullptr;
};

class cACTION_CONTROL_SLAVE_JOINED_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_SLAVE_JOINED_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_SLAVE_JOINED_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_SLAVE_JOINED_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_SLAVE_JOINED_RESPONSE);
        }
        std::string master_version_str();
        char* master_version(size_t length = 0);
        bool set_master_version(const std::string& str);
        bool set_master_version(const char buffer[], size_t size);
        uint8_t& err_code();
        sSonConfig& config();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        char* m_master_version = nullptr;
        size_t m_master_version_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_err_code = nullptr;
        sSonConfig* m_config = nullptr;
};

class cACTION_CONTROL_ARP_QUERY_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_ARP_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_ARP_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_ARP_QUERY_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_ARP_QUERY_REQUEST);
        }
        sArpQuery& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sArpQuery* m_params = nullptr;
};

class cACTION_CONTROL_ARP_QUERY_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_ARP_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_ARP_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_ARP_QUERY_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_ARP_QUERY_RESPONSE);
        }
        sArpMonitorData& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sArpMonitorData* m_params = nullptr;
};

class cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL : public BaseClass
{
    public:
        cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL);
        }
        sLoggingLevelChange& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sLoggingLevelChange* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_CSA_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_ACS_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION);
        }
        sDfsCacCompleted& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sDfsCacCompleted* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION);
        }
        sDfsChannelAvailable& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sDfsChannelAvailable* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST);
        }
        sApSetRestrictedFailsafe& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sApSetRestrictedFailsafe* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START);
        }
        sApChannelSwitch& cs_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST);
        }
        uint32_t& attempts();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        uint32_t* m_attempts = nullptr;
};

class cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST);
        }
        uint8_t& sync();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        uint8_t* m_sync = nullptr;
};

class cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE);
        }
        uint8_t& ap_stats_size();
        std::tuple<bool, sApStatsParams&> ap_stats(size_t idx);
        bool alloc_ap_stats(size_t count = 1);
        uint8_t& sta_stats_size();
        std::tuple<bool, sStaStatsParams&> sta_stats(size_t idx);
        bool alloc_sta_stats(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        uint8_t* m_ap_stats_size = nullptr;
        sApStatsParams* m_ap_stats = nullptr;
        size_t m_ap_stats_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_sta_stats_size = nullptr;
        sStaStatsParams* m_sta_stats = nullptr;
        size_t m_sta_stats_idx__ = 0;
};

class cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION);
        }
        sApLoadNotificationParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sApLoadNotificationParams* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST);
        }
        sNeighborSetParams11k& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sNeighborSetParams11k* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST);
        }
        sNeighborRemoveParams11k& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sNeighborRemoveParams11k* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION);
        }
        sApActivityNotificationParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sApActivityNotificationParams* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION);
        }
        sVapsList& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sVapsList* m_params = nullptr;
};

class cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION);
        }
        int8_t& vap_id();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        int8_t* m_vap_id = nullptr;
};

class cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION);
        }
        int8_t& vap_id();
        sVapInfo& vap_info();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        int8_t* m_vap_id = nullptr;
        sVapInfo* m_vap_info = nullptr;
};

class cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_START_MONITORING_REQUEST);
        }
        sClientMonitoringParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sClientMonitoringParams* m_params = nullptr;
};

class cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST);
        }
        sNodeRssiMeasurementRequest& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sNodeRssiMeasurementRequest* m_params = nullptr;
};

class cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE);
        }
        sNodeRssiMeasurement& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION);
        }
        sNodeRssiMeasurement& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION);
        }
        sMacAddr& mac();
        beerocks::net::sIpv4Addr& ipv4();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
        beerocks::net::sIpv4Addr* m_ipv4 = nullptr;
};

class cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_DISCONNECT_REQUEST);
        }
        sMacAddr& mac();
        int8_t& vap_id();
        eDisconnectType& type();
        uint32_t& reason();
        eClientDisconnectSource& src();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
        int8_t* m_vap_id = nullptr;
        eDisconnectType* m_type = nullptr;
        uint32_t* m_reason = nullptr;
        eClientDisconnectSource* m_src = nullptr;
};

class cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE);
        }
        sClientDisconnectResponse& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sClientDisconnectResponse* m_params = nullptr;
};

class cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION);
        }
        sMacAddr& mac();
        beerocks::net::sIpv4Addr& ipv4();
        std::string name_str();
        char* name(size_t length = 0);
        bool set_name(const std::string& str);
        bool set_name(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
        beerocks::net::sIpv4Addr* m_ipv4 = nullptr;
        char* m_name = nullptr;
        size_t m_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION);
        }
        sArpMonitorData& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sArpMonitorData* m_params = nullptr;
};

class cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_BEACON_11K_REQUEST);
        }
        sBeaconRequest11k& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sBeaconRequest11k* m_params = nullptr;
};

class cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE);
        }
        sBeaconResponse11k& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sBeaconResponse11k* m_params = nullptr;
};

class cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST);
        }
        sSteeringSetGroupRequest& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sSteeringSetGroupRequest* m_params = nullptr;
};

class cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE);
        }
        sSteeringSetGroupResponse& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sSteeringSetGroupResponse* m_params = nullptr;
};

class cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_STEERING_CLIENT_SET_REQUEST);
        }
        sSteeringClientSetRequest& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sSteeringClientSetRequest* m_params = nullptr;
};

class cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE);
        }
        sSteeringClientSetResponse& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sSteeringClientSetResponse* m_params = nullptr;
};

class cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION);
        }
        sSteeringEvActivity& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sSteeringEvActivity* m_params = nullptr;
};

class cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION);
        }
        sSteeringEvSnrXing& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sSteeringEvSnrXing* m_params = nullptr;
};

class cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION);
        }
        sSteeringEvProbeReq& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sSteeringEvProbeReq* m_params = nullptr;
};

class cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION);
        }
        sSteeringEvAuthFail& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sSteeringEvAuthFail* m_params = nullptr;
};

class cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST);
        }
        sTriggerChannelScanParams& scan_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sTriggerChannelScanParams* m_scan_params = nullptr;
};

class cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE : public BaseClass
{
    public:
        cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST : public BaseClass
{
    public:
        cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
};

class cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION);
        }
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

class cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION);
        }
        sChannelScanResults& scan_results();
        sMacAddr& radio_mac();
        //1 - notification contains a result dump, 0 - notification that results are ready
        uint8_t& is_dump();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sChannelScanResults* m_scan_results = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        uint8_t* m_is_dump = nullptr;
};

class cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION);
        }
        uint8_t& reason();
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        uint8_t* m_reason = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

class cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION();

        static eActionOp_CONTROL get_action_op(){
            return (eActionOp_CONTROL)(ACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION);
        }
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CONTROL* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_CONTROL_H_
