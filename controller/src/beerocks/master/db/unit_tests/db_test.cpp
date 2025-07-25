/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_event_loop_mock.h>

#include "ambiorix_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "../db.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::InvokeWithoutArgs;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::StrEq;
using ::testing::StrictMock;

namespace {

constexpr auto g_assoc_event_path = DATAELEMENTS_ROOT_DM ".AssociationEvent.AssociationEventData";
constexpr auto g_device_path      = DATAELEMENTS_ROOT_DM ".Network.Device";
constexpr auto g_controller_data_model_path = "config/odl/controller.odl";
constexpr auto g_zero_mac                   = "00:00:00:00:00:00";
constexpr auto g_bridge_mac                 = "46:55:66:77:00:00";
constexpr auto g_bridge_oui                 = "465566";
constexpr auto g_radio_mac_1                = "46:55:66:77:00:21";
constexpr auto g_radio_mac_2                = "46:55:66:77:00:22";
constexpr auto g_client_mac                 = "46:55:66:77:00:31";
constexpr auto g_vap_id_1                   = 1;
constexpr auto g_bssid_1                    = "46:55:66:77:00:03";
constexpr auto g_ssid_1                     = "dummy_ssid";
constexpr auto g_interface_mac_1            = "46:55:66:77:00:41";
constexpr auto g_interface_mac_2            = "46:55:66:77:00:42";
const std::string g_device_path_multiapcaps = std::string(g_device_path) + ".1.MultiAPCapabilities";
const std::string g_radio_path_1            = std::string(g_device_path) + ".1.Radio.1";
const std::string g_radio_path_2            = std::string(g_device_path) + ".1.Radio.2";
const std::string g_radio_1_bss_path_1      = std::string(g_radio_path_1) + ".BSS.1";
const std::string g_radio_1_bss_path_2      = std::string(g_radio_path_1) + ".BSS.2";
const std::string g_radio_2_bss_path_1      = std::string(g_radio_path_2) + ".BSS.1";
const std::string g_radio_2_bss_path_2      = std::string(g_radio_path_2) + ".BSS.2";
const std::string g_sta_path_1              = std::string(g_radio_1_bss_path_1) + ".STA.1";
const std::string g_assoc_event_path_1      = std::string(g_assoc_event_path) + ".1";
const std::string g_interface_path_1        = std::string(g_device_path) + ".1.Interface.1";
const std::string g_interface_path_2        = std::string(g_device_path) + ".1.Interface.2";

class DbTest : public ::testing::Test {

protected:
    std::shared_ptr<StrictMock<beerocks::nbapi::AmbiorixMock>> m_ambiorix;
    std::shared_ptr<son::db> m_db;

    void SetUp() override
    {
        m_ambiorix = std::make_shared<StrictMock<beerocks::nbapi::AmbiorixMock>>();

        beerocks::config_file::sConfigMaster beerocks_master_conf;
        son::db::sDbMasterConfig master_conf;
        beerocks::logging logger("logger", beerocks_master_conf.sLog);
        logger.set_log_level_state(beerocks::LOG_LEVEL_ERROR, true);

        m_db = std::make_shared<son::db>(master_conf, logger, tlvf::mac_from_string(g_bridge_mac),
                                         m_ambiorix);

        ASSERT_TRUE(m_db != nullptr);

        EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bridge_mac)).WillRepeatedly(Return(0));
        EXPECT_CALL(*m_ambiorix, add_instance(g_device_path))
            .WillOnce(Return(std::string(g_device_path) + ".1"));
        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_device_path) + ".1", "ID",
                        Matcher<const sMacAddr &>(tlvf::mac_from_string(g_bridge_mac))))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set(g_device_path_multiapcaps, "AgentInitiatedRCPIBasedSteering",
                                     Matcher<const bool &>(false)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix,
                    set(g_device_path_multiapcaps, "UnassociatedSTALinkMetricsCurrentlyOn",
                        Matcher<const bool &>(false)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix,
                    set(g_device_path_multiapcaps, "UnassociatedSTALinkMetricsCurrentlyOff",
                        Matcher<const bool &>(false)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_device_path) + ".1", "CollectionInterval",
                                     Matcher<const uint32_t &>(_)))
            .WillOnce(Return(true));

        EXPECT_CALL(*m_ambiorix, set(std::string(g_device_path) + ".1.MultiAPDevice",
                                     "ManufacturerOUI", Matcher<const std::string &>(g_bridge_oui)))
            .WillOnce(Return(true));

        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_device_path) + ".1.MultiAPDevice",
                        "EasyMeshControllerOperationMode", Matcher<const std::string &>("Running")))
            .WillOnce(Return(true));

        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_device_path) + ".1.MultiAPDevice",
                        "EasyMeshAgentOperationMode", Matcher<const std::string &>("Running")))
            .WillOnce(Return(true));

        m_db->set_prplmesh(tlvf::mac_from_string(g_bridge_mac));
        EXPECT_EQ(std::string(g_device_path) + ".1",
                  m_db->get_agent_data_model_path(tlvf::mac_from_string(g_bridge_mac)));
        EXPECT_CALL(*m_ambiorix, set_current_time(_, _)).WillRepeatedly(Return(true));
    }
};

