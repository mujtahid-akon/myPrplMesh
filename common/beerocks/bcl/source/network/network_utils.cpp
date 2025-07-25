/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_string_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/network/swap.h>

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <linux/ethtool.h>
#include <linux/if_ether.h>  // ETH_P_ARP = 0x0806
#include <linux/if_packet.h> // struct sockaddr_ll (see man 7 packet)
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netlink/route/neighbour.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <easylogging++.h>

using namespace beerocks::net;

#define NL_BUFSIZE 8192
#define MAC_ADDR_CHAR_SIZE 17
#define IPV4_ADDR_CHAR_SIZE 15
#define MAX_FDB_ENTRIES 128

struct route_info {
    struct in_addr dstAddr;
    struct in_addr srcAddr;
    struct in_addr gateWay;
    char ifName[IF_NAMESIZE];
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Fucntions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static int read_iface_flags(const std::string &strIface, struct ifreq &if_req)
{
    int socId = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (socId < 0)
        return errno;

    // Read the interface flags
    beerocks::string_utils::copy_string(if_req.ifr_name, strIface.c_str(), sizeof(if_req.ifr_name));
    int rv = ioctl(socId, SIOCGIFFLAGS, &if_req);
    close(socId);

    // Return the error code
    if (rv == -1)
        return errno;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Constants ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

const std::string network_utils::ZERO_IP_STRING("0.0.0.0");
const std::string network_utils::ZERO_MAC_STRING("00:00:00:00:00:00");
const sMacAddr network_utils::ZERO_MAC{.oct = {0}};
const std::string network_utils::WILD_MAC_STRING("ff:ff:ff:ff:ff:ff");
const sMacAddr network_utils::MULTICAST_1905_MAC_ADDR{.oct = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x13}};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool network_utils::is_valid_mac(std::string mac)
{
    if (mac.size() != MAC_ADDR_CHAR_SIZE) {
        return false;
    }
    std::transform(mac.begin(), mac.end(), mac.begin(), ::tolower);
    uint8_t str[20];
    tlvf::mac_from_string(str, mac);
    auto mac_out = tlvf::mac_to_string(str);
    return (mac == mac_out);
}

std::string network_utils::ipv4_to_string(const sIpv4Addr &ip)
{
    return ipv4_to_string((const uint8_t *)&ip.oct);
}

std::string network_utils::ipv4_to_string(const uint8_t *ipv4)
{
    std::string ipv4_str;
    std::for_each(ipv4, ipv4 + IP_ADDR_LEN,
                  [&](const uint8_t &oct) { ipv4_str += std::to_string(oct) + '.'; });

    ipv4_str.pop_back(); // remove last '.' character
    return ipv4_str;
}

std::string network_utils::ipv4_to_string(uint32_t ip)
{
    swap_32(ip);

    std::string ipv4_str;
    for (int i = IP_ADDR_LEN; i > 0; i--) {
        uint8_t oct = ip >> ((i - 1) * 8) & 0xFF;
        ipv4_str += (std::to_string(oct) + '.');
    }

    ipv4_str.pop_back(); // remove last '.' character

    return ipv4_str;
}

sIpv4Addr network_utils::ipv4_from_string(const std::string &ip)
{
    sIpv4Addr ret = {};

    ipv4_from_string(ret.oct, ip);

    return ret;
}

void network_utils::ipv4_from_string(uint8_t *buf, const std::string &ip_str)
{
    if (ip_str.empty()) {
        memset(buf, 0, IP_ADDR_LEN);
    } else {
        std::stringstream ipv4_ss(ip_str);
        std::string token;

        for (int i = 0; i < IP_ADDR_LEN; i++) {
            std::getline(ipv4_ss, token, '.');
            buf[i] = string_utils::stoi(token);
        }
    }
}

uint32_t network_utils::uint_ipv4_from_array(void *ip)
{
    uint32_t ret;

    if (!ip) {
        return 0;
    }

    ret = (((uint32_t(*(((char *)ip) + 0)) << 24) & 0xFF000000) |
           ((uint32_t(*(((char *)ip) + 1)) << 16) & 0x00FF0000) |
           ((uint32_t(*(((char *)ip) + 2)) << 8) & 0x0000FF00) |
           (uint32_t(*(((char *)ip) + 3)) & 0x000000FF));

    swap_32(ret);

    return ret;
}

uint32_t network_utils::uint_ipv4_from_string(const std::string &ip_str)
{
    uint8_t ip_arr[IP_ADDR_LEN];
    ipv4_from_string(ip_arr, ip_str);

    uint32_t ret = ((uint32_t(ip_arr[0]) << 24) | (uint32_t(ip_arr[1]) << 16) |
                    (uint32_t(ip_arr[2]) << 8) | (uint32_t(ip_arr[3])));

    swap_32(ret);
    return ret;
}

int network_utils::get_iface_info(network_utils::iface_info &info, const std::string &iface_name)
{
    info.iface = iface_name;
    info.mac.clear();
    info.ip.clear();
    info.netmask.clear();
    info.ip_gw.clear();

    std::vector<network_utils::ip_info> ip_list = network_utils::get_ip_list();
    for (auto &ip_info : ip_list) {
        if (ip_info.iface == iface_name) {
            info.mac          = ip_info.mac;
            info.ip           = ip_info.ip;
            info.netmask      = ip_info.netmask;
            info.ip_gw        = ip_info.gw;
            info.broadcast_ip = ip_info.broadcast_ip;
            break;
        }
    }

    if (info.mac.empty()) {
        if (!network_utils::linux_iface_get_mac(iface_name, info.mac)) {
            return -1;
        }
    }

    return 0;
}

bool network_utils::get_raw_iface_info(const std::string &iface_name, raw_iface_info &info)
{
    int fd;
    struct ifreq ifr;

    if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
        LOG(ERROR) << "Failed opening DGRAM socket: " << strerror(errno);
        return false;
    }

    // Clear the memory
    info = {};

    // MAC Address
    ifr = {};
    string_utils::copy_string(ifr.ifr_name, iface_name.c_str(), IF_NAMESIZE);
    if ((ioctl(fd, SIOCGIFHWADDR, &ifr)) == -1) {
        LOG(ERROR) << "ioctl failed: " << strerror(errno);
        close(fd);
        return false;
    }
    std::copy_n(ifr.ifr_hwaddr.sa_data, 6, (uint8_t *)&info.hwa);

    // IP Address
    ifr = {};
    string_utils::copy_string(ifr.ifr_name, iface_name.c_str(), IF_NAMESIZE);
    if ((ioctl(fd, SIOCGIFADDR, &ifr)) == -1) {
        LOG(ERROR) << "ioctl failed: " << strerror(errno);
        close(fd);
        return false;
    }
    std::copy_n((uint8_t *)&(*(sockaddr_in *)&ifr.ifr_addr).sin_addr, 4, (uint8_t *)&info.ipa);

    // Network Mask
    ifr = {};
    string_utils::copy_string(ifr.ifr_name, iface_name.c_str(), IF_NAMESIZE);
    if ((ioctl(fd, SIOCGIFNETMASK, &ifr)) == -1) {
        LOG(ERROR) << "ioctl failed: " << strerror(errno);
        close(fd);
        return false;
    }
    std::copy_n((uint8_t *)&(*(sockaddr_in *)&ifr.ifr_netmask).sin_addr, 4, (uint8_t *)&info.nmask);

