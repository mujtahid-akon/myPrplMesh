#!/bin/sh

set -e

# VLAN interface to control the device separatly:
ip link add link lan4 name lan4.200 type vlan id 200
ip addr add 192.168.200.140/24 dev lan4.200
iptables -P INPUT ACCEPT

# Prevent DHCP/missing IP log spamming on the serial console
ip addr add 10.10.0.1/24 dev erouter0 || true
rm /lib/rdk/udhcpc_check.sh || true