class DbTestRadio1 : public ::DbTest {

protected:
    void SetUp() override
    {

        // Load base settings.
        DbTest::SetUp();

        //device always exists
        EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bridge_mac)).WillRepeatedly(Return(1));

        //expectations for add_radio
        EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_radio_mac_1)).WillRepeatedly(Return(1));
        EXPECT_CALL(*m_ambiorix, add_instance(std::string(g_device_path) + ".1.Radio"))
            .WillOnce(Return(g_radio_path_1));
        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_radio_path_1), "ID",
                        Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
            .WillOnce(Return(true));

        //prepare scenario
        EXPECT_TRUE(m_db->add_radio(tlvf::mac_from_string(g_radio_mac_1),
                                    tlvf::mac_from_string(g_bridge_mac)));
        EXPECT_TRUE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));
        EXPECT_EQ(std::string(g_device_path) + ".1.Radio.1",
                  m_db->get_radio_data_model_path(tlvf::mac_from_string(g_radio_mac_1)));
    }
};

class DbTestRadio1Bss1 : public ::DbTestRadio1 {

protected:
    void SetUp() override
    {

        // Load base settings with Radio.
        DbTestRadio1::SetUp();

        const std::string radio_path = std::string(g_device_path) + ".1.Radio";
        const std::string bss_path   = radio_path + ".1.BSS";

        //expectations for add_bss
        EXPECT_CALL(*m_ambiorix, add_instance(bss_path)).WillOnce(Return(bss_path + ".1"));
        EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "BSSID",
                                     Matcher<const sMacAddr &>(tlvf::mac_from_string(g_bssid_1))))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix,
                    set(bss_path + ".1", "SSID", Matcher<const std::string &>(g_ssid_1)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set(_, "Enabled", Matcher<const bool &>(false)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "FronthaulUse", Matcher<const bool &>(false)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "BackhaulUse", Matcher<const bool &>(false)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "IsVBSS", Matcher<const bool &>(false)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix,
                    set(bss_path + ".1", "ByteCounterUnits", Matcher<const uint32_t &>(0U)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "LastChange", Matcher<const uint32_t &>(_)))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set_current_time(_, _)).WillOnce(Return(true));

        //add virtual AP to radio
        auto radio = m_db->get_radio(tlvf::mac_from_string(g_bridge_mac),
                                     tlvf::mac_from_string(g_radio_mac_1));
        if (radio) {
            EXPECT_TRUE(m_db->add_bss(*radio, tlvf::mac_from_string(g_bssid_1), g_ssid_1,
                                      g_vap_id_1) != nullptr);
        }
    }
};

class DbTestRadio1Sta1 : public ::DbTestRadio1Bss1 {

protected:
    void SetUp() override
    {

        // Load base settings with Radio and BSS added.
        DbTestRadio1Bss1::SetUp();

        //expectations for add_station
        EXPECT_CALL(*m_ambiorix, add_instance(std::string(g_radio_1_bss_path_1) + ".STA"))
            .WillOnce(Return(std::string(g_radio_1_bss_path_1) + ".STA.1"));
        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_sta_path_1), "MACAddress",
                        Matcher<const sMacAddr &>(tlvf::mac_from_string(g_client_mac))))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set_current_time(std::string(g_sta_path_1), _))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set_current_time(std::string(g_sta_path_1 + ".MultiAPSTA"), _))
            .WillOnce(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1 + ".MultiAPSTA.SteeringSummaryStats"),
                                     "BlacklistAttempts", Matcher<const uint64_t &>(_)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1 + ".MultiAPSTA.SteeringSummaryStats"),
                                     "BlacklistSuccesses", Matcher<const uint64_t &>(_)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1 + ".MultiAPSTA.SteeringSummaryStats"),
                                     "BlacklistFailures", Matcher<const uint64_t &>(_)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1 + ".MultiAPSTA.SteeringSummaryStats"),
                                     "BTMAttempts", Matcher<const uint64_t &>(_)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1 + ".MultiAPSTA.SteeringSummaryStats"),
                                     "BTMSuccesses", Matcher<const uint64_t &>(_)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1 + ".MultiAPSTA.SteeringSummaryStats"),
                                     "BTMFailures", Matcher<const uint64_t &>(_)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1 + ".MultiAPSTA.SteeringSummaryStats"),
                                     "BTMQueryResponses", Matcher<const uint64_t &>(_)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1 + ".MultiAPSTA.SteeringSummaryStats"),
                                     "LastSteerTime", Matcher<const uint32_t &>(_)))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_sta_path_1), "LastConnectTime", Matcher<const uint64_t &>(_)))
            .WillOnce(Return(true));

        EXPECT_CALL(*m_ambiorix, add_instance(std::string(g_assoc_event_path)))
            .WillRepeatedly(Return(std::string(g_assoc_event_path_1)));
        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_assoc_event_path_1), "BSSID",
                        Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_assoc_event_path_1), "MACAddress",
                        Matcher<const sMacAddr &>(tlvf::mac_from_string(g_client_mac))))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*m_ambiorix, set(std::string(g_assoc_event_path_1), "StatusCode",
                                     Matcher<const uint16_t &>(static_cast<uint16_t>(0))))
            .WillRepeatedly(Return(true));

        //prepare scenario
        EXPECT_TRUE(m_db->add_station(tlvf::mac_from_string(g_bridge_mac),
                                      tlvf::mac_from_string(g_client_mac),
                                      tlvf::mac_from_string(g_bssid_1)));
        EXPECT_TRUE(m_db->has_station(tlvf::mac_from_string(g_client_mac)));
        EXPECT_EQ(std::string(g_radio_1_bss_path_1) + ".STA.1",
                  m_db->get_sta_data_model_path(tlvf::mac_from_string(g_client_mac)));

        EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_client_mac)).WillRepeatedly(Return(1));
    }
};