    // Broadcast Address
    ifr = {};
    string_utils::copy_string(ifr.ifr_name, iface_name.c_str(), IF_NAMESIZE);
    if ((ioctl(fd, SIOCGIFBRDADDR, &ifr)) == -1) {
        LOG(ERROR) << "ioctl failed: " << strerror(errno);
        close(fd);
        return false;
    }
    std::copy_n((uint8_t *)&(*(sockaddr_in *)&ifr.ifr_broadaddr).sin_addr, 4,
                (uint8_t *)&info.bcast);

    close(fd);

    return true;
}

static int readNlSock(int fd, char *msg, uint32_t seq, uint32_t pid)
{
    int nl_msg_len = 0;

    for (;;) {

        if (nl_msg_len >= NL_BUFSIZE) {
            LOG(ERROR) << "Error: nl_msg_len exceeds or equals NL_BUFSIZE";
            return -1;
        }

        // Read Netlink message
        auto ret = recv(fd, msg, NL_BUFSIZE - nl_msg_len, 0);

        if (ret == 0) {
            LOG(WARNING) << "netlink connection closed: recv returned 0";
            return -1;
        } else if (ret < 0) {
            LOG(ERROR) << "Failed reading netlink socket: " << strerror(errno);
            return -1;
        }

        // Netlink header
        auto header = (struct nlmsghdr *)msg;

        // Validate the header
        if (ret < int(sizeof(struct nlmsghdr)) || ret < int(header->nlmsg_len) ||
            header->nlmsg_len < sizeof(struct nlmsghdr)) {
            LOG(WARNING) << "Invalid netlink message header - msg len = " << int(header->nlmsg_len)
                         << " (" << int(ret) << ")";
            return -1;
        }

        if (header->nlmsg_type == NLMSG_ERROR) {
            LOG(WARNING) << "Read netlink error message";
            return -1;
        }

        // Not the last message
        if (header->nlmsg_type != NLMSG_DONE) {
            msg += ret;
            nl_msg_len += ret;
        } else {
            break;
        }

        // Multipart of someother message
        if (((header->nlmsg_flags & NLM_F_MULTI) == 0) || (header->nlmsg_seq != seq) ||
            (header->nlmsg_pid != pid)) {
            break;
        }
    }

    // Return the length of the read message
    return nl_msg_len;
}

/* For parsing the route info returned */
static int parseRoutes(struct nlmsghdr *nlHdr, std::shared_ptr<route_info> rtInfo)
{
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
    int rtLen;

    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

    // If the route is not for AF_INET then return
    if (rtMsg->rtm_family != AF_INET)
        return (0);

    // get the rtattr field
    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen  = RTM_PAYLOAD(nlHdr);
    for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen)) {
        switch (rtAttr->rta_type) {
        case RTA_OIF: {
            auto index            = *(int *)RTA_DATA(rtAttr);
            auto iface_index_name = network_utils::linux_get_iface_name(index);
            std::copy_n(iface_index_name.begin(), iface_index_name.length(), rtInfo->ifName);
            break;
        }
        case RTA_GATEWAY:
            rtInfo->gateWay.s_addr = *(u_int *)RTA_DATA(rtAttr);
            break;
        case RTA_PREFSRC:
            rtInfo->srcAddr.s_addr = *(u_int *)RTA_DATA(rtAttr);
            break;
        case RTA_DST:
            rtInfo->dstAddr.s_addr = *(u_int *)RTA_DATA(rtAttr);
            break;
        default:
            break;
        }
    }

    if ((rtInfo->dstAddr.s_addr == 0) && (rtInfo->gateWay.s_addr != 0)) {
        return (1);
    }
    if (rtInfo->dstAddr.s_addr == rtInfo->srcAddr.s_addr) {
        return (2);
    }
    return (0);
}

std::shared_ptr<std::unordered_map<std::string, std::string>>
network_utils::get_arp_table(bool mac_as_key)
{
    auto arp_table_ptr = std::make_shared<std::unordered_map<std::string, std::string>>();
    auto &arp_table    = *arp_table_ptr.get();

    // Allocate a new netlink socket
    std::unique_ptr<nl_sock, decltype(&nl_socket_free)> nl_socket(nl_socket_alloc(),
                                                                  &nl_socket_free);
    if (!nl_socket) {
        LOG(ERROR) << "Failed allocating netlink socket!";
        return nullptr;
    }

    int err = nl_connect(nl_socket.get(), NETLINK_ROUTE);
    if (err != 0) {
        LOG(ERROR) << "Failed connecting the netlink socket: " << nl_geterror(err);
        return nullptr;
    }

    // ARP table cache
    struct nl_cache *neighbor_table_cache = nullptr;

    // Allocate the cache and fill it with data
    err = rtnl_neigh_alloc_cache(nl_socket.get(), &neighbor_table_cache);
    if (err != 0) {
        LOG(ERROR) << "Failed probing ARP table: " << nl_geterror(err);
        return nullptr;
    }

    auto entries_count = nl_cache_nitems(neighbor_table_cache);
    if (entries_count <= 0) {
        return arp_table_ptr;
    }

    auto neighbor_entry = reinterpret_cast<rtnl_neigh *>(nl_cache_get_first(neighbor_table_cache));
    if (!neighbor_entry) {
        LOG(ERROR) << "neighbor_entry is null";
        nl_cache_free(neighbor_table_cache);
        return nullptr;
    }

    char buffer_mac[MAC_ADDR_CHAR_SIZE + 1]; // +1 for null terminator
    char buffer_ip[IPV4_ADDR_CHAR_SIZE + 1]; // +1 for null terminator

    for (int i = 0; i < entries_count; i++) {

        if (neighbor_entry == nullptr) {
            break;
        }

        // Skip NOARP state and non IPv4 records
        if (rtnl_neigh_get_state(neighbor_entry) == NUD_NOARP ||
            rtnl_neigh_get_family(neighbor_entry) != AF_INET) {
            // Advance to the next record
            neighbor_entry = reinterpret_cast<rtnl_neigh *>(
                nl_cache_get_next(reinterpret_cast<nl_object *>(neighbor_entry)));
            continue;
        }

        auto nl_addr_mac = rtnl_neigh_get_lladdr(neighbor_entry);
        if (!nl_addr_mac) {
            // Advance to the next record
            neighbor_entry = reinterpret_cast<rtnl_neigh *>(
                nl_cache_get_next(reinterpret_cast<nl_object *>(neighbor_entry)));
            continue;
        }

        auto nl_addr_ip = rtnl_neigh_get_dst(neighbor_entry);
        if (!nl_addr_ip) {
            // Advance to the next record
            neighbor_entry = reinterpret_cast<rtnl_neigh *>(
                nl_cache_get_next(reinterpret_cast<nl_object *>(neighbor_entry)));
            continue;
        }

        char *mac = nl_addr2str(nl_addr_mac, buffer_mac, sizeof(buffer_mac));
        char *ip  = nl_addr2str(nl_addr_ip, buffer_ip, sizeof(buffer_ip));

        if (mac_as_key) {
            arp_table[mac] = ip;
        } else {
            arp_table[ip] = mac;
        }

        // Advance to the next record
        neighbor_entry = reinterpret_cast<rtnl_neigh *>(
            nl_cache_get_next(reinterpret_cast<nl_object *>(neighbor_entry)));
    }

    // Free the cache
    nl_cache_free(neighbor_table_cache);

    return arp_table_ptr;
}

