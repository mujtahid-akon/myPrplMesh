/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NL80211_CLIENT_H__
#define __BWL_NL80211_CLIENT_H__

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_message_structs.h>
#include <bcl/son/son_wireless_utils.h>
#include <bwl/base_wlan_hal_types.h>
#include <tlvf/AssociationRequestFrame/AssocReqFrame.h>
#include <tlvf/common/sMacAddr.h>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Length of HT MCS set.
 *
 * According to <linux/nl80211.h>, NL80211_BAND_ATTR_HT_MCS_SET is a 16-byte attribute containing
 * the MCS set as defined in 802.11n
 */
static constexpr size_t ht_mcs_set_size = beerocks::message::HT_MCS_SET_SIZE;

/**
 * @brief Length of VHT MCS set.
 *
 * According to <linux/nl80211.h>, NL80211_BAND_ATTR_VHT_MCS_SET is a 32-byte attribute containing
 * the MCS set as defined in 802.11ac
 */
static constexpr size_t vht_mcs_set_size = beerocks::message::VHT_MCS_SET_SIZE;

/**
 * @brief Length of HE MCS set.
 *
 * According to <linux/nl80211.h>, NL80211_BAND_IFTYPE_ATTR_HE_CAP_MCS_SET is a 32-byte attribute containing
 * the MCS set as defined in 802.11ax
 */
static constexpr size_t he_mcs_set_size = beerocks::message::HE_MCS_SET_SIZE;

namespace bwl {

/**
 * @brief NL80211 client interface.
 *
 * This interface lists the methods to read and write wireless hardware status and configuration
 * through a NL80211 socket.
 *
 * Programming to an interface allows dependent classes to remain unaware of the different
 * implementations of the interface as well as facilitates the creation of mock implementations for
 * unit testing.
 *
 * This is a C++ interface: an abstract class that is designed to be specifically used as a base
 * class and which derived classes (implementations) will override each pure virtual function.
 *
 * Known implementations: nl80211_client_impl (uses NL80211 socket), nl80211_client_dummy (fake
 * implementation that returns dummy data).
 */
class nl80211_client {

public:
    /**
     * @brief Interface information
     *
     * Information obtained with NL80211_CMD_GET_INTERFACE command through a NL80211 socket.
     * See <linux/nl80211.h> for a description of each field.
     */
    struct interface_info {

        /**
         * Network interface name.
         * Obtained from NL80211_ATTR_IFNAME
         */
        std::string name;

        /**
         * Network interface index of the device to operate on.
         * Obtained from NL80211_ATTR_IFINDEX
         */
        uint32_t index = 0;

        /**
         * MAC address of the interface.
         * Obtained from NL80211_ATTR_MAC
         */
        sMacAddr addr;

        /**
         * SSID of the interface.
         * Obtained from NL80211_ATTR_SSID
         */
        std::string ssid;

        /**
         * Type of virtual interface.
         * Obtained from NL80211_ATTR_IFTYPE
         * Possible values are defined in enum nl80211_iftype in <linux/nl80211.h>
         */
        uint32_t type = 0;

        /**
         * Index of wiphy to operate on.
         * Obtained from NL80211_ATTR_WIPHY
         */
        uint32_t wiphy = 0;

        /**
         * frequency of the selected channel in MHz.
         * Obtained from NL80211_ATTR_WIPHY_FREQ
         */
        uint32_t frequency = 0;
        uint32_t channel   = 0;

        /**
         * current channel width.
         * Obtained from NL80211_ATTR_CHANNEL_WIDTH
         */
        uint8_t bandwidth = 0;

        /*
         * Center frequency of the first part of the
         * channel, used for anything but 20 MHz bandwidth
         * Obtained from NL80211_ATTR_CENTER_FREQ1
         */
        uint32_t frequency_center1 = 0;

        /*
         * Center frequency of the second part of the
         * channel, used only for 80+80 MHz bandwidth
         * Obtained from NL80211_ATTR_CENTER_FREQ2
         */
        uint32_t frequency_center2 = 0;
    };

    /**
     * @brief Channel information
     *
     * Used in band_info structure as each band contains a map of supported channels.
     *
     * Information obtained with NL80211_CMD_GET_WIPHY command through a NL80211 socket.
     * See NL80211_FREQUENCY_ATTR_* in <linux/nl80211.h> for a description of each field.
     */
    struct channel_info {

        /**
         * Channel number.
         * Obtained from NL80211_FREQUENCY_ATTR_FREQ (see 802.11-2007 17.3.8.3.2 and Annex J)
         */
        uint8_t number = 0;

        /*
        Channel's center frequency
        */
        uint32_t center_freq = 0;