class DbTestInterface1 : public ::DbTest {

protected:
    void SetUp() override
    {
        DbTest::SetUp();

        // expectations for interface creation
        EXPECT_CALL(*m_ambiorix, add_instance(std::string(g_device_path) + ".1.Interface"))
            .WillOnce(Return(std::string(g_interface_path_1)))
            .WillOnce(Return(std::string(g_interface_path_2)));

        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_interface_path_1), "MACAddress",
                        Matcher<const sMacAddr &>(tlvf::mac_from_string(g_interface_mac_1))))
            .WillRepeatedly(Return(true));

        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_interface_path_2), "MACAddress",
                        Matcher<const sMacAddr &>(tlvf::mac_from_string(g_interface_mac_2))))
            .WillRepeatedly(Return(true));

        EXPECT_CALL(*m_ambiorix, set(std::string(g_interface_path_1), "Status",
                                     Matcher<const std::string &>(std::string("Up"))))
            .WillOnce(Return(true));

        EXPECT_CALL(*m_ambiorix, set(std::string(g_interface_path_2), "Status",
                                     Matcher<const std::string &>(std::string("Up"))))
            .WillOnce(Return(true));

        EXPECT_CALL(*m_ambiorix, set(std::string(g_interface_path_1), "Name",
                                     Matcher<const std::string &>(std::string("eth0"))))
            .WillOnce(Return(true));
        ;

        EXPECT_CALL(*m_ambiorix, set(std::string(g_interface_path_2), "Name",
                                     Matcher<const std::string &>(std::string("eth1"))))
            .WillOnce(Return(true));
        ;
        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_interface_path_1), "MediaType",
                        Matcher<const std::string &>(std::string("IEEE_802_3AB_GIGABIT_ETHERNET"))))
            .WillOnce(Return(true));
        ;

        EXPECT_CALL(*m_ambiorix,
                    set(std::string(g_interface_path_2), "MediaType",
                        Matcher<const std::string &>(std::string("IEEE_802_3AB_GIGABIT_ETHERNET"))))
            .WillOnce(Return(true));
        ;

        //prepare scenario
        EXPECT_TRUE(m_db->add_interface(
            tlvf::mac_from_string(g_bridge_mac), tlvf::mac_from_string(g_interface_mac_1),
            ieee1905_1::eMediaType::IEEE_802_3AB_GIGABIT_ETHERNET, "Up", "eth0"));
        EXPECT_TRUE(m_db->add_interface(
            tlvf::mac_from_string(g_bridge_mac), tlvf::mac_from_string(g_interface_mac_2),
            ieee1905_1::eMediaType::IEEE_802_3AB_GIGABIT_ETHERNET, "Up", "eth1"));
    }
};

TEST_F(DbTest, dm_should_have_controller_bridge)
{
    EXPECT_TRUE(m_db->get_agent(tlvf::mac_from_string(g_bridge_mac)));
}

TEST_F(DbTest, test_add_node_radio)
{
    const std::string radio_path = std::string(g_device_path) + ".1.Radio";

    //radio node and path may not exist
    EXPECT_FALSE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));

    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bridge_mac)).WillRepeatedly(Return(1));
    EXPECT_CALL(*m_ambiorix, add_instance(std::string(radio_path)))
        .WillOnce(Return(std::string(radio_path) + ".1"));
    EXPECT_CALL(*m_ambiorix, set(std::string(radio_path) + ".1", "ID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
        .WillOnce(Return(true));

    //add radio node
    EXPECT_TRUE(
        m_db->add_radio(tlvf::mac_from_string(g_radio_mac_1), tlvf::mac_from_string(g_bridge_mac)));

    //radio node and path must exist
    EXPECT_TRUE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));
    EXPECT_EQ(std::string(radio_path) + ".1",
              m_db->get_radio_data_model_path(tlvf::mac_from_string(g_radio_mac_1)));
}

