/* SPDX-License-Identifier: BSD-2-Clause-Patent
*
* SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
*
* This code is subject to the terms of the BSD+Patent license.
* See LICENSE file for more details.
*/

#include "service_prio_utils_osp_mxl.h"

#include <bcl/beerocks_event_loop_impl.h>
#include <bcl/beerocks_os_utils.h>
#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>

namespace beerocks {
namespace bpl {

void create_custom_ebtable_chains(const std::string &prerouting_chain_name,
                                  const std::string &postrouting_chain_name)
{
    // create custom chains
    std::string cmd_preset = "ebtables -t nat -N ";
    std::string cmd        = cmd_preset + prerouting_chain_name;
    beerocks::os_utils::system_call(cmd);
    cmd = cmd_preset + postrouting_chain_name;
    beerocks::os_utils::system_call(cmd);

    // Append custom chains in PREROUTING and POSTROUTING NAT tables
    cmd_preset = "ebtables -t nat -A ";
    cmd        = cmd_preset + "PREROUTING -j " + prerouting_chain_name;
    beerocks::os_utils::system_call(cmd);
    cmd = cmd_preset + "POSTROUTING -j " + postrouting_chain_name;
    beerocks::os_utils::system_call(cmd);
}

void apply_ebtables_rules(const std::string &prerouting_chain_name,
                          const std::string &postrouting_chain_name, const std::string &iface_name,
                          ServicePrioritizationUtils::ePortMode tag_mode, uint8_t default_pcp)
{
    std::string cmd_preset = "ebtables -t nat -A ";
    std::string cmd;
    LOG(DEBUG) << "Apply rules for " << iface_name;
    cmd.reserve(200);

    /* Below are the example commands for different tagged modes
TAGGED_PORT_PRIMARY_TAGGED
	Ingress
	ebtables -t nat -A service_prio_in -i sw_1 -p 802_1Q -j --set-pcp-prio
	Egress
	ebtables -t nat -A service_prio_out -o sw_1 -p 802_1Q -j prio --set-prio-pcp

TAGGED_PORT_PRIMARY_UNTAGGED
	Ingress
	ebtables -t nat -A service_prio_in -i wlan0.0 -j prio --set-prio 0
	ebtables -t nat -A service_prio_in -i wlan0.0 -p IPv4 -j prio --set-dscp-prio

UNTAGGED_PORT
	Ingress
	ebtables -t nat -A service_prio_in -i wlan0.0 -j prio --set-prio 0
	ebtables -t nat -A service_prio_in -i wlan0.0 -p IPv4 -j prio --set-dscp-prio
*/

    if (tag_mode == ServicePrioritizationUtils::ePortMode::TAGGED_PORT_PRIMARY_TAGGED) {
        //ingress rule
        cmd = cmd_preset + prerouting_chain_name + " -i " + iface_name +
              "  -p 802_1Q -j --set-pcp-prio";
        beerocks::os_utils::system_call(cmd);

        //egress rule
        cmd = cmd_preset + postrouting_chain_name + " -o " + iface_name +
              " -p 802_1Q -j prio --set-prio-pcp";
        beerocks::os_utils::system_call(cmd);
    } else if ((tag_mode == ServicePrioritizationUtils::ePortMode::TAGGED_PORT_PRIMARY_UNTAGGED) ||
               (tag_mode == ServicePrioritizationUtils::ePortMode::UNTAGGED_PORT)) {
        //ingress rules
        cmd = cmd_preset + prerouting_chain_name + " -i " + iface_name + " -j prio --set-prio " +
              std::to_string(default_pcp);

        beerocks::os_utils::system_call(cmd);

        cmd = cmd_preset + prerouting_chain_name + " -i " + iface_name +
              " -p IPv4 -j prio --set-dscp-prio";
        beerocks::os_utils::system_call(cmd);
    }
}

void remove_ebtables_rules(const std::string &custom_chain_name, bool flush_rule, bool delete_rule,
                           bool delete_chain, enum routing_direction route)
{
    std::string cmd;

    LOG(DEBUG) << "Remove " << custom_chain_name << " rules";
    cmd.reserve(150);
    if (flush_rule) {
        cmd.assign("ebtables -t nat ").append("-F ");
        cmd.append(custom_chain_name);
        beerocks::os_utils::system_call(cmd);
    }
    if (delete_rule) {
        cmd.assign("ebtables -t nat ").append("-D ");
        if (route == PREROUTING) {
            cmd.append("PREROUTING -j ");
        } else if (route == POSTROUTING) {
            cmd.append("POSTROUTING -j ");
        }
        cmd.append(custom_chain_name);
        beerocks::os_utils::system_call(cmd);
    }

    if (delete_chain) {
        cmd.assign("ebtables -t nat ").append("-X ");
        cmd.append(custom_chain_name);
        beerocks::os_utils::system_call(cmd);
    }
}

bool write_dscp_map_to_proc(ServicePrioritizationUtils::sDscpMap *map, const std::string &filename)
{
    std::ofstream dscp_proc_entry(filename);

    if (!dscp_proc_entry) {
        LOG(ERROR) << "unable to open DSCP proc file " << filename;
        return false;
    }

    for (int i = 0; i < DSCP_MAP_LENGTH; i++) {
        dscp_proc_entry << "dscp " << i << " " << std::to_string(map->dscp[i]) << std::endl;
    }

    dscp_proc_entry.close();

    return true;
}

bool ServicePrioritizationUtils_osp_mxl::flush_rules()
{
    LOG(DEBUG) << "Flushing ebtable rules";
    remove_ebtables_rules(PREROUTING_CHAIN_NAME, true, true, true, PREROUTING);
    remove_ebtables_rules(POSTROUTING_CHAIN_NAME, true, true, true, POSTROUTING);
    return true;
}

bool ServicePrioritizationUtils_osp_mxl::apply_single_value_map(
    std::list<sInterfaceTagInfo> *iface_list, uint8_t pcp)
{
    LOG(ERROR) << __func__ << ": not Supported in OSP";
    return false;
}

bool ServicePrioritizationUtils_osp_mxl::apply_dscp_map(std::list<sInterfaceTagInfo> *iface_list,
                                                        sDscpMap *map, uint8_t default_pcp)
{
    LOG(DEBUG) << "Applying Ebtables";

    if (map == NULL) {
        LOG(ERROR) << "DSCP map value is null";
        return false;
    }

    LOG(DEBUG) << "Interface details are";
    for (auto itr = iface_list->rbegin(); itr != iface_list->rend(); itr++) {
        LOG(DEBUG) << "Iface name = " << itr->iface_name << ", type = " << itr->tag_info;
    }

    create_custom_ebtable_chains(PREROUTING_CHAIN_NAME, POSTROUTING_CHAIN_NAME);

    for (auto itr = iface_list->rbegin(); itr != iface_list->rend(); itr++) {
        apply_ebtables_rules(PREROUTING_CHAIN_NAME, POSTROUTING_CHAIN_NAME, itr->iface_name,
                             itr->tag_info, default_pcp);
    }

    if (!write_dscp_map_to_proc(map, dscp_proc_file_name)) {
        LOG(ERROR) << "DSCP proc writing failed";
        return false;
    }

    return true;
}

bool ServicePrioritizationUtils_osp_mxl::apply_up_map(std::list<sInterfaceTagInfo> *iface_list,
                                                      uint8_t default_pcp)
{
    LOG(ERROR) << __func__ << ": not Supported in OSP";
    return false;
}

std::shared_ptr<ServicePrioritizationUtils> register_service_prio_utils()
{
    return std::make_shared<bpl::ServicePrioritizationUtils_osp_mxl>();
}

} // namespace bpl

} // namespace beerocks
