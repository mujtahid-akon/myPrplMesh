/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "beerocks_cli_bml.h"
#include "bml_utils.h"

#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <beerocks/tlvf/beerocks_message.h>
#include <easylogging++.h>

using namespace beerocks;
using namespace net;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void fill_conn_map_node(
    std::unordered_multimap<std::string, std::shared_ptr<cli_bml::conn_map_node_t>> &conn_map_nodes,
    struct BML_NODE *node)
{
    auto n                         = std::make_shared<cli_bml::conn_map_node_t>();
    n->type                        = node->type;
    n->state                       = node->state;
    n->channel                     = node->channel;
    n->bw                          = node->bw;
    n->freq_type                   = node->freq_type;
    n->channel_ext_above_secondary = node->channel_ext_above_secondary;
    n->rx_rssi                     = node->rx_rssi;
    n->isWiFiBH                    = node->isWiFiBH;
    n->mac                         = tlvf::mac_to_string(node->mac);
    n->status                      = node->status;
    n->ip_v4                       = network_utils::ipv4_to_string(node->ip_v4);
    n->name.assign(node->name[0] ? node->name : "N/A");

    if (node->type != BML_NODE_TYPE_CLIENT) { // GW or IRE

        n->gw_ire.backhaul_mac = tlvf::mac_to_string(node->data.gw_ire.backhaul_mac);

        // RADIO
        int radio_length = sizeof(node->data.gw_ire.radio) / sizeof(node->data.gw_ire.radio[0]);
        for (int i = 0; i < radio_length; i++) {
            if (node->data.gw_ire.radio[i].channel != 0 && node->data.gw_ire.radio[i].ap_active) {
                auto r           = std::make_shared<cli_bml::conn_map_node_t::gw_ire_t::radio_t>();
                r->channel       = node->data.gw_ire.radio[i].channel;
                r->cac_completed = node->data.gw_ire.radio[i].cac_completed;
                r->bw            = node->data.gw_ire.radio[i].bw;
                r->freq_type     = node->data.gw_ire.radio[i].freq_type;
                r->channel_ext_above_secondary =
                    node->data.gw_ire.radio[i].channel_ext_above_secondary;
                r->radio_identifier =
                    tlvf::mac_to_string(node->data.gw_ire.radio[i].radio_identifier);
                r->radio_mac = tlvf::mac_to_string(node->data.gw_ire.radio[i].radio_mac);
                r->ifname.assign(node->data.gw_ire.radio[i].iface_name);

                // VAP
                int vap_length = sizeof(node->data.gw_ire.radio[i].vap) /
                                 sizeof(node->data.gw_ire.radio[i].vap[0]);

                // The index of of the VAP 'j' represents the VAP ID.
                for (int j = 0; j < vap_length; j++) {
                    auto vap_mac = tlvf::mac_to_string(node->data.gw_ire.radio[i].vap[j].bssid);
                    if (vap_mac != network_utils::ZERO_MAC_STRING) {
                        auto v =
                            std::make_shared<cli_bml::conn_map_node_t::gw_ire_t::radio_t::vap_t>();
                        v->bssid = vap_mac;
                        v->ssid.assign(node->data.gw_ire.radio[i].vap[j].ssid[0]
                                           ? node->data.gw_ire.radio[i].vap[j].ssid
                                           : std::string("N/A"));
                        v->backhaul_vap = node->data.gw_ire.radio[i].vap[j].backhaul_vap;
                        v->vap_id       = j;
                        r->vap.push_back(v);
                    }
                }
                n->gw_ire.radio.push_back(r);
            }
        }
    }

    conn_map_nodes.insert({tlvf::mac_to_string(node->parent_bssid), n});
}

static std::string &ind_inc(std::string &ind)
{
    static const std::string basic_ind("    "); // 4 spaces
    ind += basic_ind;
    return ind;
}

static std::string &ind_dec(std::string &ind)
{
    ind.erase(ind.end() - 4, ind.end()); // erase last 4 space chars
    return ind;
}

static std::string node_type_to_conn_map_string(uint8_t type)
{
    std::string ret;

    switch (type) {
    case BML_NODE_TYPE_GW:
        ret = "GW_BRIDGE:";
        break;
    case BML_NODE_TYPE_IRE:
        ret = "IRE_BRIDGE:";
        break;
    case BML_NODE_TYPE_CLIENT:
        ret = "CLIENT:";
        break;

    default:
        ret = "N/A";
    }

    return ret;
}

static void bml_utils_dump_conn_map(
    std::unordered_multimap<std::string, std::shared_ptr<cli_bml::conn_map_node_t>> &conn_map_nodes,
    const std::string &parent_bssid, const std::string &ind, std::stringstream &ss)
{
    std::string ind_str = ind;

    // ss << "***" << " parent mac: " << parent_bssid << "***" << std::endl;

    auto range = conn_map_nodes.equal_range(parent_bssid);
    for (auto it = range.first; it != range.second; it++) {
        auto node = it->second;

        // ss << "***" << " node mac: " << node->mac << "***" << std::endl;

        // CLIENT
        if (node->type == BML_NODE_TYPE_CLIENT) {
            ss << ind_inc(ind_str) << node_type_to_conn_map_string(node->type)
               << " mac: " << node->mac << ", ipv4: " << node->ip_v4
               << (node->name != "N/A" ? ", name: " + node->name : "");

            if (node->channel) { // channel != 0
                ss << ", ch: " << std::to_string(node->channel) << ", bw: "
                   << utils::convert_bandwidth_to_string((beerocks::eWiFiBandwidth)node->bw)
                   << utils::convert_channel_ext_above_to_string(node->channel_ext_above_secondary,
                                                                 (beerocks::eWiFiBandwidth)node->bw)
                   << ", rx_rssi: " << std::to_string(node->rx_rssi);
            }
            ss << std::endl;

        } else { //PLATFORM

            // IRE BACKHAUL
            if (node->type == BML_NODE_TYPE_IRE &&
                node->gw_ire.backhaul_mac != beerocks::net::network_utils::ZERO_MAC_STRING) {
                ss << ind_inc(ind_str)
                   << std::string(node->isWiFiBH ? "WiFi_BACKHAUL:" : "Eth_BACKHAUL:")
                   << " mac: " << node->gw_ire.backhaul_mac;
                if (!node->isWiFiBH) {
                    ss << std::endl;
                } else {
                    ss << ", ch: " << std::to_string(node->channel) << ", bw: "
                       << utils::convert_bandwidth_to_string((beerocks::eWiFiBandwidth)node->bw)
                       << utils::convert_channel_ext_above_to_string(
                              node->channel_ext_above_secondary, (beerocks::eWiFiBandwidth)node->bw)
                       << std::endl;
                }
                //<< ", rx_rssi: "       << std::to_string(node->rx_rssi)
            }

            // BRIDGE
            if (parent_bssid != network_utils::ZERO_MAC_STRING)
                ind_inc(ind_str);

            ss << ind_str << node_type_to_conn_map_string(node->type)
               << (node->name != "N/A" ? (" name: " + node->name + ", AL-MAC: " + node->mac)
                                       : (" AL-MAC: " + node->mac))
               << ", ipv4: " << node->ip_v4
               << ", Status: " << (node->status ? "Inactive" : "Active") << std::endl;

            // ETHERNET
            // generate eth address from bridge address
            auto eth_sw_mac_binary =
                network_utils::get_eth_sw_mac_from_bridge_mac(tlvf::mac_from_string(node->mac));
            auto eth_mac = tlvf::mac_to_string(eth_sw_mac_binary);
            ss << ind_inc(ind_str) << "ETHERNET:"
               << " mac: " << eth_mac << std::endl;
            // add clients which are connected to the Ethernet
            bml_utils_dump_conn_map(conn_map_nodes, eth_mac, ind_str, ss);

            // RADIO
            for (auto radio : node->gw_ire.radio) {

                ss << ind_str << (radio->ifname != "N/A" ? "RADIO: " + radio->ifname : "RADIO")
                   << " mac: " << radio->radio_mac << ", ch: "
                   << (radio->channel != 255 ? std::to_string(radio->channel) : std::string("N/A"))
                   << ((son::wireless_utils::is_dfs_channel(radio->channel) &&
                        !radio->cac_completed)
                           ? std::string("(CAC)")
                           : std::string())
                   << ", bw: "
                   << utils::convert_bandwidth_to_string((beerocks::eWiFiBandwidth)radio->bw)
                   << utils::convert_channel_ext_above_to_string(
                          radio->channel_ext_above_secondary, (beerocks::eWiFiBandwidth)radio->bw)
                   << ", freq: "
                   << std::to_string(son::wireless_utils::channel_to_freq(
                          radio->channel, static_cast<beerocks::eFreqType>(radio->freq_type)))
                   << "MHz" << std::endl;

                // VAP
                ind_inc(ind_str);
                uint8_t j = 0;
                for (auto vap = radio->vap.begin(); vap != radio->vap.end(); vap++) {
                    if ((*vap)->bssid != network_utils::ZERO_MAC_STRING) {
                        ss << ind_str << std::string((*vap)->backhaul_vap ? "b" : "f") << "VAP["
                           << int(j) << "]:"
                           << " "
                           << ((*vap)->vap_id >= 0
                                   ? (radio->ifname != "N/A"
                                          ? (radio->ifname + "." + std::to_string((*vap)->vap_id))
                                          : "")
                                   : "")
                           << " bssid: " << (*vap)->bssid << ", ssid: " << (*vap)->ssid
                           << std::endl;
                        // add clients which are connected to the vap
                        bml_utils_dump_conn_map(conn_map_nodes, (*vap)->bssid, ind_str, ss);
                        j++;
                    }
                }
                ind_dec(ind_str);
            }
        }
        ind_str = ind; // return the indentation to original level
    }
}

#ifdef FEATURE_PRE_ASSOCIATION_STEERING
static void steering_set_group_string_to_struct(const std::vector<std::string> &str_ap_cfgs,
                                                BML_STEERING_AP_CONFIG *&cfgs)
{
    for (size_t i = 0; i < str_ap_cfgs.size(); i++) {
        auto v_str_ap_cfg = string_utils::str_split(str_ap_cfgs[i], ',');
        for (const auto &elem : v_str_ap_cfg) {
            std::cout << "v_str_ap_cfg No." << i + 1 << ": " << elem << std::endl;
        }
        std::cout << std::endl;
        tlvf::mac_to_array(tlvf::mac_from_string(v_str_ap_cfg[0]), cfgs[i].bssid);
        cfgs[i].utilCheckIntervalSec   = string_utils::stoi(v_str_ap_cfg[1]);
        cfgs[i].utilAvgCount           = string_utils::stoi(v_str_ap_cfg[2]);
        cfgs[i].inactCheckIntervalSec  = string_utils::stoi(v_str_ap_cfg[3]);
        cfgs[i].inactCheckThresholdSec = string_utils::stoi(v_str_ap_cfg[4]);
        sMacAddr bssid                 = tlvf::mac_from_array(cfgs[i].bssid);
        std::cout << "cfg_" << i + 1 << ".bssid = " << bssid << std::endl
                  << "cfg_" << i + 1 << ".utilCheckIntervalSec = " << cfgs[i].utilCheckIntervalSec
                  << std::endl
                  << "cfg_" << i + 1 << ".utilAvgCount = " << cfgs[i].utilAvgCount << std::endl
                  << "cfg_" << i + 1 << ".inactCheckIntervalSec = " << cfgs[i].inactCheckIntervalSec
                  << std::endl
                  << "cfg_" << i + 1
                  << ".inactCheckThresholdSec = " << cfgs[i].inactCheckThresholdSec << std::endl;
        std::cout << std::endl;
    }
}
static void steering_client_set_string_to_struct(const std::string &str_config,
                                                 BML_STEERING_CLIENT_CONFIG &config)
{
    auto v_str_config = string_utils::str_split(str_config, ',');

    config.snrProbeHWM      = string_utils::stoi(v_str_config[0]);
    config.snrProbeLWM      = string_utils::stoi(v_str_config[1]);
    config.snrAuthHWM       = string_utils::stoi(v_str_config[2]);
    config.snrAuthLWM       = string_utils::stoi(v_str_config[3]);
    config.snrInactXing     = string_utils::stoi(v_str_config[4]);
    config.snrHighXing      = string_utils::stoi(v_str_config[5]);
    config.snrLowXing       = string_utils::stoi(v_str_config[6]);
    config.authRejectReason = string_utils::stoi(v_str_config[7]);

    std::cout << "config.snrProbeHWM = " << config.snrProbeHWM << std::endl
              << "config.snrProbeLWM = " << config.snrProbeLWM << std::endl
              << "config.snrAuthHWM = " << config.snrAuthHWM << std::endl
              << "config.snrAuthLWM = " << config.snrAuthLWM << std::endl
              << "config.snrInactXing = " << config.snrInactXing << std::endl
              << "config.snrHighXing = " << config.snrHighXing << std::endl
              << "config.snrLowXing = " << config.snrLowXing << std::endl
              << "config.authRejectReason = " << config.authRejectReason << std::endl;
    return;
}
#endif //FEATURE_PRE_ASSOCIATION_STEERING
//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SocketClient *cli_bml::m_analyzer_socket = nullptr;

