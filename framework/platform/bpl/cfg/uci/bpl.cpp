/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bpl/bpl.h>

#include <mapf/common/logger.h>

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace beerocks {
namespace bpl {

int bpl_init()
{
    // Do nothing
    return 0;
}

void set_ambiorix_impl_ptr(const std::shared_ptr<beerocks::nbapi::Ambiorix> &ptr) {}

void bpl_close()
{
    // Do nothing
}

} // namespace bpl
} // namespace beerocks