TEST_F(DbTest, test_add_vap)
{
    const std::string radio_path = std::string(g_device_path) + ".1.Radio";
    const std::string bss_path   = radio_path + ".1.BSS";

    //device always exists
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bridge_mac)).WillRepeatedly(Return(1));

    //BSS node and path may not exist
    EXPECT_FALSE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));

    //expectations for add_radio
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_radio_mac_1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*m_ambiorix, add_instance(std::string(radio_path)))
        .WillOnce(Return(std::string(radio_path) + ".1"));
    EXPECT_CALL(*m_ambiorix, set(std::string(radio_path) + ".1", "ID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
        .WillOnce(Return(true));

    //prepare scenario
    EXPECT_TRUE(
        m_db->add_radio(tlvf::mac_from_string(g_radio_mac_1), tlvf::mac_from_string(g_bridge_mac)));
    EXPECT_TRUE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));
    EXPECT_EQ(std::string(radio_path) + ".1",
              m_db->get_radio_data_model_path(tlvf::mac_from_string(g_radio_mac_1)));

    //expectations for add_bss
    EXPECT_CALL(*m_ambiorix, add_instance(bss_path)).WillOnce(Return(bss_path + ".1"));
    EXPECT_CALL(*m_ambiorix, set(std::string(bss_path) + ".1", "BSSID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_bssid_1))))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(std::string(bss_path) + ".1", "SSID", Matcher<const std::string &>(g_ssid_1)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(std::string(bss_path) + ".1", "Enabled", Matcher<const bool &>(false)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "FronthaulUse", Matcher<const bool &>(false)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "BackhaulUse", Matcher<const bool &>(false)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(std::string(bss_path) + ".1", "IsVBSS", Matcher<const bool &>(false)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(bss_path) + ".1", "ByteCounterUnits",
                                 Matcher<const uint32_t &>(0U)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(std::string(bss_path) + ".1", "LastChange", Matcher<const uint32_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set_current_time(std::string(bss_path) + ".1", _))
        .WillOnce(Return(true));

    //add virtual AP to radio
    auto radio =
        m_db->get_radio(tlvf::mac_from_string(g_bridge_mac), tlvf::mac_from_string(g_radio_mac_1));
    if (radio) {
        EXPECT_TRUE(m_db->add_bss(*radio, tlvf::mac_from_string(g_bssid_1), g_ssid_1, g_vap_id_1) !=
                    nullptr);
    }
}

TEST_F(DbTest, test_set_ap_ht_capabilities)
{
    const std::string capabilities1    = g_radio_path_1 + ".Capabilities.";
    const std::string capabilities2    = g_radio_path_2 + ".Capabilities.";
    const std::string ht_capabilities1 = capabilities1 + "HTCapabilities.";
    const std::string ht_capabilities2 = capabilities2 + "HTCapabilities.";

    wfa_map::tlvApHtCapabilities::sFlags flags1    = {};
    flags1.reserved                                = 0;
    flags1.ht_support_40mhz                        = 1;
    flags1.short_gi_support_40mhz                  = 0;
    flags1.short_gi_support_20mhz                  = 1;
    flags1.max_num_of_supported_rx_spatial_streams = 2;
    flags1.max_num_of_supported_tx_spatial_streams = 3;

    wfa_map::tlvApHtCapabilities::sFlags flags2    = {};
    flags2.reserved                                = 0;
    flags2.ht_support_40mhz                        = 0;
    flags2.short_gi_support_40mhz                  = 1;
    flags2.short_gi_support_20mhz                  = 0;
    flags2.max_num_of_supported_rx_spatial_streams = 1;
    flags2.max_num_of_supported_tx_spatial_streams = 2;

    //device always exists
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bridge_mac)).WillRepeatedly(Return(1));

    //cannot set capabilities to not existent radio
    EXPECT_FALSE(m_db->set_ap_ht_capabilities(tlvf::mac_from_string(g_radio_mac_1), flags1));
    EXPECT_FALSE(m_db->set_ap_ht_capabilities(tlvf::mac_from_string(g_radio_mac_2), flags2));

    //expectations for add_node_radios
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_radio_mac_1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_radio_mac_2)).WillRepeatedly(Return(2));
    EXPECT_CALL(*m_ambiorix, add_instance(std::string(g_device_path) + ".1.Radio"))
        .WillOnce(Return(std::string(g_device_path) + ".1.Radio.1"))
        .WillOnce(Return(std::string(g_device_path) + ".1.Radio.2"));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_path_1), "ID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_path_2), "ID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_2))))
        .WillOnce(Return(true));

    //prepare scenario
    EXPECT_TRUE(
        m_db->add_radio(tlvf::mac_from_string(g_radio_mac_1), tlvf::mac_from_string(g_bridge_mac)));
    EXPECT_TRUE(
        m_db->add_radio(tlvf::mac_from_string(g_radio_mac_2), tlvf::mac_from_string(g_bridge_mac)));
    EXPECT_TRUE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));
    EXPECT_EQ(std::string(g_device_path) + ".1.Radio.1",
              m_db->get_radio_data_model_path(tlvf::mac_from_string(g_radio_mac_1)));
    EXPECT_TRUE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_2)));
    EXPECT_EQ(std::string(g_device_path) + ".1.Radio.2",
              m_db->get_radio_data_model_path(tlvf::mac_from_string(g_radio_mac_2)));

    // expectations for set_ap_ht_capabilities
    EXPECT_CALL(*m_ambiorix, add_optional_subobject(capabilities1, "HTCapabilities"))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, add_optional_subobject(capabilities2, "HTCapabilities"))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, set(ht_capabilities1, "HTShortGI20", Matcher<const bool &>(true)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(ht_capabilities1, "HTShortGI40", Matcher<const bool &>(false)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(ht_capabilities1, "HT40", Matcher<const bool &>(true)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(ht_capabilities1, "MaxNumberOfTxSpatialStreams",
                    Matcher<const int32_t &>(flags1.max_num_of_supported_tx_spatial_streams + 1)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(ht_capabilities1, "MaxNumberOfRxSpatialStreams",
                    Matcher<const int32_t &>(flags1.max_num_of_supported_rx_spatial_streams + 1)))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, set(ht_capabilities2, "HTShortGI20", Matcher<const bool &>(false)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(ht_capabilities2, "HTShortGI40", Matcher<const bool &>(true)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(ht_capabilities2, "HT40", Matcher<const bool &>(false)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(ht_capabilities2, "MaxNumberOfTxSpatialStreams",
                    Matcher<const int32_t &>(flags2.max_num_of_supported_tx_spatial_streams + 1)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(ht_capabilities2, "MaxNumberOfRxSpatialStreams",
                    Matcher<const int32_t &>(flags2.max_num_of_supported_rx_spatial_streams + 1)))
        .WillOnce(Return(true));

    //execute test
    EXPECT_TRUE(m_db->set_ap_ht_capabilities(tlvf::mac_from_string(g_radio_mac_1), flags1));
    EXPECT_TRUE(m_db->set_ap_ht_capabilities(tlvf::mac_from_string(g_radio_mac_2), flags2));
}

TEST_F(DbTest, test_add_hostap_supported_operating_class)
{
    const std::string operating_classes =
        std::string(g_radio_path_1) + ".Capabilities.OperatingClasses";
    const std::string non_operable = std::string(operating_classes) + ".1.NonOperable";

    //device always exists
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bridge_mac)).WillRepeatedly(Return(1));

    //BSS node and path may not exist
    EXPECT_FALSE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));

    //expectations for add_radio
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_radio_mac_1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*m_ambiorix, add_instance(std::string(g_device_path) + ".1.Radio"))
        .WillOnce(Return(std::string(g_device_path) + ".1.Radio.1"));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_path_1), "ID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
        .WillOnce(Return(true));

    //prepare scenario
    EXPECT_TRUE(
        m_db->add_radio(tlvf::mac_from_string(g_radio_mac_1), tlvf::mac_from_string(g_bridge_mac)));
    EXPECT_TRUE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));
    EXPECT_EQ(std::string(g_device_path) + ".1.Radio.1",
              m_db->get_radio_data_model_path(tlvf::mac_from_string(g_radio_mac_1)));

    //expectations for add_hostap_supported_operating_class
    EXPECT_CALL(*m_ambiorix, add_instance(std::string(operating_classes)))
        .WillOnce(Return(std::string(operating_classes) + ".1"));
    EXPECT_CALL(*m_ambiorix, set(std::string(operating_classes) + ".1", "MaxTxPower",
                                 Matcher<const uint8_t &>(0x01)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(std::string(operating_classes) + ".1", "Class", Matcher<const uint8_t &>(0xFF)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, add_instance(std::string(non_operable)))
        .WillOnce(Return(std::string(non_operable) + ".1"))
        .WillOnce(Return(std::string(non_operable) + ".2"))
        .WillOnce(Return(std::string(non_operable) + ".3"));
    EXPECT_CALL(*m_ambiorix, set(std::string(non_operable) + ".1", "NonOpChannelNumber",
                                 Matcher<const uint8_t &>(0x01)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(non_operable) + ".2", "NonOpChannelNumber",
                                 Matcher<const uint8_t &>(0x02)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(non_operable) + ".3", "NonOpChannelNumber",
                                 Matcher<const uint8_t &>(0x03)))
        .WillOnce(Return(true));

    //execute test
    EXPECT_TRUE(m_db->add_hostap_supported_operating_class(
        tlvf::mac_from_string(g_radio_mac_1), 0xFF, 0x01, std::vector<uint8_t>{0x01, 0x02, 0x03}));
}

TEST_F(DbTest, test_add_current_op_class)
{
    const std::string radio_path_1_operating_classes =
        std::string(g_radio_path_1) + ".CurrentOperatingClasses";

    //device always exists
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bridge_mac)).WillRepeatedly(Return(1));

    // must fail because parent does not exists
    EXPECT_FALSE(m_db->add_current_op_class(tlvf::mac_from_string(g_radio_mac_1), 0x01, 0x02, 10));

    //expectations for add_radio
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_radio_mac_1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*m_ambiorix, add_instance(std::string(g_device_path) + ".1.Radio"))
        .WillOnce(Return(g_radio_path_1));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_path_1), "ID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
        .WillOnce(Return(true));

    //prepare scenario
    EXPECT_TRUE(
        m_db->add_radio(tlvf::mac_from_string(g_radio_mac_1), tlvf::mac_from_string(g_bridge_mac)));
    EXPECT_TRUE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));
    EXPECT_EQ(std::string(g_device_path) + ".1.Radio.1",
              m_db->get_radio_data_model_path(tlvf::mac_from_string(g_radio_mac_1)));

    //expectations for add_current_op_class
    EXPECT_CALL(*m_ambiorix, add_instance(radio_path_1_operating_classes))
        .WillOnce(Return(radio_path_1_operating_classes + ".1"));
    EXPECT_CALL(*m_ambiorix,
                set_current_time(std::string(radio_path_1_operating_classes + ".1"), _))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(radio_path_1_operating_classes) + ".1", "Class",
                                 Matcher<const uint8_t &>(0x01)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(radio_path_1_operating_classes) + ".1", "Channel",
                                 Matcher<const uint8_t &>(0x02)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(radio_path_1_operating_classes) + ".1", "TxPower",
                                 Matcher<const int8_t &>(10)))
        .WillOnce(Return(true));

    //execute test
    EXPECT_TRUE(m_db->add_current_op_class(tlvf::mac_from_string(g_radio_mac_1), 0x01, 0x02, 10));
}