cli_bml::cli_bml(const std::string &beerocks_conf_path_)
{
    cli_bml::setFunctionsMapAndArray();
    beerocks_conf_path = beerocks_conf_path_;

    // Set BML easylogging context
    auto log_storage = el::Helpers::storage();
    bml_set_local_log_context((void *)&log_storage);

    //set select timeout
    timeval tval;
    tval.tv_sec  = 0;
    tval.tv_usec = 1000 * 2000; // 2sec
    select.setTimeout(&tval);
}

cli_bml::~cli_bml() { cli_bml::disconnect(); }

bool cli_bml::connect()
{
    disconnect(); // Disconnect if already connected
    const char *c = beerocks_conf_path.c_str();
    int ret       = bml_connect(&ctx, c, this);
    printBmlReturnVals("bml_connect", ret);
    return (ret == BML_RET_OK ? true : false);
}

void cli_bml::disconnect()
{
    is_analyzer = false;
    if (ctx != nullptr) {
        int ret = bml_disconnect(ctx);
        printBmlReturnVals("bml_disconnect", ret);
        ctx = nullptr;
    }
    if (m_analyzer_socket != nullptr) {
        select.removeSocket(m_analyzer_socket);
        m_analyzer_socket->closeSocket();
        delete m_analyzer_socket;
        m_analyzer_socket = nullptr;
    }
}

bool cli_bml::is_connected()
{
    if (!ctx) {
        return false;
    }
    return true;
}

//
// Help functions
//

