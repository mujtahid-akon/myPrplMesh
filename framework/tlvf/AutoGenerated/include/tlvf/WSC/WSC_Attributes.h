///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_WSC_ATTRIBUTES_H_
#define _TLVF_WSC_WSC_ATTRIBUTES_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include <tuple>
#include "tlvf/WSC/eWscLengths.h"
#include "tlvf/WSC/eWscAuth.h"
#include "tlvf/WSC/eWscVendorId.h"
#include "tlvf/WSC/eWscVendorExt.h"
#include "tlvf/WSC/eWscDev.h"
#include "tlvf/WSC/eWscWfaVendorExtSubelement.h"
#include "tlvf/common/sMacAddr.h"
#include "tlvf/WSC/eWscAttributes.h"
#include "tlvf/WSC/eWscEncr.h"
#include "tlvf/WSC/eWscValues8.h"
#include "tlvf/WSC/eWscMessageType.h"
#include "tlvf/WSC/eWscConn.h"
#include "tlvf/WSC/eWscRfBands.h"
#include "tlvf/WSC/eWscAssoc.h"
#include "tlvf/WSC/eWscValues16.h"
#include "tlvf/WSC/eWscState.h"

namespace WSC {

class cWscAttrVendorExtension;
class cConfigData;
class cWscAttrEncryptedSettings;
class cWscAttrVersion;
class cWscAttrMessageType;
class cWscAttrEnrolleeNonce;
class cWscAttrPublicKey;
class cWscAttrAuthenticationTypeFlags;
class cWscAttrEncryptionTypeFlags;
class cWscAttrConnectionTypeFlags;
class cWscAttrConfigurationMethods;
class cWscAttrManufacturer;
class cWscAttrModelName;
class cWscAttrModelNumber;
class cWscAttrSerialNumber;
class cWscAttrPrimaryDeviceType;
class cWscAttrDeviceName;
class cWscAttrRfBands;
class cWscAttrAssociationState;
class cWscAttrDevicePasswordID;
class cWscAttrConfigurationError;
class cWscAttrOsVersion;
class cWscAttrMac;
class cWscAttrUuidE;
class cWscAttrWscState;
class cWscAttrUuidR;
class cWscAttrAuthenticator;
class cWscAttrRegistrarNonce;
class cWscAttrSsid;
class cWscAttrAuthenticationType;
class cWscAttrEncryptionType;
class cWscAttrNetworkKey;
typedef struct sWscAttrAuthenticationType {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscAuth data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_AUTH_TYPE;
        data_length = 0x2;
        data = eWscAuth::WSC_AUTH_WPA2PSK;
    }
} __attribute__((packed)) sWscAttrAuthenticationType;

typedef struct sWscAttrEncryptionType {
    eWscAttributes attribute_type;
    uint16_t data_length;
    eWscEncr data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data));
    }
    void struct_init(){
        attribute_type = ATTR_ENCR_TYPE;
        data_length = 0x2;
        data = eWscEncr::WSC_ENCR_AES;
    }
} __attribute__((packed)) sWscAttrEncryptionType;

typedef struct sWscAttrBssid {
    eWscAttributes attribute_type;
    uint16_t data_length;
    sMacAddr data;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&attribute_type));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&data_length));
        data.struct_swap();
    }
    void struct_init(){
        attribute_type = ATTR_MAC_ADDR;
        data_length = WSC_MAC_LENGTH;
    }
} __attribute__((packed)) sWscAttrBssid;

typedef struct sWscWfaVendorExtSubelementVersion2 {
    uint8_t id;
    uint8_t length;
    uint8_t value;
    void struct_swap(){
    }
    void struct_init(){
        id = VERSION2;
        length = 0x1;
        value = WSC_VERSION2;
    }
} __attribute__((packed)) sWscWfaVendorExtSubelementVersion2;

typedef struct sWscWfaVendorExtSubelementMultiApIdentifier {
    uint8_t id;
    uint8_t length;
    uint8_t value;
    void struct_swap(){
    }
    void struct_init(){
        id = MULTI_AP_IDENTIFIER;
        length = 0x1;
        value = TEARDOWN;
    }
} __attribute__((packed)) sWscWfaVendorExtSubelementMultiApIdentifier;


class cWscAttrKeyWrapAuthenticator : public BaseClass
{
    public:
        cWscAttrKeyWrapAuthenticator(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrKeyWrapAuthenticator(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrKeyWrapAuthenticator();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* data(size_t idx = 0);
        bool set_data(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_data = nullptr;
        size_t m_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrVendorExtension : public BaseClass
{
    public:
        cWscAttrVendorExtension(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrVendorExtension(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrVendorExtension();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t& vendor_id_0();
        uint8_t& vendor_id_1();
        uint8_t& vendor_id_2();
        size_t vendor_data_length() { return m_vendor_data_idx__ * sizeof(uint8_t); }
        uint8_t* vendor_data(size_t idx = 0);
        bool set_vendor_data(const void* buffer, size_t size);
        bool alloc_vendor_data(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_vendor_id_0 = nullptr;
        uint8_t* m_vendor_id_1 = nullptr;
        uint8_t* m_vendor_id_2 = nullptr;
        uint8_t* m_vendor_data = nullptr;
        size_t m_vendor_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cConfigData : public BaseClass
{
    public:
        cConfigData(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cConfigData(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cConfigData();

        eWscAttributes& ssid_type();
        uint16_t& ssid_length();
        std::string ssid_str();
        char* ssid(size_t length = 0);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        bool alloc_ssid(size_t count = 1);
        sWscAttrAuthenticationType& authentication_type_attr();
        sWscAttrEncryptionType& encryption_type_attr();
        eWscAttributes& network_key_type();
        uint16_t& network_key_length();
        std::string network_key_str();
        char* network_key(size_t length = 0);
        bool set_network_key(const std::string& str);
        bool set_network_key(const char buffer[], size_t size);
        bool alloc_network_key(size_t count = 1);
        sWscAttrBssid& bssid_attr();
        uint8_t& bss_type();
        int8_t& mld_id();
        uint8_t& hidden_ssid();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_ssid_type = nullptr;
        uint16_t* m_ssid_length = nullptr;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
        sWscAttrAuthenticationType* m_authentication_type_attr = nullptr;
        sWscAttrEncryptionType* m_encryption_type_attr = nullptr;
        eWscAttributes* m_network_key_type = nullptr;
        uint16_t* m_network_key_length = nullptr;
        char* m_network_key = nullptr;
        size_t m_network_key_idx__ = 0;
        sWscAttrBssid* m_bssid_attr = nullptr;
        uint8_t* m_bss_type = nullptr;
        int8_t* m_mld_id = nullptr;
        uint8_t* m_hidden_ssid = nullptr;
};

class cWscAttrEncryptedSettings : public BaseClass
{
    public:
        cWscAttrEncryptedSettings(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrEncryptedSettings(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrEncryptedSettings();

        const eWscAttributes& type();
        const uint16_t& length();
        std::string iv_str();
        char* iv(size_t length = 0);
        bool set_iv(const std::string& str);
        bool set_iv(const char buffer[], size_t size);
        size_t encrypted_settings_length() { return m_encrypted_settings_idx__ * sizeof(char); }
        std::string encrypted_settings_str();
        char* encrypted_settings(size_t length = 0);
        bool set_encrypted_settings(const std::string& str);
        bool set_encrypted_settings(const char buffer[], size_t size);
        bool alloc_encrypted_settings(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_iv = nullptr;
        size_t m_iv_idx__ = 0;
        int m_lock_order_counter__ = 0;
        char* m_encrypted_settings = nullptr;
        size_t m_encrypted_settings_idx__ = 0;
};

class cWscAttrVersion : public BaseClass
{
    public:
        cWscAttrVersion(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrVersion(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrVersion();

        eWscAttributes& type();
        const uint16_t& length();
        eWscValues8& data();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscValues8* m_data = nullptr;
};

class cWscAttrMessageType : public BaseClass
{
    public:
        cWscAttrMessageType(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrMessageType(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrMessageType();

        eWscAttributes& type();
        const uint16_t& length();
        eWscMessageType& msg_type();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscMessageType* m_msg_type = nullptr;
};

class cWscAttrEnrolleeNonce : public BaseClass
{
    public:
        cWscAttrEnrolleeNonce(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrEnrolleeNonce(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrEnrolleeNonce();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* nonce(size_t idx = 0);
        bool set_nonce(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_nonce = nullptr;
        size_t m_nonce_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrPublicKey : public BaseClass
{
    public:
        cWscAttrPublicKey(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrPublicKey(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrPublicKey();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* public_key(size_t idx = 0);
        bool set_public_key(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_public_key = nullptr;
        size_t m_public_key_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrAuthenticationTypeFlags : public BaseClass
{
    public:
        cWscAttrAuthenticationTypeFlags(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrAuthenticationTypeFlags(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrAuthenticationTypeFlags();

        eWscAttributes& type();
        const uint16_t& length();
        eWscAuth& auth_type_flags();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscAuth* m_auth_type_flags = nullptr;
};

class cWscAttrEncryptionTypeFlags : public BaseClass
{
    public:
        cWscAttrEncryptionTypeFlags(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrEncryptionTypeFlags(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrEncryptionTypeFlags();

        eWscAttributes& type();
        const uint16_t& length();
        uint16_t& encr_type_flags();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_encr_type_flags = nullptr;
};

class cWscAttrConnectionTypeFlags : public BaseClass
{
    public:
        cWscAttrConnectionTypeFlags(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrConnectionTypeFlags(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrConnectionTypeFlags();

        eWscAttributes& type();
        const uint16_t& length();
        eWscConn& conn_type_flags();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscConn* m_conn_type_flags = nullptr;
};

class cWscAttrConfigurationMethods : public BaseClass
{
    public:
        cWscAttrConfigurationMethods(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrConfigurationMethods(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrConfigurationMethods();

        eWscAttributes& type();
        const uint16_t& length();
        uint16_t& conf_methods();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_conf_methods = nullptr;
};

class cWscAttrManufacturer : public BaseClass
{
    public:
        cWscAttrManufacturer(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrManufacturer(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrManufacturer();

        eWscAttributes& type();
        const uint16_t& length();
        size_t manufacturer_length() { return m_manufacturer_idx__ * sizeof(char); }
        std::string manufacturer_str();
        char* manufacturer(size_t length = 0);
        bool set_manufacturer(const std::string& str);
        bool set_manufacturer(const char buffer[], size_t size);
        bool alloc_manufacturer(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_manufacturer = nullptr;
        size_t m_manufacturer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrModelName : public BaseClass
{
    public:
        cWscAttrModelName(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrModelName(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrModelName();

        eWscAttributes& type();
        const uint16_t& length();
        size_t model_length() { return m_model_idx__ * sizeof(char); }
        std::string model_str();
        char* model(size_t length = 0);
        bool set_model(const std::string& str);
        bool set_model(const char buffer[], size_t size);
        bool alloc_model(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_model = nullptr;
        size_t m_model_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrModelNumber : public BaseClass
{
    public:
        cWscAttrModelNumber(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrModelNumber(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrModelNumber();

        eWscAttributes& type();
        const uint16_t& length();
        size_t model_number_length() { return m_model_number_idx__ * sizeof(char); }
        std::string model_number_str();
        char* model_number(size_t length = 0);
        bool set_model_number(const std::string& str);
        bool set_model_number(const char buffer[], size_t size);
        bool alloc_model_number(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_model_number = nullptr;
        size_t m_model_number_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrSerialNumber : public BaseClass
{
    public:
        cWscAttrSerialNumber(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrSerialNumber(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrSerialNumber();

        eWscAttributes& type();
        const uint16_t& length();
        size_t serial_number_length() { return m_serial_number_idx__ * sizeof(char); }
        std::string serial_number_str();
        char* serial_number(size_t length = 0);
        bool set_serial_number(const std::string& str);
        bool set_serial_number(const char buffer[], size_t size);
        bool alloc_serial_number(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_serial_number = nullptr;
        size_t m_serial_number_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrPrimaryDeviceType : public BaseClass
{
    public:
        cWscAttrPrimaryDeviceType(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrPrimaryDeviceType(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrPrimaryDeviceType();

        eWscAttributes& type();
        const uint16_t& length();
        uint16_t& category_id();
        uint32_t& oui();
        uint16_t& sub_category_id();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_category_id = nullptr;
        uint32_t* m_oui = nullptr;
        uint16_t* m_sub_category_id = nullptr;
};

class cWscAttrDeviceName : public BaseClass
{
    public:
        cWscAttrDeviceName(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrDeviceName(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrDeviceName();

        eWscAttributes& type();
        const uint16_t& length();
        size_t device_name_length() { return m_device_name_idx__ * sizeof(char); }
        std::string device_name_str();
        char* device_name(size_t length = 0);
        bool set_device_name(const std::string& str);
        bool set_device_name(const char buffer[], size_t size);
        bool alloc_device_name(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_device_name = nullptr;
        size_t m_device_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrRfBands : public BaseClass
{
    public:
        cWscAttrRfBands(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrRfBands(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrRfBands();

        eWscAttributes& type();
        const uint16_t& length();
        eWscRfBands& bands();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscRfBands* m_bands = nullptr;
};

class cWscAttrAssociationState : public BaseClass
{
    public:
        cWscAttrAssociationState(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrAssociationState(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrAssociationState();

        eWscAttributes& type();
        const uint16_t& length();
        eWscAssoc& assoc_state();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscAssoc* m_assoc_state = nullptr;
};

class cWscAttrDevicePasswordID : public BaseClass
{
    public:
        cWscAttrDevicePasswordID(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrDevicePasswordID(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrDevicePasswordID();

        eWscAttributes& type();
        const uint16_t& length();
        eWscValues16& pw();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscValues16* m_pw = nullptr;
};

class cWscAttrConfigurationError : public BaseClass
{
    public:
        cWscAttrConfigurationError(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrConfigurationError(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrConfigurationError();

        eWscAttributes& type();
        const uint16_t& length();
        eWscValues16& cfg_err();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscValues16* m_cfg_err = nullptr;
};

class cWscAttrOsVersion : public BaseClass
{
    public:
        cWscAttrOsVersion(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrOsVersion(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrOsVersion();

        eWscAttributes& type();
        const uint16_t& length();
        uint32_t& os_version();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint32_t* m_os_version = nullptr;
};

class cWscAttrMac : public BaseClass
{
    public:
        cWscAttrMac(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrMac(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrMac();

        eWscAttributes& type();
        const uint16_t& length();
        sMacAddr& data();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_data = nullptr;
};

class cWscAttrUuidE : public BaseClass
{
    public:
        cWscAttrUuidE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrUuidE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrUuidE();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* data(size_t idx = 0);
        bool set_data(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_data = nullptr;
        size_t m_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrWscState : public BaseClass
{
    public:
        cWscAttrWscState(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrWscState(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrWscState();

        eWscAttributes& type();
        const uint16_t& length();
        eWscState& state();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscState* m_state = nullptr;
};

class cWscAttrUuidR : public BaseClass
{
    public:
        cWscAttrUuidR(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrUuidR(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrUuidR();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* data(size_t idx = 0);
        bool set_data(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_data = nullptr;
        size_t m_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrAuthenticator : public BaseClass
{
    public:
        cWscAttrAuthenticator(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrAuthenticator(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrAuthenticator();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* data(size_t idx = 0);
        bool set_data(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_data = nullptr;
        size_t m_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrRegistrarNonce : public BaseClass
{
    public:
        cWscAttrRegistrarNonce(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrRegistrarNonce(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrRegistrarNonce();

        eWscAttributes& type();
        const uint16_t& length();
        uint8_t* nonce(size_t idx = 0);
        bool set_nonce(const void* buffer, size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_nonce = nullptr;
        size_t m_nonce_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrSsid : public BaseClass
{
    public:
        cWscAttrSsid(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrSsid(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrSsid();

        eWscAttributes& type();
        const uint16_t& length();
        size_t ssid_length() { return m_ssid_idx__ * sizeof(char); }
        std::string ssid_str();
        char* ssid(size_t length = 0);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        bool alloc_ssid(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cWscAttrAuthenticationType : public BaseClass
{
    public:
        cWscAttrAuthenticationType(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrAuthenticationType(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrAuthenticationType();

        eWscAttributes& type();
        const uint16_t& length();
        eWscAuth& data();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscAuth* m_data = nullptr;
};

class cWscAttrEncryptionType : public BaseClass
{
    public:
        cWscAttrEncryptionType(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrEncryptionType(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrEncryptionType();

        eWscAttributes& type();
        const uint16_t& length();
        eWscEncr& data();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eWscEncr* m_data = nullptr;
};

class cWscAttrNetworkKey : public BaseClass
{
    public:
        cWscAttrNetworkKey(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cWscAttrNetworkKey(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cWscAttrNetworkKey();

        eWscAttributes& type();
        const uint16_t& length();
        size_t key_length() { return m_key_idx__ * sizeof(char); }
        std::string key_str();
        char* key(size_t length = 0);
        bool set_key(const std::string& str);
        bool set_key(const char buffer[], size_t size);
        bool alloc_key(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eWscAttributes* m_type = nullptr;
        uint16_t* m_length = nullptr;
        char* m_key = nullptr;
        size_t m_key_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: WSC

#endif //_TLVF/WSC_WSC_ATTRIBUTES_H_
