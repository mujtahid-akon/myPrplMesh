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

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_BML_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_BML_H_

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


class cACTION_BML_PING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_PING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_PING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_PING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_PING_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_PING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_PING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_PING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_PING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_PING_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_NW_MAP_REQUEST : public BaseClass
{
    public:
        cACTION_BML_NW_MAP_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_NW_MAP_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_NW_MAP_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_NW_MAP_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_NW_MAP_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_NW_MAP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_NW_MAP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_NW_MAP_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_NW_MAP_RESPONSE);
        }
        uint32_t& node_num();
        uint32_t& buffer_size();
        uint8_t* buffer(size_t idx = 0);
        bool set_buffer(const void* buffer, size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_node_num = nullptr;
        uint32_t* m_buffer_size = nullptr;
        uint8_t* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_NW_MAP_UPDATE : public BaseClass
{
    public:
        cACTION_BML_NW_MAP_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_NW_MAP_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_NW_MAP_UPDATE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_NW_MAP_UPDATE);
        }
        uint32_t& node_num();
        uint32_t& buffer_size();
        uint8_t* buffer(size_t idx = 0);
        bool set_buffer(const void* buffer, size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_node_num = nullptr;
        uint32_t* m_buffer_size = nullptr;
        uint8_t* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_STATS_UPDATE : public BaseClass
{
    public:
        cACTION_BML_STATS_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STATS_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STATS_UPDATE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STATS_UPDATE);
        }
        uint32_t& num_of_stats_bulks();
        uint32_t& buffer_size();
        uint8_t* buffer(size_t idx = 0);
        bool set_buffer(const void* buffer, size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_num_of_stats_bulks = nullptr;
        uint32_t* m_buffer_size = nullptr;
        uint8_t* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_EVENTS_UPDATE : public BaseClass
{
    public:
        cACTION_BML_EVENTS_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_EVENTS_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_EVENTS_UPDATE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_EVENTS_UPDATE);
        }
        uint32_t& buffer_size();
        uint8_t* buffer(size_t idx = 0);
        bool set_buffer(const void* buffer, size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_buffer_size = nullptr;
        uint8_t* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_DFS_REENTRY_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_DFS_REENTRY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_DFS_REENTRY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_DFS_REENTRY_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_DFS_REENTRY_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_DFS_REENTRY_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_DFS_REENTRY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_DFS_REENTRY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_DFS_REENTRY_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_DFS_REENTRY_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_DFS_REENTRY_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_DFS_REENTRY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_DFS_REENTRY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_DFS_REENTRY_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_DFS_REENTRY_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_DFS_REENTRY_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_DFS_REENTRY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_DFS_REENTRY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_DFS_REENTRY_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_DFS_REENTRY_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_IRE_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_IRE_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_IRE_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_IRE_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_IRE_ROAMING_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_IRE_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_IRE_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_IRE_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_IRE_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_IRE_ROAMING_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_IRE_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_IRE_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_IRE_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_IRE_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_IRE_ROAMING_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_IRE_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_IRE_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_IRE_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_IRE_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_IRE_ROAMING_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_LOAD_BALANCER_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_LOAD_BALANCER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_LOAD_BALANCER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_LOAD_BALANCER_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_LOAD_BALANCER_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_LOAD_BALANCER_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_LOAD_BALANCER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_LOAD_BALANCER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_LOAD_BALANCER_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_LOAD_BALANCER_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_LOAD_BALANCER_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_LOAD_BALANCER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_LOAD_BALANCER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_LOAD_BALANCER_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_LOAD_BALANCER_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_LOAD_BALANCER_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_LOAD_BALANCER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_LOAD_BALANCER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_LOAD_BALANCER_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_LOAD_BALANCER_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_SERVICE_FAIRNESS_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_SERVICE_FAIRNESS_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST);
        }
        sLoggingLevelChange& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sLoggingLevelChange* m_params = nullptr;
};

class cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST : public BaseClass
{
    public:
        cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_WIFI_CREDENTIALS_SET_REQUEST);
        }
        sMacAddr& al_mac();
        uint16_t& authentication_type();
        uint16_t& encryption_type();
        uint8_t& fronthaul();
        uint8_t& backhaul();
        uint8_t& ssid_size();
        std::string ssid_str();
        char* ssid(size_t length = 0);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        bool alloc_ssid(size_t count = 1);
        uint8_t& network_key_size();
        std::string network_key_str();
        char* network_key(size_t length = 0);
        bool set_network_key(const std::string& str);
        bool set_network_key(const char buffer[], size_t size);
        bool alloc_network_key(size_t count = 1);
        uint8_t& operating_classes_size();
        uint8_t* operating_classes(size_t idx = 0);
        bool set_operating_classes(const void* buffer, size_t size);
        bool alloc_operating_classes(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_al_mac = nullptr;
        uint16_t* m_authentication_type = nullptr;
        uint16_t* m_encryption_type = nullptr;
        uint8_t* m_fronthaul = nullptr;
        uint8_t* m_backhaul = nullptr;
        uint8_t* m_ssid_size = nullptr;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_network_key_size = nullptr;
        char* m_network_key = nullptr;
        size_t m_network_key_idx__ = 0;
        uint8_t* m_operating_classes_size = nullptr;
        uint8_t* m_operating_classes = nullptr;
        size_t m_operating_classes_idx__ = 0;
};

class cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE);
        }
        uint32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_error_code = nullptr;
};

class cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST);
        }
        sRestrictedChannels& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sRestrictedChannels* m_params = nullptr;
};

class cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE);
        }
        uint32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_error_code = nullptr;
};

class cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST);
        }
        sRestrictedChannels& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sRestrictedChannels* m_params = nullptr;
};

class cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE);
        }
        sRestrictedChannels& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sRestrictedChannels* m_params = nullptr;
};

class cACTION_BML_SET_CERTIFICATION_MODE_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_CERTIFICATION_MODE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CERTIFICATION_MODE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CERTIFICATION_MODE_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CERTIFICATION_MODE_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CERTIFICATION_MODE_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CERTIFICATION_MODE_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_CERTIFICATION_MODE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CERTIFICATION_MODE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CERTIFICATION_MODE_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CERTIFICATION_MODE_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CERTIFICATION_MODE_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST);
        }
        uint32_t& result();
        uint8_t& vap_list_size();
        std::tuple<bool, sConfigVapInfo&> vap_list(size_t idx);
        bool alloc_vap_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
        uint8_t* m_vap_list_size = nullptr;
        sConfigVapInfo* m_vap_list = nullptr;
        size_t m_vap_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE);
        }
        uint32_t& result();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE);
        }
        uint32_t& result();
        uint8_t& vap_list_size();
        std::tuple<bool, sConfigVapInfo&> vap_list(size_t idx);
        bool alloc_vap_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
        uint8_t* m_vap_list_size = nullptr;
        sConfigVapInfo* m_vap_list = nullptr;
        size_t m_vap_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST);
        }
        uint32_t& result();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_BML_STEERING_SET_GROUP_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_SET_GROUP_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_SET_GROUP_REQUEST);
        }
        uint32_t& steeringGroupIndex();
        size_t ap_cfgs_length() { return m_ap_cfgs_idx__ * sizeof(sSteeringApConfig); }
        std::tuple<bool, sSteeringApConfig&> ap_cfgs(size_t idx);
        bool alloc_ap_cfgs(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_steeringGroupIndex = nullptr;
        sSteeringApConfig* m_ap_cfgs = nullptr;
        size_t m_ap_cfgs_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_STEERING_SET_GROUP_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_SET_GROUP_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_SET_GROUP_RESPONSE);
        }
        int32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_CLIENT_SET_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_CLIENT_SET_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_SET_REQUEST);
        }
        uint32_t& steeringGroupIndex();
        sMacAddr& bssid();
        sMacAddr& client_mac();
        sSteeringClientConfig& config();
        uint8_t& remove();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_steeringGroupIndex = nullptr;
        sMacAddr* m_bssid = nullptr;
        sMacAddr* m_client_mac = nullptr;
        sSteeringClientConfig* m_config = nullptr;
        uint8_t* m_remove = nullptr;
};

