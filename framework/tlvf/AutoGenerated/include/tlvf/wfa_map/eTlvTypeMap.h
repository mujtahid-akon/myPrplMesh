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

#ifndef _TLVF_WFA_MAP_ETLVTYPEMAP_H_
#define _TLVF_WFA_MAP_ETLVTYPEMAP_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <ostream>

namespace wfa_map {

enum class eTlvTypeMap : uint8_t {
    TLV_SUPPORTED_SERVICE = 0x80,
    TLV_SEARCHED_SERVICE = 0x81,
    TLV_AP_RADIO_IDENTIFIER = 0x82,
    TLV_AP_OPERATIONAL_BSS = 0x83,
    TLV_ASSOCIATED_CLIENTS = 0x84,
    TLV_AP_RADIO_BASIC_CAPABILITIES = 0x85,
    TLV_AP_HT_CAPABILITIES = 0x86,
    TLV_AP_VHT_CAPABILITIES = 0x87,
    TLV_AP_HE_CAPABILITIES = 0x88,
    TLV_STEERING_POLICY = 0x89,
    TLV_METRIC_REPORTING_POLICY = 0x8a,
    TLV_CHANNEL_PREFERENCE = 0x8b,
    TLV_RADIO_OPERATION_RESTRICTION = 0x8c,
    TLV_TRANSMIT_POWER_LIMIT = 0x8d,
    TLV_CHANNEL_SELECTION_RESPONSE = 0x8e,
    TLV_OPERATING_CHANNEL_REPORT = 0x8f,
    TLV_CLIENT_INFO = 0x90,
    TLV_CLIENT_CAPABILITY_REPORT = 0x91,
    TLV_CLIENT_ASSOCIATION_EVENT = 0x92,
    TLV_AP_METRIC_QUERY = 0x93,
    TLV_AP_METRIC = 0x94,
    TLV_STAMAC_ADDRESS_TYPE = 0x95,
    TLV_ASSOCIATED_STA_LINK_METRICS = 0x96,
    TLV_UNASSOCIATED_STA_LINK_METRICS_QUERY = 0x97,
    TLV_UNASSOCIATED_STA_LINK_METRICS_RESPONSE = 0x98,
    TLV_BEACON_METRICS_QUERY = 0x99,
    TLV_BEACON_METRICS_RESPONSE = 0x9a,
    TLV_STEERING_REQUEST = 0x9b,
    TLV_STEERING_BTM_REPORT = 0x9c,
    TLV_CLIENT_ASSOCIATION_CONTROL_REQUEST = 0x9d,
    TLV_BACKHAUL_STEERING_REQUEST = 0x9e,
    TLV_BACKHAUL_STEERING_RESPONSE = 0x9f,
    TLV_HIGHER_LAYER_DATA = 0xa0,
    TLV_AP_CAPABILITY = 0xa1,
    TLV_ASSOCIATED_STA_TRAFFIC_STATS = 0xa2,
    TLV_ERROR_CODE = 0xa3,
    TLV_CHANNEL_SCAN_REPORTING_POLICY = 0xa4,
    TLV_CHANNEL_SCAN_CAPABILITIES = 0xa5,
    TLV_CHANNEL_SCAN_REQUEST = 0xa6,
    TLV_CHANNEL_SCAN_RESULT = 0xa7,
    TLV_TIMESTAMP = 0xa8,
    TLV_1905_LAYER_SECURITY_CAPABILITY = 0xa9,
    TLV_MIC = 0xab,
    TLV_ENCRYPTED_PAYLOAD = 0xac,
    TLV_PROFILE2_CAC_REQUEST = 0xad,
    TLV_PROFILE2_CAC_TERMINATION = 0xae,
    TLV_PROFILE2_CAC_COMPLETION_REPORT = 0xaf,
    TLV_ASSOCIATED_WIFI_6_STA_STATUS_REPORT = 0xb0,
    TLV_PROFILE2_CAC_STATUS_REPORT = 0xb1,
    TLV_PROFILE2_CAC_CAPABILITIES = 0xb2,
    TLV_PROFILE2_MULTIAP_PROFILE = 0xb3,
    TLV_PROFILE2_AP_CAPABILITY = 0xb4,
    TLV_PROFILE2_DEFAULT_802_1Q_SETTINGS = 0xb5,
    TLV_PROFILE2_TRAFFIC_SEPARATION_POLICY = 0xb6,
    TLV_BSS_CONFIGURATION_REPORT = 0xb7,
    TLV_BSSID = 0xb8,
    TLV_SERVICE_PRIORITIZATION_RULE = 0xb9,
    TLV_DSCP_MAPPING_TABLE = 0xba,
    TLV_BSS_CONFIGURATION_REQUEST = 0xbb,
    TLV_PROFILE2_ERROR_CODE = 0xbc,
    TLV_BSS_CONFIGURATION_RESPONSE = 0xbd,
    TLV_PROFILE2_AP_RADIO_ADVANCED_CAPABILITIES = 0xbe,
    TLV_PROFILE2_ASSOCIATION_STATUS_NOTIFICATION = 0xbf,
    TLV_TUNNELLED_SOURCE_INFO = 0xc0,
    TLV_TUNNELLED_PROTOCOL_TYPE = 0xc1,
    TLV_TUNNELLED_DATA = 0xc2,
    TLV_PROFILE2_STEERING_REQUEST = 0xc3,
    TLV_PROFILE2_UNSUCCESSFUL_ASSOCIATION_POLICY = 0xc4,
    TLV_PROFILE2_METRIC_COLLECTION_INTERVAL = 0xc5,
    TLV_PROFILE2_RADIO_METRICS = 0xc6,
    TLV_AP_EXTENDED_METRICS = 0xc7,
    TLV_ASSOCIATED_STA_EXTENDED_LINK_METRICS = 0xc8,
    TLV_PROFILE2_STATUS_CODE = 0xc9,
    TLV_PROFILE2_REASON_CODE = 0xca,
    TLV_BACKHAUL_STA_RADIO_CAPABILITIES = 0xcb,
    TLV_AKM_SUITE_CAPABILITIES = 0xcc,
    TLV_1905_ENCAP_DPP = 0xcd,
    TLV_1905_ENCAP_EAPOL = 0xce,
    TLV_DPP_BOOTSTRAPPING_URI_NOTIFICATION = 0xcf,
    TLV_BACKHAUL_BSS_CONFIGURATION = 0xd0,
    TLV_DPP_MESSAGE = 0xd1,
    TLV_DPP_CCE_INDICATION = 0xd2,
    TLV_DPP_CHIRP_VALUE = 0xd3,
    TLV_DEVICE_INVENTORY = 0xd4,
    TLV_AGENT_LIST = 0xd5,
    TLV_ANTICIPATED_CHANNEL_PREFERENCE = 0xd6,
    TLV_SPATIAL_REUSE_CONFIG_RESPONSE = 0xda,
    TLV_QOS_MANAGEMENT_POLICY = 0xdb,
    TLV_QOS_MANAGEMENT_DESCRIPTOR = 0xdc,
    TLV_CONTROLLER_CAPABILITY = 0xdd,
    TLV_VIRTUAL_BSS = 0xde,
    TLV_SPATIAL_REUSE_REQUEST = 0xd8,
    TLV_SPATIAL_REUSE_REPORT = 0xd9,
    TLV_WIFI_7_AGENT_CAPABILITIES = 0xdf,
    TLV_AGENT_AP_MLD_CONFIGURATION = 0xe0,
    TLV_BACKHAUL_STA_MLD_CONFIGURATION = 0xe1,
    TLV_ASSOCIATED_STA_MLD_CONFIGURATION_REPORT = 0xe2,
    TLV_MLD_STRUCTURE = 0xe3,
    TLV_AFFILIATED_STA_METRICS = 0xe4,
    TLV_AFFILIATED_AP_METRICS = 0xe5,
    TLV_TID_TO_LINK_MAPPING_POLICY = 0xe6,
    TLV_EHT_OPERATIONS = 0xe7,
    TLV_AVAILABLE_SPECTRUM_INQUIRY_REQUEST = 0xe8,
    TLV_AVAILABLE_SPECTRUM_INQUIRY_RESPONSE = 0xe9,
    TLV_AP_WIFI_6_CAPABILITIES = 0xaa,
};
// Enum AutoPrint generated code snippet begining- DON'T EDIT!
// clang-format off
static const char *eTlvTypeMap_str(eTlvTypeMap enum_value) {
    switch (enum_value) {
    case eTlvTypeMap::TLV_SUPPORTED_SERVICE:                        return "eTlvTypeMap::TLV_SUPPORTED_SERVICE";
    case eTlvTypeMap::TLV_SEARCHED_SERVICE:                         return "eTlvTypeMap::TLV_SEARCHED_SERVICE";
    case eTlvTypeMap::TLV_AP_RADIO_IDENTIFIER:                      return "eTlvTypeMap::TLV_AP_RADIO_IDENTIFIER";
    case eTlvTypeMap::TLV_AP_OPERATIONAL_BSS:                       return "eTlvTypeMap::TLV_AP_OPERATIONAL_BSS";
    case eTlvTypeMap::TLV_ASSOCIATED_CLIENTS:                       return "eTlvTypeMap::TLV_ASSOCIATED_CLIENTS";
    case eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES:              return "eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES";
    case eTlvTypeMap::TLV_AP_HT_CAPABILITIES:                       return "eTlvTypeMap::TLV_AP_HT_CAPABILITIES";
    case eTlvTypeMap::TLV_AP_VHT_CAPABILITIES:                      return "eTlvTypeMap::TLV_AP_VHT_CAPABILITIES";
    case eTlvTypeMap::TLV_AP_HE_CAPABILITIES:                       return "eTlvTypeMap::TLV_AP_HE_CAPABILITIES";
    case eTlvTypeMap::TLV_STEERING_POLICY:                          return "eTlvTypeMap::TLV_STEERING_POLICY";
    case eTlvTypeMap::TLV_METRIC_REPORTING_POLICY:                  return "eTlvTypeMap::TLV_METRIC_REPORTING_POLICY";
    case eTlvTypeMap::TLV_CHANNEL_PREFERENCE:                       return "eTlvTypeMap::TLV_CHANNEL_PREFERENCE";
    case eTlvTypeMap::TLV_RADIO_OPERATION_RESTRICTION:              return "eTlvTypeMap::TLV_RADIO_OPERATION_RESTRICTION";
    case eTlvTypeMap::TLV_TRANSMIT_POWER_LIMIT:                     return "eTlvTypeMap::TLV_TRANSMIT_POWER_LIMIT";
    case eTlvTypeMap::TLV_CHANNEL_SELECTION_RESPONSE:               return "eTlvTypeMap::TLV_CHANNEL_SELECTION_RESPONSE";
    case eTlvTypeMap::TLV_OPERATING_CHANNEL_REPORT:                 return "eTlvTypeMap::TLV_OPERATING_CHANNEL_REPORT";
    case eTlvTypeMap::TLV_CLIENT_INFO:                              return "eTlvTypeMap::TLV_CLIENT_INFO";
    case eTlvTypeMap::TLV_CLIENT_CAPABILITY_REPORT:                 return "eTlvTypeMap::TLV_CLIENT_CAPABILITY_REPORT";
    case eTlvTypeMap::TLV_CLIENT_ASSOCIATION_EVENT:                 return "eTlvTypeMap::TLV_CLIENT_ASSOCIATION_EVENT";
    case eTlvTypeMap::TLV_AP_METRIC_QUERY:                          return "eTlvTypeMap::TLV_AP_METRIC_QUERY";
    case eTlvTypeMap::TLV_AP_METRIC:                                return "eTlvTypeMap::TLV_AP_METRIC";
    case eTlvTypeMap::TLV_STAMAC_ADDRESS_TYPE:                      return "eTlvTypeMap::TLV_STAMAC_ADDRESS_TYPE";
    case eTlvTypeMap::TLV_ASSOCIATED_STA_LINK_METRICS:              return "eTlvTypeMap::TLV_ASSOCIATED_STA_LINK_METRICS";
    case eTlvTypeMap::TLV_UNASSOCIATED_STA_LINK_METRICS_QUERY:      return "eTlvTypeMap::TLV_UNASSOCIATED_STA_LINK_METRICS_QUERY";
    case eTlvTypeMap::TLV_UNASSOCIATED_STA_LINK_METRICS_RESPONSE:   return "eTlvTypeMap::TLV_UNASSOCIATED_STA_LINK_METRICS_RESPONSE";
    case eTlvTypeMap::TLV_BEACON_METRICS_QUERY:                     return "eTlvTypeMap::TLV_BEACON_METRICS_QUERY";
    case eTlvTypeMap::TLV_BEACON_METRICS_RESPONSE:                  return "eTlvTypeMap::TLV_BEACON_METRICS_RESPONSE";
    case eTlvTypeMap::TLV_STEERING_REQUEST:                         return "eTlvTypeMap::TLV_STEERING_REQUEST";
    case eTlvTypeMap::TLV_STEERING_BTM_REPORT:                      return "eTlvTypeMap::TLV_STEERING_BTM_REPORT";
    case eTlvTypeMap::TLV_CLIENT_ASSOCIATION_CONTROL_REQUEST:       return "eTlvTypeMap::TLV_CLIENT_ASSOCIATION_CONTROL_REQUEST";
    case eTlvTypeMap::TLV_BACKHAUL_STEERING_REQUEST:                return "eTlvTypeMap::TLV_BACKHAUL_STEERING_REQUEST";
    case eTlvTypeMap::TLV_BACKHAUL_STEERING_RESPONSE:               return "eTlvTypeMap::TLV_BACKHAUL_STEERING_RESPONSE";
    case eTlvTypeMap::TLV_HIGHER_LAYER_DATA:                        return "eTlvTypeMap::TLV_HIGHER_LAYER_DATA";
    case eTlvTypeMap::TLV_AP_CAPABILITY:                            return "eTlvTypeMap::TLV_AP_CAPABILITY";
    case eTlvTypeMap::TLV_ASSOCIATED_STA_TRAFFIC_STATS:             return "eTlvTypeMap::TLV_ASSOCIATED_STA_TRAFFIC_STATS";
    case eTlvTypeMap::TLV_ERROR_CODE:                               return "eTlvTypeMap::TLV_ERROR_CODE";
    case eTlvTypeMap::TLV_CHANNEL_SCAN_REPORTING_POLICY:            return "eTlvTypeMap::TLV_CHANNEL_SCAN_REPORTING_POLICY";
    case eTlvTypeMap::TLV_CHANNEL_SCAN_CAPABILITIES:                return "eTlvTypeMap::TLV_CHANNEL_SCAN_CAPABILITIES";
    case eTlvTypeMap::TLV_CHANNEL_SCAN_REQUEST:                     return "eTlvTypeMap::TLV_CHANNEL_SCAN_REQUEST";
    case eTlvTypeMap::TLV_CHANNEL_SCAN_RESULT:                      return "eTlvTypeMap::TLV_CHANNEL_SCAN_RESULT";
    case eTlvTypeMap::TLV_TIMESTAMP:                                return "eTlvTypeMap::TLV_TIMESTAMP";
    case eTlvTypeMap::TLV_1905_LAYER_SECURITY_CAPABILITY:           return "eTlvTypeMap::TLV_1905_LAYER_SECURITY_CAPABILITY";
    case eTlvTypeMap::TLV_MIC:                                      return "eTlvTypeMap::TLV_MIC";
    case eTlvTypeMap::TLV_ENCRYPTED_PAYLOAD:                        return "eTlvTypeMap::TLV_ENCRYPTED_PAYLOAD";
    case eTlvTypeMap::TLV_PROFILE2_CAC_REQUEST:                     return "eTlvTypeMap::TLV_PROFILE2_CAC_REQUEST";
    case eTlvTypeMap::TLV_PROFILE2_CAC_TERMINATION:                 return "eTlvTypeMap::TLV_PROFILE2_CAC_TERMINATION";
    case eTlvTypeMap::TLV_PROFILE2_CAC_COMPLETION_REPORT:           return "eTlvTypeMap::TLV_PROFILE2_CAC_COMPLETION_REPORT";
    case eTlvTypeMap::TLV_ASSOCIATED_WIFI_6_STA_STATUS_REPORT:      return "eTlvTypeMap::TLV_ASSOCIATED_WIFI_6_STA_STATUS_REPORT";
    case eTlvTypeMap::TLV_PROFILE2_CAC_STATUS_REPORT:               return "eTlvTypeMap::TLV_PROFILE2_CAC_STATUS_REPORT";
    case eTlvTypeMap::TLV_PROFILE2_CAC_CAPABILITIES:                return "eTlvTypeMap::TLV_PROFILE2_CAC_CAPABILITIES";
    case eTlvTypeMap::TLV_PROFILE2_MULTIAP_PROFILE:                 return "eTlvTypeMap::TLV_PROFILE2_MULTIAP_PROFILE";
    case eTlvTypeMap::TLV_PROFILE2_AP_CAPABILITY:                   return "eTlvTypeMap::TLV_PROFILE2_AP_CAPABILITY";
    case eTlvTypeMap::TLV_PROFILE2_DEFAULT_802_1Q_SETTINGS:         return "eTlvTypeMap::TLV_PROFILE2_DEFAULT_802_1Q_SETTINGS";
    case eTlvTypeMap::TLV_PROFILE2_TRAFFIC_SEPARATION_POLICY:       return "eTlvTypeMap::TLV_PROFILE2_TRAFFIC_SEPARATION_POLICY";
    case eTlvTypeMap::TLV_BSS_CONFIGURATION_REPORT:                 return "eTlvTypeMap::TLV_BSS_CONFIGURATION_REPORT";
    case eTlvTypeMap::TLV_BSSID:                                    return "eTlvTypeMap::TLV_BSSID";
    case eTlvTypeMap::TLV_SERVICE_PRIORITIZATION_RULE:              return "eTlvTypeMap::TLV_SERVICE_PRIORITIZATION_RULE";
    case eTlvTypeMap::TLV_DSCP_MAPPING_TABLE:                       return "eTlvTypeMap::TLV_DSCP_MAPPING_TABLE";
    case eTlvTypeMap::TLV_BSS_CONFIGURATION_REQUEST:                return "eTlvTypeMap::TLV_BSS_CONFIGURATION_REQUEST";
    case eTlvTypeMap::TLV_PROFILE2_ERROR_CODE:                      return "eTlvTypeMap::TLV_PROFILE2_ERROR_CODE";
    case eTlvTypeMap::TLV_BSS_CONFIGURATION_RESPONSE:               return "eTlvTypeMap::TLV_BSS_CONFIGURATION_RESPONSE";
    case eTlvTypeMap::TLV_PROFILE2_AP_RADIO_ADVANCED_CAPABILITIES:  return "eTlvTypeMap::TLV_PROFILE2_AP_RADIO_ADVANCED_CAPABILITIES";
    case eTlvTypeMap::TLV_PROFILE2_ASSOCIATION_STATUS_NOTIFICATION: return "eTlvTypeMap::TLV_PROFILE2_ASSOCIATION_STATUS_NOTIFICATION";
    case eTlvTypeMap::TLV_TUNNELLED_SOURCE_INFO:                    return "eTlvTypeMap::TLV_TUNNELLED_SOURCE_INFO";
    case eTlvTypeMap::TLV_TUNNELLED_PROTOCOL_TYPE:                  return "eTlvTypeMap::TLV_TUNNELLED_PROTOCOL_TYPE";
    case eTlvTypeMap::TLV_TUNNELLED_DATA:                           return "eTlvTypeMap::TLV_TUNNELLED_DATA";
    case eTlvTypeMap::TLV_PROFILE2_STEERING_REQUEST:                return "eTlvTypeMap::TLV_PROFILE2_STEERING_REQUEST";
    case eTlvTypeMap::TLV_PROFILE2_UNSUCCESSFUL_ASSOCIATION_POLICY: return "eTlvTypeMap::TLV_PROFILE2_UNSUCCESSFUL_ASSOCIATION_POLICY";
    case eTlvTypeMap::TLV_PROFILE2_METRIC_COLLECTION_INTERVAL:      return "eTlvTypeMap::TLV_PROFILE2_METRIC_COLLECTION_INTERVAL";
    case eTlvTypeMap::TLV_PROFILE2_RADIO_METRICS:                   return "eTlvTypeMap::TLV_PROFILE2_RADIO_METRICS";
    case eTlvTypeMap::TLV_AP_EXTENDED_METRICS:                      return "eTlvTypeMap::TLV_AP_EXTENDED_METRICS";
    case eTlvTypeMap::TLV_ASSOCIATED_STA_EXTENDED_LINK_METRICS:     return "eTlvTypeMap::TLV_ASSOCIATED_STA_EXTENDED_LINK_METRICS";
    case eTlvTypeMap::TLV_PROFILE2_STATUS_CODE:                     return "eTlvTypeMap::TLV_PROFILE2_STATUS_CODE";
    case eTlvTypeMap::TLV_PROFILE2_REASON_CODE:                     return "eTlvTypeMap::TLV_PROFILE2_REASON_CODE";
    case eTlvTypeMap::TLV_BACKHAUL_STA_RADIO_CAPABILITIES:          return "eTlvTypeMap::TLV_BACKHAUL_STA_RADIO_CAPABILITIES";
    case eTlvTypeMap::TLV_AKM_SUITE_CAPABILITIES:                   return "eTlvTypeMap::TLV_AKM_SUITE_CAPABILITIES";
    case eTlvTypeMap::TLV_1905_ENCAP_DPP:                           return "eTlvTypeMap::TLV_1905_ENCAP_DPP";
    case eTlvTypeMap::TLV_1905_ENCAP_EAPOL:                         return "eTlvTypeMap::TLV_1905_ENCAP_EAPOL";
    case eTlvTypeMap::TLV_DPP_BOOTSTRAPPING_URI_NOTIFICATION:       return "eTlvTypeMap::TLV_DPP_BOOTSTRAPPING_URI_NOTIFICATION";
    case eTlvTypeMap::TLV_BACKHAUL_BSS_CONFIGURATION:               return "eTlvTypeMap::TLV_BACKHAUL_BSS_CONFIGURATION";
    case eTlvTypeMap::TLV_DPP_MESSAGE:                              return "eTlvTypeMap::TLV_DPP_MESSAGE";
    case eTlvTypeMap::TLV_DPP_CCE_INDICATION:                       return "eTlvTypeMap::TLV_DPP_CCE_INDICATION";
    case eTlvTypeMap::TLV_DPP_CHIRP_VALUE:                          return "eTlvTypeMap::TLV_DPP_CHIRP_VALUE";
    case eTlvTypeMap::TLV_DEVICE_INVENTORY:                         return "eTlvTypeMap::TLV_DEVICE_INVENTORY";
    case eTlvTypeMap::TLV_AGENT_LIST:                               return "eTlvTypeMap::TLV_AGENT_LIST";
    case eTlvTypeMap::TLV_ANTICIPATED_CHANNEL_PREFERENCE:           return "eTlvTypeMap::TLV_ANTICIPATED_CHANNEL_PREFERENCE";
    case eTlvTypeMap::TLV_SPATIAL_REUSE_CONFIG_RESPONSE:            return "eTlvTypeMap::TLV_SPATIAL_REUSE_CONFIG_RESPONSE";
    case eTlvTypeMap::TLV_QOS_MANAGEMENT_POLICY:                    return "eTlvTypeMap::TLV_QOS_MANAGEMENT_POLICY";
    case eTlvTypeMap::TLV_QOS_MANAGEMENT_DESCRIPTOR:                return "eTlvTypeMap::TLV_QOS_MANAGEMENT_DESCRIPTOR";
    case eTlvTypeMap::TLV_CONTROLLER_CAPABILITY:                    return "eTlvTypeMap::TLV_CONTROLLER_CAPABILITY";
    case eTlvTypeMap::TLV_VIRTUAL_BSS:                              return "eTlvTypeMap::TLV_VIRTUAL_BSS";
    case eTlvTypeMap::TLV_SPATIAL_REUSE_REQUEST:                    return "eTlvTypeMap::TLV_SPATIAL_REUSE_REQUEST";
    case eTlvTypeMap::TLV_SPATIAL_REUSE_REPORT:                     return "eTlvTypeMap::TLV_SPATIAL_REUSE_REPORT";
    case eTlvTypeMap::TLV_WIFI_7_AGENT_CAPABILITIES:                return "eTlvTypeMap::TLV_WIFI_7_AGENT_CAPABILITIES";
    case eTlvTypeMap::TLV_AGENT_AP_MLD_CONFIGURATION:               return "eTlvTypeMap::TLV_AGENT_AP_MLD_CONFIGURATION";
    case eTlvTypeMap::TLV_BACKHAUL_STA_MLD_CONFIGURATION:           return "eTlvTypeMap::TLV_BACKHAUL_STA_MLD_CONFIGURATION";
    case eTlvTypeMap::TLV_ASSOCIATED_STA_MLD_CONFIGURATION_REPORT:  return "eTlvTypeMap::TLV_ASSOCIATED_STA_MLD_CONFIGURATION_REPORT";
    case eTlvTypeMap::TLV_MLD_STRUCTURE:                            return "eTlvTypeMap::TLV_MLD_STRUCTURE";
    case eTlvTypeMap::TLV_AFFILIATED_STA_METRICS:                   return "eTlvTypeMap::TLV_AFFILIATED_STA_METRICS";
    case eTlvTypeMap::TLV_AFFILIATED_AP_METRICS:                    return "eTlvTypeMap::TLV_AFFILIATED_AP_METRICS";
    case eTlvTypeMap::TLV_TID_TO_LINK_MAPPING_POLICY:               return "eTlvTypeMap::TLV_TID_TO_LINK_MAPPING_POLICY";
    case eTlvTypeMap::TLV_EHT_OPERATIONS:                           return "eTlvTypeMap::TLV_EHT_OPERATIONS";
    case eTlvTypeMap::TLV_AVAILABLE_SPECTRUM_INQUIRY_REQUEST:       return "eTlvTypeMap::TLV_AVAILABLE_SPECTRUM_INQUIRY_REQUEST";
    case eTlvTypeMap::TLV_AVAILABLE_SPECTRUM_INQUIRY_RESPONSE:      return "eTlvTypeMap::TLV_AVAILABLE_SPECTRUM_INQUIRY_RESPONSE";
    case eTlvTypeMap::TLV_AP_WIFI_6_CAPABILITIES:                   return "eTlvTypeMap::TLV_AP_WIFI_6_CAPABILITIES";
    }
    static std::string out_str = std::to_string(int(enum_value));
    return out_str.c_str();
}
inline std::ostream &operator<<(std::ostream &out, eTlvTypeMap value) { return out << eTlvTypeMap_str(value); }
// clang-format on
// Enum AutoPrint generated code snippet end
class eTlvTypeMapValidate {
public:
    static bool check(uint8_t value) {
        bool ret = false;
        switch (value) {
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9a:
        case 0x9b:
        case 0x9c:
        case 0x9d:
        case 0x9e:
        case 0x9f:
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xa7:
        case 0xa8:
        case 0xa9:
        case 0xab:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xaf:
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
        case 0xbd:
        case 0xbe:
        case 0xbf:
        case 0xc0:
        case 0xc1:
        case 0xc2:
        case 0xc3:
        case 0xc4:
        case 0xc5:
        case 0xc6:
        case 0xc7:
        case 0xc8:
        case 0xc9:
        case 0xca:
        case 0xcb:
        case 0xcc:
        case 0xcd:
        case 0xce:
        case 0xcf:
        case 0xd0:
        case 0xd1:
        case 0xd2:
        case 0xd3:
        case 0xd4:
        case 0xd5:
        case 0xd6:
        case 0xda:
        case 0xdb:
        case 0xdc:
        case 0xdd:
        case 0xde:
        case 0xd8:
        case 0xd9:
        case 0xdf:
        case 0xe0:
        case 0xe1:
        case 0xe2:
        case 0xe3:
        case 0xe4:
        case 0xe5:
        case 0xe6:
        case 0xe7:
        case 0xe8:
        case 0xe9:
        case 0xaa:
                ret = true;
                break;
            default:
                ret = false;
                break;
        }
        return ret;
    }
};


}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_ETLVTYPEMAP_H_