TEST_F(DbTestRadio1Sta1, test_set_sta_stats_info)
{

    //expectations for dm_set_sta_extended_link_metrics
    wfa_map::tlvAssociatedStaExtendedLinkMetrics::sMetrics metrics;
    metrics.last_data_down_link_rate = 1;
    metrics.last_data_up_link_rate   = 2;
    metrics.utilization_receive      = 3;
    metrics.utilization_transmit     = 4;

    //expectations for dm_set_sta_traffic_stats
    son::db::sAssociatedStaTrafficStats stats;
    stats.m_byte_received        = 5;
    stats.m_byte_sent            = 6;
    stats.m_packets_received     = 7;
    stats.m_packets_sent         = 8;
    stats.m_retransmission_count = 9;
    stats.m_rx_packets_error     = 10;
    stats.m_tx_packets_error     = 11;

    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "LastDataDownlinkRate",
                                 Matcher<const uint32_t &>(metrics.last_data_down_link_rate)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "LastDataUplinkRate",
                                 Matcher<const uint32_t &>(metrics.last_data_up_link_rate)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "UtilizationReceive",
                                 Matcher<const uint32_t &>(metrics.utilization_receive)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "UtilizationTransmit",
                                 Matcher<const uint32_t &>(metrics.utilization_transmit)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "BytesSent",
                                 Matcher<const uint64_t &>(stats.m_byte_sent)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "BytesReceived",
                                 Matcher<const uint64_t &>(stats.m_byte_received)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "PacketsSent",
                                 Matcher<const uint64_t &>(stats.m_packets_sent)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "PacketsReceived",
                                 Matcher<const uint64_t &>(stats.m_packets_received)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "RetransCount",
                                 Matcher<const uint32_t &>(stats.m_retransmission_count)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "ErrorsSent",
                                 Matcher<const uint32_t &>(stats.m_tx_packets_error)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "ErrorsReceived",
                                 Matcher<const uint32_t &>(stats.m_rx_packets_error)))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, set_current_time(g_sta_path_1, _)).WillOnce(Return(true));

    EXPECT_TRUE(
        m_db->dm_set_sta_extended_link_metrics(tlvf::mac_from_string(g_client_mac), metrics));
    EXPECT_TRUE(m_db->dm_set_sta_traffic_stats(tlvf::mac_from_string(g_client_mac), stats));
}

TEST_F(DbTest, test_set_vap_stats_info)
{
    const std::string radio_path = std::string(g_device_path) + ".1.Radio";
    const std::string bss_path   = radio_path + ".1.BSS";

    //device always exists
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bridge_mac)).WillRepeatedly(Return(1));

    //must fail because VAD does not exists
    EXPECT_FALSE(m_db->set_vap_stats_info(tlvf::mac_from_string(g_bssid_1), 1, 2, 3, 4, 5, 6));

    //expectations for add_radio
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_radio_mac_1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*m_ambiorix, add_instance(std::string(radio_path)))
        .WillOnce(Return(std::string(radio_path) + ".1"));
    EXPECT_CALL(*m_ambiorix, set(std::string(radio_path) + ".1", "ID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
        .WillOnce(Return(true));

    //prepare scenario
    EXPECT_TRUE(
        m_db->add_radio(tlvf::mac_from_string(g_radio_mac_1), tlvf::mac_from_string(g_bridge_mac)));
    EXPECT_TRUE(m_db->get_radio_by_uid(tlvf::mac_from_string(g_radio_mac_1)));
    EXPECT_EQ(std::string(radio_path) + ".1",
              m_db->get_radio_data_model_path(tlvf::mac_from_string(g_radio_mac_1)));

    //expectations for add_bss
    EXPECT_CALL(*m_ambiorix, add_instance(bss_path)).WillOnce(Return(bss_path + ".1"));
    EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "BSSID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_bssid_1))))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(bss_path + ".1", "SSID", Matcher<const std::string &>(g_ssid_1)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(_, _, Matcher<const bool &>(_))).WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix, set(_, _, Matcher<const uint32_t &>(_))).WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix, set_current_time(_, _)).WillOnce(Return(true));

    //add virtual AP to radio
    auto radio =
        m_db->get_radio(tlvf::mac_from_string(g_bridge_mac), tlvf::mac_from_string(g_radio_mac_1));
    if (radio) {
        EXPECT_TRUE(m_db->add_bss(*radio, tlvf::mac_from_string(g_bssid_1), g_ssid_1, g_vap_id_1) !=
                    nullptr);
    }

    //expectations for set_vap_stats_info
    EXPECT_CALL(*m_ambiorix, get_instance_index(_, g_bssid_1)).WillRepeatedly(Return(1));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_1_bss_path_1), "UnicastBytesSent",
                                 Matcher<const uint64_t &>(1U)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_1_bss_path_1), "UnicastBytesReceived",
                                 Matcher<const uint64_t &>(2U)))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_1_bss_path_1), "MulticastBytesSent",
                                 Matcher<const uint64_t &>(3U)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_1_bss_path_1), "MulticastBytesReceived",
                                 Matcher<const uint64_t &>(4U)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_1_bss_path_1), "BroadcastBytesSent",
                                 Matcher<const uint64_t &>(5U)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_radio_1_bss_path_1), "BroadcastBytesReceived",
                                 Matcher<const uint64_t &>(6U)))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, set_current_time(_, _)).WillOnce(Return(true));

    //execute test
    EXPECT_TRUE(m_db->set_vap_stats_info(tlvf::mac_from_string(g_bssid_1), 1, 2, 3, 4, 5, 6));
}

