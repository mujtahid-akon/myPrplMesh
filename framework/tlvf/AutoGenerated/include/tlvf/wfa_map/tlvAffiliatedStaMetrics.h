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

#ifndef _TLVF_WFA_MAP_TLVAFFILIATEDSTAMETRICS_H_
#define _TLVF_WFA_MAP_TLVAFFILIATEDSTAMETRICS_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>

namespace wfa_map {


class tlvAffiliatedStaMetrics : public BaseClass
{
    public:
        tlvAffiliatedStaMetrics(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvAffiliatedStaMetrics(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvAffiliatedStaMetrics();

        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& sta_mac_addr();
        uint32_t& bytes_sent();
        uint32_t& bytes_received();
        uint32_t& packets_sent();
        uint32_t& packets_received();
        uint32_t& packets_sent_errors();
        //Reserved for future expansion (length inferred from tlvLength field)
        size_t reserved_length() { return m_reserved_idx__ * sizeof(uint8_t); }
        uint8_t* reserved(size_t idx = 0);
        bool set_reserved(const void* buffer, size_t size);
        bool alloc_reserved(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_sta_mac_addr = nullptr;
        uint32_t* m_bytes_sent = nullptr;
        uint32_t* m_bytes_received = nullptr;
        uint32_t* m_packets_sent = nullptr;
        uint32_t* m_packets_received = nullptr;
        uint32_t* m_packets_sent_errors = nullptr;
        uint8_t* m_reserved = nullptr;
        size_t m_reserved_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAFFILIATEDSTAMETRICS_H_
