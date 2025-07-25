/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee1905_transport.h"
#include <tlvf/ieee_1905_1/eMessageType.h>

#include <arpa/inet.h>
#include <chrono>

#define ETHER_IS_SAME(addr1, addr2) (memcmp((addr1), (addr2), ETH_ALEN) == 0)

namespace beerocks {
namespace transport {

// Use transport messaging classes
using namespace beerocks::transport::messages;

static const uint8_t ieee1905_max_message_version = 0x00;
constexpr sMacAddr Ieee1905Transport::ieee1905_multicast_addr;
const std::set<uint16_t> Ieee1905Transport::Packet::reliable_multicast_msg_types = {
    static_cast<uint16_t>(ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE),
    static_cast<uint16_t>(ieee1905_1::eMessageType::ASSOCIATION_STATUS_NOTIFICATION_MESSAGE)};

void Ieee1905Transport::update_neighbours(const Packet &packet)
{
    if (packet.ether_type != ETH_P_1905_1) {
        MAPF_WARN("Ignoring non-1905 packet");
        return;
    }
    if (packet.src_if_type != CmduRxMessage::IF_TYPE_NET) {
        MAPF_DBG("Ignoring packet from src_if_type " << int(packet.src_if_type));
        return;
    }

    auto now = std::chrono::steady_clock::now();

    // Start iterating on the neighbors map to delete aged neighbors if more than the time it will
    // take for the most aged neighbor to expire and non less than 1 seconds has passed since last
    // time entered here.
    if (now > removed_aged_neighbors_timeout_) {
        auto oldest_neighbor_wait_sec = kMaximumNeighbourAge;
        for (auto neigh = neighbors_map_.begin(); neigh != neighbors_map_.end();) {
            if (neigh->first != packet.src) {
                auto age =
                    std::chrono::duration_cast<std::chrono::seconds>(now - neigh->second.last_seen);
                if (age > kMaximumNeighbourAge) {
                    MAPF_INFO("Deleting aged out neighbor with AL MAC " << neigh->first);
                    neigh = neighbors_map_.erase(neigh);
                    continue;
                }
                auto diff_sec            = kMaximumNeighbourAge - age;
                oldest_neighbor_wait_sec = std::min(oldest_neighbor_wait_sec, diff_sec);
            }
            ++neigh;
        }
        constexpr uint8_t REMOVE_AGED_NEIGHBORS_CYCLES_SEC = 1;
        removed_aged_neighbors_timeout_ =
            now + std::max(oldest_neighbor_wait_sec,
                           std::chrono::seconds(REMOVE_AGED_NEIGHBORS_CYCLES_SEC));
    }

    // Add new neighbors
    auto neighbor_iter = neighbors_map_.find(packet.src);
    if (neighbor_iter == neighbors_map_.end()) {
        MAPF_INFO("Adding new neighbor with AL MAC " << packet.src);
        neighbors_map_.emplace(packet.src, ieee1905_neighbor{packet.src, packet.src_if_index, now});
    } else {
        // Update last seen for new / updated neighbors
        MAPF_DBG("Updating last seen for neighbor with AL MAC " << packet.src);
        neighbor_iter->second.last_seen = now;
        neighbor_iter->second.if_index  = packet.src_if_index;
    }
}

void Ieee1905Transport::handle_packet(Packet &packet)
{
    if (!verify_packet(packet)) {
        // Silently drop packets that failed verification
        return;
    }

    MAPF_DBG("handling packet:" << std::endl << packet);

    if (packet.ether_type != ETH_P_1905_1) {
        // Silently ignore non IEEE 1905.1 packets
        return;
    }

    update_neighbours(packet);

    if (!de_duplicate_packet(packet)) {
        MAPF_DBG("packet is duplicate (will not be forwarded)");
        return;
    }

    if (!forward_packet(packet)) {
        MAPF_ERR("packet forwarding failed.");
        remove_packet_from_de_duplication_map(packet);
    }
}

// do some basic sanity checking on the packet
bool Ieee1905Transport::verify_packet(const Packet &packet)
{
    if (packet.ether_type == ETH_P_1905_1) {
        // verify minimum packet length (should at least contain an IEEE1905 header + End of Message TLV)
        if (packet.payload.iov_len < sizeof(Ieee1905CmduHeader) + 3) {
            MAPF_DBG("packet verification failed - IEEE1905 packet is too short: "
                     << (unsigned)packet.payload.iov_len << " bytes");

            return false;
        }

        Ieee1905CmduHeader *ch = reinterpret_cast<Ieee1905CmduHeader *>(packet.payload.iov_base);
        if (ch->messageVersion > ieee1905_max_message_version) {
            MAPF_DBG("packet verification failed - unsupported IEEE1905 messageVersion: "
                     << (unsigned)ch->messageVersion);

            return false;
        }

        if (ch->_reservedField0 != 0) {
            // don't drop packet - just print a warning message
            MAPF_DBG("packet verification warning - non zero IEEE1905 reservedField");
        }

        if ((ch->flags & 0x3F) != 0) {
            // don't drop packet - just print a warning message
            MAPF_DBG("packet verification warning - non zero IEEE1905 reservedField (flags): 0x"
                     << std::hex << ch->flags);
        }
    } else if (packet.ether_type == ETH_P_LLDP) {
        // not much to verify at this layer...
        // verify minimum packet length (should at least contain a single empty TLV)
        if (packet.payload.iov_len < 3) {
            MAPF_DBG("packet verification failed - LLDP packet is too short: "
                     << (unsigned)packet.payload.iov_len << " bytes");

            return false;
        }
    } else {
        // this should never happen (due to packet filter)
        // This print is changed to debug in order to prevent log-flood on the platforms
        // TODO: change back to error-log-print once PPM-80 is merged
        MAPF_ERR("packet verification failed - unsupported etherType: 0x"
                 << std::hex << packet.ether_type << std::dec);

        return false;
    }

    return true;
}

void Ieee1905Transport::remove_packet_from_de_duplication_map(const Packet &packet)
{
    // search for entry matching packet
    DeDuplicationKey key;
    key.src         = packet.src;
    key.dst         = packet.dst;
    key.messageType = static_cast<Ieee1905CmduHeader *>(packet.payload.iov_base)->messageType;
    key.messageId   = static_cast<Ieee1905CmduHeader *>(packet.payload.iov_base)->messageId;
    key.fragmentId  = static_cast<Ieee1905CmduHeader *>(packet.payload.iov_base)->fragmentId;

    auto it = de_duplication_map_.find(key);
    if (it != de_duplication_map_.end()) {
        MAPF_DBG("Removing packet from de-duplication map:" << std::endl << packet);
        de_duplication_map_.erase(it);
    }
}

// Check if the specified packet is a duplicate (based on src address and messageId)
//
// return false iff the packet is duplicate
//
// A note about de-duplication: messageId's of received packets (for a specific source address) are a monotonously
// rising number - that is true, at least within a reasonable timeout. - i.e. they can go back upon wrap-around
// event or when a device reboots. Anyway we can safely assume that if two packets, with the same messageId, are
// received from a specific station are not a duplicate if some minimal duration of time has passed between them.
// This duration should be long enough to allow for any buffered/in-transit duplicate packets to be flushed; and
// should be short enough to handle the case when a device reboots (and reuses the same messageId).
//
// Since de-duplication happens before relaying - multicast duplicate packets will not be relayed. (duplicate
// unicast packets for other devices will be forwarded by the bridge interface).
//
bool Ieee1905Transport::de_duplicate_packet(Packet &packet)
{
    // only try to detect duplicate IEEE1905 packets
    if (packet.ether_type != ETH_P_1905_1) {
        MAPF_WARN("Ignoring non-1905 packet");
        return true;
    }

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    // de-duplication map aging - go over all entries and remove old ones
    for (auto it = de_duplication_map_.begin(); it != de_duplication_map_.end();) {
        auto &dupval = it->second;

        if (now > dupval.time + kMaximumDeDuplicationAge) {
            it = de_duplication_map_.erase(it);
        } else {
            ++it;
        }
    }

    // search for entry matching packet
    DeDuplicationKey key;
    key.src         = packet.src;
    key.dst         = packet.dst;
    key.messageType = static_cast<Ieee1905CmduHeader *>(packet.payload.iov_base)->messageType;
    key.messageId   = static_cast<Ieee1905CmduHeader *>(packet.payload.iov_base)->messageId;
    key.fragmentId  = static_cast<Ieee1905CmduHeader *>(packet.payload.iov_base)->fragmentId;

    auto it           = de_duplication_map_.find(key);
    bool is_duplicate = (it != de_duplication_map_.end());

    // On reliable multicast messages, we receive the same message twice, one with multicast
    // destination address, and a second one with a unicast destination address.
    // If we receive one of them first, treat the other one as a duplicate.
    if (!is_duplicate && packet.src_if_type == CmduRxMessage::IF_TYPE_NET &&
        packet.is_reliable_multicast()) {
        // If the destination address is al_mac, replace with multicast address
        if (ETHER_IS_SAME(al_mac_addr_, key.dst.oct)) {
            key.dst = Ieee1905Transport::ieee1905_multicast_addr;
        }
        // If the destination address is multicast, replace with al_mac address
        else if (ETHER_IS_SAME(Ieee1905Transport::ieee1905_multicast_addr.oct, key.dst.oct)) {
            memcpy(key.dst.oct, al_mac_addr_, ETH_ALEN);
        }
        it           = de_duplication_map_.find(key);
        is_duplicate = (it != de_duplication_map_.end());
    }

    if (is_duplicate) {
        // this is a duplicate packet - update timestamp
        counters_[CounterId::DUPLICATE_PACKETS]++;
        auto &val = it->second;
        val.time  = now;
    } else if (int(de_duplication_map_.size()) >= kMaximumDeDuplicationThreads) {
        // this is not really a duplicate but we cannot track it so it will be dropped now
        MAPF_WARN("too many de-duplication threads - dropping packet as duplicate");
        is_duplicate = true;
    } else {
        // this is not a duplicate packet - add new entry to the de-duplication map
        DeDuplicationValue val;

        val.time                 = now;
        de_duplication_map_[key] = val;
    }

    return !is_duplicate;
}

// De-fragmentation (reassembly) of received packets will be done before publishing a CMDU to the broker (defragmentation is
// not required for tunneling and relayed packets).
//
// De-fragmentation could fail if one or more fragments are never received - thus the fragments of a CMDU should be
// collected and buffered until either the complete CMDU is available or a certain timeout has elapsed
// (IEEE1905 does not specify the duration of this timeout).
//
// The current implementation does not support out-of-order arrival of fragments.
//
// see paragraph 7.1.2 of IEEE1905.1-2013
bool Ieee1905Transport::de_fragment_packet(Packet &packet)
{
    // only try to de-fragment IEEE1905 packets
    if (packet.ether_type != ETH_P_1905_1) {
        return true;
    }

    Ieee1905CmduHeader *ch = reinterpret_cast<Ieee1905CmduHeader *>(packet.payload.iov_base);
    if (ch->fragmentId == 0 && ch->GetLastFragmentIndicator() == 1) {
        return true; // not a fragmented CMDU.
    }

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    // de-fragmentation map cleanup - go over all entries and remove old ones or complete ones
    for (auto it = de_fragmentation_map_.begin(); it != de_fragmentation_map_.end();) {
        auto &val = it->second;

        if (val.complete) {
            it = de_fragmentation_map_.erase(it);
        } else if (now > val.time + kMaximumDeFragmentationAge) {
            // Note: this is not necessarily related to the current fragment referenced by the argument to this method
            MAPF_DBG("defragmentation timeout - dropping a packet from defragmentation buffer");
            it = de_fragmentation_map_.erase(it);
            counters_[CounterId::DEFRAGMENTATION_FAILURE]++;
        } else {
            ++it;
        }
    }

    // build the key to the de-fragmentation map
    DeFragmentationKey key;
    key.src         = packet.src;
    key.messageType = ch->messageType;
    key.messageId   = ch->messageId;

    auto &val = de_fragmentation_map_[key]; // find a match (or create a new entry)

    // limit the map size (to prevent a possible memory exhaustion attack)
    // Note: at this stage there could be one extra entry in the map (this is still effective and the code is simple)
    if (int(de_fragmentation_map_.size()) > kMaximumDeFragmentationThreads) {
        de_fragmentation_map_.erase(key);
        MAPF_WARN("too many de-fragmentation threads - dropping packet");
        return false;
    }

    val.time = now;

    // only accept fragments in order - check if this is the expected fragment
    if (val.numFragments != ch->fragmentId) {
        MAPF_DBG("dropping an out-of-order fragment");
        return false;
    }

    // copy the IEEE1905 header from the first fragment
    if (ch->fragmentId == 0) {
        std::copy_n((uint8_t *)packet.payload.iov_base, sizeof(Ieee1905CmduHeader),
                    (uint8_t *)val.buf);
        val.bufIndex = sizeof(Ieee1905CmduHeader);

        // set the last fragment indicator flag as this will be the header of a complete CMDU
        Ieee1905CmduHeader *hdr = reinterpret_cast<Ieee1905CmduHeader *>(val.buf);
        hdr->SetLastFragmentIndicator(1);
    }

    // concat the fragment body (excluding the IEEE1905 header)
    size_t fragmentTlvsLength = packet.payload.iov_len - sizeof(Ieee1905CmduHeader);
    if (val.bufIndex + fragmentTlvsLength >= kMaximumDeFragmentionSize) {
        MAPF_WARN("defragmentation buffer overflow - dropping fragment");
        return false;
    }

    // Only count end of message TLV for the last fragment
    if (ch->GetLastFragmentIndicator() == 0) {
        Tlv *tlv          = reinterpret_cast<Tlv *>((uint8_t *)packet.payload.iov_base +
                                           sizeof(Ieee1905CmduHeader));
        size_t tlvsLength = fragmentTlvsLength;

        // Find EOM TLV in the fragment
        while (tlv->type() != 0) {
            if (tlv->size() > tlvsLength) {
                // This can only happen if one of the TLVs is corrupt
                MAPF_ERR("Found corrupt TLV! Dropping the fragment.");
                return false;
            }

            tlvsLength -= tlv->size();

            if (tlvsLength < sizeof(Tlv)) {
                // No EOM TLV or some padding. Good.
                break;
            }

            tlv = tlv->next();
        }

        // Remove any leftover at the end of the buffer.
        // It might be as a result of padding or EOM TLV.
        fragmentTlvsLength -= tlvsLength;
    }

    std::copy_n((uint8_t *)packet.payload.iov_base + sizeof(Ieee1905CmduHeader), fragmentTlvsLength,
                val.buf + val.bufIndex);
    val.bufIndex += fragmentTlvsLength;
    val.numFragments++;

    // detect defragmentation completion and update Packet object
    if (ch->GetLastFragmentIndicator()) {
        val.complete =
            true; // mark as complete - the map entry will be deleted on the next invocation
        packet.payload.iov_base = val.buf; // buffer is valid until next invocation of this method
        packet.payload.iov_len  = val.bufIndex;

        return true;
    }

    MAPF_DBG("buffering a fragment of an incomplete CMDU");
    return false;
}

// When an IEEE1905 packet (CMDU) is larger than a standard defined threshold (1500 bytes) it should be
// fragmented into smaller than 1500 bytes fragments.
//
// Fragmentation should be done on TLV boundaries.
//
// The CMDU length is considered starting from the IEEE1905 header (it does not include the Ethernet header)
//
// Fragmentation will only be done when transmitting a packet to the network (and not for the tunneled
// alternative transport interface).
//
// see paragraph 7.1.1 of IEEE1905.1-2013
bool Ieee1905Transport::fragment_and_send_packet_to_network_interface(unsigned int if_index,
                                                                      Packet &packet)
{
    // only fragment IEEE1905 packets longer than the threashold, that originate from the local device
    LOG(DEBUG) << "send_packet_to_network_interface: packet.ether_type=" << std::hex
               << packet.ether_type << std::dec
               << ", packet.src_if_type=" << int(packet.src_if_type);
    if (packet.ether_type != ETH_P_1905_1 ||
        packet.payload.iov_len <= kIeee1905FragmentationThreashold + sizeof(Ieee1905CmduHeader) ||
        packet.src_if_type != CmduRxMessage::IF_TYPE_LOCAL_BUS) {
        return send_packet_to_network_interface(if_index, packet);
    }

    // reuse most of the original packet metadata for all fragments (except for the payload iov)
    Packet fragment_packet                                                     = packet;
    uint8_t buf[kIeee1905FragmentationThreashold + sizeof(Ieee1905CmduHeader)] = {
        0}; // fragment size + TLV end of message size

    // copy the IEEE1905 header (will be reused by all fragments)
    std::copy_n((uint8_t *)packet.payload.iov_base, sizeof(Ieee1905CmduHeader), buf);
    int remainingPacketLength = packet.payload.iov_len - sizeof(Ieee1905CmduHeader);

    // points to the first TLV in the currently built fragment
    Tlv *firstTlvInFragment =
        (Tlv *)((uint8_t *)packet.payload.iov_base + sizeof(Ieee1905CmduHeader));

    for (int fragmentId = 0; fragmentId < 256 && remainingPacketLength > 0; fragmentId++) {
        // search for the optimal TLV boundary for fragmentation
        // include as many TLVs as can fit into the maximum fragment size (threashold).
        int fragmentTlvsLength = 0;
        Tlv *nextTlv           = firstTlvInFragment;
        while (1) {
            if (remainingPacketLength == 0) {
                // we are all done
                break;
            }
            if (remainingPacketLength < int(sizeof(Tlv))) {
                // packet has a few padding bytes
                MAPF_WARN("bad packet format - extra bytes at end of message.");
                return false;
            }
            if (nextTlv->type() == 0 && remainingPacketLength > int(nextTlv->size())) {
                MAPF_WARN("bad packet format - extra bytes after end of message TLV. "
                          << remainingPacketLength);
                return false;
            }
            if (nextTlv->size() > kIeee1905FragmentationThreashold) {
                // this TLV is too large to fit in any fragment
                MAPF_WARN("bad packet format - oversized TLV found.");
                return false;
            }
            if (int(nextTlv->size()) > remainingPacketLength) {
                MAPF_WARN("bad packet format - TLV exceeds packet bounds.");
                return false;
            }
            if (fragmentTlvsLength + nextTlv->size() >= kIeee1905FragmentationThreashold) {
                // including the next TLV will go over the threashold - let's fragment at this TLV's boundary
                break;
            }
            fragmentTlvsLength += nextTlv->size();
            remainingPacketLength -= nextTlv->size();
            nextTlv = nextTlv->next();
        }
        if (fragmentId == 256) {
            MAPF_WARN("bad packet format - packet is too long.");
            return false;
        }

        // copy the TLVs into the fragment packet buffer
        std::copy_n((uint8_t *)firstTlvInFragment, fragmentTlvsLength,
                    buf + sizeof(Ieee1905CmduHeader));

        // update IEEE1905 header (fragmentId and lastFragmentIndicator)
        Ieee1905CmduHeader *hdr = reinterpret_cast<Ieee1905CmduHeader *>(buf);
        hdr->fragmentId         = fragmentId;
        hdr->SetLastFragmentIndicator(remainingPacketLength == 0 ? 1 : 0);

        fragment_packet.payload.iov_base = buf;
        fragment_packet.payload.iov_len  = sizeof(Ieee1905CmduHeader) + fragmentTlvsLength;

        if (remainingPacketLength > 0 &&
            fragment_packet.payload.iov_len < kIeee1905FragmentationMinimum) {
            MAPF_WARN("Fragment is too small - possibly padded EOM!");
        }

        // send the fragment
        MAPF_DBG("sending fragment " << (int)fragmentId
                                     << " (length=" << fragment_packet.payload.iov_len << ").");
        if (!send_packet_to_network_interface(if_index, fragment_packet)) {
            return false;
        }

        firstTlvInFragment = nextTlv;
    }

    return true;
}

// IEEE1905 or Multi-AP Packet Forwarding Rules (if no interface is specified):
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Origin       Packet Type                   Forward To                                                      Comments
//                                            Local Bus   Bridge Interface    Individual Network Interfaces
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Local Bus    Multicast                     +           -                   + (all)
// Network      Multicast && relay==1         +           -                   + (all but originating intf)
// Network      Multicast && relay==0         +           -                   -
// Network      Unicast not to local device   -           -                   -                               will be forwarded by linux bridge interface
// Local Bus    Unicast not to local device   -           +                   -
// Any          Unicast to local device       +           -                   -                               send to bridge - packet will loopback to all interfaces ???
//
// In addition any (Multicast && relay==1 && msgType==0x0001) packet should be *first* sent as a unicast packet to each known neighbor
// (this is called "reliable multicast"). Such a packet should only be published once on the local bus (de duplicated). - we can use
// cmdu.tx for that (with original src address)
// ------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Futuristic Notes About an Alternative Transport:
//   - A "cmdu.tx" marked for alternative transport - will only go the alternative transport
//   - A received alternative transport packet will only be relayed as a "cmdu.rx" on the local bus.
//
// Notes:
//  - A packet is considered "unicast to local device" if its destination address is either the local AL MAC address or the receicing
//    local interface's MAC address.
//  - Unicast packets will be transmitted via the specified bridge interface, taking advantage of the standard linux bridge implementation
//    and learning algorithms (to decide on which interface(s) to send the packet on). This is also true for locally generated multicast
//    packets (which should flood all the bridges ports anyway).
//  - IEEE1905 multicast packet forwarding *should be disabled in the linux bridge* (using an ebtables rule).
//  - IEEE1905 multicast packet forwarding *should be disabled in any switching hardware acceleration engine* (if such exists).
//
// LLDP Packet Forwarding Rules (if no interface is specified):
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Origin       Packet Type                   Forward To                                                      Comments
//                                            Local Bus   Bridge Interface    Individual Network Interfaces
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Local Bus    Multicast                     +           -                   + (all)
// Network      Multicast                     +           -                   -                               will be forwarded by linux bridge interface
// ------------------------------------------------------------------------------------------------------------------------------------------------------
//
bool Ieee1905Transport::forward_packet_single(Packet &packet)
{
    if (packet.dst_if_type == CmduRxMessage::IF_TYPE_LOCAL_BUS) {
        MAPF_DBG("forwarding packet to broker.");
        if (!send_packet_to_broker(packet)) {
            return false;
        }
    } else if (packet.dst_if_type == CmduRxMessage::IF_TYPE_NET && packet.dst_if_index != 0) {
        std::string ifname = if_index2name(packet.dst_if_index);
        if (ifname.empty()) {
            MAPF_ERR("Couldn't find ifname for index " << packet.dst_if_index);
            return false;
        }
        MAPF_DBG("forwarding packet to interface " << ifname << ".");
        if (network_interfaces_.count(ifname) == 0) {
            MAPF_ERR("un-tracked interface " << ifname << ".");
            return false;
        }
        if (!fragment_and_send_packet_to_network_interface(packet.dst_if_index, packet)) {
            return false;
        }
    } else { // no dst interface is specified
        bool forward_to_broker             = false;
        bool forward_to_bridge             = false;
        bool forward_to_network_interfaces = false;

        //
        // Step 1: decide on forwarding sink(s)
        //
        if (ETHER_IS_MULTICAST(packet.dst.oct)) {
            forward_to_broker = true;

            // Note: IEEE1905 multicast packets have special treatment due to the conditional forwarding logic (based on the
            // relayIndicator field)
            if (packet.ether_type == ETH_P_1905_1) {
                // only forward locally generated packets or "relayed multicast" packets (IEEE1905 packets comming from the network
                // will be not be forwarded by the linux bridge due to an ebtables rule)
                if (packet.src_if_type == CmduRxMessage::IF_TYPE_LOCAL_BUS) {
                    forward_to_network_interfaces = true;
                }

                Ieee1905CmduHeader *ch =
                    reinterpret_cast<Ieee1905CmduHeader *>(packet.payload.iov_base);
                if (ch->GetRelayIndicator()) {
                    forward_to_network_interfaces = true;
                }
            } else {
                // only forward locally generated packets (packets comming from the network will be forwarded by the linux bridge)
                if (packet.src_if_type == CmduRxMessage::IF_TYPE_LOCAL_BUS) {
                    forward_to_network_interfaces = true;
                }
            }
        } else {
            // find if unicast address matches any of the local device's addresses
            // Note to developer: maybe it is possible to just send the packet to the bridge - will it then "come back" on the interfaces?
            bool addressed_to_this_device = false;
            if (ETHER_IS_SAME(al_mac_addr_, packet.dst.oct)) {
                addressed_to_this_device = true;
            } else {
                for (auto it = network_interfaces_.begin(); it != network_interfaces_.end(); ++it) {
                    auto &network_interface = it->second;

                    if (ETHER_IS_SAME(network_interface.addr, packet.dst.oct)) {
                        addressed_to_this_device = true;
                        break;
                    }
                }
            }
            if (addressed_to_this_device) {
                forward_to_broker = true;
            } else if (packet.src_if_type == CmduRxMessage::IF_TYPE_LOCAL_BUS) {
                forward_to_bridge = true;
            }
        }

        //
        // Step 2: forward to sink(s) decided in Step 1
        //
        if (forward_to_broker || forward_to_bridge || forward_to_network_interfaces) {
            MAPF_DBG("forwarding packet to"
                     << (forward_to_broker ? " broker," : "")
                     << (forward_to_bridge ? " bridge," : "")
                     << (forward_to_network_interfaces ? " network interfaces" : ""));
        } else {
            MAPF_ERR("dropping packet: " << std::endl << packet);
            return false;
        }
        if (forward_to_broker) {
            Packet defragmented_packet =
                packet; // create a copy because de_fragment_packet may modify Packet.
            if (de_fragment_packet(defragmented_packet)) {
                if (!send_packet_to_broker(defragmented_packet)) {
                    MAPF_ERR("cannot forward packet to broker.");
                    return false;
                }
            }
        }

        LOG(DEBUG) << "forwarding to bridge=" << forward_to_bridge
                   << ", to net_ifaces=" << forward_to_network_interfaces;
        if (forward_to_network_interfaces || forward_to_bridge) {
            for (auto it = network_interfaces_.begin(); it != network_interfaces_.end(); ++it) {
                auto &network_interface = it->second;
                auto &ifname            = it->second.ifname;
                unsigned int if_index   = if_nametoindex(ifname.c_str());
                LOG(DEBUG) << "Candidate if=" << ifname
                           << ", is_bridge=" << network_interface.is_bridge
                           << ", fd_null=" << !network_interface.fd << ", if_index=" << if_index
                           << ", packet.src_if_type=" << int(packet.src_if_type)
                           << ", packet.src_if_index=" << int(packet.src_if_index);

                if (((forward_to_bridge && network_interface.is_bridge) ||
                     (forward_to_network_interfaces && !network_interface.is_bridge)) &&
                    (network_interface.fd) &&
                    (packet.dst_if_index == 0 || packet.dst_if_index == if_index) &&
                    !(packet.src_if_type == CmduRxMessage::IF_TYPE_NET &&
                      packet.src_if_index == if_index)) { /* avoid loop-back */
                    LOG(DEBUG) << "Forwarding the packet on " << ifname;
                    if (!fragment_and_send_packet_to_network_interface(if_index, packet)) {
                        MAPF_ERR("cannot forward packet to network inteface " << ifname << ".");
                        return false;
                    }
                }

                // Assuming there is only one bridge, and the bridge is the first on
                // 'network_interfaces_' list, breaking the loop for optimization in case that
                // forwarding is only required to the bridge.
                if (network_interface.is_bridge && forward_to_bridge &&
                    !forward_to_network_interfaces) {
                    break;
                }
            }
        }
    }

    return true;
}

/**
 * @brief forward a 1905.1 packet.
 *
 * Basically a wrapper to forward_packet_single, which also takes into account the reliable
 * multicast procedure - according to section 15.1 in the Multi-AP specification, a reliable multicast
 * transmission includes sending the packet with the relay bit set to a multicast address once,
 * and once for each discovered Multi-AP agent in the network as unicast
 * with the same MID and the relay bit unset.
 *
 * @param packet packet to forward
 * @return true on success
 * @return false on failure (one or more packets failed in transmission)
 */
bool Ieee1905Transport::forward_packet(Packet &packet)
{
    // only try to forward IEEE1905 packets
    if (packet.ether_type != ETH_P_1905_1) {
        MAPF_WARN("Ignoring non-1905 packet");
        return false;
    }
    // First, forward the packet as is.
    MAPF_DBG("First forward_packet_single");
    bool success = forward_packet_single(packet);
    if (!success) {
        MAPF_DBG("Failed to forward packet " << packet);
        return false;
    }

    // Check if this packet should also go through reliable multicast, return if not:
    // 1. Destination is multicast
    LOG(DEBUG) << "ETHER_IS_MULTICAST=" << std::hex << std::boolalpha
               << bool(ETHER_IS_MULTICAST(packet.dst.oct));
    if (!ETHER_IS_MULTICAST(packet.dst.oct)) {
        return true;
    }
    // 2. Message is relayed multicast
    Ieee1905CmduHeader *ch = reinterpret_cast<Ieee1905CmduHeader *>(packet.payload.iov_base);
    LOG(DEBUG) << "GetRelayIndicator=" << ch->GetRelayIndicator();
    if (!ch->GetRelayIndicator()) {
        return true;
    }
    // 3. If Message type is a reliable multicast according to the EasyMesh specification (See EasyMesh
    //    specification Table 5) need to send it also as unicast. Otherwise stop handling.
    if (!packet.is_reliable_multicast()) {
        return true;
    }

    // 4. Message is locally generated - the spec says to duplicate the message as part
    // of the transmission procedure (section 7.3 of 1905.1) while relaying is part of
    // the reception procedure (section 7.6 of 1905.1).
    LOG(DEBUG) << "packet.src_if_type=" << packet.src_if_type;
    if (packet.src_if_type != CmduRxMessage::IF_TYPE_LOCAL_BUS) {
        return true;
    }

    // Reliable multicast - send to all known neighbors.
    // Note - we are reusing the same packet, so this part has to come
    // after the original packet (relayed multicast) has been sent.
    // Change the packet destination each time to the neighbor AL MAC
    MAPF_DBG("Second forward_packet_single");
    ch->SetRelayIndicator(false);
    for (const auto &neigh : neighbors_map_) {
        packet.dst = neigh.second.al_mac;
        if (!forward_packet_single(packet)) {
            MAPF_ERR("Reliable multicast failed, dropped packet: " << packet);
            success = false;
        }
    }

    return success;
}

std::ostream &Ieee1905Transport::Packet::print(std::ostream &os) const
{
    std::stringstream ss;

    ss << "ethernet header:" << std::endl;
    ss << "  dst        : " << dst << std::endl;
    ss << "  src        : " << src << std::endl;
    ss << "  ether_type : " << std::hex << std::setfill('0') << std::setw(4) << ether_type
       << std::endl;

    if (ether_type == ETH_P_1905_1) {
        Ieee1905CmduHeader *ch = (Ieee1905CmduHeader *)payload.iov_base;
        ss << "  IEEE1905 header:" << std::endl;
        ss << "    messageVersion        : " << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)ch->messageVersion << std::endl;
        ss << "    messageType           : " << std::hex << std::setfill('0') << std::setw(4)
           << (unsigned)ntohs(ch->messageType) << std::endl;
        ss << "    messageId             : " << std::hex << std::setfill('0') << std::setw(4)
           << (unsigned)ntohs(ch->messageId) << std::endl;
        ss << "    fragmentId            : " << std::hex << std::setfill('0') << std::setw(2)
           << (unsigned)ch->fragmentId << std::endl;
        ss << "    lastFragmentIndicator : " << std::hex << std::setfill('0') << std::setw(1)
           << (unsigned)ch->GetLastFragmentIndicator() << std::endl;
        ss << "    relayIndicator        : " << std::hex << std::setfill('0') << std::setw(1)
           << (unsigned)ch->GetRelayIndicator() << std::endl;
    } else if (ether_type == ETH_P_LLDP) {
        ss << "  LLDP packet" << std::endl;
    }

    // dump payload
    if (payload.iov_base) {
        ss << "  payload:" << std::endl << "    ";
        uint8_t *p = (uint8_t *)payload.iov_base;
        for (size_t i = 0; i < payload.iov_len; i++) {
            ss << std::hex << std::setfill('0') << std::setw(2) << (unsigned)p[i] << " ";
            if (i % 16 == 15)
                ss << std::endl << "    ";
            else if (i % 8 == 7)
                ss << " ";
        }
    }

    return os << ss.str();
}

} // namespace transport
} // namespace beerocks
