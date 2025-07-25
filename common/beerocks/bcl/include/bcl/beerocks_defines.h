/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_DEFINES_H_
#define _BEEROCKS_DEFINES_H_

#include <cstddef>
#include <cstdint>
#include <stdint.h>

constexpr char BEEROCKS_CONTROLLER[] = "beerocks_controller";
constexpr char BEEROCKS_AGENT[]      = "beerocks_agent";
constexpr char BEEROCKS_BACKHAUL[]   = "beerocks_backhaul";
constexpr char BEEROCKS_FRONTHAUL[]  = "beerocks_fronthaul";
constexpr char BEEROCKS_AP_MANAGER[] = "beerocks_ap_manager";
constexpr char BEEROCKS_V_MESSAGE[]  = "beerocks_vendor_message";
constexpr char BEEROCKS_MONITOR[]    = "beerocks_monitor";
constexpr char BEEROCKS_CLI[]        = "beerocks_cli";

constexpr char BEEROCKS_BROKER_UDS[]     = "uds_broker";
constexpr char BEEROCKS_AGENT_UDS[]      = "uds_agent";
constexpr char BEEROCKS_FRONTHAUL_UDS[]  = "uds_fronthaul";
constexpr char BEEROCKS_CONTROLLER_UDS[] = "uds_controller";
constexpr char BEEROCKS_PLATFORM_UDS[]   = "uds_platform";
constexpr char BEEROCKS_BACKHAUL_UDS[]   = "uds_backhaul";

constexpr uint8_t IEEE80211_QOS_TID_MAX_UP =
    8; // Maximum number of user priorities for class of service

// configuration files path
#ifdef BEEROCKS_RDKB
constexpr char CONF_FILES_WRITABLE_PATH[] = "/nvram/";
#else
constexpr char CONF_FILES_WRITABLE_PATH[] = "./";
#endif

#if __GNUC__ >= 7 || __cplussplus >= 201703L
#define FALLTHROUGH __attribute__((fallthrough))
#else
// clang-format off
#define FALLTHROUGH do { } while (0)
// clang-format on
#endif

