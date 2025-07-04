/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __TLVF_AIRTIES_UTILS_DUMMY_H__
#define __TLVF_AIRTIES_UTILS_DUMMY_H__

#include <tlvf/CmduMessageTx.h>

namespace airties {

class tlvf_airties_utils {
public:
    static bool add_airties_deviceinfo_tlv(ieee1905_1::CmduMessageTx &m_cmdu_tx);
    static bool add_airties_version_reporting_tlv(ieee1905_1::CmduMessageTx &cmdu_tx);
    static bool add_device_metrics(ieee1905_1::CmduMessageTx &cmdu_tx);
    static bool add_airties_ethernet_interface_tlv(ieee1905_1::CmduMessageTx &cmdu_tx);
};
} // namespace airties

#endif // __TLVF_AIRTIES_UTILS_DUMMY_H__
