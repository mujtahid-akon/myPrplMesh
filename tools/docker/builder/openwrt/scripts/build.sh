#!/bin/bash -e

# We have to copy the source directory, because we may not have
# write access to it, and openwrt needs to at least write '.source_dir':
cp -r /home/openwrt/prplMesh_source /home/openwrt/prplMesh
# We want to make sure that we do not keep anything built from the host:
(cd /home/openwrt/prplMesh && \
    rm -rf build ipkg-* .built* .configured* .pkgdir .prepared .quilt_checked .source_dir)

make package/prplmesh/prepare USE_SOURCE_DIR="/home/openwrt/prplMesh" V=s
# Rebuild the full image:

if ! make -j"$(nproc)" ; then
    # Building failed. Rebuild with V=sc, but exit immediately even if
    # the second build succeeds (to let the user/CI know that the
    # parallel build failed).
    echo "Build failed. Rebuilding with -j1."
    make V=sc
    exit 1
fi

mkdir -p artifacts
cat << EOT >> artifacts/prplmesh.buildinfo
TARGET_SYSTEM=${TARGET_SYSTEM}
OPENWRT_VERSION=${OPENWRT_VERSION}
OPENWRT_TOOLCHAIN_VERSION=${OPENWRT_TOOLCHAIN_VERSION}
PRPLMESH_VERSION=${PRPLMESH_VERSION}
EOT

# If the target is OSP URX; move the build files from the intel_x86 target directory
TARGET_SYSTEM=${TARGET_SYSTEM//mxl_x86_osp_tb341/intel_x86}
TARGET_SYSTEM=${TARGET_SYSTEM//qca_ipq95xx/ipq95xx}

find bin -name 'prplmesh_*.ipk' -exec cp -v {} "artifacts/prplmesh.ipk" \;
find bin/targets/"$TARGET_SYSTEM"/*/ -type f -maxdepth 1 -exec cp -v {} "artifacts/" \;
# Rename the prplos image
find artifacts/ -type f -name 'prplos-*' -exec bash -c 'mv $0 ${0/\prplos/openwrt}' {} \;
cp .config artifacts/openwrt.config
cp files/etc/prplwrt-version artifacts/
