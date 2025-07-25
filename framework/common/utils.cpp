/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <algorithm>
#include <iomanip>
#include <limits.h>
#include <mapf/common/logger.h>
#include <mapf/common/utils.h>
#include <sstream>
#include <unistd.h>

namespace mapf {
namespace utils {

std::string get_install_path()
{
    char path_install_file[PATH_MAX + 1];
    std::string path_install_dir;

    auto len = readlink("/proc/self/exe", path_install_file, sizeof(path_install_file) - 1);
    if (len == -1) {
        return std::string();
    }
    path_install_file[len] = '\0';
    path_install_dir       = path_install_file;
    auto end_search        = path_install_dir.rfind("/");
    if (end_search == std::string::npos) {
        return std::string();
    }
    end_search = path_install_dir.rfind("/", end_search - 1);
    if (end_search == std::string::npos) {
        return std::string();
    }
    return path_install_dir.substr(0, end_search + 1);
}

std::string dump_buffer(uint8_t *buffer, size_t len)
{
    std::ostringstream hexdump;
    for (size_t i = 0; i < len; i += 16) {
        for (size_t j = i; j < len && j < i + 16; j++)
            hexdump << std::hex << std::setw(2) << std::setfill('0') << (unsigned)buffer[j] << " ";
        hexdump << std::endl;
    }
    return hexdump.str();
}

void copy_string(char *dst, const char *src, size_t dst_len)
{
    const char *src_end = std::find(src, src + dst_len, '\0');
    std::copy(src, src_end, dst);
    std::ptrdiff_t src_size = src_end - src;
    std::ptrdiff_t dst_size = dst_len;
    if (src_size < dst_size) {
        dst[src_size] = 0;
    } else {
        dst[dst_size - 1] = 0;
        MAPF_ERR("copy_string() overflow, src string:'" << src << "'"
                                                        << " dst_size=" << dst_size << std::endl);
    }
}

} // namespace utils
} // namespace mapf

namespace beerocks {

std::mutex amxp_signal_read_mutex;

} // namespace beerocks
