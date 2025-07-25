#!/bin/bash -e
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

printf '\033[1;35m%s Configuring prplWrt\n\033[0m' "$(date --iso-8601=seconds --universal)"
mkdir -p files/etc
#   We need to keep the hashes in the firmware, to later know if an upgrade is needed:
{
    printf '%s=%s\n' "OPENWRT_REPOSITORY" "$OPENWRT_REPOSITORY"
    printf '%s=%s\n' "OPENWRT_VERSION" "$OPENWRT_VERSION"
    printf '%s=%s\n' "OPENWRT_TOOLCHAIN_VERSION" "$OPENWRT_TOOLCHAIN_VERSION"
} >> files/etc/prplwrt-version

# Arguments to gen_config.py:
args=("$TARGET_SYSTEM")

# The additional profiles that will be used. 'debug' contains
# additional packages that are useful when developing:
args+=("debug")

if [ "$TARGET_SYSTEM" = "intel_mips" ] || [ "$TARGET_SYSTEM" = "mxl_x86_osp_tb341" ] || [ "$TARGET_SYSTEM" = "qca_ipq95xx" ]; then
    # intel_mips depends on iwlwav-iw, which clashes with iw-full:
    sed -i '/iw-full$/d' "profiles/debug.yml"
fi

if [ "$TARGET_SYSTEM" = "mxl_x86_osp_tb341" ]; then
    # add open source hostap introduced in MXL 9.1.15 code base
    args+=("mxl_wlan_hostap_ng")
fi

# args+=("webui")

# feed-prpl is in the prpl profile:
if [ -n "$WHM_ENABLE" ] ; then
    args+=("prpl")
else
    sed -e '/- pwhm/d' -e '/- libswlc/d' -e '/- libswla/d'  -e '/CONFIG_USE_PRPLMESH_WHM=y/d' -e '/CONFIG_SAH_WLD_INIT_SCRIPT="prplmesh_whm"/d' profiles/prpl.yml > profiles/prpl-no-whm.yml
    echo "          CONFIG_USE_PRPLMESH_WHM=n" >> profiles/prpl-no-whm.yml
    args+=("prpl-no-whm")
fi

./scripts/gen_config.py "${args[@]}"

# The initial 'make defconfig' invocation generates a wrong config, so
# run it again. Remove this workaround once PPM-2279 is fixed.
make defconfig

for profile in "${args[@]}" ; do
    printf "\nProfile %s:\n" "${profile}" >> files/etc/prplwrt-version
    cat "profiles/${profile}.yml" >> files/etc/prplwrt-version
done

printf '\033[1;35m%s Building prplWrt\n\033[0m' "$(date --iso-8601=seconds --universal)"
make -j"$(nproc)" V=sc

printf '\033[1;35m%s Cleaning prplMesh\n\033[0m' "$(date --iso-8601=seconds --universal)"
make package/prplmesh/clean