void cli_bml::setFunctionsMapAndArray()
{
    helpLineSize = 0;

    //Setting up functions map : <function_name>,
    //                           <help_args>,<help>,
    //                           <funcPtr>,
    //                           <minNumOfArgs>,<maxNumOfArgs>,
    //                           <type1>,<type2>,...,<type10>

    insertCommandToMap("bml_help", "", "get supported commands", &cli::help_caller, 0, 0);
    insertCommandToMap("bml_connect", "", "connect to libbml ",
                       static_cast<pFunction>(&cli_bml::connect_caller), 0, 0);
    insertCommandToMap("bml_onboard_status", "",
                       "prints '1' if the platform is in Oboarding state, '0' otherwise",
                       static_cast<pFunction>(&cli_bml::onboard_status_caller), 0, 0);
    insertCommandToMap("bml_ping", "",
                       "Check whether the connection with the beerocks platform is alive and well",
                       static_cast<pFunction>(&cli_bml::ping_caller), 0, 0);
    insertCommandToMap("bml_nw_map_register_update_cb", "[<x>]",
                       "Registers a callback functions for the network map update operation, call "
                       "with 'x' to unregister the callback ",
                       static_cast<pFunction>(&cli_bml::nw_map_register_update_cb_caller), 0, 1,
                       STRING_ARG);
    insertCommandToMap("bml_nw_map_query", "", "Query the beerocks for the latest network map",
                       static_cast<pFunction>(&cli_bml::nw_map_query_caller), 0, 0);
    insertCommandToMap("bml_conn_map", "", "dump the latest network map",
                       static_cast<pFunction>(&cli_bml::bml_connection_map_caller), 0, 0);
    insertCommandToMap("bml_get_device_operational_radios", "",
                       "returns operational status of all radios on the device",
                       static_cast<pFunction>(&cli_bml::bml_get_device_operational_radios_caller),
                       0, 1, STRING_ARG);
    insertCommandToMap("bml_stat_register_cb", "[<x>]",
                       "Registers a callback function to periodic statistics update from the "
                       "beerocks platform, call with 'x' to unregister the callback ",
                       static_cast<pFunction>(&cli_bml::stat_register_cb_caller), 0, 1, STRING_ARG);
    insertCommandToMap("bml_events_register_cb", "[<x>]",
                       "Registers a callback function to events "
                       "update from the beerocks platform, call "
                       "with 'x' to unregister the callback ",
                       static_cast<pFunction>(&cli_bml::events_register_cb_caller), 0, 1,
                       STRING_ARG);
    insertCommandToMap(
        "bml_set_wifi_credentials",
        "<al_mac> <ssid> [<network_key>] [<operating_class>] [<bss_type>] [<add_sae>]",
        "Sets WiFi credentials to the given AL-MAC "
        "al_mac - agent mac address "
        "ssid - service set identifier "
        "Optionals: "
        "network_key - password. If empty, configure open network. Default - empty. "
        "bands - can be 24g, 5g or 24g-5g. Default - 24g-5g. "
        "bss_type - can be fronthaul, backhaul, fronthaul-backhaul. Default fronthaul."
        "add_sae - 1 for true. Adds SAE to the authentication type. Must be set with network_key",
        static_cast<pFunction>(&cli_bml::set_wifi_credentials_caller), 2, 6, STRING_ARG, STRING_ARG,
        STRING_ARG, STRING_ARG, STRING_ARG, INT_ARG);
    insertCommandToMap(
        "bml_clear_wifi_credentials", "<al_mac>", "Removes wifi credentials for specific AL-MAC.",
        static_cast<pFunction>(&cli_bml::clear_wifi_credentials_caller), 1, 1, STRING_ARG);
    insertCommandToMap("bml_update_wifi_credentials", "", "Updates wifi credentials.",
                       static_cast<pFunction>(&cli_bml::update_wifi_credentials_caller), 0, 0);
    insertCommandToMap("bml_get_wifi_credentials", "[<vap_id>]",
                       "Get SSID and security type for the given VAP (or Vap=0 by default)",
                       static_cast<pFunction>(&cli_bml::get_wifi_credentials_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_set_onboarding_state", "<enable 0/1>", "Sets onboarding state.",
                       static_cast<pFunction>(&cli_bml::set_onboarding_state_caller), 1, 1,
                       INT_ARG);
    insertCommandToMap("bml_get_onboarding_state", "", "Get onboarding state.",
                       static_cast<pFunction>(&cli_bml::get_onboarding_state_caller), 0, 0);
    insertCommandToMap("bml_wps_onboarding", "[<iface>]",
                       "Start WPS onboarding process on 'iface' or all ifaces in case of empty.",
                       static_cast<pFunction>(&cli_bml::wps_onboarding_caller), 0, 1, STRING_ARG);
    insertCommandToMap("bml_get_bml_version", "", "prints bml version",
                       static_cast<pFunction>(&cli_bml::get_bml_version_caller), 0, 0);
    insertCommandToMap("bml_get_master_slave_versions", "",
                       "prints beerocks master & slave versions",
                       static_cast<pFunction>(&cli_bml::get_master_slave_versions_caller), 0, 0);
    insertCommandToMap(
        "bml_enable_legacy_client_roaming", "[<1 or 0>]",
        "if input was given - enable/disable legacy client roaming, prints current value",
        static_cast<pFunction>(&cli_bml::enable_legacy_client_roaming_caller), 0, 1, INT_ARG);
    insertCommandToMap("bml_enable_client_roaming", "[<1 or 0>]",
                       "if input was given - enable/disable client roaming, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_client_roaming_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap(
        "bml_enable_client_roaming_prefer_signal_strength", "[<1 or 0>]",
        "if input was given - enable/disable client roaming prefer signal strength, prints current "
        "value",
        static_cast<pFunction>(&cli_bml::enable_client_roaming_prefer_signal_strength_caller), 0, 1,
        INT_ARG);
    insertCommandToMap("bml_enable_client_band_steering", "[<1 or 0>]",
                       "if input was given - enable/disable band steering, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_client_band_steering_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_enable_ire_roaming", "[<1 or 0>]",
                       "if input was given - enable/disable ire roaming, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_ire_roaming_caller), 0, 1, INT_ARG);
    insertCommandToMap("bml_enable_load_balancer", "[<1 or 0>]",
                       "if input was given - enable/disable load balancer, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_load_balancer_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_enable_service_fairness", "[<1 or 0>]",
                       "if input was given - enable/disable service fairness, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_service_fairness_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_enable_dfs_reentry", "[<1 or 0>]",
                       "if input was given - enable/disable service fairness, prints current value",
                       static_cast<pFunction>(&cli_bml::enable_dfs_reentry_caller), 0, 1, INT_ARG);
    insertCommandToMap("bml_certification_mode", "[<1 or 0>]",
                       "if input was given - enable/disable certification mode, "
                       " else prints current value",
                       static_cast<pFunction>(&cli_bml::enable_certification_mode_caller), 0, 1,
                       INT_ARG);
    insertCommandToMap("bml_set_log_level", "<module_name> <log_level> <1 or 0> [<mac>]",
                       "turn 'on/off' (1 or 0) 'log_level' ('i'- info, 'd' - debug, 'e' - error, "
                       "'f' - fatal, 't' - trace, 'w' - warning, 'a' - all) on 'module_name' "
                       "(master/ slave/ monitor/ platform/ all) with 'mac' (default - all slaves) ",
                       static_cast<pFunction>(&cli_bml::set_log_level_caller), 3, 4, STRING_ARG,
                       STRING_ARG, INT_ARG, STRING_ARG);
    insertCommandToMap("bml_restricted_channels_set_global", "<restricted channls>",
                       "Set global 'restricted channels' seperated by commas",
                       static_cast<pFunction>(&cli_bml::set_global_restricted_channels_caller), 1,
                       1, STRING_ARG);
    insertCommandToMap("bml_restricted_channels_get_global", "", "Get global restricted channels",
                       static_cast<pFunction>(&cli_bml::get_global_restricted_channels_caller), 0,
                       0);
    insertCommandToMap("bml_restricted_channels_set_slave", "<restricted channels>",
                       "Set slave 'mac' 'restricted channels' seperated by commas",
                       static_cast<pFunction>(&cli_bml::set_slave_restricted_channels_caller), 2, 2,
                       STRING_ARG, STRING_ARG);
    insertCommandToMap(
        "bml_restricted_channels_get_slave", "<mac>", "Get restricted channels from slave 'mac'",
        static_cast<pFunction>(&cli_bml::get_slave_restricted_channels_caller), 1, 1, STRING_ARG);
    insertCommandToMap("bml_trigger_topology_discovery", "<al_mac (mac format)>",
                       "trigger topology query towards 'al_mac'",
                       static_cast<pFunction>(&cli_bml::bml_trigger_topology_discovery_caller), 1,
                       1, STRING_ARG);
    insertCommandToMap("bml_trigger_channel_selection",
                       "<radio mac> <channel> <bandwidth> [<csa count> by default 5]",
                       "trigger channel selection procedure",
                       static_cast<pFunction>(&cli_bml::bml_channel_selection_caller), 3, 4,
                       STRING_ARG, INT_ARG, INT_ARG, INT_ARG);
    insertCommandToMap(
        "bml_set_selection_channel_pool", "<mac> <channel pool>",
        "Set the Channel-Selection's channel pool for the Auto Channel Selection. channels are "
        "separated by commas.",
        static_cast<pFunction>(&cli_bml::bml_set_selection_channel_pool_caller), 2, 2, STRING_ARG,
        STRING_ARG);
    insertCommandToMap("bml_get_selection_channel_pool", "<mac>",
                       "Get the Channel-Selection's channel pool for the Auto Channel Selection.",
                       static_cast<pFunction>(&cli_bml::bml_get_selection_channel_pool_caller), 1,
                       1, STRING_ARG);

#ifdef FEATURE_PRE_ASSOCIATION_STEERING
    insertCommandToMap(
        "bml_pre_association_steering_set_group",
        "<steeringGroupIndex> [<ap_cfg_1>] [<ap_cfg_2>] [<ap_cfg_3>]",
        "ap_cfg_1/2/3 = <bssid>, <utilCheckIntervalSec>, <utilAvgCount>, "
        "<inactCheckIntervalSec>, <inactCheckThresholdSec> (without spaces between "
        "commas). To remove a group, provide only the Group Index",
        static_cast<pFunction>(&cli_bml::bml_pre_association_steering_set_group_caller), 1, 4,
        INT_ARG, STRING_ARG, STRING_ARG, STRING_ARG);

    insertCommandToMap(
        "bml_pre_association_steering_client_set",
        "<steeringGroupIndex> <bssid> <client_mac> [<config>]",
        "config = <snrProbeHWM>, <snrProbeLWM>, <snrAuthHWM>, <snrAuthLWM>, <snrInactXing>, "
        "<snrHighXing>, <snrLowXing>, <authRejectReason> (without spaces between commas) "
        "if 'config' is not given, client will be removed ",
        static_cast<pFunction>(&cli_bml::bml_pre_association_steering_client_set_caller), 3, 4,
        INT_ARG, STRING_ARG, STRING_ARG, STRING_ARG);
    insertCommandToMap(
        "bml_pre_association_steering_event_register", "[<x>]",
        "Registers a callback function to events update from the beerocks platform, "
        "call with 'x' to unregister the callback ",
        static_cast<pFunction>(&cli_bml::bml_pre_association_steering_event_register_caller), 0, 1,
        STRING_ARG);

    insertCommandToMap(
        "bml_pre_association_steering_client_measure", "<steeringGroupIndex> <bssid> <client_mac>",
        "", static_cast<pFunction>(&cli_bml::bml_pre_association_steering_client_measure_caller), 3,
        3, INT_ARG, STRING_ARG, STRING_ARG);

    insertCommandToMap(
        "bml_pre_association_steering_client_disconnect",
        "<steeringGroup> <apIndex> <client_mac> <type> <reason>",
        "Type: 0 - Unknown, 1 - Disassociation, 2 - Deauthentication. reason - reason code to "
        "provide in deauth/disassoc frame",
        static_cast<pFunction>(&cli_bml::bml_pre_association_steering_client_disconnect_caller), 5,
        5, INT_ARG, STRING_ARG, STRING_ARG, INT_ARG, INT_ARG);
#endif
    insertCommandToMap(
        "bml_set_dcs_continuous_scan_enable", "<mac> <1 or 0>",
        "Enable/Disable (1 or 0) the Dynamic-Channel-Selection for the given AP mac.",
        static_cast<pFunction>(&cli_bml::set_dcs_continuous_scan_enable_caller), 2, 2, STRING_ARG,
        INT_ARG);
    insertCommandToMap("bml_get_dcs_continuous_scan_enable", "<mac>",
                       "Get Dynamic-Channel-Selection enable configuration for the given AP mac.",
                       static_cast<pFunction>(&cli_bml::get_dcs_continuous_scan_enable_caller), 1,
                       1, STRING_ARG);
    insertCommandToMap("bml_set_dcs_continuous_scan_params", "<mac> [<params>]",
                       "Set the continuous scan params for the given AP mac: 'params' = "
                       "(dwell_time, interval_time, channel_pool"
                       ")=value. Params description: dwell_time - dwell time in "
                       "milliseconds, interval_time - interval time in seconds,"
                       " channel_pool - channels separated by commas.",
                       static_cast<pFunction>(&cli_bml::set_dcs_continuous_scan_params_caller), 2,
                       4, STRING_ARG, STRING_ARG, STRING_ARG, STRING_ARG);
    insertCommandToMap("bml_get_dcs_continuous_scan_params", "<mac>",
                       "Get Dynamic-Channel-Selection params for the given AP mac: dwell_time - "
                       "dwell time in milliseconds, interval_time - interval time in seconds,"
                       " channel_pool - channels seperated by commas.",
                       static_cast<pFunction>(&cli_bml::get_dcs_continuous_scan_params_caller), 1,
                       1, STRING_ARG);
    insertCommandToMap(
        "bml_start_dcs_single_scan", "<mac> <dwell_time_msec> <channel_pool>",
        "Start a single scan, for the given AP mac, with the following scan params:"
        " dwell_time - dwell time in milliseconds, channel_pool - channels seperated by commas.",
        static_cast<pFunction>(&cli_bml::start_dcs_single_scan_caller), 3, 3, STRING_ARG, INT_ARG,
        STRING_ARG);
    insertCommandToMap(
        "bml_get_dcs_scan_results", "<mac> <max-results-size> [<is-single-scan>]",
        "Get Dynamic-Channel-Selection scan results for the given AP mac:"
        " max-results-size - maximal size of the returned results,"
        " is-single-scan - 0 for continuous-scan results (default), 1 for single scan.",
        static_cast<pFunction>(&cli_bml::get_dcs_scan_results_caller), 2, 3, STRING_ARG, INT_ARG,
        INT_ARG);
    insertCommandToMap("bml_client_get_client_list", "", "Get client list.",
                       static_cast<pFunction>(&cli_bml::client_get_client_list_caller), 0, 0);
    insertCommandToMap(
        // clang-format off
        "bml_client_set_client", "<sta_mac> [<params>]",
        "Set client with the given STA MAC:"
        "selected_bands - Bitwise parameter, 1 for 2.4G, 2 for 5G, 3 for both, 0 for Disabled"
        "stay_on_initial_radio - 1 for true, 0 for false or (default) -1 for not configured,"
        "time_life_delay_minutes - 0 for non-aging, any positive (>0) value to configure or -1 for "
        "not configured",
        // clang-format on
        static_cast<pFunction>(&cli_bml::client_set_client_caller), 2, 4, STRING_ARG, STRING_ARG,
        STRING_ARG, INT_ARG);
    insertCommandToMap("bml_client_get_client", "<sta_mac>", "Get client with the given STA MAC.",
                       static_cast<pFunction>(&cli_bml::client_get_client_caller), 1, 1,
                       STRING_ARG);
    insertCommandToMap("bml_client_clear_client", "<sta_mac>",
                       "Clear persistent configuration for the client with the given STA MAC",
                       static_cast<pFunction>(&cli_bml::client_clear_client_caller), 1, 1,
                       STRING_ARG);
    insertCommandToMap(
        "bml_enable_11k_support", "[<1 or 0>]",
        "if input was given - enable/disable the 802.11k support, prints current value",
        static_cast<pFunction>(&cli_bml::enable_client_roaming_11k_support_caller), 0, 1, INT_ARG);
    insertCommandToMap("bml_add_unassociated_station_stats",
                       "[<mac> <channel> <operating_class> <agent_mac_address>]",
                       "adds station with mac_address <mac> to the list of unassociated stations."
                       "Monitoring will be done on channel <channel> on  <operating_class>."
                       "Please note that the Access point can still use the active channel instead "
                       "of the new proposed value!"
                       "If agent_mac_address is entered, only that agent will be monitoring the "
                       "station,if empty,all connected agents will be selected ",
                       static_cast<pFunction>(&cli_bml::add_unassociated_station_stats_caller), 3,
                       4, STRING_ARG, STRING_ARG, STRING_ARG, STRING_ARG);
    insertCommandToMap(
        "bml_remove_unassociated_station_stats", "[<mac> <agent_mac_address>]",
        "remove the station with mac_address <mac> from the list of monitored "
        "unassociated stations. IF <agent_mac_address> is set, only <agent_mac_address>  will stop "
        "monitoring the station, otherwise, it will be removed from all agents",
        static_cast<pFunction>(&cli_bml::remove_unassociated_station_stats_caller), 1, 2,
        STRING_ARG, STRING_ARG);
    insertCommandToMap("bml_get_unassociated_stations_stats", "",
                       " get stats for unassociated stations already being monitored. For this "
                       "first version, stats will be printed out in the controller log file",
                       static_cast<pFunction>(&cli_bml::get_unassociated_station_stats_caller), 0,
                       0);
    insertCommandToMap("bml_unassoc_sta_rcpi_query", "<sta mac> <opclass> <channel>",
                       "Sends all capable agent(s) a unassoc STA link metric query request with "
                       "given sta, opclass and a channel",
                       static_cast<pFunction>(&cli_bml::send_unassoc_sta_rcpi_query_caller), 3, 3,
                       STRING_ARG, INT_ARG, INT_ARG);
    insertCommandToMap("bml_get_unassoc_sta_query_result", "<sta mac>",
                       "Prints unassoc STA link metric query result for given sta",
                       static_cast<pFunction>(&cli_bml::get_unassoc_sta_rcpi_result_caller), 1, 1,
                       STRING_ARG);

    //bool insertCommandToMap(std::string command, std::string help_args, std::string help,  pFunction funcPtr, uint8_t minNumOfArgs, uint8_t maxNumOfArgs,
}

void cli_bml::printBmlReturnVals(const std::string &func_name, int ret_val)
{
    std::cout << func_name << ": ";
    switch (ret_val) {
    case BML_RET_OK: {
        std::cout << "return value is: BML_RET_OK, Success status" << std::endl;
        break;
    }
    case -BML_RET_OP_FAILED: {
        std::cout << "return value is: BML_RET_OP_FAILED, Operation failed" << std::endl;
        break;
    }
    case -BML_RET_INVALID_ARGS: {
        std::cout << "return value is: BML_RET_INVALID_ARGS, Invalid arguments" << std::endl;
        break;
    }
    case -BML_RET_MEM_FAIL: {
        std::cout << "return value is: BML_RET_MEM_FAIL, Memory allocation failed" << std::endl;
        break;
    }
    case -BML_RET_INIT_FAIL: {
        std::cout << "return value is: BML_RET_INIT_FAIL, BML Initialization failed" << std::endl;
        break;
    }
    case -BML_RET_CONNECT_FAIL: {
        std::cout << "return value is: BML_RET_CONNECT_FAIL, Connection to Beerocks failed"
                  << std::endl;
        break;
    }
    case -BML_RET_NO_DATA: {
        std::cout << "return value is: BML_RET_NO_DATA, No more data available" << std::endl;
        break;
    }
    case -BML_RET_INVALID_DATA: {
        std::cout << "return value is: BML_RET_INVALID_DATA, Unexpected data" << std::endl;
        break;
    }
    case -BML_RET_TIMEOUT: {
        std::cout << "return value is: BML_RET_TIMEOUT, Operation timed out" << std::endl;
        break;
    }
    default: {
        std::cout << "return value is: " << ret_val << " Unknown return value" << std::endl;
    }
    }
}

void cli_bml::map_query_cb(const struct BML_NODE_ITER *node_iter, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);

    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }

    char *p = pThis->print_buffer;

    struct BML_NODE *current_node;
    if (node_iter->first() != BML_RET_OK) {
        std::cout << "map_query_cb: node_iter.first() != BML_RET_OK, map_query_cb stops"
                  << std::endl;
        return;
    }
    current_node = node_iter->get_node();
    if (to_console) {
        bml_utils_node_to_string(current_node, pThis->print_buffer, PRINT_BUFFER_LENGTH);
        std::cout << pThis->print_buffer << std::endl;
    } else {
        //Add MARK line
        if (current_node->type == BML_NODE_TYPE_GW) {
            std::stringstream ss;
            ss << "MARK" << std::endl;
            ss.seekp(0, std::ios::end);
            std::stringstream::pos_type offset = ss.tellp();
            int free_buffer_len                = PRINT_BUFFER_LENGTH - (p - pThis->print_buffer);

            if (offset > free_buffer_len) {
                std::cout << "ERROR: MARK sign > print_buffer available length" << std::endl;
                return;
            } else {
                string_utils::copy_string(p, ss.str().c_str(), sizeof(pThis->print_buffer));
                p += offset;
            }
        }
        p += bml_utils_node_to_string(current_node, p, PRINT_BUFFER_LENGTH);
    }
    while (node_iter->next() == BML_RET_OK) {
        current_node = node_iter->get_node();
        if (to_console) {
            bml_utils_node_to_string(current_node, pThis->print_buffer, PRINT_BUFFER_LENGTH);
            std::cout << pThis->print_buffer << std::endl;
        } else {
            std::ptrdiff_t size = p - pThis->print_buffer;
            p += bml_utils_node_to_string(current_node, p, PRINT_BUFFER_LENGTH - size);
        }
    }

    if (!to_console) {
        //send_message to m_analyzer_socket
        std::ptrdiff_t size = p - pThis->print_buffer;
        if (!message_com::send_data(m_analyzer_socket, (uint8_t *)pThis->print_buffer, size)) {
            pThis->disconnect();
        }
    }
}

