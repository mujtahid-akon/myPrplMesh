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

#ifndef _TLVF_IEEE_1905_1_TLVAUTOCONFIGFREQBAND_H_
#define _TLVF_IEEE_1905_1_TLVAUTOCONFIGFREQBAND_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include <ostream>

namespace ieee1905_1 {


class tlvAutoconfigFreqBand : public BaseClass
{
    public:
        tlvAutoconfigFreqBand(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvAutoconfigFreqBand(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvAutoconfigFreqBand();

        enum eValue: uint8_t {
            IEEE_802_11_2_4_GHZ = 0x0,
            IEEE_802_11_5_GHZ = 0x1,
            IEEE_802_11_60_GHZ = 0x2,
            IEEE_802_11_6_GHZ = 0x3,
        };
        // Enum AutoPrint generated code snippet begining- DON'T EDIT!
        // clang-format off
        static const char *eValue_str(eValue enum_value) {
            switch (enum_value) {
            case IEEE_802_11_2_4_GHZ: return "IEEE_802_11_2_4_GHZ";
            case IEEE_802_11_5_GHZ:   return "IEEE_802_11_5_GHZ";
            case IEEE_802_11_60_GHZ:  return "IEEE_802_11_60_GHZ";
            case IEEE_802_11_6_GHZ:   return "IEEE_802_11_6_GHZ";
            }
            static std::string out_str = std::to_string(int(enum_value));
            return out_str.c_str();
        }
        friend inline std::ostream &operator<<(std::ostream &out, eValue value) { return out << eValue_str(value); }
        // clang-format on
        // Enum AutoPrint generated code snippet end
        
        const eTlvType& type();
        const uint16_t& length();
        eValue& value();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eValue* m_value = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVAUTOCONFIGFREQBAND_H_
