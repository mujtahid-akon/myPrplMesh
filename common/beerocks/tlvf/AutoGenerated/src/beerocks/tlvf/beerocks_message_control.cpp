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

#include <beerocks/tlvf/beerocks_message_control.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST::cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST::cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST::~cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST() {
}
void cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE::cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE::cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE::~cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE() {
}
void cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::~cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION() {
}
std::string cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::slave_version_str() {
    char *slave_version_ = slave_version();
    if (!slave_version_) { return std::string(); }
    auto str = std::string(slave_version_, m_slave_version_idx__);
    auto pos = str.find_first_of('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

char* cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::slave_version(size_t length) {
    if( (m_slave_version_idx__ == 0) || (m_slave_version_idx__ < length) ) {
        TLVF_LOG(ERROR) << "slave_version length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_slave_version);
}

bool cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::set_slave_version(const std::string& str) { return set_slave_version(str.c_str(), str.size()); }
bool cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::set_slave_version(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_slave_version received a null pointer.";
        return false;
    }
    if (size > beerocks::message::VERSION_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_slave_version);
    return true;
}
sPlatformSettings& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::platform_settings() {
    return (sPlatformSettings&)(*m_platform_settings);
}

sWlanSettings& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::wlan_settings() {
    return (sWlanSettings&)(*m_wlan_settings);
}

sBackhaulParams& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::backhaul_params() {
    return (sBackhaulParams&)(*m_backhaul_params);
}

sNodeHostapVendorSpec& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::hostap() {
    return (sNodeHostapVendorSpec&)(*m_hostap);
}

sApChannelSwitch& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

uint8_t& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::low_pass_filter_on() {
    return (uint8_t&)(*m_low_pass_filter_on);
}

uint8_t& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::enable_repeater_mode() {
    return (uint8_t&)(*m_enable_repeater_mode);
}

void cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_platform_settings->struct_swap();
    m_wlan_settings->struct_swap();
    m_backhaul_params->struct_swap();
    m_hostap->struct_swap();
    m_cs_params->struct_swap();
}

bool cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::VERSION_LENGTH * sizeof(char); // slave_version
    class_size += sizeof(sPlatformSettings); // platform_settings
    class_size += sizeof(sWlanSettings); // wlan_settings
    class_size += sizeof(sBackhaulParams); // backhaul_params
    class_size += sizeof(sNodeHostapVendorSpec); // hostap
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += sizeof(uint8_t); // low_pass_filter_on
    class_size += sizeof(uint8_t); // enable_repeater_mode
    return class_size;
}

bool cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_slave_version = reinterpret_cast<char*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::VERSION_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::VERSION_LENGTH) << ") Failed!";
        return false;
    }
    m_slave_version_idx__  = beerocks::message::VERSION_LENGTH;
    m_platform_settings = reinterpret_cast<sPlatformSettings*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sPlatformSettings))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sPlatformSettings) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_platform_settings->struct_init(); }
    m_wlan_settings = reinterpret_cast<sWlanSettings*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sWlanSettings))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWlanSettings) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_wlan_settings->struct_init(); }
    m_backhaul_params = reinterpret_cast<sBackhaulParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sBackhaulParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sBackhaulParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_backhaul_params->struct_init(); }
    m_hostap = reinterpret_cast<sNodeHostapVendorSpec*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeHostapVendorSpec))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeHostapVendorSpec) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_hostap->struct_init(); }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_low_pass_filter_on = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_enable_repeater_mode = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_SLAVE_JOINED_RESPONSE::cACTION_CONTROL_SLAVE_JOINED_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_RESPONSE::cACTION_CONTROL_SLAVE_JOINED_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_RESPONSE::~cACTION_CONTROL_SLAVE_JOINED_RESPONSE() {
}
std::string cACTION_CONTROL_SLAVE_JOINED_RESPONSE::master_version_str() {
    char *master_version_ = master_version();
    if (!master_version_) { return std::string(); }
    auto str = std::string(master_version_, m_master_version_idx__);
    auto pos = str.find_first_of('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

char* cACTION_CONTROL_SLAVE_JOINED_RESPONSE::master_version(size_t length) {
    if( (m_master_version_idx__ == 0) || (m_master_version_idx__ < length) ) {
        TLVF_LOG(ERROR) << "master_version length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_master_version);
}

bool cACTION_CONTROL_SLAVE_JOINED_RESPONSE::set_master_version(const std::string& str) { return set_master_version(str.c_str(), str.size()); }
bool cACTION_CONTROL_SLAVE_JOINED_RESPONSE::set_master_version(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_master_version received a null pointer.";
        return false;
    }
    if (size > beerocks::message::VERSION_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_master_version);
    return true;
}
uint8_t& cACTION_CONTROL_SLAVE_JOINED_RESPONSE::err_code() {
    return (uint8_t&)(*m_err_code);
}

sSonConfig& cACTION_CONTROL_SLAVE_JOINED_RESPONSE::config() {
    return (sSonConfig&)(*m_config);
}

void cACTION_CONTROL_SLAVE_JOINED_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_config->struct_swap();
}

bool cACTION_CONTROL_SLAVE_JOINED_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_SLAVE_JOINED_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::VERSION_LENGTH * sizeof(char); // master_version
    class_size += sizeof(uint8_t); // err_code
    class_size += sizeof(sSonConfig); // config
    return class_size;
}

