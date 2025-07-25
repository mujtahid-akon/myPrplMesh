/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_UTILS_H_
#define _BEEROCKS_UTILS_H_

#include "beerocks_defines.h"
#include "beerocks_string_utils.h"

#include <chrono>
#include <cstddef>
#include <fcntl.h>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_set>

namespace beerocks {
class utils {
public:
    ///
    /// @brief Function template to isolate unused variables in the source code
    /// Note : This is a tool to ONLY help building the source code locally and a way
    /// to resolve unused variables before passing onto Gerrit. <Helps isolate and search for
    /// such variables >
    ///
    template <class T> static void IGNORE_UNUSED_VARIABLE(const T &) {}

    ///
    /// @brief Template function to automatically fill in parameters to memset with type safety.
    ///
    /// @return void since the call is made through a reference, returning a pointer does not make
    ///     sense.
    ///
    template <typename _Tp> static void zero_memory(_Tp &v)
    {
        memset(static_cast<void *>(&v), 0, sizeof(v));
    }

    typedef struct {
        std::string iface_prefix;
        char iface_sep  = 0;
        int8_t iface_id = beerocks::IFACE_ID_INVALID;
        int8_t vap_id   = beerocks::IFACE_ID_INVALID;
    } sIfaceVapIds;

    template <typename A>
    static typename std::enable_if<std::is_array<A>::value, size_t>::type array_length(const A &a)
    {
        return std::extent<A>::value;
    }

    static int write_to_file(const std::string &full_path, const std::string &val);
    static beerocks::eIfaceType get_iface_type_from_string(const std::string &iface_type_name);
    static std::string get_iface_type_string(beerocks::eIfaceType iface_type);
    static bool is_device_wireless(beerocks::eIfaceType iface_type);
    /**
    * @brief This function will return the default prefix string for wireless interface names.
    *
    * @return std::string default prefix (the first element in the allowed prefix list).
    */
    static std::string get_default_ifname_prefix();

    /**
    * @brief This function will return the default prefix separator character for wireless VAP interface names.
    *
    * @return char default prefix separator (the first element in the allowed prefix separators list).
    */
    static char get_default_ifname_separator();

    /**
    * @brief This function will check if the interface name prefix is allowed.
    * The check can de done by
    * - a total match : prefix equals one of allowed prefix string list entries.
    * - a partial match: prefix string starts with one of allowed prefix string list entries.
    *
    * @param[in] prefix : interface name prefix to be checked
    * @param[in] partial: flag to require total or partial matching of the argument prefix in the allowed list.
    * @return bool true if the prefix is allowed, false otherwise.
    */
    static bool is_allowed_ifname_prefix(const std::string &prefix, bool partial = false);
    /**
    * @brief This function will return the prefix string in the argument interface name
    * following one of these patterns:
    * - PREFIXn
    * - PREFIXn.m
    * where n and m are digit sequences
    *
    * @param[in] iface : interface name string
    * @return std::string prefix if interface name matches the naming pattern, empty string otherwise.
    */
    static std::string get_prefix_from_iface_string(const std::string &iface);
    static sIfaceVapIds get_ids_from_iface_string(const std::string &iface);
    static std::string get_iface_string_from_iface_vap_ids(int8_t iface_id, int8_t vap_id);
    static std::string get_iface_string_from_iface_vap_ids(const std::string &iface, int8_t vap_id);

    static beerocks::eWiFiBandwidth convert_bandwidth_to_enum(int bandwidth_int);
    static std::string convert_bandwidth_to_string(beerocks::eWiFiBandwidth bandwidth);
    static int count_target_bandwidth(beerocks::eWiFiBandwidth bandwidth,
                                      beerocks::eWiFiBandwidth target_bandwidth);
    static std::string convert_frequency_type_to_string(beerocks::eFreqType freq_type);
    static std::string convert_channel_ext_above_to_string(bool channel_ext_above_secondary,
                                                           beerocks::eWiFiBandwidth bandwidth);
    static void hex_dump(const std::string &description, uint8_t *addr, int len,
                         const char *calling_file = __builtin_FILE(),
                         int calling_line         = __builtin_LINE());
    static std::string dump_buffer(const uint8_t *buffer, size_t len);
    /**
    * @brief This function will return a time-date string format as defined in ISO 8601.
    * ISO 8601 is aligned to the following format:
    *   '\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d+Z|[\+\-]\d{2}:\d{2})'
    * For example: 2016-09-28T14:50:31.456449Z or 2016-09-28T14:50:31.456449+06:00
    * 
    * @param[in] timestamp a system_clock timepoint, defaulted to now()
    * @return ISO 8601 aligned std::string or empty string in case of failure.
    */
    static std::string get_ISO_8601_timestamp_string(
        std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());

    /**
     * @brief Returns the value for a given enum in the flag.
     * 
     * @param[in] bitwise_flag a bitwise flag of the ZWDFS subfeatures.
     * @param[in] flag enum of the ZWDFS flag.
     * 
     * @return The specific value within the bitwise flag.
     */
    static bool compare_zwdfs_flag(const int bitwise_flag, eZWDFS_flags flag);

    /**
     * @brief Returns the boolean values of the bitwise ZWDFS flag.
     * 
     * @param[in] bitwise_flag a bitwise flag of the ZWDFS subfeatures.
     * @param[out] on_radar True if ZWDFS On-Radar is enabled, false otherwise.
     * @param[out] on_selection True if ZWDFS On-Selection is enabled, false otherwise.
     * @param[out] pre_cac True if ZWDFS Pre-CAC is enabled, false otherwise.
     */
    static void get_zwdfs_flags(const int bitwise_flag, bool &on_radar, bool &on_selection,
                                bool &pre_cac);

    /**
     * @brief Returns a string describing what ZWDFS subfeatures are supported.
     * 
     * @param[in] bitwise_flag a bitwise flag of the ZWDFS subfeatures.
     * @return String with the supported ZWDFS subfeatures.
     */
    static const std::string get_zwdfs_string(const int bitwise_flag);

    static eFreqType get_freq_type_from_op_class(const uint8_t op_class);
};

} //namespace beerocks

#endif //_BEEROCKS_UTILS_H_