std::string network_utils::get_mac_from_arp_table(const std::string &ipv4)
{
    /**
     * /proc/net/arp looks like this:
     *
     * IP address       HW type     Flags       HW address            Mask     Device
     * 192.168.12.31    0x1         0x2         00:09:6b:00:02:03     *        eth0
     * 192.168.12.70    0x1         0x2         00:01:02:38:4c:85     *        eth0
     */
    std::ifstream arp_table("/proc/net/arp");

    if (!arp_table.is_open()) {
        LOG(ERROR) << "can't open arp table";
        return std::string();
    }

    std::string line;
    while (std::getline(arp_table, line)) {
        std::string entry_ipv4 = line.substr(0, line.find_first_of(' '));

        size_t mac_start, mac_end;
        mac_start = line.find_first_of(':') - 2;
        mac_end   = line.find_last_of(':') + 3;

        if (mac_start >= line.length() || mac_end >= line.length()) {
            LOG(DEBUG) << "line doesn't match expected format, skipping";
            continue;
        }

        std::string entry_mac = line.substr(mac_start, mac_end - mac_start);

        LOG(DEBUG) << "entry_ipv4=" << entry_ipv4 << " entry_mac=" << entry_mac;

        if (entry_ipv4 == ipv4) {
            return entry_mac;
        }
    }

    return std::string();
}

std::list<std::string> network_utils::linux_get_iface_list()
{
    std::list<std::string> ifaces;

    constexpr char path[] = "/sys/class/net/";

    DIR *d;
    dirent *dir;
    d = opendir(path);
    if (!d) {
        return ifaces;
    }
    while ((dir = readdir(d)) != NULL) {
        std::string ifname(dir->d_name);
        if (ifname == "." || ifname == "..") {
            continue;
        }
        ifaces.push_back(ifname);
    }
    closedir(d);
    return ifaces;
}

std::vector<std::string> network_utils::linux_get_iface_list_from_bridge(const std::string &bridge)
{
    std::vector<std::string> ifs;

    std::string path = "/sys/class/net/" + bridge + "/brif";

    DIR *d;
    struct dirent *dir;
    d = opendir(path.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            std::string ifname = dir->d_name;
            if (ifname == "." || ifname == "..") {
                continue;
            }
            ifs.push_back(ifname);
        }
        closedir(d);
    }

    return ifs;
}

int network_utils::linux_get_iface_state_from_bridge(const std::string &bridge,
                                                     const std::string &ifname)
{
    std::string filename = "/sys/class/net/" + bridge + "/brif/" + ifname + "/state";
    std::ifstream stp_state_file(filename, std::ios::in);

    if (!stp_state_file.is_open()) {
        LOG(ERROR) << "Failed to open " << filename;
        return -1;
    }

    int state;
    if (!(stp_state_file >> state)) {
        LOG(ERROR) << "Failed to read integer state from " << filename;
        return -1;
    }

    return state;
}

uint32_t network_utils::linux_get_iface_index(const std::string &iface_name)
{
    uint32_t iface_index = if_nametoindex(iface_name.c_str());
    LOG_IF(!iface_index, ERROR) << "Failed to read the index of interface " << iface_name << ": "
                                << strerror(errno);
    return iface_index;
}

std::string network_utils::linux_get_iface_name(uint32_t iface_index)
{
    char iface_name[IF_NAMESIZE] = {0};
    if (!if_indextoname(iface_index, iface_name)) {
        LOG(ERROR) << "Failed to read the name of interface with index " << iface_index << ": "
                   << strerror(errno);
        return "";
    }

    iface_name[IF_NAMESIZE - 1] = '\0';

    return iface_name;
}

bool network_utils::linux_add_iface_to_bridge(const std::string &bridge, const std::string &iface)
{
    LOG(DEBUG) << "add iface " << iface << " to bridge " << bridge;

    struct ifreq ifr;
    int err;
    unsigned long ifindex = if_nametoindex(iface.c_str());
    if (ifindex == 0) {
        LOG(ERROR) << "invalid iface index=" << ifindex << " for " << iface;
        return false;
    }

    int br_socket_fd;
    if ((br_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        LOG(ERROR) << "can't open br_socket_fd";
        return false;
    }

    string_utils::copy_string(ifr.ifr_name, bridge.c_str(), IFNAMSIZ);
#ifdef SIOCBRADDIF
    ifr.ifr_ifindex = ifindex;
    err             = ioctl(br_socket_fd, SIOCBRADDIF, &ifr);
    if (err < 0)
#endif
    {
        unsigned long args[4] = {BRCTL_ADD_IF, ifindex, 0, 0};

        ifr.ifr_data = (char *)args;
        err          = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
    }

    close(br_socket_fd);
    return err < 0 ? false : true;
    /*
    std::string cmd;
    cmd = "brctl addif " + bridge + " " + iface;
    system(cmd.c_str());
    LOG(DEBUG) << cmd;
    return true;
    */
}

bool network_utils::linux_remove_iface_from_bridge(const std::string &bridge,
                                                   const std::string &iface)
{
    LOG(DEBUG) << "remove iface " << iface << " from bridge " << bridge;

    struct ifreq ifr;
    int err;
    unsigned long ifindex = if_nametoindex(iface.c_str());

    if (ifindex == 0) {
        LOG(ERROR) << "invalid iface index=" << ifindex << " for " << iface;
        return false;
    }

    int br_socket_fd;
    if ((br_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        LOG(ERROR) << "can't open br_socket_fd";
        return false;
    }

    string_utils::copy_string(ifr.ifr_name, bridge.c_str(), IFNAMSIZ);
#ifdef SIOCBRDELIF
    ifr.ifr_ifindex = ifindex;
    err             = ioctl(br_socket_fd, SIOCBRDELIF, &ifr);
    if (err < 0)
#endif
    {
        unsigned long args[4] = {BRCTL_DEL_IF, ifindex, 0, 0};

        ifr.ifr_data = (char *)args;
        err          = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
    }

    close(br_socket_fd);
    return err < 0 ? false : true;
    /*
    std::string cmd;
    cmd = "brctl delif " + bridge + " " + iface;
    system(cmd.c_str());
    LOG(DEBUG) << cmd;
    return true;
    */
}

bool network_utils::linux_iface_ctrl(const std::string &iface, bool up, std::string ip,
                                     const std::string &netmask)
{
    bool ret = true;
    int fd;
    struct ifreq ifr;
    bool zero_ip;

    LOG(TRACE) << "linux_iface_ctrl iface=" << iface << (up ? " up" : " down") << " ip=" << ip
               << " netmask=" << netmask;

    if (ip.empty()) {
        ip      = "0.0.0.0";
        zero_ip = true;
    } else {
        zero_ip = false;
    }

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(ERROR) << "Can't open SOCK_DGRAM socket";
        return false;
    }

    string_utils::copy_string(ifr.ifr_name, iface.c_str(), IFNAMSIZ);
    while (up) {
        ifr.ifr_addr.sa_family = AF_INET;
        if (!ip.empty()) {
            uint32_t uip = network_utils::uint_ipv4_from_string(ip);
            ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = uip;
            if (ioctl(fd, SIOCSIFADDR, &ifr) == -1) {
                LOG(ERROR) << "SIOCSIFADDR " << strerror(errno);
                ret = false;
                break;
            }

            if (!zero_ip) {
                uip |= 0xFF;
                ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr.s_addr = uip;
                if (ioctl(fd, SIOCSIFBRDADDR, &ifr) == -1) {
                    LOG(ERROR) << "SIOCSIFBRDADDR " << strerror(errno);
                    ret = false;
                    break;
                }
            }
        }
        if (!netmask.empty()) {
            ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr =
                network_utils::uint_ipv4_from_string(netmask);
            if (ioctl(fd, SIOCSIFNETMASK, &ifr) == -1) {
                LOG(ERROR) << "SIOCGIFNETMASK " << strerror(errno);
                ret = false;
                break;
            }
        }
        ifr.ifr_flags = IFF_UP | IFF_BROADCAST | IFF_RUNNING;
        if (ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
            LOG(ERROR) << "SIOCSIFFLAGS " << strerror(errno);
            ret = false;
        }
        break;
    }
    while (!up) {
        if (ioctl(fd, SIOCGIFFLAGS, &ifr) == -1) {
            LOG(ERROR) << "SIOCGIFFLAGS " << strerror(errno);
            ret = false;
        } else if (ifr.ifr_flags & IFF_UP) {
            ifr.ifr_flags &= (~IFF_UP);
            if (ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
                LOG(ERROR) << "SIOCSIFFLAGS " << strerror(errno);
                ret = false;
            }
        }
        break;
    }
    close(fd);
    return ret;
}

bool network_utils::linux_iface_get_mac(const std::string &iface, std::string &mac)
{
    struct ifreq ifr;
    int fd;

    mac.clear();

    if (iface.empty()) {
        LOG(ERROR) << "Empty interface name";
        return false;
    }

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(ERROR) << "Can't open SOCK_DGRAM socket";
        return false;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    string_utils::copy_string(ifr.ifr_name, iface.c_str(), IFNAMSIZ);
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
        LOG(ERROR) << "SIOCGIFHWADDR. iface: " << iface;
        close(fd);
        return false;
    }
    close(fd);
    mac = tlvf::mac_to_string((uint8_t *)(ifr.ifr_ifru.ifru_hwaddr.sa_data));
    std::transform(mac.begin(), mac.end(), mac.begin(), ::tolower);
    return true;
}