class cACTION_BML_STEERING_CLIENT_SET_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_CLIENT_SET_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_SET_RESPONSE);
        }
        int32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST);
        }
        uint8_t& unregister();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_unregister = nullptr;
};

class cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE);
        }
        int32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST);
        }
        uint32_t& steeringGroupIndex();
        sMacAddr& bssid();
        sMacAddr& client_mac();
        eDisconnectType& type();
        uint32_t& reason();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_steeringGroupIndex = nullptr;
        sMacAddr* m_bssid = nullptr;
        sMacAddr* m_client_mac = nullptr;
        eDisconnectType* m_type = nullptr;
        uint32_t* m_reason = nullptr;
};

class cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE);
        }
        int32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_MEASURE_REQUEST);
        }
        uint32_t& steeringGroupIndex();
        sMacAddr& bssid();
        sMacAddr& client_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_steeringGroupIndex = nullptr;
        sMacAddr* m_bssid = nullptr;
        sMacAddr* m_client_mac = nullptr;
};

class cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE);
        }
        int32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_EVENTS_UPDATE : public BaseClass
{
    public:
        cACTION_BML_STEERING_EVENTS_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_STEERING_EVENTS_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_STEERING_EVENTS_UPDATE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_EVENTS_UPDATE);
        }
        uint32_t& buffer_size();
        uint8_t* buffer(size_t idx = 0);
        bool set_buffer(const void* buffer, size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_buffer_size = nullptr;
        uint8_t* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_TRIGGER_TOPOLOGY_QUERY : public BaseClass
{
    public:
        cACTION_BML_TRIGGER_TOPOLOGY_QUERY(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_TRIGGER_TOPOLOGY_QUERY(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_TRIGGER_TOPOLOGY_QUERY();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_TRIGGER_TOPOLOGY_QUERY);
        }
        sMacAddr& al_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_al_mac = nullptr;
};

class cACTION_BML_TOPOLOGY_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_TOPOLOGY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_TOPOLOGY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_TOPOLOGY_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_TOPOLOGY_RESPONSE);
        }
        sDeviceData& device_data();
        uint32_t& result();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sDeviceData* m_device_data = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_BML_REGISTER_TOPOLOGY_QUERY : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TOPOLOGY_QUERY(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_REGISTER_TOPOLOGY_QUERY(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_REGISTER_TOPOLOGY_QUERY();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TOPOLOGY_QUERY);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_TOPOLOGY_QUERY : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_TOPOLOGY_QUERY(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_UNREGISTER_TOPOLOGY_QUERY(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_UNREGISTER_TOPOLOGY_QUERY();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_TOPOLOGY_QUERY);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST : public BaseClass
{
    public:
        cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST);
        }
        sMacAddr& radio_mac();
        uint8_t& channel();
        beerocks::eWiFiBandwidth& bandwidth();
        uint8_t& csa_count();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        uint8_t* m_channel = nullptr;
        beerocks::eWiFiBandwidth* m_bandwidth = nullptr;
        uint8_t* m_csa_count = nullptr;
};

class cACTION_BML_TRIGGER_CHANNEL_SELECTION_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_TRIGGER_CHANNEL_SELECTION_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_TRIGGER_CHANNEL_SELECTION_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_TRIGGER_CHANNEL_SELECTION_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_TRIGGER_CHANNEL_SELECTION_RESPONSE);
        }
        //0 - Success, Otherwise error according to beerocks_defines:eChannelSwitchStatus
        uint8_t& code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_code = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST);
        }
        sMacAddr& radio_mac();
        sChannelScanRequestParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        sChannelScanRequestParams* m_params = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE);
        }
        //0 - Success, Otherwise error according to beerocks_defines:eChannelScanOperationCode
        uint8_t& op_error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_op_error_code = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST);
        }
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE);
        }
        sChannelScanRequestParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sChannelScanRequestParams* m_params = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST);
        }
        sMacAddr& radio_mac();
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE);
        }
        //0 - Success, Otherwise error according to beerocks_defines:eChannelScanOperationCode
        uint8_t& op_error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_op_error_code = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST);
        }
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST);
        }
        sTriggerChannelScanParams& scan_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sTriggerChannelScanParams* m_scan_params = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE);
        }
        //0 - Success, Otherwise error according to beerocks_defines:eChannelScanOperationCode
        uint8_t& op_error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_op_error_code = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST);
        }
        sMacAddr& radio_mac();
        //0 - results for the continuous scan, 1 - results for the single scan
        uint8_t& scan_mode();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        uint8_t* m_scan_mode = nullptr;
};

class cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE);
        }
        //0 - Success, Otherwise error according to beerocks_defines:eDcsScanErrCode
        uint8_t& result_status();
        //0 - Success, Otherwise error according to beerocks_defines:eChannelScanOperationCode
        uint8_t& op_error_code();
        //0 - Not reached end of response, 1 - reached end of respons
        uint8_t& last();
        uint8_t& results_size();
        std::tuple<bool, sChannelScanResults&> results(size_t idx);
        bool alloc_results(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_result_status = nullptr;
        uint8_t* m_op_error_code = nullptr;
        uint8_t* m_last = nullptr;
        uint8_t* m_results_size = nullptr;
        sChannelScanResults* m_results = nullptr;
        size_t m_results_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST : public BaseClass
{
    public:
        cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE);
        }
        uint32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_error_code = nullptr;
};

class cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST : public BaseClass
{
    public:
        cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST);
        }
        sMacAddr& al_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_al_mac = nullptr;
};

class cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE);
        }
        uint32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_error_code = nullptr;
};

class cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_ADD_UNASSOCIATED_STATION_STATS_REQUEST);
        }
        sMacAddr& mac_address();
        sMacAddr& agent_mac_address();
        uint8_t& channel();
        uint8_t& operating_class();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_mac_address = nullptr;
        sMacAddr* m_agent_mac_address = nullptr;
        uint8_t* m_channel = nullptr;
        uint8_t* m_operating_class = nullptr;
};

class cACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REMOVE_UNASSOCIATED_STATION_STATS_REQUEST);
        }
        sMacAddr& mac_address();
        sMacAddr& agent_mac_address();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_mac_address = nullptr;
        sMacAddr* m_agent_mac_address = nullptr;
};

class cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};
typedef struct sStaMetrics {
    sMacAddr sta_mac;
    uint8_t channel_number;
    uint8_t uplink_rcpi_dbm_enc;
    char time_stamp[40];
    void struct_swap(){
        sta_mac.struct_swap();
    }
    void struct_init(){
        sta_mac.struct_init();
    }
} __attribute__((packed)) sStaMetrics;


class cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_UNASSOCIATED_STATIONS_STATS_RESPONSE);
        }
        uint8_t& sta_list_length();
        std::tuple<bool, sStaMetrics&> sta_list(size_t idx);
        bool alloc_sta_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_sta_list_length = nullptr;
        sStaMetrics* m_sta_list = nullptr;
        size_t m_sta_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE);
        }
        uint8_t& result();
        uint32_t& client_list_size();
        std::tuple<bool, sMacAddr&> client_list(size_t idx);
        bool alloc_client_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_result = nullptr;
        uint32_t* m_client_list_size = nullptr;
        sMacAddr* m_client_list = nullptr;
        size_t m_client_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_CLIENT_SET_CLIENT_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CLIENT_SET_CLIENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CLIENT_SET_CLIENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CLIENT_SET_CLIENT_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CLIENT_SET_CLIENT_REQUEST);
        }
        sMacAddr& sta_mac();
        sClientConfig& client_config();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_sta_mac = nullptr;
        sClientConfig* m_client_config = nullptr;
};

class cACTION_BML_CLIENT_SET_CLIENT_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CLIENT_SET_CLIENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CLIENT_SET_CLIENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CLIENT_SET_CLIENT_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CLIENT_SET_CLIENT_RESPONSE);
        }
        uint8_t& result();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_result = nullptr;
};

