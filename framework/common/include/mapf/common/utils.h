/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MAPFUTILS_H_
#define MAPFUTILS_H_

#include <mutex>
#include <string>

namespace mapf {

/**
 * @brief mapf utility functions
 */
namespace utils {

/**
 * @brief dump buffer in hex mode
 * 
 * Debugging utility to get a string representation of a given
 * buffer in hexadecimal, split to bytes.
 * 
 * @param buffer input buffer
 * @param len input buffer length
 * @return std::string string containing the hexadecimal buffer contets
 */
std::string dump_buffer(uint8_t *buffer, size_t len);

/**
 * @brief copy string from buffer pointed by src into buffer pointed by dst
 * 
 * @param dst destination buffer
 * @param src src buffer
 * @param dst_len destination length
 */
void copy_string(char *dst, const char *src, size_t dst_len);

/**
 * @brief Find the install directory.
 * @return  if success path to install directory (one directory up) ending with '/', otherwise
 *          empty string.
 */
std::string get_install_path();

} // namespace utils
} // namespace mapf

namespace beerocks {

extern std::mutex amxp_signal_read_mutex;
} // namespace beerocks
#endif // MAPFUTILS_H_
