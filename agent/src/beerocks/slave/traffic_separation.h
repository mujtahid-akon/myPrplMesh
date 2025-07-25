/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TRAFFIC_SEPARATION_H
#define _TRAFFIC_SEPARATION_H

#include <bcl/network/net_struct.h>
#include <btl/broker_client.h>

namespace beerocks {
namespace net {

/*
 * When defining the bridge with "vlan_filtering", the bridge becomes VLAN aware.
 * If the bridge is VLAN aware, then it is possible to define VLAN policy to each bridged interface.
 * A single interface can support multiple VLAN policies, one for each VLAN ID allowed on that
 * interface.
 * Two VLAN policies that can be defined on an interface:
 * 1. "PVID" -              Any untagged frames coming from the network (external to the switch)
 *                          will be assigned to the configured VLAN.
 *                          Only one VID can be defined as PVID, therefore, if changing the VLAN ID
 *                          with the PVID policy, it will remove the PVID policy of the previous
 *                          PVID VLAN automatically.
 * 
 * 2. "Egress Untagged" -   Any Egress frame sent to the network, will be stripped from the
 *                          configured VLAN tag.
 * 
 * If a frame with a VLAN tag is entering an interface, but the interface is not assigned to the
 * VLAN ID of the frame, the frame will be dropped.
 * 
 * An interface can be defined either as "Tagged Port" or "Untagged Port".
 * Tagged Port -    A port is a "Tagged Port" when an interface is expecting frames containing VLAN
 *                  tags. An example of this is when two switches are connected and pass tagged
 *                  traffic. The sender will send a frame with a VLAN tag. The receiving switch will
 *                  see the VLAN tags, and if the VLAN is allowed, it will forward the frame as
 *                  required.
 *                  
 *                  Although a Tagged Port should receive tagged frames, it can also receive
 *                  untagged frames. When a Tagged Port receives untagged frames, it assumes they
 *                  belong to the Primary VLAN, so it will add the Primary VLAN ID to these frames. 
 * 
 *                  A tagged port shall be defined as "PVID, Untagged Egress" on the Primary VLAN of
 *                  the network and add all the secondary VLANs (Not "PVID" and not
 *                  "Egress Untagged").
 * 
 * Untagged Port -  An untagged port, connects to network clients. A network client is unaware of
 *                  any VLAN configuration. A connected client sends its traffic without any VLAN
 *                  tag on the frames.
 *                  When the frame reaches the Untagged port on a switch, the switch will add the
 *                  VLAN tag. The switch port is configured with a VLAN ID that it will put into the
 *                  tag.
 *                  When a frame leaves an untagged port, the switch strips the VLAN tag from the
 *                  frame.
 * 
 *                  An Untagged Port shall be defined as "PVID, Untagged Egress" on as single VLAN
 *                  ID - The Primary VLAN or one of the Secondary VLANs.
 */

class TrafficSeparation final {

public:
    explicit TrafficSeparation(std::shared_ptr<btl::BrokerClient> broker_client);

    /**
     * @brief Apply traffic separation policy on the given radio interfaces, and all not
     * radio related interface (e.g bridge, LAN ports).
     * 
     * @param radio_iface Radio interface to apply VLAN policy on. If not given, apply the policy
     * only on the bridge, ethernet ports and the wireless backhaul interface.
     */
    void apply_policy(const std::string &radio_iface = {});

    /**
     * @brief Apply traffic separation rules on the new interface that are created when a
     * newly onboarding agent associates with the BH VAP.
     */
    void apply_policy_for_new_interface(const std::string &bss_iface);

    /**
     * @brief Clear the traffic separation configuration from the Agent and the platform.
     * 
     */
    void clear_configuration();

    /**
     * @brief This variable is a workaround that turn on the profile_x_disallow flag when
     * configuring the traffic separation, if the Controller sent unsupported configuration
     * on the profile disallow flags.
     * 
     * @details It is possible that the Controller will configure bBSS in a way we don't support,
     * i.e a bBSS that allows connection of profile1 and profile2 Agents.
     * When it happens during certification, we can't configure the bBSS correctly in terms of
     * traffic separation.
     * if m_profile_x_disallow_override_unsupported_configuration == 0 -> Do nothing, no effect.
     * 
     * if m_profile_x_disallow_override_unsupported_configuration == 1 -> 
     *  The backhaul_bss_disallow_profile1_agent_association will be overriden to 'true', when the
     *  profile disallow flags sent by the Controller is unsupported.
     * 
     * if m_profile_x_disallow_override_unsupported_configuration == 2 -> 
     *  The backhaul_bss_disallow_profile2_agent_association will be overriden to 'true', when the
     *  profile disallow flags sent by the Controller is unsupported.
     */
    static int m_profile_x_disallow_override_unsupported_configuration;

private:
    enum class ePortMode {
        UNTAGGED_PORT,
        TAGGED_PORT_PRIMARY_UNTAGGED,
        TAGGED_PORT_PRIMARY_TAGGED
    };

    struct sBridgeVlanInfo {
        explicit sBridgeVlanInfo(const std::string &iface_name_,
                                 const net::sIpv4Addr &subnet_ipv4_ = {},
                                 const std::string &subnetmask_     = std::string())
            : iface_name(iface_name_), subnet_ipv4(subnet_ipv4_), subnetmask(subnetmask_)
        {
        }
        std::string iface_name;
        net::sIpv4Addr subnet_ipv4;
        std::string subnetmask;
    };

    /**
     * @brief Set the VLAN policy on a given @a 'iface'.
     * 
     * @param iface An interface to set the VLAN policy on.
     * @param port_mode Indicate what is the desired port policy. 
     * @param is_bridge Should be set to true if the @a 'port_mode' is 
     * @b 'TAGGED_PORT_PRIMARY_UNTAGGED' or @b 'TAGGED_PORT_PRIMARY_TAGGED' and the given @a 'iface'
     * is a bridge interface.
     * @param untagged_port_vid The interface VID when @a 'port_mode' is @b 'UNTAGGED_PORT'. 
     */
    void set_vlan_policy(const std::string &iface, ePortMode port_mode, bool is_bridge,
                         uint16_t untagged_port_vid = 0);

    /**
     * @brief Reconfigure DHCP server with list of interfaces.
     * 
     * @details This function should be used only on the GW.
     * 
     * @param vlans_of_bridge List of VLANs of the bridge information. 
     * @return true on success, false otherwise.
     */
    bool reconf_dhcp(std::list<sBridgeVlanInfo> &vlans_of_bridge);

    /**
     * @brief Send DHCP request on each VLAN of the bridge and assigning the responded IP to the
     * VLAN interface.
     * 
     * @details This function should be used only on the Repeater.
     * 
     * @param vlans_of_bridge List of VLANs of the bridge information. 
     */
    void assign_ip_to_vlan_iface(const std::list<sBridgeVlanInfo> &vlans_of_bridge);

    /**
     * @brief Configure interface on the Transport.
     *
     * @param iface Interface to configure.
     * @param add true for adding interface, false to remove.
     * @param bridge Bridge name if the interface is inside a bridge, otherwise should be empty.
     */
    void configure_transport(const std::string &iface, bool add, const std::string &bridge);

    /**
     * Broker client to exchange CMDU messages with broker server running in transport process.
     */
    std::shared_ptr<btl::BrokerClient> m_broker_client;
};
} // namespace net
} // namespace beerocks

#endif
