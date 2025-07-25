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

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_BACKHAUL_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_BACKHAUL_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include <tuple>
#include "beerocks/tlvf/beerocks_message_common.h"
#include "structs/sCacStartedNotificationParams.h"
#include "tlvf/WSC/WSC_Attributes.h"

namespace beerocks_message {


class cACTION_BACKHAUL_REGISTER_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_REGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_REGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_REGISTER_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_REGISTER_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_REGISTER_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_REGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_REGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_REGISTER_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_REGISTER_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_ENABLE : public BaseClass
{
    public:
        cACTION_BACKHAUL_ENABLE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_ENABLE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_ENABLE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_ENABLE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_CONNECTED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_CONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CONNECTED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CONNECTED_NOTIFICATION);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_DISCONNECTED_NOTIFICATION);
        }
        uint8_t& stopped();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_stopped = nullptr;
};

class cACTION_BACKHAUL_ENABLE_APS_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_ENABLE_APS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_ENABLE_APS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_ENABLE_APS_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_ENABLE_APS_REQUEST);
        }
        std::string iface_str();
        char* iface(size_t length = 0);
        bool set_iface(const std::string& str);
        bool set_iface(const char buffer[], size_t size);
        uint8_t& channel();
        beerocks::eWiFiBandwidth& bandwidth();
        uint8_t& center_channel();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        char* m_iface = nullptr;
        size_t m_iface_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_channel = nullptr;
        beerocks::eWiFiBandwidth* m_bandwidth = nullptr;
        uint8_t* m_center_channel = nullptr;
};

class cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST);
        }
        uint32_t& attempts();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint32_t* m_attempts = nullptr;
};

class cACTION_BACKHAUL_AP_DISABLED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_AP_DISABLED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_AP_DISABLED_NOTIFICATION);
        }
        std::string iface_str();
        char* iface(size_t length = 0);
        bool set_iface(const std::string& str);
        bool set_iface(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        char* m_iface = nullptr;
        size_t m_iface_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST);
        }
        sNodeRssiMeasurementRequest& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sNodeRssiMeasurementRequest* m_params = nullptr;
};

class cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE);
        }
        sNodeRssiMeasurement& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_BACKHAUL_START_WPS_PBC_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_START_WPS_PBC_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_START_WPS_PBC_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_START_WPS_PBC_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_START_WPS_PBC_REQUEST);
        }
        std::string iface_str();
        char* iface(size_t length = 0);
        bool set_iface(const std::string& str);
        bool set_iface(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        char* m_iface = nullptr;
        size_t m_iface_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BACKHAUL_SET_ASSOC_DISALLOW_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_SET_ASSOC_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_SET_ASSOC_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_SET_ASSOC_DISALLOW_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_SET_ASSOC_DISALLOW_REQUEST);
        }
        uint8_t& enable();
        sMacAddr& bssid();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_enable = nullptr;
        sMacAddr* m_bssid = nullptr;
};