TEST_F(DbTestRadio1Sta1, test_set_sta_capabilities)
{
    std::string ht_capabilities1  = std::string(g_sta_path_1) + ".HTCapabilities.";
    std::string vht_capabilities1 = std::string(g_sta_path_1) + ".VHTCapabilities.";
    std::string he_capabilities1  = std::string(g_sta_path_1) + ".WiFi6Capabilities.";
    std::string ht_capabilities2  = std::string(g_assoc_event_path_1) + ".HTCapabilities.";
    std::string vht_capabilities2 = std::string(g_assoc_event_path_1) + ".VHTCapabilities.";
    std::string he_capabilities2  = std::string(g_assoc_event_path_1) + ".WiFi6Capabilities.";

    //expectations for set_sta_stats_info
    beerocks::message::sRadioCapabilities sta_cap;
    //hint: set ht_bw and vht_bw to add HT and VHT mibs
    sta_cap.ht_bw         = beerocks::BANDWIDTH_20;
    sta_cap.vht_bw        = beerocks::BANDWIDTH_80;
    sta_cap.he_bw         = beerocks::BANDWIDTH_160;
    sta_cap.wifi_standard = beerocks::STANDARD_N | beerocks::STANDARD_AC | beerocks::STANDARD_AX;

    EXPECT_CALL(*m_ambiorix, remove_optional_subobject(g_sta_path_1 + '.', "HTCapabilities"))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, remove_optional_subobject(g_sta_path_1 + '.', "VHTCapabilities"))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, remove_optional_subobject(g_sta_path_1 + '.', "WiFi6Capabilities"))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, add_optional_subobject(g_sta_path_1 + '.', "HTCapabilities"))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, add_optional_subobject(g_sta_path_1 + '.', "VHTCapabilities"))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, add_optional_subobject(g_sta_path_1 + '.', "WiFi6Capabilities"))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, set(ht_capabilities1, "HTShortGI20", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(ht_capabilities1, "HTShortGI40", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(ht_capabilities1, "HT40", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(ht_capabilities1, "MaxNumberOfTxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(ht_capabilities1, "MaxNumberOfRxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, set(vht_capabilities1, "MCSNSSTxSet", Matcher<const uint16_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities1, "MCSNSSRxSet", Matcher<const uint16_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(vht_capabilities1, "MaxNumberOfTxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(vht_capabilities1, "MaxNumberOfRxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities1, "VHTShortGI80", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities1, "VHTShortGI160", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities1, "VHT8080", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities1, "VHT160", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities1, "SUBeamformer", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities1, "MUBeamformer", Matcher<const bool &>(_)))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix,
                set(he_capabilities1, "MaxNumberOfTxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(he_capabilities1, "MaxNumberOfRxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "HE160", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "HE8080", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "SUBeamformer", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "SUBeamformee", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "MUBeamformer", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "Beamformee80orLess", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "BeamformeeAbove80", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "ULMUMIMO", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "ULOFDMA", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "DLOFDMA", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "MaxDLMUMIMO", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "MaxULMUMIMO", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "MaxDLOFDMA", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "MaxULOFDMA", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "RTS", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "MURTS", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "MultiBSSID", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "MUEDCA", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "TWTRequestor", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "TWTResponder", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities1, "SpatialReuse", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(he_capabilities1, "AnticipatedChannelUsage", Matcher<const bool &>(_)))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix,
                remove_optional_subobject(g_assoc_event_path_1 + '.', "HTCapabilities"))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix,
                remove_optional_subobject(g_assoc_event_path_1 + '.', "VHTCapabilities"))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix,
                remove_optional_subobject(g_assoc_event_path_1 + '.', "WiFi6Capabilities"))
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*m_ambiorix, add_optional_subobject(g_assoc_event_path_1 + '.', "HTCapabilities"))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix, add_optional_subobject(g_assoc_event_path_1 + '.', "VHTCapabilities"))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix,
                add_optional_subobject(g_assoc_event_path_1 + '.', "WiFi6Capabilities"))
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*m_ambiorix, set(ht_capabilities2, "HTShortGI20", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(ht_capabilities2, "HTShortGI40", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(ht_capabilities2, "HT40", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(ht_capabilities2, "MaxNumberOfTxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(ht_capabilities2, "MaxNumberOfRxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix, set(vht_capabilities2, "MCSNSSTxSet", Matcher<const uint16_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities2, "MCSNSSRxSet", Matcher<const uint16_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(vht_capabilities2, "MaxNumberOfTxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(vht_capabilities2, "MaxNumberOfRxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities2, "VHTShortGI80", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities2, "VHTShortGI160", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities2, "VHT8080", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities2, "VHT160", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities2, "SUBeamformer", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(vht_capabilities2, "MUBeamformer", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set_current_time(g_assoc_event_path_1, _)).WillOnce(Return(true));

    EXPECT_CALL(*m_ambiorix,
                set(he_capabilities2, "MaxNumberOfTxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(he_capabilities2, "MaxNumberOfRxSpatialStreams", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "HE160", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "HE8080", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "SUBeamformer", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "SUBeamformee", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "MUBeamformer", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "Beamformee80orLess", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "BeamformeeAbove80", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "ULMUMIMO", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "ULOFDMA", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "DLOFDMA", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "MaxDLMUMIMO", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "MaxULMUMIMO", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "MaxDLOFDMA", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "MaxULOFDMA", Matcher<const uint8_t &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "RTS", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "MURTS", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "MultiBSSID", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "MUEDCA", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "TWTRequestor", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "TWTResponder", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(he_capabilities2, "SpatialReuse", Matcher<const bool &>(_)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(he_capabilities2, "AnticipatedChannelUsage", Matcher<const bool &>(_)))
        .WillOnce(Return(true));

    //execute test
    EXPECT_TRUE(m_db->set_sta_capabilities(g_client_mac, sta_cap));
    auto cur_sta_caps = m_db->get_sta_current_capabilities(g_client_mac);
    EXPECT_NE(cur_sta_caps, nullptr);
    EXPECT_EQ(m_db->dm_add_association_event(tlvf::mac_from_string(g_radio_mac_1),
                                             tlvf::mac_from_string(g_client_mac)),
              std::string(g_assoc_event_path_1));
    EXPECT_TRUE(m_db->dm_add_assoc_event_sta_caps(g_assoc_event_path_1, *cur_sta_caps));
}

TEST_F(DbTestRadio1Sta1, test_set_sta_link_metrics)
{
    const std::string ht_capabilities1  = std::string(g_sta_path_1) + ".HTCapabilities";
    const std::string vht_capabilities1 = std::string(g_sta_path_1) + ".VHTCapabilities";

    //expectations for set_sta_link_metrics
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "EstMACDataRateDownlink",
                                 Matcher<const uint32_t &>(1U)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "EstMACDataRateUplink",
                                 Matcher<const uint32_t &>(2U)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix,
                set(std::string(g_sta_path_1), "SignalStrength", Matcher<const uint8_t &>(3)))
        .WillOnce(Return(true));

    //execute test
    EXPECT_TRUE(m_db->dm_set_sta_link_metrics(tlvf::mac_from_string(g_client_mac), 1, 2, 3));
}

TEST_F(DbTestRadio1Sta1, test_add_sta_twice_with_same_mac)
{

    //expectations for add_station second time
    EXPECT_CALL(*m_ambiorix, set(std::string(g_assoc_event_path_1), "BSSID",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_radio_mac_1))))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_assoc_event_path_1), "MACAddress",
                                 Matcher<const sMacAddr &>(tlvf::mac_from_string(g_client_mac))))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_assoc_event_path_1), "StatusCode",
                                 Matcher<const uint16_t &>(static_cast<uint16_t>(0))))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_ambiorix, set_current_time(std::string(g_assoc_event_path_1), _))
        .WillRepeatedly(Return(true));
    //prepare scenario
    EXPECT_TRUE(m_db->add_station(tlvf::mac_from_string(g_client_mac),
                                  tlvf::mac_from_string(g_client_mac),
                                  tlvf::mac_from_string(g_radio_mac_1)));
    EXPECT_TRUE(m_db->has_station(tlvf::mac_from_string(g_client_mac)));
    EXPECT_EQ(std::string(g_radio_1_bss_path_1) + ".STA.1",
              m_db->get_sta_data_model_path(tlvf::mac_from_string(g_client_mac)));
}