namespace beerocks {
namespace ieee1905_1_consts {
static constexpr int DISCOVERY_NOTIFICATION_TIMEOUT_SEC = 60;
static constexpr uint8_t AUTOCONFIG_M2_TIMEOUT_SECONDS  = 5;
} // namespace ieee1905_1_consts

namespace message {

enum eStructsConsts {
    VERSION_LENGTH            = 16,
    NODE_NAME_LENGTH          = 32,
    IFACE_NAME_LENGTH         = 32 + 4, //need extra 1 byte for null termination + alignment
    SUPPORTED_CHANNELS_LENGTH = 128,    //support upto # channels, every channel item is 32-bit
    HOSTAP_ERR_MSG_LENGTH     = 64,
    WIFI_SSID_MAX_LENGTH      = 32 + 1 + 3, //need extra 1 byte for null termination + alignment
    // The absolute maximum size of any frame according to the 802.11 specification
    // is MMPDU size of 2304 bytes. The actual size of an (re)association frame
    // should be in the range of a couple of hundreds of bytes, so be on the safe side
    // and set the maximum size to 2KB
    ASSOCIATION_MAX_LENGTH        = 2048,
    WIFI_PASS_MAX_LENGTH          = 64 + 1 + 3, //need extra 1 byte for null termination + alignment
    USER_PASS_LEN                 = 64 + 1 + 3, //need extra 1 byte for null termination + alignment
    DEV_INFO_STR_MAX_LEN          = 32,
    WPA_SCAN_FREQ_SEGMENTS        = 3,
    WPA_SCAN_MAX_RESULTS          = 8,
    RESTRICTED_CHANNEL_LENGTH     = 40,
    WIFI_SECURITY_TYPE_MAX_LENGTH = 32,
    BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH = 16,
    PLATFORM_ERROR_DATA_SIZE             = 256,
    WIFI_GENERIC_STRING_LENGTH           = 64,
    WIFI_OPERATING_STRING_LENGTH         = 16,
    WIFI_DATA_TRANSFER_RATES_LIST_LENGTH = 256,
    CHANNEL_SCAN_LIST_LENGTH             = 8,
    HT_MCS_SET_SIZE                      = 16,
    VHT_MCS_SET_SIZE                     = 32,
    HE_MCS_SET_SIZE                      = 32,
    DEV_MAX_RADIOS                       = 3,
    DSCP_MAPPING_LIST_LENGTH             = 64,
    CHIPSET_VENDOR_LENGTH                = 64,
};

enum eMessageConsts {
    MESSAGE_VERSION       = 6,
    MESSAGE_MAGIC         = 0x55CDABEF,
    MESSAGE_BUFFER_LENGTH = 8192,
};
} //namespace message

enum eGlobals {
    HIERARCHY_MAX                           = 14,
    MAX_RADIOS_PER_AGENT                    = 4,
    RSSI_MAX                                = 20,
    RSSI_MIN                                = -100,
    RSSI_INVALID                            = -127,
    RCPI_MIN                                = 0,
    RCPI_MAX                                = 220,
    RCPI_INVALID                            = 255,
    SNR_MIN                                 = 1,
    SNR_INVALID                             = 0,
    BRIDGE_RATE_100KB                       = 20000,
    PHY_RATE_100KB_MAX                      = 8666,
    PHY_RATE_100KB_MIN                      = 72,
    PHY_RATE_100KB_INVALID                  = 0,
    BSS_STEER_DISASSOC_TIMER_MS             = 200, // ~200ms
    SON_SLAVE_WATCHDOG_INTERVAL_MSC         = 5000,
    SON_SLAVE_INTERFACE_STATUS_INTERVAL_MSC = 2000,
    BH_SIGNAL_RSSI_THRESHOLD_LOW            = -75,
    BH_SIGNAL_RSSI_THRESHOLD_HIGH           = -40,
    BH_SIGNAL_RSSI_THRESHOLD_HYSTERESIS     = 8,
    TOUCH_PID_TIMEOUT_SECONDS               = 4, // beerocks_watchdog cycle (10 secs) / 2 - 1
    UCC_LISTENER_PORT                       = 8002,
};

enum eBeeRocksProcesses : uint8_t {
    BEEROCKS_PROCESS_NONE = 0,
    BEEROCKS_PROCESS_ALL,
    BEEROCKS_PROCESS_MASTER,
    BEEROCKS_PROCESS_SLAVE,
    BEEROCKS_PROCESS_MONITOR,
    BEEROCKS_PROCESS_PLATFORM,
};

enum eBeeRocksEntities : uint8_t {
    BEEROCKS_ENTITY_NONE             = 0,
    BEEROCKS_ENTITY_MASTER           = 1,
    BEEROCKS_ENTITY_SLAVE            = 2,
    BEEROCKS_ENTITY_AP_MANAGER       = 3,
    BEEROCKS_ENTITY_MONITOR          = 4,
    BEEROCKS_ENTITY_BACKHAUL_MANAGER = 5,
    BEEROCKS_ENTITY_PLATFORM_MANAGER = 6,
};

enum eBeeRocksIfaceIds {
    IFACE_ID_INVALID = -2,
    IFACE_RADIO_ID   = -1,
    IFACE_VAP_ID_MIN = 0,
    IFACE_TOTAL_VAPS = 16,
    IFACE_VAP_ID_MAX = IFACE_TOTAL_VAPS - 1,
};

enum eBeeRocksMessageDirection : uint8_t {
    BEEROCKS_DIRECTION_CONTROLLER = 0,
    BEEROCKS_DIRECTION_AGENT      = 1,
};

enum eManagementMode : int8_t {
    // same as on bpl_cfg.h
    MGMT_MODE_UNDEFINED                = -1,
    MGMT_MODE_MULTIAP_CONTROLLER_AGENT = 0,
    MGMT_MODE_MULTIAP_CONTROLLER       = 1,
    MGMT_MODE_MULTIAP_AGENT            = 2,
    MGMT_MODE_NOT_MULTIAP              = 3,
};

enum eProtocolPorts {
    MASTER_TCP_PORT          = 15060,
    MASTER_UDP_DISCOVER_PORT = 15060,
    CLI_PROXY_TCP_PORT       = 15061,
    ANALYZER_TCP_PORT        = 10000,
    UDP_4ADDR_PORT           = 16000,
};

enum eWiFiBandwidth : uint8_t {
    BANDWIDTH_UNKNOWN = 0,
    BANDWIDTH_20,
    BANDWIDTH_40,
    BANDWIDTH_80,
    BANDWIDTH_80_80,
    BANDWIDTH_160,
    BANDWIDTH_320_1,
    BANDWIDTH_320_2,
    BANDWIDTH_320,
    BANDWIDTH_MAX,
};

/**
 * enum eDfsState  DFS states for channels
 *
 * Channel states used by the DFS code.
 *
 * @USABLE: The channel can be used, but channel availability
 *	check (CAC) must be performed before using it for AP or IBSS.
 * @UNAVAILABLE: A radar has been detected on this channel, it
 *	is therefore marked as not available.
 * @AVAILABLE: The channel has been CAC checked and is available.
 */
enum eDfsState : uint8_t {
    USABLE,
    UNAVAILABLE,
    AVAILABLE,
    DFS_STATE_MAX,
};

enum eWiFiGuardInterval : uint8_t {
    LONG_GI  = 0,
    SHORT_GI = 1,
    MAX_GI,
};

enum eWiFiSS : uint8_t {
    SS_1 = 1,
    SS_2,
    SS_3,
    SS_4,
    SS_MAX,
};

enum eWiFiMCS : uint8_t {
    MCS_0 = 0,
    MCS_1,
    MCS_2,
    MCS_3,
    MCS_4,
    MCS_5,
    MCS_6,
    MCS_7,
    MCS_8,
    MCS_9,
    MCS_10,
    MCS_11,
    MCS_MAX,
};

enum eWiFiAntNum : uint8_t {
    ANT_NONE = 0,
    ANT_1X1,
    ANT_2X2,
    ANT_3X3,
    ANT_4X4,
    ANT_NUM_MAX,
};

enum eWiFiAntMode : uint8_t {
    ANT_MODE_1X1_SS1 = 0,
    ANT_MODE_2X2_SS1,
    ANT_MODE_2X2_SS2,
    ANT_MODE_3X3_SS1,
    ANT_MODE_3X3_SS2,
    ANT_MODE_3X3_SS3,
    ANT_MODE_4X4_SS1,
    ANT_MODE_4X4_SS2,
    ANT_MODE_4X4_SS3,
    ANT_MODE_4X4_SS4,
};

enum eWiFiStandard : uint8_t {
    STANDARD_NONE = 0x00,
    STANDARD_A    = 0x01,
    STANDARD_B    = 0x02,
    STANDARD_G    = 0x04,
    STANDARD_N    = 0x08,
    STANDARD_AC   = 0x10,
    STANDARD_AX   = 0x20,
};

enum eWiFiRfFilterFreq : uint16_t {
    RF_FILTER_FREQ_2_4G_LOW_START = 2412,
    RF_FILTER_FREQ_2_4G_LOW_END   = 2484,
    RF_FILTER_FREQ_5G_LOW_START   = 5180,
    RF_FILTER_FREQ_5G_LOW_END     = 5330,
    RF_FILTER_FREQ_5G_HIGH_START  = 5490,
    RF_FILTER_FREQ_5G_HIGH_END    = 5835,
};

enum eWifiChannelType : uint8_t {
    CH_PRIMARY   = 0,
    CH_SECONDARY = 1,
};

enum eWiFiChSwitchReason : uint8_t {
    CH_SWITCH_REASON_UNKNOWN        = 0,
    CH_SWITCH_REASON_RADAR          = 1,
    CH_SWITCH_REASON_20_COEXISTANCE = 2,
    CH_SWITCH_REASON_40_COEXISTANCE = 3,
};

enum eWifiChannelOverLaptype : uint8_t {
    CHANNEL_1_START_CH  = 1,
    CHANNEL_1_END_CH    = 5,
    CHANNEL_3_START_CH  = 1,
    CHANNEL_3_END_CH    = 8,
    CHANNEL_6_START_CH  = 6,
    CHANNEL_6_END_CH    = 10,
    CHANNEL_11_START_CH = 11,
    CHANNEL_11_END_CH   = 14,
};

enum eBeaconMeasurementSupportLevel : uint8_t {
    // bit field
    BEACON_MEAS_UNSUPPORTED     = 0x00,
    BEACON_MEAS_SSID_SUPPORTED  = 0x01,
    BEACON_MEAS_BSSID_SUPPORTED = 0x02,
    BEACON_MEAS_ALL_SUPPORTED   = 0x03,
};

enum eMeasurementMode11K : uint8_t {
    MEASURE_MODE_PASSIVE = 0,
    MEASURE_MODE_ACTIVE,
    MEASURE_MODE_TABLE,
    MEASURE_MODE_UNDEFINED,
};

enum eWifiDefaultBeaconMeasurementParams : int8_t {
    BEACON_MEASURE_DEFAULT_RANDOMIZATION_INTERVAL   = 0,
    BEACON_MEASURE_DEFAULT_ACTIVE_DURATION          = 50,
    BEACON_MEASURE_DEFAULT_PASSIVE_DURATION         = 120,
    BEACON_MEASURE_DEFAULT_ALL_BANDS_OPERATION_CODE = 0,
    BEACON_MEASURE_DEFAULT_AUTO_OPERATION_CODE      = -1,
    BEACON_MEASURE_DEFAULT_CHANNEL_ALL_CHANNELS     = 0,
    BEACON_MEASURE_DEFAULT_REPEATS                  = 0,
};

enum eHtCapsSmPowerSaveMode : uint8_t {
    HT_SM_POWER_SAVE_MODE_STATIC   = 0,
    HT_SM_POWER_SAVE_MODE_DYNAMIC  = 1,
    HT_SM_POWER_SAVE_MODE_RESERVED = 2,
    HT_SM_POWER_SAVE_MODE_DISABLED = 3,
};

enum eLogLevel : uint8_t {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ALL,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_WARNING,
};

enum eIfaceType : uint8_t {
    IFACE_TYPE_UNSUPPORTED = 0x00,
    //Wi-Fi
    IFACE_TYPE_WIFI_INTEL       = 0x01,
    IFACE_TYPE_WIFI_UNSPECIFIED = 0x40,
    IFACE_TYPE_WIFI_END         = 0xF0,
    //
    IFACE_TYPE_ETHERNET  = 0xF1,
    IFACE_TYPE_BRIDGE    = 0xFE,
    IFACE_TYPE_GW_BRIDGE = 0xFF,
};

constexpr char IFACE_TYPE_STR_UNSUPPORTED[]      = "UNSUPPORTED";
constexpr char IFACE_TYPE_STR_WIFI_INTEL[]       = "WIFI_INTEL";
constexpr char IFACE_TYPE_STR_WIFI_UNSPECIFIED[] = "WIFI_UNSPECIFIED";

constexpr char IFACE_TYPE_STR_ETHERNET[]  = "ETHERNET";
constexpr char IFACE_TYPE_STR_BRIDGE[]    = "BRIDGE";
constexpr char IFACE_TYPE_STR_GW_BRIDGE[] = "GW_BRIDGE";

enum eNodeState : uint8_t {
    STATE_DISCONNECTED = 0,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_ANY,
    STATE_MAX,
};

enum eType : uint8_t {
    TYPE_GW           = 0, // GW Bridge
    TYPE_IRE          = 1, // IRE Bridge
    TYPE_IRE_BACKHAUL = 2, // IRE Backhaul
    TYPE_SLAVE        = 3, // HOSTAP managed by BeeRocks slave
    TYPE_CLIENT       = 4, // Client Wi-Fi or Eth
    TYPE_ETH_SWITCH   = 5, // Eth switch under GW or IRE Bridge
    TYPE_ANY          = 6,
    TYPE_UNDEFINED    = 7,
    TYPE_MAX,
};

enum eArpStatus : uint8_t {
    ARP_NUD_INCOMPLETE = 0x01,
    ARP_NUD_REACHABLE  = 0x02,
    ARP_NUD_STALE      = 0x04,
    ARP_NUD_DELAY      = 0x08,
    ARP_NUD_PROBE      = 0x10,
    ARP_NUD_FAILED     = 0x20,
};

enum eArpSource : uint8_t {
    ARP_SRC_ETH_BACK = 0,
    ARP_SRC_ETH_FRONT,
    ARP_SRC_WIRELESS_BACK,
    ARP_SRC_WIRELESS_FRONT
};

enum eArpType : uint8_t { ARP_TYPE_NEWNEIGH = 0, ARP_TYPE_DELNEIGH, ARP_TYPE_GETNEIGH };

enum eFreqType {
    FREQ_24G    = 0,
    FREQ_5G     = 1,
    FREQ_58G    = 2,
    FREQ_24G_5G = 3,
    FREQ_6G     = 4,
    FREQ_AUTO,
    FREQ_UNKNOWN,
};

enum eSubbandType {
    LOW_SUBBAND          = 0,
    LOW_SUBBAND_NON_DFS  = 1,
    LOW_SUBBAND_DFS      = 2,
    HIGH_SUBBAND         = 3,
    HIGH_SUBBAND_NON_DFS = 4,
    HIGH_SUBBAND_DFS,
    ANY_SUBBAND,
    SUBBAND_UNKNOWN,
};

enum eDfsSubbandType {
    DFS_LOW_SUBBAND         = 0,
    DFS_HIGH_FIRST_SUBBAND  = 1,
    DFS_HIGH_SECOND_SUBBAND = 2,
    DFS_HIGH_THIRD_SUBBAND  = 3,
    DFS_SUBBAND_UNKNOWN,
};

enum eRadioBandCapability {
    LOW_SUBBAND_ONLY = 0,
    HIGH_SUBBAND_ONLY,
    BOTH_SUBBAND,
    SUBBAND_CAPABILITY_UNKNOWN,
};

enum eSlaveJoinResponseErrCode {
    JOIN_RESP_NO_ERROR = 0,
    JOIN_RESP_VERSION_MISMATCH,
    JOIN_RESP_SSID_MISMATCH,
    JOIN_RESP_REJECT,
};

enum eApActiveMode : uint8_t { AP_IDLE_MODE = 0, AP_ACTIVE_MODE, AP_INVALID_MODE };

enum eBssType {
    BSS_TYPE_TEARDOWN = 0,
    BSS_TYPE_BACKHAUL,
    BSS_TYPE_FRONTHAUL,
    BSS_TYPE_BACK_FRONTHAUL,
    BSS_TYPE_INVALID
};

enum class eChannelScanStatusCode : uint8_t {
    SUCCESS = 0,
    INTERNAL_FAILURE,
    POOL_TOO_BIG,
    TRIGGERED_EVENT_TIMEOUT,
    RESULTS_READY_EVENT_TIMEOUT,
    RESULTS_DUMP_EVENT_TIMEOUT,
    ABORTED_BY_DRIVER,
    RESULTS_EMPTY,
    INVALID_PARAMS,
    CHANNEL_SCAN_REPORT_TIMEOUT
};

enum class eChannelScanOperationCode : uint8_t {
    SUCCESS = 0,
    ERROR,
    SCAN_IN_PROGRESS,
    INVALID_PARAMS_ENABLE,
    INVALID_PARAMS_DWELLTIME,
    INVALID_PARAMS_SCANTIME,
    INVALID_PARAMS_CHANNELPOOL
};

enum class eChannelSwitchStatus : uint8_t {
    SUCCESS = 0,
    ERROR,
    INVALID_BANDWIDTH_AND_CHANNEL,
    INOPERABLE_CHANNEL
};

enum class eChannelPreferenceRankingConsts : int8_t {
    INVALID      = -1,
    NON_OPERABLE = 0,
    LOWEST       = 1,
    BEST         = 15
};

enum class eUnAssocStaLinkMetricErrCode : uint8_t {
    SUCCESS = 0,
    ERROR, /* Generic error apart from below self explained errors */
    LINK_METRICS_COLLECTION_NOT_DONE,
    RESULT_NOT_AVAILABLE_FOR_STA
};

constexpr int CHANNEL_SCAN_INVALID_PARAM = -1;
constexpr int SCAN_ALL_CHANNELS          = 0;

constexpr int PARAMETER_NOT_CONFIGURED = -1;

// array of allowed ifname prefix strings
static const char *const ifname_prefix_list[] = {"wlan", "wl"};

// string of separator characters delimiting the ifname prefix
static const char *const ifname_separators = ".-";

enum class eZWDFS_flags : uint8_t {
    DISABLE      = 0b00000000,
    ON_RADAR     = 0b00000001,
    ON_SELECTION = 0b00000010,
    PRE_CAC      = 0b00000100,
    ALL          = 0b11111111
};

} // namespace beerocks

#endif //_BEEROCKS_DEFINES_H_