void cli_bml::connection_map_cb(const struct BML_NODE_ITER *node_iter, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);

    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }

    struct BML_NODE *current_node;
    if (node_iter->first() != BML_RET_OK) {
        std::cout << "map_query_cb: node_iter.first() != BML_RET_OK, map_query_cb stops"
                  << std::endl;
        return;
    }

    current_node = node_iter->get_node();
    if (current_node) {
        fill_conn_map_node(pThis->conn_map_nodes, current_node);

        while (node_iter->next() == BML_RET_OK) {
            current_node = node_iter->get_node();
            fill_conn_map_node(pThis->conn_map_nodes, current_node);
        }
    }

    if (node_iter->last_node) {
        std::stringstream ss;
        std::string ind;
        if (pThis->conn_map_nodes.empty()) {
            ss << "Connection map is empty..." << std::endl;
        } else {
            bml_utils_dump_conn_map(pThis->conn_map_nodes, network_utils::ZERO_MAC_STRING, ind, ss);
            pThis->conn_map_nodes.clear();
        }
        // Printing the connection map
        std::cout << std::endl << ss.str();
        //No need to wait anymore - this is the last fragment
        pThis->pending_response = false;
    }
}

void cli_bml::map_query_to_console_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::map_query_cb(node_iter, true);
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }
    pThis->pending_response = false;
}

void cli_bml::connection_map_to_console_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::connection_map_cb(node_iter, true);
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }
}

void cli_bml::map_query_to_socket_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::map_query_cb(node_iter, false);
}

void cli_bml::map_update_cb(const struct BML_NODE_ITER *node_iter, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(node_iter->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }

    struct BML_NODE *current_node;
    if (node_iter->first() != BML_RET_OK) {
        std::cout << "map_update_cb: node_iter.first() != BML_RET_OK, map_update_cb stops"
                  << std::endl;
        return;
    }
    current_node = node_iter->get_node();
    int size     = bml_utils_node_to_string(current_node, pThis->print_buffer, PRINT_BUFFER_LENGTH);
    if (to_console) {
        std::cout << pThis->print_buffer << std::endl;
    } else {
        //send_message to m_analyzer_socket
        if (!message_com::send_data(m_analyzer_socket, (uint8_t *)pThis->print_buffer, size)) {
            pThis->disconnect();
        }
    }
}

void cli_bml::map_update_to_console_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::map_update_cb(node_iter, true);
}

void cli_bml::map_update_to_socket_cb(const struct BML_NODE_ITER *node_iter)
{
    cli_bml::map_update_cb(node_iter, false);
}

void cli_bml::stats_update_cb(const struct BML_STATS_ITER *stats_iter, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(stats_iter->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }
    char *p = pThis->print_buffer;

    struct BML_STATS *current_stats;
    if (stats_iter->first() != BML_RET_OK) {
        std::cout << "stats_update_cb: stats_iter->first() != BML_RET_OK, stats_update_cb stops"
                  << std::endl;
        return;
    }
    current_stats = stats_iter->get_node();
    if (to_console) {
        bml_utils_stats_to_string(current_stats, pThis->print_buffer, PRINT_BUFFER_LENGTH);
        std::cout << pThis->print_buffer << std::endl;
    } else {
        p += bml_utils_stats_to_string_raw(current_stats, p, PRINT_BUFFER_LENGTH);
    }

    while (stats_iter->next() == BML_RET_OK) {
        current_stats = stats_iter->get_node();
        if (to_console) {
            bml_utils_stats_to_string(current_stats, pThis->print_buffer, PRINT_BUFFER_LENGTH);
            std::cout << pThis->print_buffer << std::endl;
        } else {
            std::ptrdiff_t size = p - pThis->print_buffer;
            p += bml_utils_stats_to_string_raw(current_stats, p, PRINT_BUFFER_LENGTH - size);
        }
    }

    if (!to_console) {
        //send_message to m_analyzer_socket
        std::ptrdiff_t size = p - pThis->print_buffer;
        if (!message_com::send_data(m_analyzer_socket, (uint8_t *)pThis->print_buffer, size)) {
            pThis->disconnect();
        }
    }
}

void cli_bml::stats_update_to_console_cb(const struct BML_STATS_ITER *stats_iter)
{
    cli_bml::stats_update_cb(stats_iter, true);
}

void cli_bml::stats_update_to_socket_cb(const struct BML_STATS_ITER *stats_iter)
{
    cli_bml::stats_update_cb(stats_iter, false);
}

void cli_bml::events_update_cb(const struct BML_EVENT *event, bool to_console)
{
    cli_bml *pThis = (cli_bml *)bml_get_user_data(event->ctx);
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << std::endl;
        return;
    }
    char *p = pThis->print_buffer;

    if (to_console) {
        bml_utils_event_to_string(event, pThis->print_buffer, PRINT_BUFFER_LENGTH);
        std::cout << pThis->print_buffer << std::endl;
    } else {
        p += bml_utils_event_to_string(event, p, PRINT_BUFFER_LENGTH);
    }

    if (!to_console) {
        //send_message to m_analyzer_socket
        std::ptrdiff_t size = p - pThis->print_buffer;
        if (!message_com::send_data(m_analyzer_socket, (uint8_t *)pThis->print_buffer, size)) {
            pThis->disconnect();
        }
    }
}

void cli_bml::events_update_to_console_cb(const struct BML_EVENT *event)
{
    cli_bml::events_update_cb(event, true);
}

void cli_bml::events_update_to_socket_cb(const struct BML_EVENT *event)
{
    cli_bml::events_update_cb(event, false);
}

//
// Caller functions
//
int cli_bml::connect_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return connect();
}

int cli_bml::onboard_status_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return onboard_status();
}

int cli_bml::ping_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return ping();
}

int cli_bml::nw_map_register_update_cb_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return nw_map_register_update_cb();
    return nw_map_register_update_cb(args.stringArgs[0]);
}

int cli_bml::nw_map_query_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return nw_map_query();
}

int cli_bml::bml_connection_map_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return connection_map();
}

int cli_bml::bml_get_device_operational_radios_caller(int numOfArgs)
{
    if (numOfArgs != 1) {
        return -1;
    }
    return get_device_operational_radios(args.stringArgs[0]);
}

int cli_bml::stat_register_cb_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return stat_register_cb();
    return stat_register_cb(args.stringArgs[0]);
}

int cli_bml::events_register_cb_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return events_register_cb();
    return events_register_cb(args.stringArgs[0]);
}

int cli_bml::set_wifi_credentials_caller(int numOfArgs)
{
    if (numOfArgs == 2)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1]);
    else if (numOfArgs == 3)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1], args.stringArgs[2]);
    else if (numOfArgs == 4)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1], args.stringArgs[2],
                                    args.stringArgs[3]);
    else if (numOfArgs == 5)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1], args.stringArgs[2],
                                    args.stringArgs[3], args.stringArgs[4]);
    else if (numOfArgs == 6)
        return set_wifi_credentials(args.stringArgs[0], args.stringArgs[1], args.stringArgs[2],
                                    args.stringArgs[3], args.stringArgs[4], args.intArgs[5]);
    else
        return -1;
}

int cli_bml::clear_wifi_credentials_caller(int numOfArgs)
{
    if (numOfArgs == 1)
        return clear_wifi_credentials(args.stringArgs[0]);
    else
        return -1;
}

int cli_bml::update_wifi_credentials_caller(int numOfArgs)
{
    if (numOfArgs == 0)
        return update_wifi_credentials();
    else
        return -1;
}
int cli_bml::get_wifi_credentials_caller(int numOfArgs)
{
    if (numOfArgs == 0)
        return get_wifi_credentials();
    else if (numOfArgs == 1)
        return get_wifi_credentials(args.intArgs[0]);
    else
        return -1;
}

int cli_bml::set_onboarding_state_caller(int numOfArgs)
{
    if (numOfArgs < 1)
        return -1;
    return set_onboarding_state(args.intArgs[0]);
}

int cli_bml::get_onboarding_state_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return get_onboarding_state();
}

int cli_bml::wps_onboarding_caller(int numOfArgs)
{
    if (numOfArgs == 0)
        return wps_onboarding();
    else
        return wps_onboarding(args.stringArgs[0]);
}

int cli_bml::get_bml_version_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return get_bml_version();
}

int cli_bml::get_master_slave_versions_caller(int numOfArgs)
{
    if (numOfArgs != 0)
        return -1;
    else
        return get_master_slave_versions();
}

int cli_bml::enable_legacy_client_roaming_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_legacy_client_roaming();
    return enable_legacy_client_roaming(args.intArgs[0]);
}

int cli_bml::enable_client_roaming_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_client_roaming();
    return enable_client_roaming(args.intArgs[0]);
}

int cli_bml::enable_client_roaming_11k_support_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_client_roaming_11k_support();
    return enable_client_roaming_11k_support(args.intArgs[0]);
}

int cli_bml::enable_client_roaming_prefer_signal_strength_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_client_roaming_prefer_signal_strength();
    return enable_client_roaming_prefer_signal_strength(args.intArgs[0]);
}

int cli_bml::enable_client_band_steering_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_client_band_steering();
    return enable_client_band_steering(args.intArgs[0]);
}

int cli_bml::enable_ire_roaming_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_ire_roaming();
    return enable_ire_roaming(args.intArgs[0]);
}

int cli_bml::enable_load_balancer_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_load_balancer();
    return enable_load_balancer(args.intArgs[0]);
}

int cli_bml::enable_service_fairness_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_service_fairness();
    return enable_service_fairness(args.intArgs[0]);
}

int cli_bml::enable_dfs_reentry_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_dfs_reentry();
    return enable_dfs_reentry(args.intArgs[0]);
}

int cli_bml::enable_certification_mode_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_certification_mode();
    return enable_certification_mode(args.intArgs[0]);
}

int cli_bml::set_log_level_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return set_log_level(args.stringArgs[0], args.stringArgs[1], args.intArgs[2]);
    } else if (numOfArgs == 4) {
        return set_log_level(args.stringArgs[0], args.stringArgs[1], args.intArgs[2],
                             args.stringArgs[3]);
    }
    return -1;
}

int cli_bml::set_global_restricted_channels_caller(int numOfArgs)
{
    std::cout << "numOfArgs= " << int(numOfArgs) << std::endl;
    if (numOfArgs == 1) {
        std::cout << "args.stringArgs[0] " << args.stringArgs[0] << std::endl;
        return set_global_restricted_channels(args.stringArgs[0]);
    }
    return -1;
}
int cli_bml::get_global_restricted_channels_caller(int numOfArgs)
{
    if (numOfArgs == 0) {
        return get_global_restricted_channels();
    }
    return -1;
}

int cli_bml::set_slave_restricted_channels_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return set_slave_restricted_channels(args.stringArgs[0], args.stringArgs[1]);
    }
    return -1;
}

int cli_bml::get_slave_restricted_channels_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return get_slave_restricted_channels(args.stringArgs[0]);
    }
    return -1;
}

int cli_bml::bml_trigger_topology_discovery_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return topology_discovery(args.stringArgs[0]);
    }
    return -1;
}

int cli_bml::bml_channel_selection_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return channel_selection(args.stringArgs[0], args.intArgs[1], args.intArgs[2]);
    } else if (numOfArgs == 4) {
        return channel_selection(args.stringArgs[0], args.intArgs[1], args.intArgs[2],
                                 args.intArgs[3]);
    }
    return -1;
}

int cli_bml::bml_set_selection_channel_pool_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return set_selection_pool(args.stringArgs[0], args.stringArgs[1]);
    }
    return -1;
}

int cli_bml::bml_get_selection_channel_pool_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return get_selection_pool(args.stringArgs[0]);
    }
    return -1;
}

#ifdef FEATURE_PRE_ASSOCIATION_STEERING
int cli_bml::bml_pre_association_steering_set_group_caller(int numOfArgs)
{
    std::vector<std::string> strApCfgs;
    if (numOfArgs >= 1 && numOfArgs <= 4) {
        for (int i = 1; i < numOfArgs; i++) {
            strApCfgs.push_back(args.stringArgs[i]);
        }
        return steering_set_group(args.intArgs[0], strApCfgs);
    }
    return -1;
}
int cli_bml::bml_pre_association_steering_client_set_caller(int numOfArgs)
{
    if (numOfArgs == 4) {
        return steering_client_set(args.intArgs[0], args.stringArgs[1], args.stringArgs[2],
                                   args.stringArgs[3]);
    } else if (numOfArgs == 3) {
        return steering_client_set(args.intArgs[0], args.stringArgs[1], args.stringArgs[2]);
    }
    return -1;
}
int cli_bml::bml_pre_association_steering_event_register_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return steering_event_register();
    return steering_event_register(args.stringArgs[0]);
}