TEST_F(DbTestRadio1Sta1, test_remove_sta)
{

    //expectations for add_station second time
    EXPECT_CALL(*m_ambiorix, remove_instance(std::string(g_radio_1_bss_path_1) + ".STA", 1))
        .WillRepeatedly(Return(true));

    //prepare scenario
    auto sta = m_db->get_station(tlvf::mac_from_string(g_client_mac));
    EXPECT_TRUE(m_db->dm_remove_sta(*sta));
}

TEST_F(DbTestRadio1Sta1, test_dhcp_v4_lease_sta)
{

    //expectations for set_sta_dhcp_v4_lease
    constexpr char host_name[] = "IPv4_HOST";
    constexpr char ip_addr[]   = "192.168.1.100";

    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "Hostname", std::string(host_name)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "IPV4Address", std::string(ip_addr)))
        .WillOnce(Return(true));

    //execute test
    EXPECT_TRUE(
        m_db->set_sta_dhcp_v4_lease(tlvf::mac_from_string(g_client_mac), host_name, ip_addr));
}

TEST_F(DbTestRadio1Sta1, test_dhcp_v6_lease_sta)
{

    //expectations for set_sta_dhcp_v6_lease
    constexpr char host_name[] = "IPv6_HOST";
    constexpr char ip_addr[]   = "fe80::b6a9:cccc:aaaa:bbbb";

    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "Hostname", std::string(host_name)))
        .WillOnce(Return(true));
    EXPECT_CALL(*m_ambiorix, set(std::string(g_sta_path_1), "IPV6Address", std::string(ip_addr)))
        .WillOnce(Return(true));

    //execute test
    EXPECT_TRUE(
        m_db->set_sta_dhcp_v6_lease(tlvf::mac_from_string(g_client_mac), host_name, ip_addr));
}

TEST_F(DbTestInterface1, test_interface_1_creation)
{
    EXPECT_TRUE(m_db->get_interface_on_agent(tlvf::mac_from_string(g_bridge_mac),
                                             tlvf::mac_from_string(g_interface_mac_1)));

    EXPECT_TRUE(m_db->get_interface_on_agent(tlvf::mac_from_string(g_bridge_mac),
                                             tlvf::mac_from_string(g_interface_mac_2)));

    // Remove Interface.1
    std::vector<sMacAddr> remaining_interfaces = {tlvf::mac_from_string(g_interface_mac_2)};
    EXPECT_CALL(*m_ambiorix, remove_instance(std::string(g_device_path) + ".1.Interface", 1))
        .WillOnce(Return(true));
    EXPECT_TRUE(m_db->dm_update_interface_elements(tlvf::mac_from_string(g_bridge_mac),
                                                   remaining_interfaces));

    EXPECT_TRUE(m_db->get_interface_on_agent(tlvf::mac_from_string(g_bridge_mac),
                                             tlvf::mac_from_string(g_interface_mac_2)));

    EXPECT_FALSE(m_db->get_interface_on_agent(tlvf::mac_from_string(g_bridge_mac),
                                              tlvf::mac_from_string(g_interface_mac_1)));
}

} // namespace
