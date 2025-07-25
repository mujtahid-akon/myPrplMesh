#!/bin/sh

set -e

# Start with a new log file:
rm -f /var/log/messages ; syslog-ng-ctl reload || true

data_overlay_not_initialized()
{
  grep -q overlayfs:/tmp/root /proc/mounts || test -f /tmp/.switch_jffs2 || pgrep 'mount_root done'
}

if data_overlay_not_initialized; then
  logger -t prplmesh -p daemon.info "Waiting for data overlay initialization..."
  while data_overlay_not_initialized; do
    sleep 2
  done
  logger -t prplmesh -p daemon.info "Data overlay is initialized."
fi
sleep 5

ubus wait_for DHCPv4
ubus wait_for DHCPv6

# Stop and disable the DHCP clients and servers:
ubus call DHCPv4.Client.1 _set '{"parameters": { "Enable": False }}'
ubus call DHCPv6.Client.1 _set '{"parameters": { "Enable": False }}'
ubus call DHCPv4.Server _set '{"parameters": { "Enable": False }}'
ubus call DHCPv6.Server _set '{"parameters": { "Enable": False }}'

# Save the IP settings persistently (PPM-2351):
sed -ri 's/(dm-save.*) = false/\1 = true/g' /etc/amx/ip-manager/ip-manager.odl
/etc/init.d/ip-manager restart
sleep 15

ubus wait_for IP.Interface

# Set the LAN bridge IP:
ubus call "IP.Interface" _set '{ "rel_path": ".[Name == \"br-lan\"].IPv4Address.[Alias == \"lan\"].", "parameters": { "IPAddress": "192.165.100.180" } }'

# Move the WAN port into the LAN bridge if it's not there yet (to use it for data):
ubus wait_for Bridging.Bridge
ubus call "Bridging.Bridge" _get '{ "rel_path": ".[Alias == \"lan\"].Port.[Name == \"eth1\"]." }' || {
    echo "Adding interface to bridge"
    ubus call "Bridging.Bridge" _add '{ "rel_path": ".[Alias == \"lan\"].Port.",  "parameters": { "Name": "eth1", "Alias": "ETH1", "Enable": true } }'
}

# One of the LAN ports is used for control. Create a section for it:
uci set network.cert=interface
# Setting ifname is not supported in the current version of the TR-181
# IP manager (v1.11.1), set it in UCI instead:
uci set network.cert.ifname='eth0_4'

# Remove the control interface from the LAN bridge if it's not already the case:
ubus wait_for Bridging.Bridge
ubus call "Bridging.Bridge" _get '{ "rel_path": ".[Alias == \"lan\"].Port.[Name == \"eth0_4\"]." }' && {
    echo "Removing interface from bridge"
    ubus call "Bridging.Bridge" _del '{ "rel_path": ".[Alias == \"lan\"].Port.[Name == \"eth0_4\"]." }'
}

# To set the IP on the control interface, we first need to find the
# corresponding Ethernet.Interface:
ETH_IF="$(ubus call Ethernet.Interface _list | jsonfilter -e '@.instances[@.name="ETH0_4"].index')"
# Then if there is no corresponding Ethernet.Link yet, we need to add
# one:
ubus call Ethernet.Link _get '{ "rel_path": ".[Name == \"eth0_4\"]." }' || {
    echo "Adding Ethernet Link"
    ETH_LINK="$(ubus call Ethernet.Link _add "{ \"parameters\": { \"Name\": \"eth0_4\", \"Alias\": \"eth0_4\",\"LowerLayers\": \"Device.Ethernet.Interface.$ETH_IF.\", \"Enable\": true } }" | jsonfilter -e '@.index')"
}
# We can now create an IP.Interface if there is none yet:
LAN_INTERFACE="IP.Interface"
ubus call IP.Interface _get '{ "rel_path": ".[Name == \"eth0_4\"]." }' || {
    echo "Adding IP.Interface"
    LAN_INTERFACE="IP.Interface.""$(ubus call IP.Interface _add "{ \"parameters\": { \"Name\": \"eth0_4\", \"UCISectionNameIPv4\": \"cert\", \"Alias\": \"eth0_4\", \"LowerLayers\": \"Device.Ethernet.Link.$ETH_LINK.\", \"Enable\": true } }" | jsonfilter -e '@.index')"
    
    # Create an SSH server on the control interface if there is none
    echo "Adding SSH server on control interface"
    ubus call SSH.Server _get '{ "rel_path": ".[Alias == \"control\"]." }' || {
        sleep 2
        ubus call "SSH.Server" _add "{ \"rel_path\": \".\", \"parameters\": { \"Interface\": \"Device.$LAN_INTERFACE.\", \"AllowRootPasswordLogin\": true, \"Alias\": \"control\" } }"
    }
}

# Wait until the interface is created, it seems like we can not add to the newly created interface object directly after creating it
ubus wait_for "$LAN_INTERFACE"
sleep 15

