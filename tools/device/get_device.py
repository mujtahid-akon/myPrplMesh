###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020-2022 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################
"""Utility module to get the correct device."""

# Standard library
from typing import Union

# Third party
import device.generic
import device.prplos
import device.turris_prplos
import device.turris_rdk_b
import device.haze
import device.urx_osp
import device.freedom


def device_from_name(name: str, target_name: str, image: Union[str, None] = None
                     ) -> device.generic.GenericDevice:
    """Construct a device based on its name and type.

    Parameters
    ----------
    name: str
        The name of the device
    target_name: str
        The name of the target.
    image: image: Union[str, None]
        The name of the image (optional, defaults to None).
    """
    if name == "turris-omnia-rdk":
        dev = device.turris_rdk_b.TurrisRdkb(name, target_name, image)
    elif name == "turris-omnia":
        dev = device.turris_prplos.TurrisPrplOS(name, target_name, image)
    elif name == "haze":
        dev = device.haze.Haze(name, target_name, image)
    elif name == "urx_osp" or name == "urx_ospv2":
        dev = device.urx_osp.URXOSP(name, target_name, image)
    elif name == "freedom":
        dev = device.freedom.Freedom(name, target_name, image)
    else:
        # if no device matched, try the generic prplOS (sysupgrade)
        print("No specific device matched, using GenericPrplOS.")
        dev = device.prplos.GenericPrplOS(name, target_name, image)
    return dev