int cli_bml::bml_pre_association_steering_client_measure_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return steering_client_measure(args.intArgs[0], args.stringArgs[1], args.stringArgs[2]);
    }
    return -1;
}

int cli_bml::bml_pre_association_steering_client_disconnect_caller(int numOfArgs)
{
    if (numOfArgs == 5) {
        return steering_client_disconnect(args.intArgs[0], args.stringArgs[1], args.stringArgs[2],
                                          args.intArgs[3], args.intArgs[4]);
    }
    return -1;
}
#endif //FEATURE_PRE_ASSOCIATION_STEERING

/**
 * caller function for set_dcs_continuous_scan_enable
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::set_dcs_continuous_scan_enable_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return set_dcs_continuous_scan_enable(args.stringArgs[0], args.intArgs[1]);
    }
    return -1;
}

/**
 * caller function for triggering unassoc link metrics query
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::send_unassoc_sta_rcpi_query_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return send_unassoc_sta_rcpi_query(args.stringArgs[0], args.intArgs[1],
                                           uint8_t(args.intArgs[2]));
    }
    return -1;
}

/**
 * caller function for fetching unassoc link metric query result for a sta
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::get_unassoc_sta_rcpi_result_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return get_unassoc_sta_rcpi_query_result(args.stringArgs[0]);
    }
    return -1;
}

/**
 * caller function for get_dcs_continuous_scan_enable
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_continuous_scan_enable_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return get_dcs_continuous_scan_enable(args.stringArgs[0]);
    }
    return -1;
}

/**
 * caller function for set_dcs_continuous_scan_params
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::set_dcs_continuous_scan_params_caller(int numOfArgs)
{
    std::string radio_mac(network_utils::WILD_MAC_STRING);
    int32_t dwell_time    = BML_CHANNEL_SCAN_INVALID_PARAM;
    int32_t interval_time = BML_CHANNEL_SCAN_INVALID_PARAM;
    std::string channel_pool;

    std::string::size_type pos;
    //[radio_mac=<radio_mac> dwell_time=<dwell_time> interval_time='<interval_time>']"
    for (int i = 0; i < numOfArgs; i++) { //first optional arg
        if ((pos = args.stringArgs[i].find("radio_mac=")) != std::string::npos) {
            radio_mac = args.stringArgs[i].substr(pos + sizeof("radio_mac"));
        } else if ((pos = args.stringArgs[i].find("dwell_time=")) != std::string::npos) {
            dwell_time = string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("dwell_time")));
        } else if ((pos = args.stringArgs[i].find("interval_time=")) != std::string::npos) {
            interval_time =
                string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("interval_time")));
        } else if ((pos = args.stringArgs[i].find("channel_pool=")) != std::string::npos) {
            channel_pool = args.stringArgs[i].substr(pos + sizeof("channel_pool"));
        }
    }
    if (numOfArgs > 1) {
        return set_dcs_continuous_scan_params(radio_mac, dwell_time, interval_time, channel_pool);
    }
    return -1;
}

/**
 * caller function for get_dcs_continuous_scan_params
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_continuous_scan_params_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return get_dcs_continuous_scan_params(args.stringArgs[0]);
    }
    return -1;
}

/**
 * caller function for start_dcs_single_scan
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::start_dcs_single_scan_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return start_dcs_single_scan(args.stringArgs[0], args.intArgs[1], args.stringArgs[2]);
    }
    return -1;
}

/**
 * caller function for get_dcs_scan_results
 *
 * @param [in] numOfArgs Num of received arguments
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_scan_results_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return get_dcs_scan_results(args.stringArgs[0], args.intArgs[1]);
    } else if (numOfArgs == 3) {
        bool single_scan = (args.intArgs[2] == 1);
        return get_dcs_scan_results(args.stringArgs[0], args.intArgs[1], single_scan);
    }
    return -1;
}

/**
 * Caller function for client_get_client_list_caller.
 *
 * @param [in] numOfArgs Number of received arguments
 * @return 0 on success.
 */
int cli_bml::client_get_client_list_caller(int numOfArgs)
{
    if (numOfArgs == 0) {
        return client_get_client_list();
    }
    return -1;
}

/**
 * Caller function for client_set_client_caller.
 *
 * @param [in] numOfArgs Number of received arguments
 * @return 0 on success.
 */
int cli_bml::client_set_client_caller(int numOfArgs)
{

    /*
     * Mandatory:
     *  sta_mac=<sta_mac>
     * Optional:
     *  selected_bands=<selected_bands>
     *  stay_on_initial_radio=<stay_on_initial_radio>
     *  time_life_delay_minutes=<time_life_delay_minutes>
    ]*/
    std::string::size_type pos;
    int8_t selected_bands           = BML_PARAMETER_NOT_CONFIGURED;
    int8_t stay_on_initial_radio    = BML_PARAMETER_NOT_CONFIGURED;
    int32_t time_life_delay_minutes = BML_PARAMETER_NOT_CONFIGURED;

    if (numOfArgs > 1) {
        std::string sta_mac = args.stringArgs[0];
        for (int i = 1; i < numOfArgs; i++) { //first optional arg
            if ((pos = args.stringArgs[i].find("selected_bands=")) != std::string::npos) {
                selected_bands = beerocks::string_utils::stoi(
                    args.stringArgs[i].substr(pos + sizeof("selected_bands")));
            } else if ((pos = args.stringArgs[i].find("stay_on_initial_radio=")) !=
                       std::string::npos) {
                stay_on_initial_radio = string_utils::stoi(
                    args.stringArgs[i].substr(pos + sizeof("stay_on_initial_radio")));
            } else if ((pos = args.stringArgs[i].find("time_life_delay_minutes")) !=
                       std::string::npos) {
                time_life_delay_minutes = string_utils::stoi(
                    args.stringArgs[i].substr(pos + sizeof("time_life_delay_minutes")));
            }
        }
        return client_set_client(sta_mac, selected_bands, stay_on_initial_radio,
                                 time_life_delay_minutes);
    }
    return -1;
}

/**
 * Caller function for client_get_client_caller.
 *
 * @param [in] numOfArgs Number of received arguments
 * @return 0 on success.
 */
int cli_bml::client_get_client_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return client_get_client(args.stringArgs[0]);
    }
    return -1;
}

/**
 * @brief Caller function for client_clear_client_caller.
 *
 * @param [in] numOfArgs Number of received arguments
 * @return 0 on success, -1 on failure.
 */
int cli_bml::client_clear_client_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return client_clear_client(args.stringArgs[0]);
    }
    return -1;
}

//
// Functions
//

int cli_bml::get_onboarding_status()
{
    onboard_status();
    return is_onboarding;
}

bool cli_bml::is_pending_response() { return pending_response; }

int cli_bml::onboard_status()
{
    is_onboarding = bml_onboard_status(ctx);
    std::cout << "onboard_status = " << is_onboarding << std::endl;
    return 0;
}

int cli_bml::ping()
{
    int ret = bml_ping(ctx);
    printBmlReturnVals("bml_ping", ret);
    return 0;
}

int cli_bml::nw_map_register_update_cb(const std::string &optional)
{
    int ret;
    if (optional == "x") {
        ret = bml_nw_map_register_update_cb(ctx, NULL);
    } else {
        ret = bml_nw_map_register_update_cb(ctx, map_update_to_console_cb);
    }
    printBmlReturnVals("bml_nw_map_register_update_cb", ret);
    return 0;
}

int cli_bml::nw_map_query()
{
    // Register the query callback
    bml_nw_map_register_query_cb(ctx, cli_bml::map_query_to_console_cb);
    pending_response = true;
    int ret          = bml_nw_map_query(ctx);
    printBmlReturnVals("bml_nw_map_query", ret);
    return 0;
}

int cli_bml::connection_map()
{
    // Register the query callback
    bml_nw_map_register_query_cb(ctx, cli_bml::connection_map_to_console_cb);
    pending_response = true;
    int ret          = bml_nw_map_query(ctx);
    printBmlReturnVals("bml_nw_map_query", ret);
    return 0;
}

int cli_bml::get_device_operational_radios(const std::string &al_mac)
{
    BML_DEVICE_DATA device_data = {};
    device_data.al_mac          = al_mac.c_str();

    int ret = bml_device_oper_radios_query(ctx, &device_data);

    // Main agent
    std::cout << ((ret == BML_RET_OK) ? "OK" : "FAIL") << " Main radio agent operational"
              << std::endl;
    for (const auto &radio : device_data.radios) {
        if (radio.is_connected) {
            // wlan radio agents
            std::cout << (radio.is_operational ? "OK " : "FAIL ") << radio.iface_name
                      << " radio agent operational" << std::endl;
        }
    }

    printBmlReturnVals("bml_device_oper_radios_query", ret);
    return 0;
}

int cli_bml::stat_register_cb(const std::string &optional)
{
    int ret;
    if (optional == "x") {
        ret = bml_stat_register_cb(ctx, NULL);
    } else {
        ret = bml_stat_register_cb(ctx, stats_update_to_console_cb);
    }
    printBmlReturnVals("bml_stat_register_cb", ret);
    return 0;
}

int cli_bml::events_register_cb(const std::string &optional)
{
    int ret;
    if (optional == "x") {
        ret = bml_event_register_cb(ctx, NULL);
    } else {
        ret = bml_event_register_cb(ctx, events_update_to_console_cb);
    }
    printBmlReturnVals("bml_event_register_cb", ret);
    return 0;
}

int cli_bml::set_wifi_credentials(const std::string &al_mac, const std::string &ssid,
                                  const std::string &network_key, const std::string &bands,
                                  const std::string &bss_type, bool add_sae)
{

    int ret = bml_set_wifi_credentials(ctx, al_mac.c_str(), ssid.c_str(), network_key.c_str(),
                                       bands.c_str(), bss_type.c_str(), add_sae);

    printBmlReturnVals("bml_set_wifi_credentials", ret);
    return 0;
}

int cli_bml::clear_wifi_credentials(const std::string &al_mac)
{

    int ret = bml_clear_wifi_credentials(ctx, al_mac.c_str());

    printBmlReturnVals("bml_clear_wifi_credentials", ret);
    return 0;
}

int cli_bml::update_wifi_credentials()
{

    int ret = bml_update_wifi_credentials(ctx);

    printBmlReturnVals("bml_update_wifi_credentials", ret);
    return 0;
}

int cli_bml::get_wifi_credentials(int vap_id)
{
    char ssid[BML_NODE_SSID_LEN];
    int sec = -1;
    ssid[0] = 0;
    int ret = bml_get_wifi_credentials(ctx, vap_id, ssid, nullptr, &sec);
    if (ret != BML_RET_OK) {
        return -1;
    }
    std::string sec_str;

    switch (sec) {
    case BML_WLAN_SEC_NONE:
        sec_str = BML_WLAN_SEC_NONE_STR;
        break;

    case BML_WLAN_SEC_WEP64:
        sec_str = BML_WLAN_SEC_WEP64_STR;
        break;

    case BML_WLAN_SEC_WEP128:
        sec_str = BML_WLAN_SEC_WEP128_STR;
        break;

    case BML_WLAN_SEC_WPA_PSK:
        sec_str = BML_WLAN_SEC_WPA_PSK_STR;
        break;

    case BML_WLAN_SEC_WPA2_PSK:
        sec_str = BML_WLAN_SEC_WPA2_PSK_STR;
        break;

    case BML_WLAN_SEC_WPA_WPA2_PSK:
        sec_str = BML_WLAN_SEC_WPA_WPA2_PSK_STR;
        break;

    default:
        sec_str = "Unknown";
    }

    std::cout << "SSID='" << ssid << "', Security=" << sec_str << std::endl;

    printBmlReturnVals("bml_get_wifi_credentials", ret);
    return 0;
}

int cli_bml::set_onboarding_state(int enable)
{
    int ret = bml_set_onboarding_state(ctx, enable);
    printBmlReturnVals("bml_set_onboarding_state", ret);
    return 0;
}

int cli_bml::get_onboarding_state()
{
    int enable;
    int ret = bml_get_onboarding_state(ctx, &enable);
    std::cout << "Onboarding=" << enable << std::endl;
    printBmlReturnVals("bml_get_onboarding_state", ret);
    return 0;
}