# We can now add the IP address if there is none yet:
ubus call IP.Interface _get '{ "rel_path": ".[Alias == \"eth0_4\"].IPv4Address.[Alias == \"eth0_4\"]." }' || {
    echo "Adding IP address $IP"
    ubus call "IP.Interface" _add '{ "rel_path": ".[Alias == \"eth0_4\"].IPv4Address.", "parameters": { "IPAddress": "192.168.250.180", "SubnetMask": "255.255.255.0", "AddressingType": "Static", "Alias": "eth0_4", "Enable" : true } }'
}
sleep 5
# Finally, we can enable it:
ubus call "IP.Interface" _set '{ "rel_path": ".[Alias == \"eth0_4\"].", "parameters": { "IPv4Enable": true } }'

# Set the wired backhaul interface:
if ba-cli "X_PRPLWARE-COM_Agent.Configuration.?" | grep -Eq "No data found|ERROR"; then
  # Prplmesh agent is not running. Data model isn't up.
  echo "Prplmesh agent is not running"
else
  # Prplmesh agent is running, configure it over the bus
  echo "Setting prplMesh BackhaulWireInterface over DM"
  ba-cli X_PRPLWARE-COM_Agent.Configuration.BackhaulWireInterface="eth1"
fi

# For now there is no way to disable the firewall (see PCF-590).
# Instead, wait for it in the datamodel, then set the whole INPUT
# chain to ACCEPT:
ubus wait_for Firewall
iptables -P INPUT ACCEPT

uci batch << 'EOF'
# TODO: The current channel selection does not work correctly when 80Mhz bandwidths are involved.
# This temporary workaround forces the use of 20Mhz bands, and will need to be reverted when the 
# issue is fixed (see https://jira.prplfoundation.org/browse/PPM-258)
set wireless.radio0.htmode='HT20'
set wireless.radio2.htmode='HT20'

################ needed for R2 certification #################
# Enable action/managment frames hostapd notifiecations
set wireless.radio0.notify_action_frame='1'
set wireless.radio2.notify_action_frame='1'
##############################################################

# Add backhaul STAs:
set wireless.default_radio26=wifi-iface
set wireless.default_radio26.device=radio0
set wireless.default_radio26.ifname=wlan1
set wireless.default_radio26.mode=sta
set wireless.default_radio26.config_methods=push_button
set wireless.default_radio26.wds=1
set wireless.default_radio26.multi_ap_profile=2
set wireless.default_radio26.pmf1

set wireless.default_radio58=wifi-iface
set wireless.default_radio58.device=radio2
set wireless.default_radio58.ifname=wlan3
set wireless.default_radio58.mode=sta
set wireless.default_radio58.config_methods=push_button
set wireless.default_radio58.wds=1
set wireless.default_radio58.multi_ap_profile=2
set wireless.default_radio58.pmf1

# radios are disabled by default in prplwrt
set wireless.radio0.disabled=0
set wireless.default_radio100.start_disabled=0
set wireless.radio2.disabled=0
set wireless.default_radio102.start_disabled=0

# Make guest interfaces part of lan again until prplMesh supports it (PPM-2019):
set wireless.default_radio11.network='lan'
set wireless.default_radio11.ssid='prplOS'
set wireless.default_radio43.network='lan'
set wireless.default_radio43.ssid='prplOS'
EOF

# Generate a MAC address for the new bSTA interfaces:
sh /rom/etc/uci-defaults/15_wireless-generate-macaddr || true

uci commit
/etc/init.d/system restart
# /etc/init.d/network restart
sleep 10

# Try to work around PCF-681: if we don't have a connectivity, restart
# tr181-bridging
# Check the status of the LAN bridge
ip a |grep "br-lan:" |grep "state UP" >/dev/null || (echo "LAN Bridge DOWN, restarting bridge manager" && /etc/init.d/tr181-bridging restart && sleep 15)
# If we can't ping the UCC, restart the IP manager
ping -i 1 -c 2 192.168.250.199 || (/etc/init.d/ip-manager restart && sleep 15)
ping -i 1 -c 2 192.168.250.199 || (/etc/init.d/ip-manager restart && sleep 15)

# Remove the default lan/wan SSH servers if they exist
ubus call "SSH.Server" _del '{ "rel_path": ".[Alias == \"lan\"]" }' || true
ubus call "SSH.Server" _del '{ "rel_path": ".[Alias == \"wan\"]" }' || true

# Trigger the startup of the SSH server
ubus call "SSH.Server" _set '{ "rel_path": ".[Alias == \"control\"].", "parameters": { "Enable": false } }'
sleep 5
ubus call "SSH.Server" _set '{ "rel_path": ".[Alias == \"control\"].", "parameters": { "Enable": true } }'

# Still start an SSH server manually
dropbear -F -T 10 -p192.168.250.180:22 &