bool network_utils::linux_iface_get_name(const sMacAddr &mac, std::string &iface)
{
    bool found = false;
    struct if_nameindex *pif;
    struct if_nameindex *head;
    std::string mac_to_find = tlvf::mac_to_string(mac);

    head = pif = if_nameindex();
    while (pif->if_index && (!found)) {
        std::string if_mac;
        if (linux_iface_get_mac(pif->if_name, if_mac) && (if_mac == mac_to_find)) {
            iface = pif->if_name;
            found = true;
        }
        pif++;
    }

    if_freenameindex(head);

    return found;
}

bool network_utils::linux_iface_get_ip(const std::string &iface, std::string &ip)
{
    struct ifreq ifr;
    int fd;

    ip.clear();

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(ERROR) << "Can't open SOCK_DGRAM socket";
        return false;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    string_utils::copy_string(ifr.ifr_name, iface.c_str(), IFNAMSIZ);

    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
        LOG(ERROR) << "SIOCGIFADDR";
        close(fd);
        return false;
    }
    close(fd);
    uint32_t ip_uint = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    ip               = network_utils::ipv4_to_string(ip_uint);
    return true;
}

bool network_utils::linux_iface_get_pci_info(const std::string &iface, std::string &pci_id)
{
    std::ifstream phy_device_file("/sys/class/net/" + iface + "/phy80211/device/device",
                                  std::ios::in);

    if (!phy_device_file.is_open()) {
        // LOG(ERROR) << "can't open phy80211 device file for interface " << iface;
        return false;
    }

    std::string device;
    std::getline(phy_device_file, device);
    device = device.substr(2); // remove trailing 0x

    std::ifstream phy_vendor_file("/sys/class/net/" + iface + "/phy80211/device/vendor",
                                  std::ios::in);

    if (!phy_vendor_file.is_open()) {
        // LOG(ERROR) << "can't open phy80211 vendor file for interface " << iface;
        return false;
    }

    std::string vendor;
    std::getline(phy_vendor_file, vendor);
    vendor = vendor.substr(2); // remove trailing 0x

    pci_id = vendor + ":" + device;

    return true;
}

std::string network_utils::linux_iface_get_host_bridge(const std::string &iface)
{
    std::string bridge_path("/sys/class/net/" + iface + "/brport/bridge");
    char resolvedPath[PATH_MAX];
    if (!realpath(bridge_path.c_str(), resolvedPath)) {
        return "";
    }
    std::string pathStr = resolvedPath;
    return pathStr.substr(pathStr.rfind('/') + 1);
}

bool network_utils::linux_iface_exists(const std::string &iface)
{
    struct ifreq flags;

    int err = read_iface_flags(iface, flags);
    return (err == 0);
}

bool network_utils::linux_iface_is_up(const std::string &iface)
{
    struct ifreq flags;

    int err = read_iface_flags(iface, flags);

    // Check if the interface us UP
    if (!err) {
        return (flags.ifr_flags & IFF_UP);
    }

    LOG(ERROR) << "Failed to read iface " << iface << " flags!!";

    return (false);
}

bool network_utils::linux_iface_is_up_and_running(const std::string &iface)
{
    struct ifreq flags;

    int err = read_iface_flags(iface, flags);

    // Check if the interface us UP and RUNNING (plugged)
    if (!err) {
        return (flags.ifr_flags & IFF_UP) && (flags.ifr_flags & IFF_RUNNING);
    }

    return (false);
}

#define ETHTOOL_LINK_MODE_MASK_MAX_KERNEL_NU32 (SCHAR_MAX)