        /**
         * Supported channel bandwidths.
         */
        std::vector<beerocks::eWiFiBandwidth> supported_bandwidths;

        /**
         * Maximum transmission power in dBm.
         * Obtained as 0.01 * NL80211_FREQUENCY_ATTR_MAX_TX_POWER
         */
        uint8_t tx_power = 0;

        /**
         * Radar detection is mandatory on this channel in current regulatory domain.
         * Set to true if attribute NL80211_FREQUENCY_ATTR_RADAR is present.
         */
        bool is_dfs = false;

        /**
         * The DFS State defines if it is possible to switch to the channel, and if it is possible,
         * then, with or without CAC.
         * Obtained from NL80211_FREQUENCY_ATTR_DFS_STATE.
         */
        beerocks::eDfsState dfs_state;

        /**
         * True if current state for DFS is NL80211_DFS_UNAVAILABLE.
         * Obtained from NL80211_FREQUENCY_ATTR_DFS_STATE.
         */
        bool radar_affected = false;

        /**
         * This channel does not initiate radiation, meaning it is supported only when radar
         * detection is supported as well.
         * Set to true if attribute NL80211_FREQUENCY_ATTR_NO_IR is present
         */
        bool no_ir = false;

        /**
         * @brief Gets the maximum supported bandwidth by the channel.
         *
         * @return Maximum supported bandwidth
         */
        beerocks::eWiFiBandwidth get_max_bandwidth() const
        {
            if (supported_bandwidths.empty()) {
                return beerocks::eWiFiBandwidth::BANDWIDTH_UNKNOWN;
            }

            return *std::max_element(supported_bandwidths.begin(), supported_bandwidths.end());
        }
    };

    /**
     * @brief Band information
     *
     * Used in radio_info structure as each radio contains a list of bands.
     *
     * Information obtained with NL80211_CMD_GET_WIPHY command through a NL80211 socket.
     * See NL80211_BAND_ATTR_* in <linux/nl80211.h> for a description of each field.
     */
    struct band_info {

        /**
         * Is HT supported flag.
         * Set to true if NL80211_BAND_ATTR_HT_CAPA attribute is included in response.
         */
        bool ht_supported = false;

        /**
         * Value of NL80211_BAND_ATTR_HT_CAPA, see iw/util.c print_ht_capability() as a bit
         * interpretation example.
         */
        uint16_t ht_capability = 0;

        /**
         * Value of NL80211_BAND_ATTR_HT_MCS_SET, see iw/util.c print_ht_mcs() as a byte
         * interpretation example.
         */
        uint8_t ht_mcs_set[ht_mcs_set_size];

        /**
         * Is VHT supported flag.
         * Set to true if NL80211_BAND_ATTR_VHT_CAPA attribute is included in response.
         */
        bool vht_supported = false;

        /**
         * Value of NL80211_BAND_ATTR_VHT_CAPA, see iw/util.c print_vht_info() as a bit
         * interpretation example.
         */
        uint32_t vht_capability = 0;

        /**
         * Value of NL80211_BAND_ATTR_VHT_MCS_SET, see iw/util.c print_vht_info() as a byte
         * interpretation example.
         */
        uint8_t vht_mcs_set[vht_mcs_set_size];

        /**
         * Is HE supported flag.
         * Set to true if NL80211_BAND_ATTR_IFTYPE_DATA attribute is included in response.
         */
        bool he_supported = false;

        /**
         * Value of NL80211_BAND_IFTYPE_ATTR_HE_CAP_MCS_SET, see iw/util.c.
         */
        uint8_t he_mcs_set[he_mcs_set_size];

        /**
	 * Information obtained with NL80211_BAND_ATTR_IFTYPE_DATA command through a NL80211 socket.
	 * See nl80211_band_iftype_attr_* in <linux/nl80211.h> for a description of each field.
	 *
	 * HE capabilities are stored in structure and required bits are set in he_capability
	 * if it is available in NL80211_BAND_ATTR_IFTYPE_DATA
	 */
        uint16_t he_capability    = 0;
        uint64_t wifi6_capability = 0;
        struct ieee80211_he_capabilities {
            uint8_t he_mac_capab_info[6];
            uint8_t he_phy_capab_info[11];
            /**
	     * Followed by 4, 8, or 12 octets of Supported HE-MCS And NSS Set field
	     * and optional variable length PPE Thresholds field. PPE thresholds size can be
	     * a max of 25 octets : 7 bits HDR + (8 NSS * 4 RU * 6 bits) = 199 bits => 25 octets.
	     */
            uint8_t he_mcs_nss_set[12];
            uint8_t optional[25];
        } he;

        uint8_t ext_capability[15];