int cli_bml::wps_onboarding(const std::string &iface)
{
    int ret = bml_wps_onboarding(ctx, iface.c_str());
    printBmlReturnVals("bml_wps_onboarding", ret);
    return 0;
}

int cli_bml::get_bml_version()
{
    std::string ver = bml_get_bml_version();
    std::cout << "Bml version = " << ver << std::endl;
    return 0;
}

int cli_bml::get_master_slave_versions()
{
    beerocks_message::sVersions versions;
    versions.master_version[0] = 0;
    versions.slave_version[0]  = 0;
    int ret = bml_get_master_slave_versions(ctx, versions.master_version, versions.slave_version);
    std::cout << "Beerocks master version = " << versions.master_version
              << ", Beerocks slave version = " << versions.slave_version << std::endl;

    printBmlReturnVals("bml_get_master_slave_versions", ret);
    return 0;
}

int cli_bml::enable_client_roaming(int8_t isEnable)
{
    int result = -1;
    int ret    = (isEnable < 0) ? bml_get_client_roaming(ctx, &result)
                             : bml_set_client_roaming(ctx, isEnable);
    printBmlReturnVals("bml_enable_client_roaming", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "client_roaming mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_client_roaming_11k_support(int8_t isEnable)
{
    int result = -1;
    int ret    = (isEnable < 0) ? bml_get_client_roaming_11k_support(ctx, &result)
                             : bml_set_client_roaming_11k_support(ctx, isEnable);
    printBmlReturnVals("bml_enable_client_roaming_11k_support", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "client_roaming_11k_support mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_legacy_client_roaming(int8_t isEnable)
{
    int result = -1;
    int ret    = (isEnable < 0) ? bml_get_legacy_client_roaming(ctx, &result)
                             : bml_set_legacy_client_roaming(ctx, isEnable);
    printBmlReturnVals("bml_enable_legacy_client_roaming", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "legacy_client_roaming mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_client_roaming_prefer_signal_strength(int8_t isEnable)
{
    int result = -1;
    int ret    = (isEnable < 0) ? bml_get_client_roaming_prefer_signal_strength(ctx, &result)
                             : bml_set_client_roaming_prefer_signal_strength(ctx, isEnable);
    printBmlReturnVals("bml_enable_client_roaming_prefer_signal_strength", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "client roaming prefer signal strength mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_client_band_steering(int8_t isEnable)
{
    int result = -1;
    int ret    = (isEnable < 0) ? bml_get_client_band_steering(ctx, &result)
                             : bml_set_client_band_steering(ctx, isEnable);
    printBmlReturnVals("bml_enable_client_band_steering", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "client_band_steering mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_ire_roaming(int8_t isEnable)
{
    int result = -1;
    int ret =
        (isEnable < 0) ? bml_get_ire_roaming(ctx, &result) : bml_set_ire_roaming(ctx, isEnable);
    printBmlReturnVals("bml_enable_ire_roaming", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "ire_roaming mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_load_balancer(int8_t isEnable)
{
    int result = -1;
    int ret =
        (isEnable < 0) ? bml_get_load_balancer(ctx, &result) : bml_set_load_balancer(ctx, isEnable);
    printBmlReturnVals("bml_load_balancer", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "load_balancer mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_service_fairness(int8_t isEnable)
{
    int result = -1;
    int ret    = (isEnable < 0) ? bml_get_service_fairness(ctx, &result)
                             : bml_set_service_fairness(ctx, isEnable);
    printBmlReturnVals("bml_enable_service_fairness", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "service_fairness mode = " << result << std::endl;
    return 0;
}

int cli_bml::enable_dfs_reentry(int8_t isEnable)
{
    int result = -1;
    int ret =
        (isEnable < 0) ? bml_get_dfs_reentry(ctx, &result) : bml_set_dfs_reentry(ctx, isEnable);
    printBmlReturnVals("bml_enable_dfs_reentry", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "service_dfs_reentry = " << result << std::endl;
    return 0;
}

int cli_bml::enable_certification_mode(int8_t isEnable)
{
    int result = -1;
    int ret    = (isEnable < 0) ? bml_get_certification_mode(ctx, &result)
                             : bml_set_certification_mode(ctx, isEnable);
    printBmlReturnVals("bml_enable_certification_mode", ret);
    if (isEnable < 0 && ret == BML_RET_OK)
        std::cout << "enable_certification_mode = " << result << std::endl;
    return 0;
}

int cli_bml::set_log_level(const std::string &module_name, const std::string &log_level, uint8_t on,
                           const std::string &mac)
{
    int ret = bml_set_log_level(ctx, module_name.c_str(), log_level.c_str(), on, mac.c_str());
    printBmlReturnVals("bml_set_log_level", ret);
    return 0;
}

int cli_bml::analyzer_init(std::string remote_pc_ip)
{
    //connect to master via bml
    int retries    = 36; //36 retries of 5 sec = 3 minutes
    bool connected = connect();
    while (!connected && retries > 0) {
        retries--;
        UTILS_SLEEP_MSEC(5000);
        connected = connect();
    }
    if (!connected) {
        std::cout << "failed connecting to master via bml, exiting... " << std::endl;
        return -1;
    }

    //Open tcp client socket to remote pc
    m_analyzer_socket = new SocketClient(remote_pc_ip, beerocks::ANALYZER_TCP_PORT);
    if (!m_analyzer_socket->getError().empty()) {
        LOG(ERROR) << "Failed connecting to remote pc: " << m_analyzer_socket->getError()
                   << ", exiting..." << std::endl;

        m_analyzer_socket->closeSocket();
        delete m_analyzer_socket;
        m_analyzer_socket = nullptr;

        return -1;
    }

    is_analyzer = true;
    select.addSocket(m_analyzer_socket);

    //register map_query_to_socket_cb and send a query request
    if (bml_nw_map_register_query_cb(ctx, cli_bml::map_query_to_socket_cb)) {
        std::cout << "failed to register nw_map_query to socket cb" << std::endl;
    }
    if (bml_nw_map_query(ctx)) {
        std::cout << "failed to send bml_nw_map_query request" << std::endl;
    }

    //register to stats and map updates
    if (bml_nw_map_register_update_cb(ctx, map_update_to_socket_cb)) {
        std::cout << "failed to register nw_map_update to socket cb" << std::endl;
    }
    if (bml_stat_register_cb(ctx, stats_update_to_socket_cb)) {
        std::cout << "failed to register statistics to socket cb" << std::endl;
    }
    if (bml_event_register_cb(ctx, events_update_to_socket_cb)) {
        std::cout << "failed to register events to socket cb" << std::endl;
    }

    int t = 0;
    while (is_analyzer) {
        UTILS_SLEEP_MSEC(1000);
        int sel_ret = select.selectSocket();
        if (sel_ret > 0) {
            m_analyzer_socket->readBytes(rx_buffer, message::MESSAGE_BUFFER_LENGTH,
                                         m_analyzer_socket->getBytesReady());
            if (bml_nw_map_query(ctx)) {
                std::cout << "failed to send bml_nw_map_query request" << std::endl;
                break;
            }
        }
        if (t < 19) {
            t++;
        } else {
            t = 0;
            if (bml_nw_map_query(ctx)) {
                std::cout << "failed to send bml_nw_map_query request" << std::endl;
                break;
            }
        }
    }

    return 0;
}

int cli_bml::set_global_restricted_channels(const std::string &restricted_channels)
{
    std::cout << "restricted_channels string = " << restricted_channels << std::endl;
    uint8_t restricted_channel[BML_NODE_RESTRICTED_CHANNELS_LEN] = {};
    auto vec_substr = string_utils::str_split(restricted_channels, ',');
    std::cout << "restricted_channels string = " << restricted_channels << std::endl;

    int count = 0;
    for (const auto &elment : vec_substr) {
        std::cout << elment << " count = " << int(count) << std::endl;
        restricted_channel[count] = uint8_t(string_utils::stoi(elment));
        count++;
    }

    for (const auto &channel : restricted_channel) {
        std::cout << "restricted_channels int = " << int(channel) << std::endl;
    }
    // std::for_each(std::begin(vec_substr) , std::end(vec_substr) , [&](std::string elm_substr) {
    //     std::cout << "elm_substr = " << elm_substr << std::endl;
    //     restricted_channel[count] = uint8_t(string_utils::stoi(elm_substr));
    //     count++;
    // });
    int ret = bml_set_global_restricted_channels(ctx, restricted_channel, uint8_t(count + 1));
    printBmlReturnVals("bml_set_global_restricted_channels", ret);
    return 0;
}

int cli_bml::get_global_restricted_channels()
{
    uint8_t restricted_channel[BML_NODE_RESTRICTED_CHANNELS_LEN];
    int ret = bml_get_global_restricted_channels(ctx, restricted_channel);
    std::cout << "Restricted_channels: = " << std::endl;
    for (auto &elem : restricted_channel) {
        std::cout << int(elem) << " ,";
    }
    printBmlReturnVals("bml_get_global_restricted_channels", ret);
    return 0;
}

int cli_bml::set_slave_restricted_channels(const std::string &restricted_channels,
                                           const std::string &hostap_mac)
{
    uint8_t restricted_channel[BML_NODE_RESTRICTED_CHANNELS_LEN];
    auto vec_substr = string_utils::str_split(restricted_channels, ',');
    int count       = 0;
    std::for_each(vec_substr.begin(), vec_substr.end(), [&](std::string substr) {
        restricted_channel[count] = uint8_t(string_utils::stoi(substr));
        count++;
    });
    std::cout << "hostap_mac = " << hostap_mac << std::endl;
    int ret = bml_set_slave_restricted_channels(ctx, restricted_channel, hostap_mac.c_str(),
                                                uint8_t(count));
    printBmlReturnVals("set_slave_restricted_channels", ret);
    return 0;
}

int cli_bml::get_slave_restricted_channels(const std::string &hostap_mac)
{
    uint8_t restricted_channel[BML_NODE_RESTRICTED_CHANNELS_LEN];
    int ret = bml_get_slave_restricted_channels(ctx, restricted_channel, hostap_mac.c_str());
    std::cout << "Restricted_channels for hostap_mac = " << hostap_mac << std::endl;
    for (auto &elem : restricted_channel) {
        std::cout << int(elem) << " ,";
    }
    printBmlReturnVals("get_slave_restricted_channels", ret);
    return 0;
}

int cli_bml::topology_discovery(const std::string &al_mac)
{
    int ret = bml_trigger_topology_discovery(ctx, al_mac.c_str());
    printBmlReturnVals("topology discovery", ret);
    return 0;
}

int cli_bml::channel_selection(const std::string &radio_mac, uint8_t channel, uint8_t bandwidth,
                               uint8_t csa_count)
{
    int ret = bml_channel_selection(ctx, radio_mac.c_str(), channel, bandwidth, csa_count);
    printBmlReturnVals("channel_selection", ret);
    return 0;
}

int cli_bml::set_selection_pool(const std::string &radio_mac, const std::string &channel_pool)
{
    auto channels      = string_utils::str_split(channel_pool, ',');
    auto channels_size = channels.size();

    if (channels_size > BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE) {
        std::cout << "size of channel_pool is too big. size=" << channels_size << std::endl;
        return -1;
    }

    unsigned int channel_pool_arr[BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE] = {0};
    for (size_t i = 0; i < channels_size; i++) {
        channel_pool_arr[i] = beerocks::string_utils::stoi(channels[i]);
    }

    int ret =
        bml_set_selection_channel_pool(ctx, radio_mac.c_str(), channel_pool_arr, channels_size);
    printBmlReturnVals("set_selection_pool", ret);
    return 0;
}

int cli_bml::get_selection_pool(const std::string &radio_mac)
{
    int channel_pool_size = BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE;
    unsigned int channel_pool[BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE] = {0};
    int ret =
        bml_get_selection_channel_pool(ctx, radio_mac.c_str(), channel_pool, &channel_pool_size);

    std::cout << "channel_pool=";
    for (int i = 0; (i < channel_pool_size) && (i < BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE); i++) {
        if (channel_pool[i] > 0) {
            std::cout << channel_pool[i] << " ";
        }
    }
    std::cout << std::endl;
    printBmlReturnVals("get_selection_pool", ret);
    return 0;
}

#ifdef FEATURE_PRE_ASSOCIATION_STEERING
int cli_bml::steering_set_group(uint32_t steeringGroupIndex,
                                const std::vector<std::string> &str_ap_cfgs)

{
    int ret;
    BML_STEERING_AP_CONFIG *cfgs = nullptr;
    unsigned int length          = 0;

    std::cout << "set_ap_config entries" << std::endl;
    if (!str_ap_cfgs.empty()) {
        length = str_ap_cfgs.size();
        cfgs   = new BML_STEERING_AP_CONFIG[length];
        if (!cfgs) {
            std::cout << "Can't allocate array of " << length << " BML_STEERING_AP_CONFIG"
                      << std::endl;
            return -1;
        }
        steering_set_group_string_to_struct(str_ap_cfgs, cfgs);
    }

    ret = bml_pre_association_steering_set_group(ctx, steeringGroupIndex, cfgs, length);

    if (cfgs) {
        delete[] cfgs;
        cfgs = nullptr;
    }
    printBmlReturnVals("bml_pre_association_steering_set_group", ret);
    return 0;
}

int cli_bml::steering_client_set(uint32_t steeringGroupIndex, const std::string &str_bssid,
                                 const std::string &str_client_mac, const std::string &str_config)
{
    BML_MAC_ADDR client_mac;
    tlvf::mac_from_string(client_mac, str_client_mac);
    BML_MAC_ADDR bssid;
    tlvf::mac_from_string(bssid, str_bssid);
    int ret;
    if (str_config.empty()) {
        //client remove
        ret = bml_pre_association_steering_client_set(ctx, steeringGroupIndex, bssid, client_mac,
                                                      nullptr);
    } else {
        // client add
        BML_STEERING_CLIENT_CONFIG config = {};
        steering_client_set_string_to_struct(str_config, config);
        ret = bml_pre_association_steering_client_set(ctx, steeringGroupIndex, bssid, client_mac,
                                                      &config);
    }
    printBmlReturnVals("bml_pre_association_steering_client_set ", ret);
    return 0;
}

int cli_bml::steering_event_register(const std::string &optional)
{
    int ret;
    if (optional == "x") {
        ret = bml_pre_association_steering_event_register(ctx, NULL);
    } else {
        ret = bml_pre_association_steering_event_register(ctx, events_update_to_console_cb);
    }

    printBmlReturnVals("bml_pre_association_steering_event_register", ret);
    return 0;
}

int cli_bml::steering_client_measure(uint32_t steeringGroupIndex, const std::string &str_bssid,
                                     const std::string &str_client_mac)
{
    BML_MAC_ADDR client_mac;
    tlvf::mac_from_string(client_mac, str_client_mac);
    BML_MAC_ADDR bssid;
    tlvf::mac_from_string(bssid, str_bssid);
    int ret =
        bml_pre_association_steering_client_measure(ctx, steeringGroupIndex, bssid, client_mac);
    printBmlReturnVals("bml_pre_association_steering_client_measure", ret);
    return 0;
}

int cli_bml::steering_client_disconnect(uint32_t steeringGroupIndex, const std::string &str_bssid,
                                        const std::string &str_client_mac, uint32_t type,
                                        uint32_t reason)
{
    BML_MAC_ADDR client_mac;
    tlvf::mac_from_string(client_mac, str_client_mac);
    BML_MAC_ADDR bssid;
    tlvf::mac_from_string(bssid, str_bssid);
    if (type >= 3) {
        //assign type BML_DISCONNECT_TYPE_UNKNOWN value in case of invalid value
        type = 0;
    }
    int ret = bml_pre_association_steering_client_disconnect(
        ctx, steeringGroupIndex, bssid, client_mac, static_cast<BML_DISCONNECT_TYPE>(type), reason);
    printBmlReturnVals("bml_pre_association_steering_client_disconnect", ret);
    return 0;
}

#endif //FEATURE_PRE_ASSOCIATION_STEERING

/**
 * Enables or disables beerocks DCS continuous scans.
 *
 * @param [in] radio_mac Radio MAC of selected radio
 * @param [in] enable Value of 1 to enable or 0 to disable.
 *
 * @return 0 on success.
 */
int cli_bml::set_dcs_continuous_scan_enable(const std::string &radio_mac, int8_t enable)
{
    std::cout << __func__ << ", mac=" << radio_mac << ", enable=" << enable << std::endl;

    int ret = bml_set_dcs_continuous_scan_enable(ctx, radio_mac.c_str(), enable);

    printBmlReturnVals("bml_set_dcs_continuous_scan_enable", ret);

    return 0;
}

/**
 * Sends unassoc sta link metrics query to agents
 *
 * @param [in] sta_mac is mac address of unassociated sta
 * @param [in] opclass is operating class
 * @param [in] channel is channel from the operating class
 *
 * @return 0 on success.
 */
int cli_bml::send_unassoc_sta_rcpi_query(const std::string &sta_mac, int16_t opclass,
                                         int16_t channel)
{
    std::cout << __func__ << ", mac=" << sta_mac << ", opclass=" << opclass
              << ", channel=" << channel << std::endl;
    int ret = bml_send_unassoc_sta_rcpi_query(ctx, sta_mac.c_str(), opclass, channel);

    printBmlReturnVals("bml_send_unassoc_sta_rcpi_query", ret);

    return 0;
}

/**
 * Fetch unassoc sta link metrics response result from db
 * if response exist for sta mac
 *
 * @param [in] sta_mac is mac address of unassociated sta
 *
 * @return 0 on success.
 */
int cli_bml::get_unassoc_sta_rcpi_query_result(const std::string &sta_mac)
{
    std::cout << __func__ << ", mac=" << sta_mac << std::endl;
    struct BML_UNASSOC_STA_LINK_METRIC sta;
    int ret = bml_get_unassoc_sta_rcpi_query_result(ctx, sta_mac.c_str(), &sta);

    if (ret == BML_RET_OK) {
        std::cout << "Unassoc sta link metric result for sta " << sta_mac << std::endl
                  << "sta opclass=" << int(sta.opclass) << std::endl
                  << "sta channel=" << int(sta.channel) << std::endl
                  << "sta uplink rcpi=" << int(sta.rcpi) << std::endl
                  << "sta measurement delta=" << sta.measurement_delta << std::endl;
    }

    printBmlReturnVals("bml_get_unassoc_sta_rcpi_query_result", ret);

    return 0;
}

/**
 * get DCS continuous scans param. Value is printed to the console.
 *
 * @param [in] radio_mac Radio MAC of selected radio
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_continuous_scan_enable(const std::string &radio_mac)
{
    std::cout << __func__ << ", mac=" << radio_mac << std::endl;

    int enable = -1;
    int ret    = bml_get_dcs_continuous_scan_enable(ctx, radio_mac.c_str(), &enable);

    if (ret == BML_RET_OK) {
        std::cout << "dcs-continuous-scan-enable=" << string_utils::bool_str(enable) << std::endl;
    }

    printBmlReturnVals("bml_get_dcs_continuous_scan_enable", ret);

    return 0;
}

/**
 * set DCS continuous scan params.
 *
 * @param [in] radio_mac Radio MAC of selected radio
 * @param [in] dwell_time Set the dwell time in milliseconds.
 * @param [in] interval_time Set the interval time in seconds.
 * @param [in] channel_pool Set the channel pool for the DCS.
 *
 * @return 0 on success.
 */
int cli_bml::set_dcs_continuous_scan_params(const std::string &radio_mac, int32_t dwell_time,
                                            int32_t interval_time, const std::string &channel_pool)
{
    std::cout << __func__ << ", mac=" << radio_mac << ", dwell_time=" << dwell_time
              << ", interval_time=" << interval_time << ", channel_pool=" << channel_pool
              << std::endl;

    int ret = -1;
    if (channel_pool.length() == 0) {
        ret = bml_set_dcs_continuous_scan_params(ctx, radio_mac.c_str(), dwell_time, interval_time,
                                                 nullptr, BML_CHANNEL_SCAN_INVALID_PARAM);
    } else {
        auto channels      = string_utils::str_split(channel_pool, ',');
        auto channels_size = channels.size();

        if (channels_size > BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE) {
            std::cout << "size of channel_pool is too big. size=" << channels_size << std::endl;
            return -1;
        }

        unsigned int channel_pool_arr[BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE] = {0};

        for (size_t i = 0; i < channels_size; i++) {
            channel_pool_arr[i] = beerocks::string_utils::stoi(channels[i]);
        }

        ret = bml_set_dcs_continuous_scan_params(ctx, radio_mac.c_str(), dwell_time, interval_time,
                                                 channel_pool_arr, uint32_t(channels_size));
    }
    printBmlReturnVals("bml_set_dcs_continuous_scan_params", ret);

    return 0;
}

/**
 * get DCS continuous scan params. Values are printed to the console.
 *
 * @param [in] radio_mac Radio MAC of selected radio
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_continuous_scan_params(const std::string &radio_mac)
{
    std::cout << __func__ << ", mac=" << radio_mac << std::endl;

    int dwell_time = BML_CHANNEL_SCAN_INVALID_PARAM, interval_time = BML_CHANNEL_SCAN_INVALID_PARAM,
        channel_pool_size = BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE;
    unsigned int channel_pool[BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE] = {0};

    int ret = bml_get_dcs_continuous_scan_params(ctx, radio_mac.c_str(), &dwell_time,
                                                 &interval_time, channel_pool, &channel_pool_size);

    if (ret == BML_RET_OK) {
        std::cout << "dcs-continuous-scan-params for mac=" << radio_mac << std::endl
                  << "dwell_time=" << dwell_time << std::endl
                  << "interval_time=" << interval_time << std::endl;

        std::cout << "channel_pool=";
        for (int i = 0; (i < channel_pool_size) && (i < BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE);
             i++) {
            if (channel_pool[i] > 0) {
                if (i > 0) {
                    std::cout << ",";
                }
                std::cout << channel_pool[i];
            }
        }
        std::cout << std::endl;
        std::cout << "channel_pool_size=" << channel_pool_size << std::endl;
    }

    printBmlReturnVals("bml_get_dcs_continuous_scan_params", ret);

    return 0;
}

/**
 * Start a single DCS scan with parameters.
 *
 * @param [in] radio_mac radio MAC of selected radio
 * @param [in] dwell_time Set the dwell time in milliseconds.
 * @param [in] channel_pool Set the channel pool for the DCS.
 *
 * @return 0 on success.
 */
int cli_bml::start_dcs_single_scan(const std::string &radio_mac, int32_t dwell_time,
                                   const std::string &channel_pool)
{
    std::cout << "start_dcs_single_scan, mac=" << radio_mac << ", dwell_time=" << dwell_time
              << ", channel_pool=" << channel_pool << std::endl;

    if (dwell_time < 0) {
        std::cout << __func__ << ", invalid input: dwell_time='" << dwell_time << "'."
                  << " Only positive values are supported!" << std::endl;
        return -1;
    }

    if (channel_pool.length() == 0) {
        std::cout << __func__
                  << "invalid channel_pool input: channel_pool.length()==" << channel_pool.length()
                  << std::endl;
        return -1;
    }

    auto channels      = string_utils::str_split(channel_pool, ',');
    auto channels_size = channels.size();

    if (channels_size > BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE) {
        std::cout << "size of channel_pool is too big. size=" << channels_size << std::endl;
        return -1;
    }

    unsigned int channel_pool_arr[BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE] = {0};

    for (size_t i = 0; i < channels_size; i++) {
        channel_pool_arr[i] = beerocks::string_utils::stoi(channels[i]);
    }

    int ret = bml_start_dcs_single_scan(ctx, radio_mac.c_str(), dwell_time, uint32_t(channels_size),
                                        channel_pool_arr);

    printBmlReturnVals("bml_start_dcs_single_scan", ret);

    return 0;
}

/**
 * get DCS continuous scan params.
 *
 * @param [in] radio_mac radio MAC of selected radio
 * @param [in] max_results_size Max number of the returned results
 * @param [in] is_single_scan Flag indicating if the results belong to a single scan or not
 *
 * @return 0 on success.
 */
int cli_bml::get_dcs_scan_results(const std::string &radio_mac, uint32_t max_results_size,
                                  bool is_single_scan)
{
    std::cout << __func__ << ", mac=" << radio_mac << ", max_results_size=" << max_results_size
              << ", is_single_scan=" << string_utils::bool_str(is_single_scan) << std::endl;

    if (max_results_size == 0) {
        std::cout << __func__ << "invalid input, max_results_size==0" << std::endl;
        return -1;
    }

    BML_NEIGHBOR_AP results[max_results_size];

    uint8_t status             = 0;
    unsigned int results_count = max_results_size;
    int ret = bml_get_dcs_scan_results(ctx, radio_mac.c_str(), results, &results_count, &status,
                                       is_single_scan);

    if (ret == BML_RET_OK) {
        if (results_count > max_results_size) {
            std::cout << __func__ << "ERROR: results_count(" << results_count << ")"
                      << " > max_results_size(" << max_results_size << ")" << std::endl;
        } else if ((status == 0) && (results_count > 0)) {
            for (size_t i = 0; i < results_count; i++) {
                auto res = results[i];
                std::cout << "result[" << i << "]:" << std::endl
                          << "  ssid=" << res.ap_SSID << std::endl
                          << "  bssid=" << tlvf::mac_to_string(res.ap_BSSID) << std::endl
                          << "  mode=" << int(res.ap_Mode) << std::endl
                          << "  channel=" << int(res.ap_Channel) << std::endl
                          << "  signal_strength_dbm=" << res.ap_SignalStrength << std::endl
                          << "  security_mode_enabled="
                          << string_from_int_array(res.ap_SecurityModeEnabled,
                                                   BML_CHANNEL_SCAN_ENUM_LIST_SIZE)
                          << std::endl
                          << "  encryption_mode="
                          << string_from_int_array(res.ap_EncryptionMode,
                                                   BML_CHANNEL_SCAN_ENUM_LIST_SIZE)
                          << std::endl
                          << "  operating_frequency_band=" << int(res.ap_OperatingFrequencyBand)
                          << std::endl
                          << "  supported_standards="
                          << string_from_int_array(res.ap_SupportedStandards,
                                                   BML_CHANNEL_SCAN_ENUM_LIST_SIZE)
                          << std::endl
                          << "  operating_standards=" << int(res.ap_OperatingStandards) << std::endl
                          << "  operating_channel_bandwidth="
                          << int(res.ap_OperatingChannelBandwidth) << std::endl
                          << "  beacon_period_ms=" << int(res.ap_BeaconPeriod) << std::endl
                          << "  noise_dbm=" << int(res.ap_Noise) << std::endl
                          << "  basic_data_transfer_rates_kbps="
                          << string_from_int_array(res.ap_BasicDataTransferRates,
                                                   BML_CHANNEL_SCAN_ENUM_LIST_SIZE)
                          << std::endl
                          << "  supported_data_transfer_rates_kbps="
                          << string_from_int_array(res.ap_SupportedDataTransferRates,
                                                   BML_CHANNEL_SCAN_ENUM_LIST_SIZE)
                          << std::endl
                          << "  dtim_period=" << int(res.ap_DTIMPeriod) << std::endl
                          << "  channel_utilization=" << int(res.ap_ChannelUtilization) << std::endl
                          << "  station_count=" << int(res.ap_StationCount) << std::endl;
            }
        }
    }
    printBmlReturnVals("bml_get_dcs_scan_results", ret);

    return 0;
}

/**
 * get all client list.
 *
 * @return 0 on success.
 */
int cli_bml::client_get_client_list()
{
    char client_list[256]         = {0};
    unsigned int client_list_size = 256;

    int ret = bml_client_get_client_list(ctx, client_list, &client_list_size);
    auto client_list_vec =
        beerocks::string_utils::str_split(std::string(client_list, client_list_size), ',');

    std::cout << "client list:" << std::endl;
    for (const auto &client : client_list_vec) {
        std::cout << "- " << client << std::endl;
    }

    printBmlReturnVals("bml_client_get_client_list", ret);

    return 0;
}

/**
 * Set specific client according to MAC.
 *
 * @param [in] sta_mac MAC address of requested client.
 * @param [in] selected_bands comma-seperated selected bands.
 * @param [in] stay_on_initial_radio Whather to stay on initial radio or not.
 * @param [in] time_life_delay_minutes the period of time after which the client is cleared,
.
 * @return 0 on success.
 */
int cli_bml::client_set_client(const std::string &sta_mac, int8_t selected_bands,
                               int8_t stay_on_initial_radio, int32_t time_life_delay_minutes)
{
    std::cout << "client_set_client: " << std::endl
              << "  sta_mac: " << sta_mac << std::endl
              << "  selected_bands: " << int(selected_bands) << std::endl
              << "  stay_on_initial_radio: " << int(stay_on_initial_radio) << std::endl
              << "  time_life_delay_minutes: " << int(time_life_delay_minutes) << std::endl;

    BML_CLIENT_CONFIG cfg{
        .stay_on_initial_radio   = stay_on_initial_radio,
        .selected_bands          = selected_bands,
        .time_life_delay_minutes = time_life_delay_minutes,
    };

    int ret = bml_client_set_client(ctx, sta_mac.c_str(), &cfg);

    printBmlReturnVals("bml_client_set_client", ret);

    return 0;
}

/**
 * get specific client according to MAC.
 *
 * @param [in] sta_mac MAC address of requested client
 *
 * @return 0 on success.
 */
int cli_bml::client_get_client(const std::string &sta_mac)
{
    BML_CLIENT client;
    int ret = bml_client_get_client(ctx, sta_mac.c_str(), &client);

    if (ret == BML_RET_OK) {
        auto client_bool_print = [](int8_t val) -> std::string {
            if (val == BML_PARAMETER_NOT_CONFIGURED)
                return "Not configured";
            return val ? "True" : "False";
        };
        auto client_selected_bands_print = [](int8_t val) -> std::string {
            std::string ret = "";
            if (val == BML_CLIENT_SELECTED_BANDS_DISABLED)
                return "Disabled";
            if (val & BML_CLIENT_SELECTED_BANDS_24G)
                ret += "2.4GHz,";
            if (val & BML_CLIENT_SELECTED_BANDS_5G)
                ret += "5GHz,";
            if (val & BML_CLIENT_SELECTED_BANDS_6G)
                ret += "6GHz,";

            // remove ending comma
            if (!ret.empty() && (ret.back() == ',')) {
                ret.pop_back();
            }

            return ret;
        };
        auto print_configured_mac = [](uint8_t *mac) -> std::string {
            uint8_t ZERO_MAC[BML_MAC_ADDR_LEN] = {0};
            if (std::equal(mac, mac + BML_MAC_ADDR_LEN, ZERO_MAC)) {
                return "Not configured";
            }
            return tlvf::mac_to_string(mac);
        };

        std::cout << "client: " << tlvf::mac_to_string(client.sta_mac) << std::endl
                  << " timestamp_sec: " << client.timestamp_sec << std::endl
                  << " stay_on_initial_radio: " << client_bool_print(client.stay_on_initial_radio)
                  << std::endl
                  << " initial_radio: " << print_configured_mac(client.initial_radio) << std::endl
                  << " selected_bands: " << client_selected_bands_print(client.selected_bands)
                  << std::endl
                  << " single_band: " << client_bool_print(client.single_band) << std::endl;
        if (client.time_life_delay_minutes == 0) {
            std::cout << " time_life_delay_minutes: configured and not aging." << std::endl;
        } else if (client.time_life_delay_minutes == -1) {
            std::cout << " time_life_delay_minutes: unconfigured and aging." << std::endl;
        } else if (client.time_life_delay_minutes > 0) {
            std::cout << " time_life_delay_minutes: configured and aging." << std::endl
                      << client.time_life_delay_minutes / 60 << " hours, "
                      << client.time_life_delay_minutes % 60 << " minutes" << std::endl;
        }
    }

    printBmlReturnVals("bml_client_get_client", ret);

    return 0;
}

/**
 * @brief clear persistent DB information for the specified client.
 *
 * @param [in] sta_mac MAC address of requested client.
 * @return 0 on success, -1 on failure.
 */
int cli_bml::client_clear_client(const std::string &sta_mac)
{
    int ret = bml_client_clear_client(ctx, sta_mac.c_str());

    if (ret == BML_RET_OK) {
        std::cout << "Client " << sta_mac
                  << " has been removed from the persistent DB, successfully" << std::endl;
    } else {
        std::cout << "Failed to remove client persistent DB info for mac=" << sta_mac;
    }

    printBmlReturnVals("bml_client_clear_client", ret);

    return ret;
}

template <typename T> const std::string cli_bml::string_from_int_array(T *arr, size_t arr_max_size)
{
    std::stringstream ss;
    if (arr) {
        for (size_t i = 0; i < arr_max_size; i++) {
            ss << ((i != 0) ? ", " : "") << (unsigned int)(arr[i]);
        }
    }
    return ss.str();
}

int cli_bml::get_unassociated_station_stats_caller(int numOfArgs)
{
    if (numOfArgs != 0) {
        LOG(ERROR) << "wrong argument! needs no arguments";
        return -1;
    }
    return get_unassociated_stations_stats();
}

int cli_bml::get_unassociated_stations_stats()
{
    unsigned int number_characters_max                  = 3000;
    char un_station_stats_report[number_characters_max] = {
        0}; //random value that should be safe to use!
    unsigned int size_report = number_characters_max;
    if (bml_get_unassociated_station_stats(ctx, un_station_stats_report, &size_report) ==
        BML_RET_OK) {
        std::cout << un_station_stats_report;
        return BML_RET_OP_FAILED;
    } else {
        std::cout << "Error while getting the stats of unassociated stations!";
    }
    return BML_RET_OK;
}

int cli_bml::add_unassociated_station_stats_caller(int numOfArgs)
{
    if ((numOfArgs != 3) && (numOfArgs != 4)) {
        LOG(ERROR) << "wrong argument! needs 3 or 4 arguments: "
                      "mac_address,channel,operating_class and agent_mac_ad(optional) but "
                   << numOfArgs << " provided!";
        return -1;
    }
    if (!beerocks::net::network_utils::is_valid_mac(args.stringArgs[0])) {
        LOG(ERROR) << "entered station mac_addr " << args.stringArgs[0]
                   << " is not a valid mac_address!";
        return -1;
    }

    if (!std::all_of(args.stringArgs[1].begin(), args.stringArgs[1].end(), ::isdigit)) {
        LOG(ERROR) << args.stringArgs[1] << " is not a valid channel value! ";
        return -1;
    }
    if (!std::all_of(args.stringArgs[2].begin(), args.stringArgs[2].end(), ::isdigit)) {
        LOG(ERROR) << args.stringArgs[2] << " is not a valid operating_class value! ";
        return -1;
    }

    if (numOfArgs == 3) {
        return add_unassociated_station_stats(args.stringArgs[0], args.stringArgs[1],
                                              args.stringArgs[2], std::string());
    } else {
        if (!beerocks::net::network_utils::is_valid_mac(args.stringArgs[3])) {
            LOG(ERROR) << "entered agent mac_addr " << args.stringArgs[3]
                       << "is not a valid mac_address!";
            return -1;
        }
        return add_unassociated_station_stats(args.stringArgs[0], args.stringArgs[1],
                                              args.stringArgs[2], args.stringArgs[3]);
    }
}

int cli_bml::remove_unassociated_station_stats_caller(int numOfArgs)
{
    if ((numOfArgs != 1) && (numOfArgs != 2)) {
        LOG(ERROR)
            << "wrong argument! needs  1 or 2 arguments: mac_address and agent_mac_addr(optional)";
        return -1;
    }
    if (!beerocks::net::network_utils::is_valid_mac(args.stringArgs[0])) {
        LOG(ERROR) << "entered station mac_addr " << args.stringArgs[0]
                   << "is not a valid mac_address!";
        return -1;
    }
    if (numOfArgs == 2) {
        if (!beerocks::net::network_utils::is_valid_mac(args.stringArgs[1])) {
            LOG(ERROR) << "entered agent mac_addr " << args.stringArgs[1]
                       << "is not a valid mac_address!";
            return -1;
        }
        return remove_unassociated_station_stats(args.stringArgs[0], args.stringArgs[1]);
    } else {
        return remove_unassociated_station_stats(args.stringArgs[0], std::string());
    }
}

int cli_bml::add_unassociated_station_stats(const std::string &mac_address,
                                            const std::string &channel,
                                            const std::string &operating_class,
                                            const std::string &agent_mac_addr)
{
    return bml_add_unassociated_station_stats(
        ctx, mac_address.c_str(), channel.c_str(), operating_class.c_str(),
        agent_mac_addr.empty() ? nullptr : agent_mac_addr.c_str());
}

int cli_bml::remove_unassociated_station_stats(std::string &mac_address,
                                               const std::string &agent_mac_addr)
{
    return bml_remove_unassociated_station_stats(
        ctx, mac_address.c_str(), agent_mac_addr.empty() ? nullptr : agent_mac_addr.c_str());
}
