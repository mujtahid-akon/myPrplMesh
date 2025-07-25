/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <algorithm>
#include <bcl/beerocks_string_utils.h>
#include <easylogging++.h>
#include <fstream>
#include <hostapd/configuration.h>
#include <mapf/common/encryption.h>

namespace prplmesh {
namespace hostapd {

Configuration::Configuration(const std::string &file_name) : m_configuration_file(file_name) {}

Configuration::operator bool() const { return m_ok; }

bool Configuration::update_required() const { return m_update_required; }

bool Configuration::load(const std::set<std::string> &vap_indications)
{
    // please take a look at README.md (common/beerocks/hostapd/README.md) for
    // the expected format of hostapd configuration file.
    // loading the file relies on the expected format
    // otherwise the load fails

    // for cases when load is called more than once, we
    // first clear internal data
    m_hostapd_config_head.clear();
    m_hostapd_config_vaps.clear();

    // start reading
    std::ifstream ifs(m_configuration_file);
    std::string line;

    bool parsing_vaps = false;
    std::string cur_vap;

    // go over line by line in the file
    while (getline(ifs, line)) {

        // skip empty lines
        if (std::all_of(line.begin(), line.end(), isspace)) {
            continue;
        }

        // check if the string belongs to a vap config part and capture which one.
        auto end_comment = line.find_first_not_of('#');
        auto end_key     = line.find_first_of('=');

        std::string current_key(line, end_comment, end_key + 1);

        auto vap_iterator = vap_indications.find(current_key);
        if (vap_iterator != vap_indications.end()) {
            // from now on we are in the vaps area, all
            // key/value pairs belongs to vaps
            parsing_vaps = true;

            // copy the vap value
            cur_vap = std::string(line, end_key + 1);
            m_hostapd_config_vaps.push_back(std::make_pair(cur_vap, std::vector<std::string>()));
        }

        // if not a vap line store it in the header part of the config,
        // otherwise add to the currently being parsed vap storage.
        if (!parsing_vaps) {
            m_hostapd_config_head.push_back(line);
        } else {
            // we always adding to the last vap that was inserted
            m_hostapd_config_vaps.back().second.push_back(line);
        }
    }

    std::stringstream load_message;
    load_message << "load() final message: os - " << strerror(errno) << "; existing vaps - "
                 << parsing_vaps;
    m_last_message = load_message.str();

    // if we've got to parsing vaps and no read errors, assume all is good
    m_ok = parsing_vaps && !ifs.bad();

    // reset the update required default state
    m_update_required = false;
    // return this as bool
    return *this;
}

bool Configuration::store()
{
    std::ofstream out_file(m_configuration_file, std::ofstream::out | std::ofstream::trunc);

    // store the head
    for (const auto &line : m_hostapd_config_head) {
        out_file << line << "\n";
    }

    // store the next ones ('bss=')
    for (auto &vap : m_hostapd_config_vaps) {
        std::stringstream bss_conf;
        bool config_id_supported = false;

        // add empty line for readability
        out_file << "\n";
        for (auto &line : vap.second) {
            // skip any existing config_id line (e.g. set by netfid),
            // it will be set again at the end.
            if (line.rfind("config_id=", 0) == 0) {
                config_id_supported = true;
                continue;
            }
            out_file << line << "\n";
            bss_conf << line << "\n";
        }
        if (config_id_supported) {
            // add the config_id at the end:
            uint8_t config_id[32];
            mapf::encryption::sha256 sha;
            sha.update(reinterpret_cast<const uint8_t *>(bss_conf.str().c_str()),
                       bss_conf.str().size());
            if (!sha.digest(config_id)) {
                LOG(ERROR) << "Unable to compute sha256 of the configuration!";
                return false;
            };
            out_file << "config_id=";
            for (const auto &c : config_id) {
                out_file << beerocks::string_utils::int_to_hex_string(c, 2);
            }
            out_file << "\n";
        }
    }

    m_ok              = true;
    m_last_message    = m_configuration_file + " was stored";
    m_update_required = false;

    // close the file
    out_file.close();
    if (out_file.fail()) {
        m_last_message    = strerror(errno);
        m_ok              = false;
        m_update_required = true;
    }

    return *this;
}

bool Configuration::set_create_head_value(const std::string &key, const std::string &value)
{
    // search for the key
    std::string key_eq(key + "=");
    auto line_iter = std::find_if(
        m_hostapd_config_head.begin(), m_hostapd_config_head.end(),
        [&key_eq, this](const std::string &line) -> bool { return is_key_in_line(line, key_eq); });

    // we first delete the key, and if the requested value is non empty
    // we push it to the end of the array

    // delete the key-value if found
    if (line_iter != m_hostapd_config_head.end()) {
        line_iter = m_hostapd_config_head.erase(line_iter);
    } else {
        m_last_message =
            std::string(__FUNCTION__) + " the key '" + key + "' for head was not found";
    }

    // when the new value is provided add the key back with that new value
    if (value.length() != 0) {
        m_hostapd_config_head.push_back(key_eq + value);
        m_last_message = std::string(__FUNCTION__) + " the key '" + key + "' was (re)added to head";
    } else {
        m_last_message = std::string(__FUNCTION__) + " the key '" + key + "' was deleted from head";
    }

    m_ok = true;
    return *this;
}

bool Configuration::set_create_head_value(const std::string &key, const int value)
{
    return set_create_head_value(key, std::to_string(value));
}

std::string Configuration::get_head_value(const std::string &key)
{
    std::string key_eq(key + "=");
    auto line_iter = std::find_if(
        m_hostapd_config_head.begin(), m_hostapd_config_head.end(),
        [&key_eq, this](const std::string &line) -> bool { return is_key_in_line(line, key_eq); });

    if (line_iter == m_hostapd_config_head.end()) {
        m_last_message = std::string(__FUNCTION__) + " couldn't find requested key in head: " + key;
        return "";
    }

    // return from just after the '=' sign to the end of the string
    return line_iter->substr(line_iter->find('=') + 1);
}

const std::string Configuration::get_vap_by_bssid(const std::string &bssid) const
{
    auto bssid_matches = [&bssid](const std::string &line) -> bool {
        // Split the line into two parts according to the following pattern:
        //      "Key=Value"
        const auto parts = beerocks::string_utils::str_split(line, '=');
        if (parts.size() < 2) {
            // Line does not match pattern
            return false;
        }
        if (parts.at(0) != "bssid") {
            // Line is not the bssid parameter
            return false;
        }
        // Check if BSSID matches using case-insensitive comparison.
        return beerocks::string_utils::case_insensitive_compare(parts.at(1), bssid);
    };
    for (const auto &vap_iter : m_hostapd_config_vaps) {
        // Search the current vap if its BSSID matches the given one.
        if (std::find_if(vap_iter.second.begin(), vap_iter.second.end(), bssid_matches) !=
            vap_iter.second.end()) {
            // Found the vap we are searching for, return it's name (key in the hostapd map).
            return vap_iter.first;
        }
    }
    // Return an empty string, meaning that the vap was not found.
    return std::string();
}

bool Configuration::set_create_vap_value(const std::string &vap, const std::string &key,
                                         const std::string &value)
{
    // search for the requested vap
    auto find_vap = get_vap(std::string(__FUNCTION__) + " key/value: " + key + '/' + value, vap);
    if (!std::get<0>(find_vap)) {
        return false;
    }
    auto existing_vap           = std::get<1>(find_vap);
    bool existing_vap_commented = existing_vap->front()[0] == '#';

    std::string key_eq(key + "=");
    auto line_iter = std::find_if(
        existing_vap->begin(), existing_vap->end(),
        [&key_eq, this](const std::string &line) -> bool { return is_key_in_line(line, key_eq); });

    // we first delete the key, and if the requested value is non empty
    // we push it to the end of the array

    // delete the key-value if found
    if (line_iter != existing_vap->end()) {
        line_iter = existing_vap->erase(line_iter);
    } else {
        m_last_message =
            std::string(__FUNCTION__) + " the key '" + key + "' for vap " + vap + " was not found";
    }

    // when the new value is provided add the key back with that new value
    if (value.length() != 0) {
        if (existing_vap_commented) {
            existing_vap->push_back('#' + key_eq + value);
        } else {
            existing_vap->push_back(key_eq + value);
        }
        m_last_message =
            std::string(__FUNCTION__) + " the key '" + key + "' for vap " + vap + " was (re)added";
    } else {
        m_last_message =
            std::string(__FUNCTION__) + " the key '" + key + "' for vap " + vap + " was deleted";
    }

    m_ok              = true;
    m_update_required = true;
    return *this;
}

bool Configuration::set_create_vap_value(const std::string &vap, const std::string &key,
                                         const int value)
{
    return set_create_vap_value(vap, key, std::to_string(value));
}

bool Configuration::set_create_value(const std::string &vap, const std::string &key,
                                     const std::string &value)
{
    if (vap.empty()) {
        return set_create_head_value(key, value);
    }
    return set_create_vap_value(vap, key, value);
}

bool Configuration::set_create_value(const std::string &vap, const std::string &key,
                                     const int value)
{
    if (vap.empty()) {
        return set_create_head_value(key, std::to_string(value));
    }
    return set_create_vap_value(vap, key, std::to_string(value));
}

std::string Configuration::get_vap_value(const std::string &vap, const std::string &key)
{
    // search for the requested vap
    auto find_vap = get_vap(std::string(__FUNCTION__), vap);
    if (!std::get<0>(find_vap)) {
        return "";
    }
    const auto &existing_vap = std::get<1>(find_vap);

    // from now on this function is ok with all situations
    // (e.g. not finding the requested key)
    m_ok = true;

    std::string key_eq(key + "=");
    auto line_iter = std::find_if(
        existing_vap->begin(), existing_vap->end(),
        [&key_eq, this](const std::string &line) -> bool { return is_key_in_line(line, key_eq); });

    if (line_iter == existing_vap->end()) {
        m_last_message = std::string(__FUNCTION__) +
                         " couldn't find requested key for vap: " + vap + "; requested key: " + key;
        return "";
    }

    // return from the just after the '=' sign to the end of the string
    return line_iter->substr(line_iter->find('=') + 1);
}

bool Configuration::disable_vap(const std::string &vap)
{
    if (!set_create_vap_value(vap, "start_disabled", "1")) {
        LOG(ERROR) << "Unable to set start_disabled on vap '" << vap << "'.";
        return false;
    }
    if (!set_create_vap_value(vap, "ssid", "")) {
        LOG(ERROR) << "Unable to remove ssid on vap '" << vap << "'.";
        return false;
    }
    if (!set_create_vap_value(vap, "multi_ap", "0")) {
        LOG(ERROR) << "Unable to set multi_ap on vap '" << vap << "'.";
        return false;
    }
    if (!set_create_vap_value(vap, "multi_ap_backhaul_ssid", "")) {
        LOG(ERROR) << "Unable to remove multi_ap_backhaul_ssid on vap '" << vap << "'.";
        return false;
    }
    if (!set_create_vap_value(vap, "multi_ap_backhaul_wpa_passphrase", "")) {
        LOG(ERROR) << "Unable to remove multi_ap_backhaul_wpa_passphrase on vap '" << vap << "'.";
        return false;
    }
    return true;
}

const std::string &Configuration::get_last_message() const { return m_last_message; }

std::tuple<bool, std::vector<std::string> *>
Configuration::get_vap(const std::string &calling_function, const std::string &vap)
{
    // search for the requested vap - ignore comments
    // by searching from the back of the saved vap (rfind)
    auto existing_vap =
        std::find_if(m_hostapd_config_vaps.begin(), m_hostapd_config_vaps.end(),
                     [&vap](const std::pair<std::string, std::vector<std::string>> &current_vap) {
                         return current_vap.first.rfind(vap) != std::string::npos;
                     });

    if (existing_vap == m_hostapd_config_vaps.end()) {
        m_last_message = calling_function + " couldn't find requested vap: " + vap;
        m_ok           = false;
        return std::make_tuple(false, nullptr);
    }

    m_ok = true;
    return std::make_tuple(true, &existing_vap->second);
}

bool Configuration::is_key_in_line(const std::string &line, const std::string &key) const
{
    // we need to make sure when searching for example
    // for "ssid", to ignore cases like:
    // multi_ap_backhaul_ssid="Multi-AP-24G-2"
    //                   ^^^^^
    // bssid=02:9A:96:FB:59:11
    //  ^^^^^
    // and we need to take into consideration
    // that the key might be or might not be commented.
    // so the search algorithm is:
    // - find the requested key and
    // - make sure it is either on the first position
    // or it has a comment sign just before it
    auto found_pos = line.rfind(key);
    bool ret = found_pos != std::string::npos && (found_pos == 0 || line.at(found_pos - 1) == '#');

    return ret;
}

std::ostream &operator<<(std::ostream &os, const Configuration &conf)
{
    os << "== configuration details ==\n"
       << "= ok:           " << conf.m_ok << '\n'
       << "= last message: " << conf.m_last_message << '\n'
       << "= file:         " << conf.m_configuration_file << '\n'
       << "= head:         " << '\n';

    for (const auto &line : conf.m_hostapd_config_head) {
        os << line << '\n';
    }

    os << "== vaps (total of: " << conf.m_hostapd_config_vaps.size() << " vaps) ==\n";

    for (const auto &vap : conf.m_hostapd_config_vaps) {
        os << "   vap: " << vap.first << "\n";
        for (const auto &line : vap.second) {
            os << line << '\n';
        }
    }

    return os;
}

} // namespace hostapd
} // namespace prplmesh