bool cACTION_CONTROL_SLAVE_JOINED_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_master_version = reinterpret_cast<char*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::VERSION_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::VERSION_LENGTH) << ") Failed!";
        return false;
    }
    m_master_version_idx__  = beerocks::message::VERSION_LENGTH;
    m_err_code = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_config = reinterpret_cast<sSonConfig*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSonConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSonConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_config->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_ARP_QUERY_REQUEST::cACTION_CONTROL_ARP_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_ARP_QUERY_REQUEST::cACTION_CONTROL_ARP_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_ARP_QUERY_REQUEST::~cACTION_CONTROL_ARP_QUERY_REQUEST() {
}
sArpQuery& cACTION_CONTROL_ARP_QUERY_REQUEST::params() {
    return (sArpQuery&)(*m_params);
}

void cACTION_CONTROL_ARP_QUERY_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_ARP_QUERY_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_ARP_QUERY_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpQuery); // params
    return class_size;
}

bool cACTION_CONTROL_ARP_QUERY_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sArpQuery*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sArpQuery))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sArpQuery) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_ARP_QUERY_RESPONSE::cACTION_CONTROL_ARP_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_ARP_QUERY_RESPONSE::cACTION_CONTROL_ARP_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_ARP_QUERY_RESPONSE::~cACTION_CONTROL_ARP_QUERY_RESPONSE() {
}
sArpMonitorData& cACTION_CONTROL_ARP_QUERY_RESPONSE::params() {
    return (sArpMonitorData&)(*m_params);
}

void cACTION_CONTROL_ARP_QUERY_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_ARP_QUERY_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_ARP_QUERY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpMonitorData); // params
    return class_size;
}

bool cACTION_CONTROL_ARP_QUERY_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sArpMonitorData*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sArpMonitorData))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sArpMonitorData) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::~cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL() {
}
sLoggingLevelChange& cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::params() {
    return (sLoggingLevelChange&)(*m_params);
}

void cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sLoggingLevelChange); // params
    return class_size;
}

bool cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sLoggingLevelChange*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sLoggingLevelChange))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sLoggingLevelChange) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::~cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::~cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::~cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::~cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION() {
}
sDfsCacCompleted& cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::params() {
    return (sDfsCacCompleted&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsCacCompleted); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sDfsCacCompleted*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sDfsCacCompleted))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDfsCacCompleted) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::~cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION() {
}
sDfsChannelAvailable& cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::params() {
    return (sDfsChannelAvailable&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsChannelAvailable); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sDfsChannelAvailable*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sDfsChannelAvailable))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDfsChannelAvailable) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::~cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST() {
}
sApSetRestrictedFailsafe& cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::params() {
    return (sApSetRestrictedFailsafe&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApSetRestrictedFailsafe); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sApSetRestrictedFailsafe*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApSetRestrictedFailsafe))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApSetRestrictedFailsafe) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::~cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE() {
}
uint8_t& cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_success = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::~cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::~cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST() {
}
uint32_t& cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::attempts() {
    return (uint32_t&)(*m_attempts);
}

void cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_attempts));
}

bool cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // attempts
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_attempts = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::~cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST() {
}
uint8_t& cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::sync() {
    return (uint8_t&)(*m_sync);
}

void cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // sync
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sync = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::~cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE() {
}
uint8_t& cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::ap_stats_size() {
    return (uint8_t&)(*m_ap_stats_size);
}