class cACTION_BML_CLIENT_GET_CLIENT_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CLIENT_GET_CLIENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CLIENT_GET_CLIENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CLIENT_GET_CLIENT_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CLIENT_GET_CLIENT_REQUEST);
        }
        sMacAddr& sta_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_sta_mac = nullptr;
};

class cACTION_BML_CLIENT_GET_CLIENT_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CLIENT_GET_CLIENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CLIENT_GET_CLIENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CLIENT_GET_CLIENT_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CLIENT_GET_CLIENT_RESPONSE);
        }
        uint8_t& result();
        sClient& client();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_result = nullptr;
        sClient* m_client = nullptr;
};

class cACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CLIENT_CLEAR_CLIENT_REQUEST);
        }
        sMacAddr& sta_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_sta_mac = nullptr;
};

class cACTION_BML_CLIENT_CLEAR_CLIENT_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CLIENT_CLEAR_CLIENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_CLIENT_CLEAR_CLIENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_CLIENT_CLEAR_CLIENT_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CLIENT_CLEAR_CLIENT_RESPONSE);
        }
        uint8_t& result();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_result = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_11K_SUPPORT_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_SELECTION_CHANNEL_POOL_REQUEST);
        }
        sMacAddr& radio_mac();
        int8_t& channel_pool_size();
        uint8_t* channel_pool(size_t idx = 0);
        bool set_channel_pool(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        int8_t* m_channel_pool_size = nullptr;
        uint8_t* m_channel_pool = nullptr;
        size_t m_channel_pool_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_SET_SELECTION_CHANNEL_POOL_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_SELECTION_CHANNEL_POOL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_SET_SELECTION_CHANNEL_POOL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_SET_SELECTION_CHANNEL_POOL_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_SELECTION_CHANNEL_POOL_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_SELECTION_CHANNEL_POOL_REQUEST);
        }
        sMacAddr& radio_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_radio_mac = nullptr;
};

class cACTION_BML_GET_SELECTION_CHANNEL_POOL_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_SELECTION_CHANNEL_POOL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_SELECTION_CHANNEL_POOL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_SELECTION_CHANNEL_POOL_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_SELECTION_CHANNEL_POOL_RESPONSE);
        }
        uint8_t& success();
        int8_t& channel_pool_size();
        uint8_t* channel_pool(size_t idx = 0);
        bool set_channel_pool(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
        int8_t* m_channel_pool_size = nullptr;
        uint8_t* m_channel_pool = nullptr;
        size_t m_channel_pool_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST : public BaseClass
{
    public:
        cACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNASSOC_STA_RCPI_QUERY_REQUEST);
        }
        sMacAddr& sta_mac();
        uint16_t& opclass();
        uint8_t& channel();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_sta_mac = nullptr;
        uint16_t* m_opclass = nullptr;
        uint8_t* m_channel = nullptr;
};

class cACTION_BML_UNASSOC_STA_RCPI_QUERY_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_UNASSOC_STA_RCPI_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_UNASSOC_STA_RCPI_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_UNASSOC_STA_RCPI_QUERY_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNASSOC_STA_RCPI_QUERY_RESPONSE);
        }
        //0 - Success, Otherwise error
        uint8_t& op_error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_op_error_code = nullptr;
};

class cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_REQUEST);
        }
        sMacAddr& sta_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_sta_mac = nullptr;
};

class cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_UNASSOC_STA_QUERY_RESULT_RESPONSE);
        }
        sMacAddr& sta_mac();
        uint16_t& opclass();
        uint8_t& channel();
        uint8_t& rcpi();
        uint32_t& measurement_delta();
        //0 - Success, Otherwise error
        uint8_t& op_error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_sta_mac = nullptr;
        uint16_t* m_opclass = nullptr;
        uint8_t* m_channel = nullptr;
        uint8_t* m_rcpi = nullptr;
        uint32_t* m_measurement_delta = nullptr;
        uint8_t* m_op_error_code = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_BML_H_