static bool linux_iface_get_max_speed_from_link_modes(const uint32_t *link_mode_flags,
                                                      uint32_t link_mode_flags_nwords,
                                                      uint32_t &max_speed)
{
    if (!link_mode_flags || link_mode_flags_nwords < 1) {
        return false;
    }

    /*
     * map speed to common legacy ethtool link mode masks
     */
    const std::map<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>>
        legacy_link_modes_to_speed_mapping = {
            {SPEED_10,
             {
                 ADVERTISED_10baseT_Half,
                 ADVERTISED_10baseT_Full,
             }},
            {SPEED_100,
             {
                 ADVERTISED_100baseT_Half,
                 ADVERTISED_100baseT_Full,
             }},
            {SPEED_1000,
             {
                 ADVERTISED_1000baseT_Half,
                 ADVERTISED_1000baseT_Full,
                 ADVERTISED_1000baseKX_Full,
             }},
            {SPEED_2500,
             {
                 ADVERTISED_2500baseX_Full,
             }},
            {SPEED_10000,
             {
                 ADVERTISED_10000baseT_Full,
                 ADVERTISED_10000baseKX4_Full,
                 ADVERTISED_10000baseKR_Full,
                 ADVERTISED_10000baseR_FEC,
             }},
            {SPEED_20000,
             {
                 ADVERTISED_20000baseMLD2_Full,
                 ADVERTISED_20000baseKR2_Full,
             }},
            {SPEED_40000,
             {
                 ADVERTISED_40000baseKR4_Full,
                 ADVERTISED_40000baseCR4_Full,
                 ADVERTISED_40000baseSR4_Full,
                 ADVERTISED_40000baseLR4_Full,
             }},
            {SPEED_56000,
             {
                 ADVERTISED_56000baseKR4_Full,
                 ADVERTISED_56000baseCR4_Full,
                 ADVERTISED_56000baseSR4_Full,
                 ADVERTISED_56000baseLR4_Full,
             }},
        };
    for (const auto &link_mode_speed : legacy_link_modes_to_speed_mapping) {
        for (const auto &mask : link_mode_speed.second) {
            if (link_mode_flags[0] & mask) {
                max_speed = link_mode_speed.first;
                return true;
            }
        }
    }

    return false;
}

bool network_utils::linux_iface_get_speed(const std::string &iface, uint32_t &link_speed,
                                          uint32_t &max_advertised_speed)
{
    int sock;
    bool result = false;

    link_speed           = SPEED_UNKNOWN;
    max_advertised_speed = link_speed;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        LOG(ERROR) << "Can't create SOCK_DGRAM socket: " << strerror(errno);
    } else {
        struct ifreq ifr;
        int rc;

        string_utils::copy_string(ifr.ifr_name, iface.c_str(), sizeof(ifr.ifr_name));
#ifdef ETHTOOL_GLINKSETTINGS
        char ecmd[sizeof(struct ethtool_link_settings) +
                  3 * ETHTOOL_LINK_MODE_MASK_MAX_KERNEL_NU32 * sizeof(__u32)] = {};
        struct ethtool_link_settings *req = reinterpret_cast<struct ethtool_link_settings *>(ecmd);
        __u32 *link_mode_data             = reinterpret_cast<__u32 *>(req + 1);
        ifr.ifr_data                      = ecmd;

        /* Handshake with kernel to determine number of words for link
         * mode bitmaps. When requested number of bitmap words is not
         * the one expected by kernel, the latter returns the integer
         * opposite of what it is expecting. We request length 0 below
         * (aka. invalid bitmap length) to get this info.
         */
        req->cmd = ETHTOOL_GLINKSETTINGS;
        rc       = ioctl(sock, SIOCETHTOOL, &ifr);
        if (0 == rc) {
            /**
             * See above: we expect a strictly negative value from kernel.
             */
            if ((req->link_mode_masks_nwords >= 0) || (ETHTOOL_GLINKSETTINGS != req->cmd)) {
                LOG(ERROR) << "ETHTOOL_GLINKSETTINGS handshake failed";
            } else {
                /**
                 * Got the real req->link_mode_masks_nwords, now send the real request
                 */
                req->cmd                    = ETHTOOL_GLINKSETTINGS;
                req->link_mode_masks_nwords = -req->link_mode_masks_nwords;
                rc                          = ioctl(sock, SIOCETHTOOL, &ifr);
                if ((0 != rc) || (req->link_mode_masks_nwords <= 0) ||
                    (req->cmd != ETHTOOL_GLINKSETTINGS)) {
                    LOG(ERROR) << "ETHTOOL_GLINKSETTINGS request failed";
                } else {
                    link_speed = req->speed;

                    /* layout of link_mode_data fields:
                     * __u32 map_supported[link_mode_masks_nwords];
                     * __u32 map_advertising[link_mode_masks_nwords];
                     * __u32 map_lp_advertising[link_mode_masks_nwords];
                     */
                    max_advertised_speed         = link_speed;
                    const __u32 *map_advertising = &link_mode_data[req->link_mode_masks_nwords];
                    linux_iface_get_max_speed_from_link_modes(
                        map_advertising, req->link_mode_masks_nwords, max_advertised_speed);
                    result = true;
                }
            }
        }
#endif

        /**
         * ETHTOOL_GSET is deprecated and must be used only if ETHTOOL_GLINKSETTINGS
         * didn't work
         * or when not supported
         */
#ifdef ETHTOOL_GLINKSETTINGS
        if (!result)
#endif
        {
            struct ethtool_cmd ecmd_legacy;

            ifr.ifr_data = reinterpret_cast<char *>(&ecmd_legacy);

            memset(&ecmd_legacy, 0, sizeof(ecmd_legacy));
            ecmd_legacy.cmd = ETHTOOL_GSET;

            rc = ioctl(sock, SIOCETHTOOL, &ifr);
            if (0 == rc) {
                link_speed           = ethtool_cmd_speed(&ecmd_legacy);
                max_advertised_speed = link_speed;
                linux_iface_get_max_speed_from_link_modes(&ecmd_legacy.advertising, 1,
                                                          max_advertised_speed);
                result = true;
            }
        }

        if (rc < 0) {
            LOG(ERROR) << "ioctl failed: " << strerror(errno);
        }

        close(sock);
    }

    LOG(DEBUG) << "iface " << iface << " has speed current: " << link_speed
               << " max: " << max_advertised_speed << " result: " << result;

    return result;
}