        /**
         * The Channels that are supported in this band.
         * The calculation of the channels is based on the frequenices
         * numbers that are obtained from NL80211_BAND_ATTR_FREQS in nl80211.h.
         * Map key is the channel number and map value is the channel information.
         */
        std::unordered_map<uint8_t, channel_info> supported_channels;

        /**
         * @brief Gets the frequency band of this band.
         *
         * Return value can be obtained from whatever of the supported channels.
         *
         * @return Frequency band
         */
        beerocks::eFreqType get_frequency_band() const
        {
            if (supported_channels.empty()) {
                return beerocks::eFreqType::FREQ_UNKNOWN;
            }

            return son::wireless_utils::which_freq_type(
                supported_channels.begin()->second.center_freq);
        }

        /**
         * @brief Checks if this band is the 5GHz band.
         *
         * @return True if this band is the 5GHz band and false otherwise.
         */
        bool is_5ghz_band() const { return beerocks::eFreqType::FREQ_5G == get_frequency_band(); }

        /**
         * @brief Gets the maximum supported bandwidth by all the channels in the band.
         *
         * @return Maximum supported bandwidth
         */
        beerocks::eWiFiBandwidth get_max_bandwidth() const
        {
            beerocks::eWiFiBandwidth max_bandwith = beerocks::eWiFiBandwidth::BANDWIDTH_UNKNOWN;

            for (const auto &it : supported_channels) {
                channel_info channel = it.second;
                max_bandwith         = std::max(max_bandwith, channel.get_max_bandwidth());
            }

            return max_bandwith;
        }
    };

    /**
     * @brief Radio information
     *
     * Information obtained with NL80211_CMD_GET_WIPHY command through a NL80211 socket.
     * See <linux/nl80211.h> for a description of each field.
     */
    struct radio_info {

        /**
         * Bands included in this radio (obtained from NL80211_ATTR_WIPHY_BANDS)
         */
        std::vector<band_info> bands;
    };

    /**
     * @brief Station information
     *
     * Information obtained with NL80211_CMD_GET_STATION command through a NL80211 socket.
     * See NL80211_STA_INFO_* in <linux/nl80211.h> for a description of each field.
     */
    struct sta_info {
        uint32_t inactive_time_ms   = 0;
        uint32_t rx_bytes           = 0;
        uint32_t rx_packets         = 0;
        uint32_t tx_bytes           = 0;
        uint32_t tx_packets         = 0;
        uint32_t tx_retries         = 0;
        uint32_t tx_failed          = 0;
        int8_t signal_dbm           = 0;
        uint8_t signal_avg_dbm      = 0;
        uint16_t tx_bitrate_100kbps = 0;
        uint16_t rx_bitrate_100kbps = 0;
        uint8_t dl_bandwidth        = 0; //beerocks::eWiFiBandwidth
    };

    /**
     * @brief Survey information for a channel.
     *
     * Information obtained with NL80211_CMD_GET_SURVEY command through a NL80211 socket.
     * See NL80211_SURVEY_INFO* in <linux/nl80211.h> for a description of each field.
     */
    struct sChannelSurveyInfo {
        /**
         * Center frequency of channel.
         */
        uint32_t frequency_mhz = 0;

        /**
         * Channel is currently being used.
         */
        bool in_use = false;

        /**
         * Noise level of channel (u8, dBm).
         */
        int8_t noise_dbm = 0;

        /**
         * Amount of time (in ms) that the radio was turned on (on channel or globally)
         */
        uint64_t time_on_ms = 0;

        /**
         * Amount of the time (in ms) the primary channel was sensed busy (either due to activity
         * or energy detect).
         */
        uint64_t time_busy_ms = 0;

        /**
         * Amount of time (in ms) the extension channel was sensed busy.
         */
        uint64_t time_ext_busy_ms = 0;

        /**
         * Amount of time (in ms) the radio spent receiving data (on channel or globally)
         */
        uint64_t time_rx_ms = 0;

        /**
         * Amount of time (in ms) the radio spent transmitting data (on channel or globally).
         */
        uint64_t time_tx_ms = 0;

        /**
         * Time (in ms) the radio spent for scan (on this channel or globally).
         */
        uint64_t time_scan_ms = 0;
    };

    class SurveyInfo {
    public:
        /**
         * List of survey information structures, one for each channel, as returned by the
         * NL80211_CMD_GET_SURVEY command.
         */
        std::vector<sChannelSurveyInfo> data;