std::tuple<bool, sApStatsParams&> cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::ap_stats(size_t idx) {
    bool ret_success = ( (m_ap_stats_idx__ > 0) && (m_ap_stats_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_ap_stats[ret_idx]);
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::alloc_ap_stats(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list ap_stats, abort!";
        return false;
    }
    size_t len = sizeof(sApStatsParams) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_ap_stats[*m_ap_stats_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_sta_stats_size = (uint8_t *)((uint8_t *)(m_sta_stats_size) + len);
    m_sta_stats = (sStaStatsParams *)((uint8_t *)(m_sta_stats) + len);
    m_ap_stats_idx__ += count;
    *m_ap_stats_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_ap_stats_idx__ - count; i < m_ap_stats_idx__; i++) { m_ap_stats[i].struct_init(); }
    }
    return true;
}

uint8_t& cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::sta_stats_size() {
    return (uint8_t&)(*m_sta_stats_size);
}

std::tuple<bool, sStaStatsParams&> cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::sta_stats(size_t idx) {
    bool ret_success = ( (m_sta_stats_idx__ > 0) && (m_sta_stats_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_sta_stats[ret_idx]);
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::alloc_sta_stats(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list sta_stats, abort!";
        return false;
    }
    size_t len = sizeof(sStaStatsParams) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_sta_stats[*m_sta_stats_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_sta_stats_idx__ += count;
    *m_sta_stats_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_sta_stats_idx__ - count; i < m_sta_stats_idx__; i++) { m_sta_stats[i].struct_init(); }
    }
    return true;
}

void cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    for (size_t i = 0; i < m_ap_stats_idx__; i++){
        m_ap_stats[i].struct_swap();
    }
    for (size_t i = 0; i < m_sta_stats_idx__; i++){
        m_sta_stats[i].struct_swap();
    }
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // ap_stats_size
    class_size += sizeof(uint8_t); // sta_stats_size
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_stats_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_ap_stats_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_ap_stats = reinterpret_cast<sApStatsParams*>(m_buff_ptr__);
    uint8_t ap_stats_size = *m_ap_stats_size;
    m_ap_stats_idx__ = ap_stats_size;
    if (!buffPtrIncrementSafe(sizeof(sApStatsParams) * (ap_stats_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApStatsParams) * (ap_stats_size) << ") Failed!";
        return false;
    }
    m_sta_stats_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_sta_stats_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_sta_stats = reinterpret_cast<sStaStatsParams*>(m_buff_ptr__);
    uint8_t sta_stats_size = *m_sta_stats_size;
    m_sta_stats_idx__ = sta_stats_size;
    if (!buffPtrIncrementSafe(sizeof(sStaStatsParams) * (sta_stats_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sStaStatsParams) * (sta_stats_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::~cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION() {
}
sApLoadNotificationParams& cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::params() {
    return (sApLoadNotificationParams&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApLoadNotificationParams); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sApLoadNotificationParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApLoadNotificationParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApLoadNotificationParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::~cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST() {
}
sNeighborSetParams11k& cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::params() {
    return (sNeighborSetParams11k&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborSetParams11k); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNeighborSetParams11k*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNeighborSetParams11k))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNeighborSetParams11k) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::~cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST() {
}
sNeighborRemoveParams11k& cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::params() {
    return (sNeighborRemoveParams11k&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborRemoveParams11k); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNeighborRemoveParams11k*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNeighborRemoveParams11k))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNeighborRemoveParams11k) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::~cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION() {
}
sApActivityNotificationParams& cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::params() {
    return (sApActivityNotificationParams&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApActivityNotificationParams); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sApActivityNotificationParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApActivityNotificationParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApActivityNotificationParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::~cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION() {
}
sVapsList& cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::params() {
    return (sVapsList&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVapsList); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sVapsList*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sVapsList))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVapsList) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::~cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION() {
}
int8_t& cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

void cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::~cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION() {
}
int8_t& cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

sVapInfo& cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::vap_info() {
    return (sVapInfo&)(*m_vap_info);
}

void cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_vap_info->struct_swap();
}

bool cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(sVapInfo); // vap_info
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_vap_info = reinterpret_cast<sVapInfo*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sVapInfo))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVapInfo) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_vap_info->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::~cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST() {
}
sClientMonitoringParams& cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::params() {
    return (sClientMonitoringParams&)(*m_params);
}

void cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientMonitoringParams); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sClientMonitoringParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sClientMonitoringParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sClientMonitoringParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE::cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE::cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE::~cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE() {
}
uint8_t& cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_success = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST() {
}
sNodeRssiMeasurementRequest& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::params() {
    return (sNodeRssiMeasurementRequest&)(*m_params);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurementRequest); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeRssiMeasurementRequest*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurementRequest))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeRssiMeasurementRequest) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE() {
}
sNodeRssiMeasurement& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::params() {
    return (sNodeRssiMeasurement&)(*m_params);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeRssiMeasurement*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurement))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeRssiMeasurement) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE() {
}
sMacAddr& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION() {
}
sNodeRssiMeasurement& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::params() {
    return (sNodeRssiMeasurement&)(*m_params);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeRssiMeasurement*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurement))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeRssiMeasurement) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::~cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::~cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::~cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

beerocks::net::sIpv4Addr& cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::ipv4() {
    return (beerocks::net::sIpv4Addr&)(*m_ipv4);
}

void cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    m_ipv4->struct_swap();
}

bool cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // ipv4
    return class_size;
}

bool cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_ipv4 = reinterpret_cast<beerocks::net::sIpv4Addr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(beerocks::net::sIpv4Addr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::net::sIpv4Addr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_ipv4->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::~cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST() {
}
sMacAddr& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

int8_t& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::vap_id() {
    return (int8_t&)(*m_vap_id);
}

eDisconnectType& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::type() {
    return (eDisconnectType&)(*m_type);
}

uint32_t& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::reason() {
    return (uint32_t&)(*m_reason);
}

eClientDisconnectSource& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::src() {
    return (eClientDisconnectSource&)(*m_src);
}

void cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    tlvf_swap(8*sizeof(eDisconnectType), reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_reason));
    tlvf_swap(8*sizeof(eClientDisconnectSource), reinterpret_cast<uint8_t*>(m_src));
}

bool cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(eDisconnectType); // type
    class_size += sizeof(uint32_t); // reason
    class_size += sizeof(eClientDisconnectSource); // src
    return class_size;
}

bool cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_type = reinterpret_cast<eDisconnectType*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(eDisconnectType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eDisconnectType) << ") Failed!";
        return false;
    }
    m_reason = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_src = reinterpret_cast<eClientDisconnectSource*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(eClientDisconnectSource))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eClientDisconnectSource) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::~cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE() {
}
sClientDisconnectResponse& cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::params() {
    return (sClientDisconnectResponse&)(*m_params);
}

void cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectResponse); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sClientDisconnectResponse*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sClientDisconnectResponse))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sClientDisconnectResponse) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::~cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

beerocks::net::sIpv4Addr& cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::ipv4() {
    return (beerocks::net::sIpv4Addr&)(*m_ipv4);
}

std::string cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::name_str() {
    char *name_ = name();
    if (!name_) { return std::string(); }
    auto str = std::string(name_, m_name_idx__);
    auto pos = str.find_first_of('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

char* cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::name(size_t length) {
    if( (m_name_idx__ == 0) || (m_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_name);
}

bool cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::set_name(const std::string& str) { return set_name(str.c_str(), str.size()); }
bool cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::set_name(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_name received a null pointer.";
        return false;
    }
    if (size > beerocks::message::NODE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_name);
    return true;
}
void cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    m_ipv4->struct_swap();
}

bool cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // ipv4
    class_size += beerocks::message::NODE_NAME_LENGTH * sizeof(char); // name
    return class_size;
}

bool cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_ipv4 = reinterpret_cast<beerocks::net::sIpv4Addr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(beerocks::net::sIpv4Addr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::net::sIpv4Addr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_ipv4->struct_init(); }
    m_name = reinterpret_cast<char*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::NODE_NAME_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::NODE_NAME_LENGTH) << ") Failed!";
        return false;
    }
    m_name_idx__  = beerocks::message::NODE_NAME_LENGTH;
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::~cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION() {
}
sArpMonitorData& cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::params() {
    return (sArpMonitorData&)(*m_params);
}

void cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpMonitorData); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sArpMonitorData*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sArpMonitorData))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sArpMonitorData) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::~cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST() {
}
sBeaconRequest11k& cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::params() {
    return (sBeaconRequest11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBeaconRequest11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sBeaconRequest11k*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sBeaconRequest11k))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sBeaconRequest11k) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::~cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE() {
}
sBeaconResponse11k& cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::params() {
    return (sBeaconResponse11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBeaconResponse11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sBeaconResponse11k*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sBeaconResponse11k))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sBeaconResponse11k) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::~cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST() {
}
sSteeringSetGroupRequest& cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::params() {
    return (sSteeringSetGroupRequest&)(*m_params);
}

void cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringSetGroupRequest); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringSetGroupRequest*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringSetGroupRequest))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringSetGroupRequest) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::~cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE() {
}
sSteeringSetGroupResponse& cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::params() {
    return (sSteeringSetGroupResponse&)(*m_params);
}

void cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringSetGroupResponse); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringSetGroupResponse*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringSetGroupResponse))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringSetGroupResponse) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::~cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST() {
}
sSteeringClientSetRequest& cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::params() {
    return (sSteeringClientSetRequest&)(*m_params);
}

void cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetRequest); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringClientSetRequest*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringClientSetRequest))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringClientSetRequest) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::~cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE() {
}
sSteeringClientSetResponse& cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::params() {
    return (sSteeringClientSetResponse&)(*m_params);
}

void cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetResponse); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringClientSetResponse*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringClientSetResponse))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringClientSetResponse) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::~cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION() {
}
sSteeringEvActivity& cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::params() {
    return (sSteeringEvActivity&)(*m_params);
}

void cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvActivity); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringEvActivity*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringEvActivity))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringEvActivity) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::~cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION() {
}
sSteeringEvSnrXing& cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::params() {
    return (sSteeringEvSnrXing&)(*m_params);
}

void cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvSnrXing); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringEvSnrXing*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringEvSnrXing))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringEvSnrXing) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::~cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION() {
}
sSteeringEvProbeReq& cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::params() {
    return (sSteeringEvProbeReq&)(*m_params);
}

void cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvProbeReq); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringEvProbeReq*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringEvProbeReq))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringEvProbeReq) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::~cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION() {
}
sSteeringEvAuthFail& cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::params() {
    return (sSteeringEvAuthFail&)(*m_params);
}

void cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvAuthFail); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringEvAuthFail*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringEvAuthFail))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringEvAuthFail) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST::cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST::cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST::~cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST() {
}
sTriggerChannelScanParams& cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST::scan_params() {
    return (sTriggerChannelScanParams&)(*m_scan_params);
}

void cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_scan_params->struct_swap();
}

bool cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sTriggerChannelScanParams); // scan_params
    return class_size;
}

bool cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_scan_params = reinterpret_cast<sTriggerChannelScanParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sTriggerChannelScanParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sTriggerChannelScanParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_scan_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE::cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE::cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE::~cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE() {
}
uint8_t& cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_success = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::~cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST() {
}
void cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION::cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION::cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION::~cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

void cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_radio_mac->struct_swap();
}

bool cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    return class_size;
}

bool cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::~cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION() {
}
sChannelScanResults& cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::scan_results() {
    return (sChannelScanResults&)(*m_scan_results);
}

sMacAddr& cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

uint8_t& cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::is_dump() {
    return (uint8_t&)(*m_is_dump);
}

void cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_scan_results->struct_swap();
    m_radio_mac->struct_swap();
}

bool cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sChannelScanResults); // scan_results
    class_size += sizeof(sMacAddr); // radio_mac
    class_size += sizeof(uint8_t); // is_dump
    return class_size;
}

bool cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_scan_results = reinterpret_cast<sChannelScanResults*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sChannelScanResults))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sChannelScanResults) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_scan_results->struct_init(); }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_is_dump = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_is_dump = 0x0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::~cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION() {
}
uint8_t& cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::reason() {
    return (uint8_t&)(*m_reason);
}

sMacAddr& cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

void cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_radio_mac->struct_swap();
}

bool cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // reason
    class_size += sizeof(sMacAddr); // radio_mac
    return class_size;
}

bool cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_reason = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION::cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION::cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION::~cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

void cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_CONTROL), reinterpret_cast<uint8_t*>(m_action_op));
    m_radio_mac->struct_swap();
}

bool cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    return class_size;
}

bool cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}