std::vector<network_utils::ip_info> network_utils::get_ip_list()
{
    std::vector<network_utils::ip_info> ip_list;
    struct nlmsghdr *nlMsg;
    struct ifreq ifr;
    char *msgBuf = NULL;

    int sock, fd, len;
    uint32_t msgSeq = 0;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(ERROR) << "Can't open SOCK_DGRAM socket";
        return ip_list;
    }

    if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
        LOG(ERROR) << "Can't open netlink socket";
        close(fd);
        return ip_list;
    }

    msgBuf = new char[NL_BUFSIZE];
    memset(msgBuf, 0, NL_BUFSIZE);

    /* point the header and the msg structure pointers into the buffer */
    nlMsg = (struct nlmsghdr *)msgBuf;

    /* Fill in the nlmsg header*/
    nlMsg->nlmsg_len  = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
    nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .

    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlMsg->nlmsg_seq   = msgSeq++;                   // Sequence of the message packet.
    nlMsg->nlmsg_pid   = (uint32_t)getpid();         // PID of process sending the request.

    /* Send the request */
    if (send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0) {
        LOG(ERROR) << "send()";
        delete[] msgBuf;
        close(sock);
        close(fd);
        return ip_list;
    }

    /* Read the response */
    if ((len = readNlSock(sock, msgBuf, msgSeq, nlMsg->nlmsg_pid)) < 0) {
        LOG(ERROR) << "readNlSock()";
        delete[] msgBuf;
        close(sock);
        close(fd);
        return ip_list;
    }

    /* Parse and print the response */
    uint32_t ip_uint;
    network_utils::ip_info gw_ip_info;
    auto rtInfo = std::make_shared<route_info>();
    if (!rtInfo) {
        delete[] msgBuf;
        close(sock);
        close(fd);
        LOG(ERROR) << "rtInfo allocation failed!";
        return std::vector<network_utils::ip_info>();
    }
    for (; NLMSG_OK(nlMsg, uint32_t(len)); nlMsg = NLMSG_NEXT(nlMsg, len)) {
        memset(rtInfo.get(), 0, sizeof(struct route_info));
        int rtInfo_ret = parseRoutes(nlMsg, rtInfo);
        if (rtInfo_ret == 1) { // GW address
            gw_ip_info.gw    = network_utils::ipv4_to_string(rtInfo->gateWay.s_addr);
            gw_ip_info.iface = std::string(rtInfo->ifName);
            LOG(DEBUG) << "gw=" << gw_ip_info.gw << " iface=" << gw_ip_info.iface;
        } else if (rtInfo_ret == 2) { // Iface /IP addr
            network_utils::ip_info ip_info;

            ip_info.iface = std::string(rtInfo->ifName);

            ifr.ifr_addr.sa_family = AF_INET;
            string_utils::copy_string(ifr.ifr_name, rtInfo->ifName, IFNAMSIZ);

            if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
                continue; // skip, if can't read ip
            }
            ip_uint           = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
            ip_info.ip        = network_utils::ipv4_to_string(ip_uint);
            ip_info.iface_idx = if_nametoindex(ifr.ifr_name);

            if (ioctl(fd, SIOCGIFNETMASK, &ifr) == -1) {
                ip_info.netmask.clear();
            } else {
                ip_uint         = ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr;
                ip_info.netmask = network_utils::ipv4_to_string(ip_uint);
            }

            ip_uint = (((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr) |
                      (~(((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr));
            ip_info.broadcast_ip = network_utils::ipv4_to_string(ip_uint);

            if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
                ip_info.mac.clear();
            } else {
                ip_info.mac = tlvf::mac_to_string((uint8_t *)(ifr.ifr_ifru.ifru_hwaddr.sa_data));
                std::transform(ip_info.mac.begin(), ip_info.mac.end(), ip_info.mac.begin(),
                               ::tolower);
            }
            ip_list.push_back(ip_info);
        }
    }
    delete[] msgBuf;
    close(sock);
    close(fd);
    // update gw ip
    for (auto &ip_item : ip_list) {
        LOG(DEBUG) << "ip_item iface=" << ip_item.iface;
        if (ip_item.iface == gw_ip_info.iface) {
            ip_item.gw = gw_ip_info.gw;
        }
    }
    return ip_list;
}

bool network_utils::arp_send(const std::string &iface, const std::string &dst_ip,
                             const std::string &src_ip, sMacAddr dst_mac, sMacAddr src_mac,
                             int count, int arp_socket)
{
    int tx_len;
    arp_hdr arphdr;
    struct sockaddr_ll sock;
    uint8_t packet_buffer[128];

    // If the destination IP is empty, there is no point sending the arp, therefore replace the
    // with broadcast IP, so all clients will receive it and answer, but since the request is
    // being sent to a specific mac address, then only the requested client will answer.
    uint32_t dst_ip_uint;
    if (dst_ip.empty() || dst_ip == ZERO_IP_STRING) {
        dst_ip_uint = 0xFFFFFFFF; // "255.255.255.255" equivalent
    } else {
        dst_ip_uint = network_utils::uint_ipv4_from_string(dst_ip);
    }

    uint32_t src_ip_uint = network_utils::uint_ipv4_from_string(src_ip);

    // Fill out sockaddr_ll.
    sock             = {};
    sock.sll_family  = AF_PACKET;
    sock.sll_ifindex = if_nametoindex(iface.c_str());
    sock.sll_halen   = MAC_ADDR_LEN;
    tlvf::mac_to_array(dst_mac, sock.sll_addr);

    // build ARP header
    arphdr.htype  = htons(1);        // type: 1 for ethernet
    arphdr.ptype  = htons(ETH_P_IP); // proto
    arphdr.hlen   = MAC_ADDR_LEN;    // mac addr len
    arphdr.plen   = IP_ADDR_LEN;     // ip addr len
    arphdr.opcode = htons(ARPOP_REQUEST);
    tlvf::mac_to_array(src_mac, arphdr.sender_mac);
    std::copy_n((uint8_t *)&src_ip_uint, IP_ADDR_LEN, arphdr.sender_ip);
    tlvf::mac_to_array(dst_mac, arphdr.target_mac);
    std::copy_n((uint8_t *)&dst_ip_uint, IP_ADDR_LEN, arphdr.target_ip);

    // build ethernet frame
    tx_len = 2 * MAC_ADDR_LEN + 2 + ARP_HDRLEN; // dest mac, src mac, type, arp header len
    tlvf::mac_to_array(dst_mac, packet_buffer);
    tlvf::mac_to_array(src_mac, packet_buffer + MAC_ADDR_LEN);
    packet_buffer[12] = ETH_P_ARP / 256;
    packet_buffer[13] = ETH_P_ARP % 256;

    // ARP header
    std::copy_n((uint8_t *)&arphdr, ARP_HDRLEN, packet_buffer + ETH_HDRLEN);

    bool new_socket = (arp_socket < 0);
    if (new_socket) {
        if ((arp_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
            LOG(ERROR) << "Opening ARP socket";
            return false;
        }
    }

    // Send ethernet frame to socket.
    for (int i = 0; i < count; i++) {
        // LOG_MONITOR(DEBUG) << "ARP to ip=" << dest_ip << " mac=" << dest_mac;
        if (sendto(arp_socket, packet_buffer, tx_len, 0, (sockaddr *)&sock, sizeof(sock)) <= 0) {
            PLOG(ERROR) << "sendto(" << arp_socket << ") failed";
        }
    }
    if (new_socket) {
        close(arp_socket);
    }
    return true;
}

bool network_utils::icmp_send(const std::string &ip, uint16_t id, int count, int icmp_socket)
{
    sockaddr_in pingaddr;
    uint8_t packet_buffer[128];

    if (icmp_socket < 0) {
        LOG(ERROR) << "icmp_socket=" << icmp_socket;
        return false;
    }

    pingaddr                 = {};
    pingaddr.sin_family      = AF_INET;
    pingaddr.sin_addr.s_addr = inet_addr(ip.c_str());

    for (int i = 0; i < count; i++) {
        icmphdr *pkt          = (icmphdr *)packet_buffer;
        pkt->type             = ICMP_ECHO;
        pkt->code             = 0;
        pkt->un.echo.id       = htons(id);
        pkt->un.echo.sequence = htons(i);
        pkt->checksum =
            0; // keep cheksum = 0 before calling icmp_checksum -> needed for checksum calculation
        pkt->checksum = htons(icmp_checksum((uint16_t *)pkt, sizeof(packet_buffer)));
        ssize_t bytes = sendto(icmp_socket, packet_buffer, sizeof(packet_buffer), 0,
                               (sockaddr *)&pingaddr, sizeof(sockaddr_in));
        if (bytes < 0) {
            LOG(ERROR) << "writeBytes() failed: " << strerror(errno);
            return false;
        } else if (bytes < (ssize_t)sizeof(packet_buffer)) {
            LOG(ERROR) << "ICMP Failed to send, sent " << int(bytes) << " out of "
                       << int(sizeof(packet_buffer));
            return false;
        }
    }
    return true;
}

uint16_t network_utils::icmp_checksum(uint16_t *buf, int32_t len)
{
    int32_t sum     = 0;
    uint16_t answer = 0;

    for (int32_t i = 0; i < len / 2; ++i) {
        sum += buf[i];
    }

    // Handle the case where there is an odd byte
    if (len % 2 == 1) {
        sum += *reinterpret_cast<uint8_t *>(&buf[len / 2]);
    }

    // Fold 32-bit sum to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    answer = ~sum;
    return answer;
}

std::vector<std::string> network_utils::get_bss_ifaces(const std::string &bss_iface,
                                                       const std::string &bridge_iface)
{
    if (bss_iface.empty()) {
        LOG(ERROR) << "bss_iface is empty!";
        return {};
    }
    if (bridge_iface.empty()) {
        LOG(ERROR) << "bridge_iface is empty!";
        return {};
    }

    auto ifaces_on_bridge = linux_get_iface_list_from_bridge(bridge_iface);

    /**
     * Find all interfaces that their name contain the base bss name.
     * On upstream Hostapd the pattern is: "<bss_iface_name>.staN"
     * (e.g wlan0.0.sta1, wlan0.0.sta2 etc)
     * On MaxLinear platforms the pattern is: "bN_<bss_iface_name>"
     * (e.g b0_wlan0.0, b1_wlan0.0 etc).
     *
     * NOTE: If the VAP interface is wlan-long0.0, then the STA interface name will use an
     * abbreviated version b0_wlan-long0 instead of b0_wlan-long0.0.
     * It doesn't really work anyway because with that truncation, you may get conflicts between
     * wlan-long0.0 and wlan-lang0.1.
     */

    std::vector<std::string> bss_ifaces;
    for (const auto &iface : ifaces_on_bridge) {
        if (iface.find(bss_iface) != std::string::npos) {
            bss_ifaces.push_back(iface);
        }
    }
    return bss_ifaces;
}

std::string network_utils::create_vlan_interface(const std::string &iface, uint16_t vid,
                                                 const std::string &suffix)
{
    if (iface.empty()) {
        LOG(ERROR) << "iface is empty!";
        return {};
    }

    if (vid < net::MIN_VLAN_ID || vid > net::MAX_VLAN_ID) {
        LOG(ERROR) << "Given VID is invalid: " << vid;
        return {};
    }

    // Command example:
    // ip link add <iface>.<vid> link <iface> type vlan id <vid>
    // ip link add <iface>.<suffix> ip link add <iface> type vlan id <vid>

    std::string cmd;
    // Reserve 80 bytes for appended data to prevent reallocations.
    cmd.reserve(80);

    std::string new_iface_name;
    new_iface_name.reserve(IFNAMSIZ);
    auto vid_str = std::to_string(vid);
    new_iface_name.assign(iface).append(".").append(suffix.empty() ? vid_str : suffix);

    cmd.assign("ip link add ")
        .append(new_iface_name)
        .append(" link ")
        .append(iface)
        .append(" type vlan id ")
        .append(vid_str);

    beerocks::os_utils::system_call(cmd);
    return new_iface_name;
}

void network_utils::delete_interface(const std::string &iface)
{
    if (iface.empty()) {
        LOG(ERROR) << "iface is empty!";
        return;
    }

    // Command example:
    // ip link delete <iface>

    std::string cmd;
    // Reserve 32 bytes for appended data to prevent reallocations.
    cmd.reserve(32);

    cmd.assign("ip link delete ").append(iface);
    beerocks::os_utils::system_call(cmd);
}

void network_utils::set_interface_state(const std::string &iface, bool is_up)
{
    if (iface.empty()) {
        LOG(ERROR) << "iface is empty!";
        return;
    }

    std::string cmd;
    // Reserve 32 bytes for appended data to prevent reallocations.
    cmd.reserve(32);

    // TODO: replace with non system calls PPM-1799
    cmd.assign("ip link set ").append(iface);
    if (is_up) {
        cmd.append(" up");
    } else {
        cmd.append(" down");
    }

    beerocks::os_utils::system_call(cmd);
}

bool network_utils::set_vlan_filtering(const std::string &bridge_iface, uint16_t default_vlan_id)
{
    if (bridge_iface.empty()) {
        LOG(ERROR) << "Given bridge interface name is empty!";
        return false;
    }

    // Command example:
    // Turn on:  ip link set <bridge_iface> type bridge vlan_filtering 1 vlan_default_pvid 10
    // Turn off: ip link set <bridge_iface> type bridge vlan_filtering 0

    std::string cmd;
    // Reserve 100 bytes for appended data to prevent reallocations.
    cmd.reserve(100);

    cmd.assign("ip link set ");

    cmd.append(bridge_iface).append(" type bridge vlan_filtering ");

    if (default_vlan_id == 0) {
        cmd.append("0 ");
    } else {
        cmd.append("1 vlan_default_pvid ").append(std::to_string(default_vlan_id));
    }

    beerocks::os_utils::system_call(cmd);
    return true;
}

bool network_utils::set_iface_vid_policy(const std::string &iface, bool del, uint16_t vid,
                                         bool is_bridge, bool pvid, bool untagged)
{
    if (vid > MAX_VLAN_ID) {
        LOG(ERROR) << "Given VID is invalid: " << vid;
        return false;
    }

    if (vid == 0 && pvid) {
        LOG(ERROR) << "Given VID is '0' (All VIDS) and PVID is set";
        return false;
    }

    // Command example:
    // bridge vlan {add|del} vid <vid> dev <iface> [ pvid ] [ untagged ] [ self (only on bridge)]

    std::string vid_str;
    vid_str.reserve(10);
    if (vid == 0) {
        vid_str.assign(std::to_string(MIN_VLAN_ID)).append("-").append(std::to_string(MAX_VLAN_ID));
    } else {
        vid_str.assign(std::to_string(vid));
    }

    std::string cmd;
    // Reserve 100 bytes for appended data to prevent reallocations.
    cmd.reserve(100);

    cmd.assign("bridge vlan ");

    if (del) {
        cmd.append("del ");
    } else {
        cmd.append("add ");
    }

    cmd.append("vid ").append(vid_str).append(" ");
    cmd.append("dev ").append(iface).append(" ");

    if (is_bridge) {
        cmd.append("self ");
    }

    if (del) {
        cmd.pop_back(); // Pop extra space.
        beerocks::os_utils::system_call(cmd);
        return true;
    }

    if (pvid) {
        cmd.append("pvid ");
    }

    if (untagged) {
        cmd.append("untagged ");
    }

    // Pop back last space character.
    cmd.pop_back();

    os_utils::system_call(cmd);
    return true;
}

bool network_utils::set_vlan_packet_filter(bool set, const std::string &bss_iface, uint16_t vid)
{
    if (bss_iface.empty()) {
        LOG(ERROR) << "Given BSS interface name is empty!";
        return false;
    }

    // VID
    if (vid > MAX_VLAN_ID) {
        LOG(ERROR) << "Skip invalid VID: " << vid;
        return false;
    }

    // Command example:
    // ebtables -t <table> -{A (Append)|D (Delete)} <Chain> [-p <protocol>]
    // [-J <jump target {ACCEPT|DROP|CONTINUE|RETURN}>] [-i <iface>]
    // If "-p 802_1q":
    //      [--vlan-id <vid>] [--vlan-encap <protocol>]

    // Using like this:
    // ebtables -t nat -{A|D} PREROUTING -p 802_1Q -j DROP -i <iface> --vlan-id <vid>
    // ebtables -t nat -{A|D} PREROUTING -p 802_1Q -j DROP -i <iface> --vlan-encap 802_1Q

    std::string cmd;
    cmd.reserve(150);

    cmd.append("ebtables -t nat ");

    // Before adding rule, remove existing rules.
    std::string vlan_filter_entry_cmd = cmd + "-L PREROUTING | grep " + bss_iface;
    std::string vlan_filter_entry_cmd_output =
        os_utils::system_call_with_output(vlan_filter_entry_cmd, true);
    auto lines = string_utils::str_split(vlan_filter_entry_cmd_output, '\n');
    for (const auto &line : lines) {
        std::string cmd_delete_old;
        cmd_delete_old.reserve(150);
        cmd_delete_old.append(cmd).append("-D PREROUTING ").append(line);
        os_utils::system_call(cmd_delete_old);
    }

    // If function called for removeing, the removal of rules finished above.
    if (!set) {
        return true;
    }

    // Append rule.
    cmd.append("-A ");

    cmd.append("PREROUTING -p 802_1Q -j DROP -i ").append(bss_iface);
    auto cmd_base_len = cmd.length();

    if (vid != 0) {
        // Filter packets carrying the VLAN tag of the interface.
        cmd.append(" --vlan-id ").append(std::to_string(vid));
        os_utils::system_call(cmd);
        cmd.erase(cmd_base_len);
    }

    // Filter double-tagged packets that are encapsulated with an S-Tag.
    cmd.append(" --vlan-encap 802_1Q");
    os_utils::system_call(cmd);
    return true;
}

sMacAddr network_utils::get_eth_sw_mac_from_bridge_mac(const sMacAddr &bridge_mac)
{
    sMacAddr mac = bridge_mac;
    /*
     * if bridge mac is already locally administrated,
     * then swap it to create base mac for eth switch
     * (as OUI is not relevant here anyway)
     */
    if (mac.oct[0] & 0x2) {
        size_t size = sizeof(mac.oct);
        for (size_t i = 0; i < size / 2; i++) {
            std::swap(mac.oct[i], mac.oct[size - 1 - i]);
        }
    }
    //then force the locally administrated mac address flag
    mac.oct[0] |= 0x2;
    return mac;
}

std::vector<std::string> network_utils::linux_get_bridges()
{
    std::vector<std::string> bridges;
    std::string net_path = "/sys/class/net/";
    DIR *dir             = opendir(net_path.c_str());
    if (!dir) {
        LOG(ERROR) << "opendir failed!";
        return {};
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] == '.') {
            continue;
        }

        std::string bridge_path = std::string(net_path) + entry->d_name + "/bridge";
        struct stat st;
        if (stat(bridge_path.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            bridges.push_back(entry->d_name);
        }
    }

    closedir(dir);
    return bridges;
}

std::string network_utils::linux_get_ifname_from_port(const std::string &br_ifname, int port_no)
{
    std::string brif_path = "/sys/class/net/" + br_ifname + "/brif/";
    DIR *dir              = opendir(brif_path.c_str());
    if (!dir) {
        return "";
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string if_name = entry->d_name;
        if (if_name == "." || if_name == "..") {
            continue;
        }

        std::string port_file = brif_path + if_name + "/port_no";
        std::ifstream file(port_file);
        if (file) {
            int file_port = 0;
            std::string content;
            file >> content;
            std::stringstream ss(content);
            ss >> std::hex >> file_port;

            if (file_port == port_no) {
                closedir(dir);
                return if_name;
            }
        }
    }

    closedir(dir);
    return "";
}

std::vector<struct __fdb_entry>
network_utils::linux_get_bridge_forwarding_table(const std::string &br_ifname, const sMacAddr &mac)
{
    std::vector<struct __fdb_entry> fdb_entries(MAX_FDB_ENTRIES);
    std::string path = "/sys/class/net/" + br_ifname + "/brforward";

    FILE *fd = fopen(path.c_str(), "r");
    if (!fd) {
        LOG(ERROR) << "Failed to open file " << path;
        return {};
    }

    size_t n = fread(fdb_entries.data(), sizeof(struct __fdb_entry), MAX_FDB_ENTRIES, fd);
    fclose(fd);

    fdb_entries.resize(n);

    if (mac != ZERO_MAC) {
        std::vector<struct __fdb_entry> filtered_entries;

        for (const auto &entry : fdb_entries) {
            auto entry_mac = tlvf::mac_from_array(entry.mac_addr);
            if (entry_mac == mac) {
                filtered_entries.push_back(entry);
            }
        }

        return filtered_entries;
    }

    return fdb_entries;
}

bool network_utils::linux_iface_is_physical(const std::string &iface)
{
    std::string iflink_path  = "/sys/class/net/" + iface + "/iflink";
    std::string ifindex_path = "/sys/class/net/" + iface + "/ifindex";

    std::ifstream iflink_file(iflink_path);
    std::ifstream ifindex_file(ifindex_path);

    if (!iflink_file || !ifindex_file) {
        LOG(ERROR) << "Failed to open iflink/ifindex files for " << iface;
        return false;
    }

    int iflink  = 0;
    int ifindex = 0;
    iflink_file >> iflink;
    ifindex_file >> ifindex;

    if (iflink_file.fail() || ifindex_file.fail()) {
        LOG(ERROR) << "Failed to read iflink/ifindex parameters for " << iface;
        return false;
    }

    /* Physical interfaces have the same 'iflink' and 'ifindex' values */
    return (iflink == ifindex) ? true : false;
}

std::vector<std::string> network_utils::linux_get_lan_interfaces()
{
    std::vector<std::string> lan_interfaces;

    auto bridges = linux_get_bridges();
    for (const auto &bridge : bridges) {
        auto iface_list = linux_get_iface_list_from_bridge(bridge);
        for (const auto &iface : iface_list) {
            std::string uevent_path = "/sys/class/net/" + iface + "/uevent";
            std::ifstream uevent_file(uevent_path);
            bool is_wlan = false;
            if (uevent_file.is_open()) {
                std::string line;
                while (std::getline(uevent_file, line)) {
                    if (line.find("DEVTYPE=wlan") != std::string::npos) {
                        is_wlan = true;
                        break;
                    }
                }
            } else {
                LOG(WARNING) << "Failed to open " << uevent_path;
                continue;
            }

            /* Skip wlan interfaces */
            if (is_wlan) {
                continue;
            }

            /* Skip non-physical interfaces (veth, dummy etc.)*/
            if (!linux_iface_is_physical(iface)) {
                continue;
            }

            lan_interfaces.push_back(iface);
        }
    }

    return lan_interfaces;
}