        /**
         * @brief Gets channel utilization.
         *
         * The channel utilization is defined as the percentage of time, linearly scaled with 255
         * representing 100%, that the AP sensed the medium was busy. When more than one channel
         * is in use for the BSS, the channel utilization value is calculated only for the primary
         * channel.
         *
         * This percentage is computed using the formula:
         * channel_utilization = (channel busy time * 255) / channel on time
         *
         * @param[out] channel_utilization Channel utilization value on success and 0 if it cannot
         * be obtained.
         *
         * @return True on success and false if there is no channel in use or time on is zero
         * (i.e.: no data available) or time busy is greater than time on.
         */
        bool get_channel_utilization(uint8_t &channel_utilization) const;
    };

    /**
     * @brief Class destructor.
     */
    virtual ~nl80211_client() = default;

    /**
     * @brief Gets a list with the names of existing wireless VAP interfaces.
     *
     * @param[out] interfaces List with the names of wireless interfaces.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_interfaces(std::vector<std::string> &interfaces) = 0;

    /**
     * @brief Gets interface information.
     *
     * Interface information contains, among others, the MAC address and SSID of the given network
     * interface.
     *
     * @param[in] interface_name Interface name, either radio or Virtual AP (VAP).
     * @param[out] interface_info Interface information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_interface_info(const std::string &interface_name,
                                    interface_info &interface_info) = 0;

    /**
     * @brief Gets radio information.
     *
     * Radio information contains HT/VHT capabilities and the list of supported channels.
     *
     * @param[in] interface_name Interface name, either radio or Virtual AP (VAP).
     * @param[out] radio_info Radio information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_radio_info(const std::string &interface_name, radio_info &radio_info) = 0;

    /**
     * @brief Gets station information.
     *
     * Station information contains basically metrics associated to the link between given local
     * interface and a the interface of a station whose MAC address is 'sta_mac_address'.
     *
     * @param[in] interface_name Virtual AP (VAP) interface name.
     * @param[in] sta_mac_address MAC address of a station connected to the local interface.
     * @param[out] sta_info Station information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_sta_info(const std::string &interface_name, const sMacAddr &sta_mac_address,
                              sta_info &sta_info) = 0;

    /**
     * @brief Gets survey information.
     *
     * Survey information includes channel occupation and noise level.
     *
     * @param[in] interface_name Interface name, either radio or Virtual AP (VAP).
     * @param[out] survey_info List of survey information structures, one for each channel,
     * as returned by the NL80211_CMD_GET_SURVEY command.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_survey_info(const std::string &interface_name, SurveyInfo &survey_info) = 0;

    /**
     * @brief Set the tx power limit
     *
     * Set tx power limit for a radio
     *
     * @param[in] interface_name radio interface name.
     * @param[in] limit tx power limit in dBm to set
     * @return true success and false otherwise
     */
    virtual bool set_tx_power_limit(const std::string &interface_name, uint32_t limit) = 0;

    /**
     * @brief Get the tx power
     *
     * @param[in] interface_name radio interface name.
     * @param[out] power tx power in dBm.
     * @return true success and false otherwise
     */
    virtual bool get_tx_power_dbm(const std::string &interface_name, uint32_t &power) = 0;

    /**
     * @brief Abort the in-progress channel scan for the interface
     *
     * @param[in] interface_name radio interface name.
     *
     * @return true on success and false otherwise.
     */
    virtual bool channel_scan_abort(const std::string &interface_name) = 0;

    /**
     * @brief Add a key for a station.
     *
     * @param[in] interface_name the name of the interface to add a station for.
     * @param[in] key_info the key to add.
     *
     * @return true on success and false otherwise.
     */
    virtual bool add_key(const std::string &interface_name, const sKeyInfo &key_info) = 0;

    /**
     * @brief Manually add a station.
     *
     * @param[in] interface_name the name of the interface to add a station for.
     * @param[in] assoc_req the association request frame of a
     * previous association of the station (used for station
     * capabilities, listen_interval, etc).
     * @param[in] aid the association ID of the station.
     *
     * @return true on success and false otherwise.
     */
    virtual bool add_station(const std::string &interface_name, const sMacAddr &mac,
                             assoc_frame::AssocReqFrame &assoc_req, uint16_t aid) = 0;

    /**
     * @brief Get a key for a station.
     *
     * @param[in] interface_name the name of the interface to get a key for.
     * @param[in/out] key_info the MAC and key index [in]. The
     * information about the retrieved key [out].
     *
     * @return true on success and false otherwise.
     */
    virtual bool get_key(const std::string &interface_name, sKeyInfo &key_info) = 0;

    /**
     * @brief Send a DELBA frame to a specific station.
     *
     * @param[in] dst the destination MAC address.
     * @param[in] src the source MAC address.
     * @param[in] bssid the BSSID.
     *
     * @return true on success, false otherwise.
     */
    virtual bool send_delba(const std::string &interface_name, const sMacAddr &dst,
                            const sMacAddr &src, const sMacAddr &bssid) = 0;
};

} // namespace bwl

#endif