class cACTION_BACKHAUL_APPLY_VLAN_POLICY_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_APPLY_VLAN_POLICY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_APPLY_VLAN_POLICY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_APPLY_VLAN_POLICY_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_APPLY_VLAN_POLICY_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_ZWDFS_RADIO_DETECTED : public BaseClass
{
    public:
        cACTION_BACKHAUL_ZWDFS_RADIO_DETECTED(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_ZWDFS_RADIO_DETECTED(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_ZWDFS_RADIO_DETECTED();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_ZWDFS_RADIO_DETECTED);
        }
        std::string front_iface_name_str();
        char* front_iface_name(size_t length = 0);
        bool set_front_iface_name(const std::string& str);
        bool set_front_iface_name(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        char* m_front_iface_name = nullptr;
        size_t m_front_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BACKHAUL_CHANNELS_LIST_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNELS_LIST_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNELS_LIST_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNELS_LIST_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNELS_LIST_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_CHANNELS_LIST_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNELS_LIST_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNELS_LIST_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNELS_LIST_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNELS_LIST_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_CHANNEL_SWITCH_ACS_START : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_CHANNEL_SWITCH_ACS_START(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_CHANNEL_SWITCH_ACS_START(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_CHANNEL_SWITCH_ACS_START();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_CHANNEL_SWITCH_ACS_START);
        }
        sApChannelSwitch& cs_params();
        int8_t& tx_limit();
        uint8_t& tx_limit_valid();
        sSpatialReuseParams& sr_params();
        uint8_t& spatial_reuse_valid();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
        int8_t* m_tx_limit = nullptr;
        uint8_t* m_tx_limit_valid = nullptr;
        sSpatialReuseParams* m_sr_params = nullptr;
        uint8_t* m_spatial_reuse_valid = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST);
        }
        sApChannelSwitch& cs_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_CSA_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_CSA_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_CSA_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_CSA_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_CSA_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_CSA_ERROR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_CSA_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_CSA_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_CSA_ERROR_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_CSA_ERROR_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_DFS_CAC_STARTED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_DFS_CAC_STARTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_DFS_CAC_STARTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_DFS_CAC_STARTED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_DFS_CAC_STARTED_NOTIFICATION);
        }
        sCacStartedNotificationParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sCacStartedNotificationParams* m_params = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION);
        }
        sDfsCacCompleted& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sDfsCacCompleted* m_params = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST);
        }
        uint8_t& ant_switch_on();
        uint8_t& channel();
        beerocks::eWiFiBandwidth& bandwidth();
        uint32_t& center_frequency();
        uint8_t& csa_count();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_ant_switch_on = nullptr;
        uint8_t* m_channel = nullptr;
        beerocks::eWiFiBandwidth* m_bandwidth = nullptr;
        uint32_t* m_center_frequency = nullptr;
        uint8_t* m_csa_count = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_BACKHAUL_RADIO_DISABLE_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_RADIO_DISABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_RADIO_DISABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_RADIO_DISABLE_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_RADIO_DISABLE_REQUEST);
        }
        std::string iface_str();
        char* iface(size_t length = 0);
        bool set_iface(const std::string& str);
        bool set_iface(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        char* m_iface = nullptr;
        size_t m_iface_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BACKHAUL_RADIO_TEAR_DOWN_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_RADIO_TEAR_DOWN_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_RADIO_TEAR_DOWN_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_RADIO_TEAR_DOWN_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_RADIO_TEAR_DOWN_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_DISCONNECT_COMMAND : public BaseClass
{
    public:
        cACTION_BACKHAUL_DISCONNECT_COMMAND(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_DISCONNECT_COMMAND(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_DISCONNECT_COMMAND();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_DISCONNECT_COMMAND);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_RECONNECT_COMMAND : public BaseClass
{
    public:
        cACTION_BACKHAUL_RECONNECT_COMMAND(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_RECONNECT_COMMAND(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_RECONNECT_COMMAND();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_RECONNECT_COMMAND);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION);
        }
        sSpatialReuseParams& sr_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sSpatialReuseParams* m_sr_params = nullptr;
};

class cACTION_BACKHAUL_WIFI_CREDENTIALS_UPDATE_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_WIFI_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_WIFI_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_WIFI_CREDENTIALS_UPDATE_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_WIFI_CREDENTIALS_UPDATE_REQUEST);
        }
        sMacAddr& radio_mac();
        bool isPostInitSucceeded() override;
        std::shared_ptr<WSC::cConfigData> create_wifi_credentials();
        bool add_wifi_credentials(std::shared_ptr<WSC::cConfigData> ptr);
        std::shared_ptr<WSC::cConfigData> wifi_credentials() { return m_wifi_credentials_ptr; }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        WSC::cConfigData *m_wifi_credentials = nullptr;
        std::shared_ptr<WSC::cConfigData> m_wifi_credentials_ptr = nullptr;
        bool m_wifi_credentials_init = false;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cACTION_BACKHAUL_TRIGGER_ON_BOOT_SCAN : public BaseClass
{
    public:
        cACTION_BACKHAUL_TRIGGER_ON_BOOT_SCAN(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_TRIGGER_ON_BOOT_SCAN(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_TRIGGER_ON_BOOT_SCAN();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_TRIGGER_ON_BOOT_SCAN);
        }
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST);
        }
        uint8_t& cert_mode();
        sTriggerChannelScanParams& scan_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_cert_mode = nullptr;
        sTriggerChannelScanParams* m_scan_params = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION);
        }
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_RESULTS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_RESULTS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_RESULTS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_RESULTS_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_RESULTS_NOTIFICATION);
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
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sChannelScanResults* m_scan_results = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        uint8_t* m_is_dump = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_ABORT_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_ABORT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_ABORT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_ABORT_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_ABORT_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_ABORT_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_ABORT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_ABORT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_ABORT_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_ABORT_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_ABORTED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_ABORTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_ABORTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_ABORTED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_ABORTED_NOTIFICATION);
        }
        uint8_t& reason();
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_reason = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

class cACTION_BACKHAUL_CHANNEL_SCAN_FINISHED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_CHANNEL_SCAN_FINISHED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BACKHAUL_CHANNEL_SCAN_FINISHED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BACKHAUL_CHANNEL_SCAN_FINISHED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CHANNEL_SCAN_FINISHED_NOTIFICATION);
        }
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_BACKHAUL_H_
