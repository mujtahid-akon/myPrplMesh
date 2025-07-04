/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2022 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef AMBIORIX_EVENT_H_
#define AMBIORIX_EVENT_H_

#include <ambiorix_variant.h>

#include <functional>

constexpr char AMX_CL_OBJECT_CHANGED_EVT[]   = "dm:object-changed";
constexpr char AMX_CL_OBJECT_ADDED_EVT[]     = "dm:object-added";
constexpr char AMX_CL_OBJECT_REMOVED_EVT[]   = "dm:object-removed";
constexpr char AMX_CL_INSTANCE_ADDED_EVT[]   = "dm:instance-added";
constexpr char AMX_CL_INSTANCE_REMOVED_EVT[] = "dm:instance-removed";
constexpr char AMX_CL_PERIODIC_INFORM_EVT[]  = "dm:periodic-inform";
constexpr char AMX_CL_WPS_PAIRING_DONE[]     = "pairingDone";
constexpr char AMX_CL_SCAN_COMPLETE_EVT[]    = "ScanComplete";
constexpr char AMX_CL_BSS_TM_RESPONSE_EVT[]  = "BSS-TM-RESP";
constexpr char AMX_CL_RSSI_UPDATE_EVT[]      = "RssiUpdate";
constexpr char AMX_CL_CHANNEL_CHANGE_EVT[]   = "Channel change event";
constexpr char AMX_CL_WPA_CTRL_EVT[]         = "wpaCtrlEvents";
constexpr char AMX_CL_MGMT_ACT_FRAME_EVT[]   = "MgmtActionFrameReceived";

namespace beerocks {
namespace wbapi {

using AmbiorixEventCallbak = std::function<void(AmbiorixVariant &event_data)>;

/**
 * @struct sAmbiorixEventHandler
 */
struct sAmbiorixEventHandler {
    std::string event_type;
    AmbiorixEventCallbak callback_fn;
};

/**
 * @struct sAmbiorixSubscriptionInfo
 */
struct sAmbiorixSubscriptionInfo {
    std::shared_ptr<sAmbiorixEventHandler> handler;
    amxb_subscription_t *subscription_ctx = nullptr;
};

} // namespace wbapi
} // namespace beerocks

#endif /* AMBIORIX_EVENT_H_ */
