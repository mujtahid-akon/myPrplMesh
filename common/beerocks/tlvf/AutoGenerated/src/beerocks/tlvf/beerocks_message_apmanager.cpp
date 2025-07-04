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

#include <beerocks/tlvf/beerocks_message_apmanager.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_APMANAGER_UP_NOTIFICATION::cACTION_APMANAGER_UP_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_UP_NOTIFICATION::cACTION_APMANAGER_UP_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_UP_NOTIFICATION::~cACTION_APMANAGER_UP_NOTIFICATION() {
}
uint8_t& cACTION_APMANAGER_UP_NOTIFICATION::iface_name_length() {
    return (uint8_t&)(*m_iface_name_length);
}

std::string cACTION_APMANAGER_UP_NOTIFICATION::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    auto str = std::string(iface_name_, m_iface_name_idx__);
    auto pos = str.find_first_of('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

char* cACTION_APMANAGER_UP_NOTIFICATION::iface_name(size_t length) {
    if( (m_iface_name_idx__ == 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_APMANAGER_UP_NOTIFICATION::set_iface_name(const std::string& str) { return set_iface_name(str.c_str(), str.size()); }
bool cACTION_APMANAGER_UP_NOTIFICATION::set_iface_name(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_iface_name received a null pointer.";
        return false;
    }
    if (m_iface_name_idx__ != 0) {
        TLVF_LOG(ERROR) << "set_iface_name was already allocated!";
        return false;
    }
    if (!alloc_iface_name(size)) { return false; }
    std::copy(str, str + size, m_iface_name);
    return true;
}
bool cACTION_APMANAGER_UP_NOTIFICATION::alloc_iface_name(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list iface_name, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_iface_name[*m_iface_name_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_iface_name_idx__ += count;
    *m_iface_name_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_APMANAGER_UP_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_UP_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_UP_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // iface_name_length
    return class_size;
}

bool cACTION_APMANAGER_UP_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_iface_name_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_iface_name = reinterpret_cast<char*>(m_buff_ptr__);
    uint8_t iface_name_length = *m_iface_name_length;
    m_iface_name_idx__ = iface_name_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (iface_name_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (iface_name_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CONFIGURE::cACTION_APMANAGER_CONFIGURE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CONFIGURE::cACTION_APMANAGER_CONFIGURE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CONFIGURE::~cACTION_APMANAGER_CONFIGURE() {
}
uint8_t& cACTION_APMANAGER_CONFIGURE::channel() {
    return (uint8_t&)(*m_channel);
}

uint8_t& cACTION_APMANAGER_CONFIGURE::certification_mode() {
    return (uint8_t&)(*m_certification_mode);
}

void cACTION_APMANAGER_CONFIGURE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_CONFIGURE::finalize()
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

size_t cACTION_APMANAGER_CONFIGURE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // channel
    class_size += sizeof(uint8_t); // certification_mode
    return class_size;
}

bool cACTION_APMANAGER_CONFIGURE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_channel = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_certification_mode = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_JOINED_NOTIFICATION::cACTION_APMANAGER_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_JOINED_NOTIFICATION::cACTION_APMANAGER_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_JOINED_NOTIFICATION::~cACTION_APMANAGER_JOINED_NOTIFICATION() {
}
sNodeHostap& cACTION_APMANAGER_JOINED_NOTIFICATION::params() {
    return (sNodeHostap&)(*m_params);
}

sApChannelSwitch& cACTION_APMANAGER_JOINED_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::isPostInitSucceeded() {
    if (!m_channel_list_init) {
        TLVF_LOG(ERROR) << "channel_list is not initialized";
        return false;
    }
    return true; 
}

std::shared_ptr<cChannelList> cACTION_APMANAGER_JOINED_NOTIFICATION::create_channel_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list channel_list, abort!";
        return nullptr;
    }
    size_t len = cChannelList::get_initial_size();
    if (m_lock_allocation__) {
        TLVF_LOG(ERROR) << "Can't create new element before adding the previous one";
        return nullptr;
    }
    if (getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_channel_list;
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_vap_list = (sVapsList *)((uint8_t *)(m_vap_list) + len);
    m_radio_max_bss = (uint8_t *)((uint8_t *)(m_radio_max_bss) + len);
    return std::make_shared<cChannelList>(src, getBuffRemainingBytes(src), m_parse__);
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::add_channel_list(std::shared_ptr<cChannelList> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_channel_list was called before add_channel_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_channel_list;
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_channel_list_init = true;
    size_t len = ptr->getLen();
    m_vap_list = (sVapsList *)((uint8_t *)(m_vap_list) + len - ptr->get_initial_size());
    m_radio_max_bss = (uint8_t *)((uint8_t *)(m_radio_max_bss) + len - ptr->get_initial_size());
    m_channel_list_ptr = ptr;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    m_lock_allocation__ = false;
    return true;
}

sVapsList& cACTION_APMANAGER_JOINED_NOTIFICATION::vap_list() {
    return (sVapsList&)(*m_vap_list);
}

uint8_t& cACTION_APMANAGER_JOINED_NOTIFICATION::radio_max_bss() {
    return (uint8_t&)(*m_radio_max_bss);
}

void cACTION_APMANAGER_JOINED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
    m_cs_params->struct_swap();
    if (m_channel_list_ptr) { m_channel_list_ptr->class_swap(); }
    m_vap_list->struct_swap();
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_JOINED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeHostap); // params
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += sizeof(sVapsList); // vap_list
    class_size += sizeof(uint8_t); // radio_max_bss
    return class_size;
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeHostap*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeHostap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeHostap) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_channel_list = reinterpret_cast<cChannelList*>(m_buff_ptr__);
    if (m_parse__) {
        auto channel_list = create_channel_list();
        if (!channel_list || !channel_list->isInitialized()) {
            TLVF_LOG(ERROR) << "create_channel_list() failed";
            return false;
        }
        if (!add_channel_list(channel_list)) {
            TLVF_LOG(ERROR) << "add_channel_list() failed";
            return false;
        }
        // swap back since channel_list will be swapped as part of the whole class swap
        channel_list->class_swap();
    }
    m_vap_list = reinterpret_cast<sVapsList*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sVapsList))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVapsList) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_vap_list->struct_init(); }
    m_radio_max_bss = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_ENABLE_APS_REQUEST::cACTION_APMANAGER_ENABLE_APS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_ENABLE_APS_REQUEST::cACTION_APMANAGER_ENABLE_APS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_ENABLE_APS_REQUEST::~cACTION_APMANAGER_ENABLE_APS_REQUEST() {
}
uint8_t& cACTION_APMANAGER_ENABLE_APS_REQUEST::channel() {
    return (uint8_t&)(*m_channel);
}

beerocks::eWiFiBandwidth& cACTION_APMANAGER_ENABLE_APS_REQUEST::bandwidth() {
    return (beerocks::eWiFiBandwidth&)(*m_bandwidth);
}

uint8_t& cACTION_APMANAGER_ENABLE_APS_REQUEST::center_channel() {
    return (uint8_t&)(*m_center_channel);
}

void cACTION_APMANAGER_ENABLE_APS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(8*sizeof(beerocks::eWiFiBandwidth), reinterpret_cast<uint8_t*>(m_bandwidth));
}

bool cACTION_APMANAGER_ENABLE_APS_REQUEST::finalize()
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

size_t cACTION_APMANAGER_ENABLE_APS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // channel
    class_size += sizeof(beerocks::eWiFiBandwidth); // bandwidth
    class_size += sizeof(uint8_t); // center_channel
    return class_size;
}

bool cACTION_APMANAGER_ENABLE_APS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_channel = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_bandwidth = reinterpret_cast<beerocks::eWiFiBandwidth*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(beerocks::eWiFiBandwidth))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::eWiFiBandwidth) << ") Failed!";
        return false;
    }
    m_center_channel = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_ENABLE_APS_RESPONSE::cACTION_APMANAGER_ENABLE_APS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_ENABLE_APS_RESPONSE::cACTION_APMANAGER_ENABLE_APS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_ENABLE_APS_RESPONSE::~cACTION_APMANAGER_ENABLE_APS_RESPONSE() {
}
uint8_t& cACTION_APMANAGER_ENABLE_APS_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_APMANAGER_ENABLE_APS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_ENABLE_APS_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_ENABLE_APS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_APMANAGER_ENABLE_APS_RESPONSE::init()
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

cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::~cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST() {
}
sApSetRestrictedFailsafe& cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::params() {
    return (sApSetRestrictedFailsafe&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApSetRestrictedFailsafe); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::init()
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

cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::~cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE() {
}
uint8_t& cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::init()
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

cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION() {
}
int8_t& cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

void cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::init()
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

cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION() {
}
int8_t& cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

sVapInfo& cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::vap_info() {
    return (sVapInfo&)(*m_vap_info);
}

void cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_vap_info->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(sVapInfo); // vap_info
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::init()
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

cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::~cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST() {
}
void cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::~cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST() {
}
void cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION() {
}
sVapsList& cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::params() {
    return (sVapsList&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVapsList); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::init()
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

cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::~cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

sSpatialReuseParams& cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::sr_params() {
    return (sSpatialReuseParams&)(*m_sr_params);
}

int8_t& cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::tx_limit() {
    return (int8_t&)(*m_tx_limit);
}

uint8_t& cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::tx_limit_valid() {
    return (uint8_t&)(*m_tx_limit_valid);
}

uint8_t& cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::spatial_reuse_valid() {
    return (uint8_t&)(*m_spatial_reuse_valid);
}

void cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
    m_sr_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += sizeof(sSpatialReuseParams); // sr_params
    class_size += sizeof(int8_t); // tx_limit
    class_size += sizeof(uint8_t); // tx_limit_valid
    class_size += sizeof(uint8_t); // spatial_reuse_valid
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::init()
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
    m_sr_params = reinterpret_cast<sSpatialReuseParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSpatialReuseParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSpatialReuseParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_sr_params->struct_init(); }
    m_tx_limit = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_tx_limit_valid = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_spatial_reuse_valid = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST::cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST::cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST::~cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_REQUEST::init()
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

cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE::cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE::cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE::~cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE() {
}
uint8_t& cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CANCEL_ACTIVE_CAC_RESPONSE::init()
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

cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::init()
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

cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::init()
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

cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::init()
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

cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION() {
}
sCacStartedNotificationParams& cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION::params() {
    return (sCacStartedNotificationParams&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sCacStartedNotificationParams); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DFS_CAC_STARTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sCacStartedNotificationParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sCacStartedNotificationParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sCacStartedNotificationParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION() {
}
sDfsCacCompleted& cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::params() {
    return (sDfsCacCompleted&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsCacCompleted); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::init()
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

cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION() {
}
sDfsChannelAvailable& cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::params() {
    return (sDfsChannelAvailable&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsChannelAvailable); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::init()
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

cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::~cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST() {
}
sNeighborSetParams11k& cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::params() {
    return (sNeighborSetParams11k&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborSetParams11k); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::init()
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

cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::~cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST() {
}
sNeighborRemoveParams11k& cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::params() {
    return (sNeighborRemoveParams11k&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborRemoveParams11k); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::init()
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

cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::~cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST() {
}
uint8_t& cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::ant_switch_on() {
    return (uint8_t&)(*m_ant_switch_on);
}

uint8_t& cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::channel() {
    return (uint8_t&)(*m_channel);
}

beerocks::eWiFiBandwidth& cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::bandwidth() {
    return (beerocks::eWiFiBandwidth&)(*m_bandwidth);
}

uint32_t& cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::center_frequency() {
    return (uint32_t&)(*m_center_frequency);
}

uint8_t& cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::csa_count() {
    return (uint8_t&)(*m_csa_count);
}

void cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(8*sizeof(beerocks::eWiFiBandwidth), reinterpret_cast<uint8_t*>(m_bandwidth));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_center_frequency));
}

bool cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // ant_switch_on
    class_size += sizeof(uint8_t); // channel
    class_size += sizeof(beerocks::eWiFiBandwidth); // bandwidth
    class_size += sizeof(uint32_t); // center_frequency
    class_size += sizeof(uint8_t); // csa_count
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ant_switch_on = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_channel = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_bandwidth = reinterpret_cast<beerocks::eWiFiBandwidth*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(beerocks::eWiFiBandwidth))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::eWiFiBandwidth) << ") Failed!";
        return false;
    }
    m_center_frequency = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_csa_count = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_csa_count = 0x5;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE::cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE::cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE::~cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE() {
}
uint8_t& cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ZWDFS_ANT_CHANNEL_SWITCH_RESPONSE::init()
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

cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST::cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST::cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST::~cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST() {
}
uint16_t& cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST::primary_vlan_id() {
    return (uint16_t&)(*m_primary_vlan_id);
}

void cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_primary_vlan_id));
}

bool cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // primary_vlan_id
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_PRIMARY_VLAN_ID_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_primary_vlan_id = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG::cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG::cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG::~cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG() {
}
sServicePrioConfig& cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG::cs_params() {
    return (sServicePrioConfig&)(*m_cs_params);
}

void cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sServicePrioConfig); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SERVICE_PRIO_CONFIG::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sServicePrioConfig*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sServicePrioConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sServicePrioConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION::cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION::cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION() {
}
sSpatialReuseParams& cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION::sr_params() {
    return (sSpatialReuseParams&)(*m_sr_params);
}

void cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_sr_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSpatialReuseParams); // sr_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SPATIAL_REUSE_REPORT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sr_params = reinterpret_cast<sSpatialReuseParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSpatialReuseParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSpatialReuseParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_sr_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::~cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

sMacAddr& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::bssid() {
    return (sMacAddr&)(*m_bssid);
}

beerocks::message::sRadioCapabilities& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::capabilities() {
    return (beerocks::message::sRadioCapabilities&)(*m_capabilities);
}

int8_t& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

uint8_t& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::multi_ap_profile() {
    return (uint8_t&)(*m_multi_ap_profile);
}

uint8_t* cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::association_frame(size_t idx) {
    if ( (m_association_frame_idx__ == 0) || (m_association_frame_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_association_frame[idx]);
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::set_association_frame(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_association_frame received a null pointer.";
        return false;
    }
    if (m_association_frame_idx__ != 0) {
        TLVF_LOG(ERROR) << "set_association_frame was already allocated!";
        return false;
    }
    if (!alloc_association_frame(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_association_frame);
    return true;
}
bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::alloc_association_frame(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list association_frame, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_association_frame[m_association_frame_idx__];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_association_frame_idx__ += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    m_bssid->struct_swap();
    m_capabilities->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(beerocks::message::sRadioCapabilities); // capabilities
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(uint8_t); // multi_ap_profile
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::init()
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
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_capabilities = reinterpret_cast<beerocks::message::sRadioCapabilities*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sRadioCapabilities))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sRadioCapabilities) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_capabilities->struct_init(); }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_multi_ap_profile = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_association_frame = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (m_parse__) {
        size_t len = getBuffRemainingBytes();
        m_association_frame_idx__ = len/sizeof(uint8_t);
        if (!buffPtrIncrementSafe(len)) {
            LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
            return false;
        }
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::~cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION() {
}
sClientDisconnectionParams& cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::params() {
    return (sClientDisconnectionParams&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectionParams); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sClientDisconnectionParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sClientDisconnectionParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sClientDisconnectionParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::~cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

int8_t& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::vap_id() {
    return (int8_t&)(*m_vap_id);
}

eDisconnectType& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::type() {
    return (eDisconnectType&)(*m_type);
}

uint32_t& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::reason() {
    return (uint32_t&)(*m_reason);
}

eClientDisconnectSource& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::src() {
    return (eClientDisconnectSource&)(*m_src);
}

void cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    tlvf_swap(8*sizeof(eDisconnectType), reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_reason));
    tlvf_swap(8*sizeof(eClientDisconnectSource), reinterpret_cast<uint8_t*>(m_src));
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(eDisconnectType); // type
    class_size += sizeof(uint32_t); // reason
    class_size += sizeof(eClientDisconnectSource); // src
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::init()
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

cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::~cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE() {
}
sClientDisconnectResponse& cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::params() {
    return (sClientDisconnectResponse&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::init()
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

cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::~cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST() {
}
uint8_t& cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::sta_list_size() {
    return (uint8_t&)(*m_sta_list_size);
}

std::tuple<bool, sStaAssociationControl&> cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::sta(size_t idx) {
    bool ret_success = ( (m_sta_idx__ > 0) && (m_sta_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_sta[ret_idx]);
}

bool cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::alloc_sta(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list sta, abort!";
        return false;
    }
    size_t len = sizeof(sStaAssociationControl) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_sta[*m_sta_list_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_bssid = (sMacAddr *)((uint8_t *)(m_bssid) + len);
    m_validity_period_sec = (uint16_t *)((uint8_t *)(m_validity_period_sec) + len);
    m_sta_idx__ += count;
    *m_sta_list_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_sta_idx__ - count; i < m_sta_idx__; i++) { m_sta[i].struct_init(); }
    }
    return true;
}

sMacAddr& cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint16_t& cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::validity_period_sec() {
    return (uint16_t&)(*m_validity_period_sec);
}

void cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    for (size_t i = 0; i < m_sta_idx__; i++){
        m_sta[i].struct_swap();
    }
    m_bssid->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_validity_period_sec));
}

bool cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // sta_list_size
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(uint16_t); // validity_period_sec
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sta_list_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_sta_list_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_sta = reinterpret_cast<sStaAssociationControl*>(m_buff_ptr__);
    uint8_t sta_list_size = *m_sta_list_size;
    m_sta_idx__ = sta_list_size;
    if (!buffPtrIncrementSafe(sizeof(sStaAssociationControl) * (sta_list_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sStaAssociationControl) * (sta_list_size) << ") Failed!";
        return false;
    }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_validity_period_sec = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::~cACTION_APMANAGER_CLIENT_ALLOW_REQUEST() {
}
uint8_t& cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::mac_list_size() {
    return (uint8_t&)(*m_mac_list_size);
}

std::tuple<bool, sMacAddr&> cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::mac(size_t idx) {
    bool ret_success = ( (m_mac_idx__ > 0) && (m_mac_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_mac[ret_idx]);
}

bool cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::alloc_mac(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list mac, abort!";
        return false;
    }
    size_t len = sizeof(sMacAddr) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_mac[*m_mac_list_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_bssid = (sMacAddr *)((uint8_t *)(m_bssid) + len);
    m_mac_idx__ += count;
    *m_mac_list_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_mac_idx__ - count; i < m_mac_idx__; i++) { m_mac[i].struct_init(); }
    }
    return true;
}

sMacAddr& cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

void cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    for (size_t i = 0; i < m_mac_idx__; i++){
        m_mac[i].struct_swap();
    }
    m_bssid->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // mac_list_size
    class_size += sizeof(sMacAddr); // bssid
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac_list_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_mac_list_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    uint8_t mac_list_size = *m_mac_list_size;
    m_mac_idx__ = mac_list_size;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr) * (mac_list_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) * (mac_list_size) << ") Failed!";
        return false;
    }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST() {
}
sNodeRssiMeasurementRequest& cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::params() {
    return (sNodeRssiMeasurementRequest&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurementRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::init()
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

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE() {
}
sNodeRssiMeasurement& cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::params() {
    return (sNodeRssiMeasurement&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::init()
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

cACTION_APMANAGER_ACK::cACTION_APMANAGER_ACK(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_ACK::cACTION_APMANAGER_ACK(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_ACK::~cACTION_APMANAGER_ACK() {
}
uint8_t& cACTION_APMANAGER_ACK::reason() {
    return (uint8_t&)(*m_reason);
}

sMacAddr& cACTION_APMANAGER_ACK::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

void cACTION_APMANAGER_ACK::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_sta_mac->struct_swap();
}

bool cACTION_APMANAGER_ACK::finalize()
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

size_t cACTION_APMANAGER_ACK::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // reason
    class_size += sizeof(sMacAddr); // sta_mac
    return class_size;
}

bool cACTION_APMANAGER_ACK::init()
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
    m_sta_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::~cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST() {
}
sNodeBssSteerRequest& cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::params() {
    return (sNodeBssSteerRequest&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeBssSteerRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeBssSteerRequest*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeBssSteerRequest))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeBssSteerRequest) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::~cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE() {
}
sNodeBssSteerResponse& cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::params() {
    return (sNodeBssSteerResponse&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeBssSteerResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeBssSteerResponse*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeBssSteerResponse))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeBssSteerResponse) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
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

cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::~cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST() {
}
sSteeringClientSetRequest& cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::params() {
    return (sSteeringClientSetRequest&)(*m_params);
}

void cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::finalize()
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

size_t cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::init()
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

cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::~cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE() {
}
sSteeringClientSetResponse& cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::params() {
    return (sSteeringClientSetResponse&)(*m_params);
}

void cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::init()
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

cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::~cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION() {
}
sSteeringEvProbeReq& cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::params() {
    return (sSteeringEvProbeReq&)(*m_params);
}

void cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvProbeReq); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::init()
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

cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::~cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION() {
}
sSteeringEvAuthFail& cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::params() {
    return (sSteeringEvAuthFail&)(*m_params);
}

void cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvAuthFail); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::init()
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

cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::~cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST() {
}
uint8_t& cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::bridge_ifname_length() {
    return (uint8_t&)(*m_bridge_ifname_length);
}

std::string cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::bridge_ifname_str() {
    char *bridge_ifname_ = bridge_ifname();
    if (!bridge_ifname_) { return std::string(); }
    auto str = std::string(bridge_ifname_, m_bridge_ifname_idx__);
    auto pos = str.find_first_of('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

char* cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::bridge_ifname(size_t length) {
    if( (m_bridge_ifname_idx__ == 0) || (m_bridge_ifname_idx__ < length) ) {
        TLVF_LOG(ERROR) << "bridge_ifname length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_bridge_ifname);
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::set_bridge_ifname(const std::string& str) { return set_bridge_ifname(str.c_str(), str.size()); }
bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::set_bridge_ifname(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_bridge_ifname received a null pointer.";
        return false;
    }
    if (m_bridge_ifname_idx__ != 0) {
        TLVF_LOG(ERROR) << "set_bridge_ifname was already allocated!";
        return false;
    }
    if (!alloc_bridge_ifname(size)) { return false; }
    std::copy(str, str + size, m_bridge_ifname);
    return true;
}
bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::alloc_bridge_ifname(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list bridge_ifname, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_bridge_ifname[*m_bridge_ifname_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_wifi_credentials_size = (uint8_t *)((uint8_t *)(m_wifi_credentials_size) + len);
    m_wifi_credentials = (WSC::cConfigData *)((uint8_t *)(m_wifi_credentials) + len);
    m_bridge_ifname_idx__ += count;
    *m_bridge_ifname_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

uint8_t& cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::wifi_credentials_size() {
    return (uint8_t&)(*m_wifi_credentials_size);
}

std::tuple<bool, WSC::cConfigData&> cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::wifi_credentials(size_t idx) {
    bool ret_success = ( (m_wifi_credentials_idx__ > 0) && (m_wifi_credentials_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_wifi_credentials_vector[ret_idx]));
}

std::shared_ptr<WSC::cConfigData> cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::create_wifi_credentials() {
    if (m_lock_order_counter__ > 1) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list wifi_credentials, abort!";
        return nullptr;
    }
    size_t len = WSC::cConfigData::get_initial_size();
    if (m_lock_allocation__) {
        TLVF_LOG(ERROR) << "Can't create new element before adding the previous one";
        return nullptr;
    }
    if (getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 1;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_wifi_credentials;
    if (m_wifi_credentials_idx__ > 0) {
        src = (uint8_t *)m_wifi_credentials_vector[m_wifi_credentials_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<WSC::cConfigData>(src, getBuffRemainingBytes(src), m_parse__);
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::add_wifi_credentials(std::shared_ptr<WSC::cConfigData> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_wifi_credentials was called before add_wifi_credentials";
        return false;
    }
    uint8_t *src = (uint8_t *)m_wifi_credentials;
    if (m_wifi_credentials_idx__ > 0) {
        src = (uint8_t *)m_wifi_credentials_vector[m_wifi_credentials_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_wifi_credentials_idx__++;
    if (!m_parse__) { (*m_wifi_credentials_size)++; }
    size_t len = ptr->getLen();
    m_wifi_credentials_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    m_lock_allocation__ = false;
    return true;
}

void cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    for (size_t i = 0; i < m_wifi_credentials_idx__; i++){
        std::get<1>(wifi_credentials(i)).class_swap();
    }
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::finalize()
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

size_t cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // bridge_ifname_length
    class_size += sizeof(uint8_t); // wifi_credentials_size
    return class_size;
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_bridge_ifname_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_bridge_ifname_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_bridge_ifname = reinterpret_cast<char*>(m_buff_ptr__);
    uint8_t bridge_ifname_length = *m_bridge_ifname_length;
    m_bridge_ifname_idx__ = bridge_ifname_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (bridge_ifname_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (bridge_ifname_length) << ") Failed!";
        return false;
    }
    m_wifi_credentials_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_wifi_credentials_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_wifi_credentials = reinterpret_cast<WSC::cConfigData*>(m_buff_ptr__);
    uint8_t wifi_credentials_size = *m_wifi_credentials_size;
    m_wifi_credentials_idx__ = 0;
    for (size_t i = 0; i < wifi_credentials_size; i++) {
        auto wifi_credentials = create_wifi_credentials();
        if (!wifi_credentials || !wifi_credentials->isInitialized()) {
            TLVF_LOG(ERROR) << "create_wifi_credentials() failed";
            return false;
        }
        if (!add_wifi_credentials(wifi_credentials)) {
            TLVF_LOG(ERROR) << "add_wifi_credentials() failed";
            return false;
        }
        // swap back since wifi_credentials will be swapped as part of the whole class swap
        wifi_credentials->class_swap();
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE::~cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE() {
}
uint8_t& cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE::number_of_bss_available() {
    return (uint8_t&)(*m_number_of_bss_available);
}

void cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // number_of_bss_available
    return class_size;
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_number_of_bss_available = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_START_WPS_PBC_REQUEST::cACTION_APMANAGER_START_WPS_PBC_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_START_WPS_PBC_REQUEST::cACTION_APMANAGER_START_WPS_PBC_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_START_WPS_PBC_REQUEST::~cACTION_APMANAGER_START_WPS_PBC_REQUEST() {
}
void cACTION_APMANAGER_START_WPS_PBC_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_START_WPS_PBC_REQUEST::finalize()
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

size_t cACTION_APMANAGER_START_WPS_PBC_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_START_WPS_PBC_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::~cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST() {
}
uint8_t& cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::enable() {
    return (uint8_t&)(*m_enable);
}

sMacAddr& cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

void cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_bssid->struct_swap();
}

bool cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::finalize()
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

size_t cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // enable
    class_size += sizeof(sMacAddr); // bssid
    return class_size;
}

bool cACTION_APMANAGER_SET_ASSOC_DISALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_enable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_RADIO_DISABLE_REQUEST::cACTION_APMANAGER_RADIO_DISABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_RADIO_DISABLE_REQUEST::cACTION_APMANAGER_RADIO_DISABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_RADIO_DISABLE_REQUEST::~cACTION_APMANAGER_RADIO_DISABLE_REQUEST() {
}
void cACTION_APMANAGER_RADIO_DISABLE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_RADIO_DISABLE_REQUEST::finalize()
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

size_t cACTION_APMANAGER_RADIO_DISABLE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_RADIO_DISABLE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_RADIO_ENABLE_REQUEST::cACTION_APMANAGER_RADIO_ENABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_RADIO_ENABLE_REQUEST::cACTION_APMANAGER_RADIO_ENABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_RADIO_ENABLE_REQUEST::~cACTION_APMANAGER_RADIO_ENABLE_REQUEST() {
}
void cACTION_APMANAGER_RADIO_ENABLE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_RADIO_ENABLE_REQUEST::finalize()
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

size_t cACTION_APMANAGER_RADIO_ENABLE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_RADIO_ENABLE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::cACTION_APMANAGER_HEARTBEAT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::cACTION_APMANAGER_HEARTBEAT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::~cACTION_APMANAGER_HEARTBEAT_NOTIFICATION() {
}
void cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CHANNELS_LIST_REQUEST::cACTION_APMANAGER_CHANNELS_LIST_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CHANNELS_LIST_REQUEST::cACTION_APMANAGER_CHANNELS_LIST_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CHANNELS_LIST_REQUEST::~cACTION_APMANAGER_CHANNELS_LIST_REQUEST() {
}
void cACTION_APMANAGER_CHANNELS_LIST_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_CHANNELS_LIST_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CHANNELS_LIST_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_CHANNELS_LIST_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::cACTION_APMANAGER_CHANNELS_LIST_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::cACTION_APMANAGER_CHANNELS_LIST_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::~cACTION_APMANAGER_CHANNELS_LIST_RESPONSE() {
}
bool cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::isPostInitSucceeded() {
    if (!m_channel_list_init) {
        TLVF_LOG(ERROR) << "channel_list is not initialized";
        return false;
    }
    return true; 
}

std::shared_ptr<cChannelList> cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::create_channel_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list channel_list, abort!";
        return nullptr;
    }
    size_t len = cChannelList::get_initial_size();
    if (m_lock_allocation__) {
        TLVF_LOG(ERROR) << "Can't create new element before adding the previous one";
        return nullptr;
    }
    if (getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_channel_list;
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cChannelList>(src, getBuffRemainingBytes(src), m_parse__);
}

bool cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::add_channel_list(std::shared_ptr<cChannelList> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_channel_list was called before add_channel_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_channel_list;
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_channel_list_init = true;
    size_t len = ptr->getLen();
    m_channel_list_ptr = ptr;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    m_lock_allocation__ = false;
    return true;
}

void cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    if (m_channel_list_ptr) { m_channel_list_ptr->class_swap(); }
}

bool cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_CHANNELS_LIST_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_channel_list = reinterpret_cast<cChannelList*>(m_buff_ptr__);
    if (m_parse__) {
        auto channel_list = create_channel_list();
        if (!channel_list || !channel_list->isInitialized()) {
            TLVF_LOG(ERROR) << "create_channel_list() failed";
            return false;
        }
        if (!add_channel_list(channel_list)) {
            TLVF_LOG(ERROR) << "add_channel_list() failed";
            return false;
        }
        // swap back since channel_list will be swapped as part of the whole class swap
        channel_list->class_swap();
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE::cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE::cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE::~cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE() {
}
wfa_map::tlvProfile2MultiApProfile::eMultiApProfile& cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE::profile() {
    return (wfa_map::tlvProfile2MultiApProfile::eMultiApProfile&)(*m_profile);
}

void cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(8*sizeof(wfa_map::tlvProfile2MultiApProfile::eMultiApProfile), reinterpret_cast<uint8_t*>(m_profile));
}

bool cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE::finalize()
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

size_t cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(wfa_map::tlvProfile2MultiApProfile::eMultiApProfile); // profile
    return class_size;
}

bool cACTION_APMANAGER_SET_MAP_CONTROLLER_PROFILE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_profile = reinterpret_cast<wfa_map::tlvProfile2MultiApProfile::eMultiApProfile*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(wfa_map::tlvProfile2MultiApProfile::eMultiApProfile))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(wfa_map::tlvProfile2MultiApProfile::eMultiApProfile) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STATE_NOTIFICATION::cACTION_APMANAGER_STATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STATE_NOTIFICATION::cACTION_APMANAGER_STATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STATE_NOTIFICATION::~cACTION_APMANAGER_STATE_NOTIFICATION() {
}
std::string cACTION_APMANAGER_STATE_NOTIFICATION::curstate_str() {
    char *curstate_ = curstate();
    if (!curstate_) { return std::string(); }
    auto str = std::string(curstate_, m_curstate_idx__);
    auto pos = str.find_first_of('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

char* cACTION_APMANAGER_STATE_NOTIFICATION::curstate(size_t length) {
    if( (m_curstate_idx__ == 0) || (m_curstate_idx__ < length) ) {
        TLVF_LOG(ERROR) << "curstate length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_curstate);
}

bool cACTION_APMANAGER_STATE_NOTIFICATION::set_curstate(const std::string& str) { return set_curstate(str.c_str(), str.size()); }
bool cACTION_APMANAGER_STATE_NOTIFICATION::set_curstate(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_curstate received a null pointer.";
        return false;
    }
    if (size > 32) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_curstate);
    return true;
}
std::string cACTION_APMANAGER_STATE_NOTIFICATION::maxstate_str() {
    char *maxstate_ = maxstate();
    if (!maxstate_) { return std::string(); }
    auto str = std::string(maxstate_, m_maxstate_idx__);
    auto pos = str.find_first_of('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

char* cACTION_APMANAGER_STATE_NOTIFICATION::maxstate(size_t length) {
    if( (m_maxstate_idx__ == 0) || (m_maxstate_idx__ < length) ) {
        TLVF_LOG(ERROR) << "maxstate length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_maxstate);
}

bool cACTION_APMANAGER_STATE_NOTIFICATION::set_maxstate(const std::string& str) { return set_maxstate(str.c_str(), str.size()); }
bool cACTION_APMANAGER_STATE_NOTIFICATION::set_maxstate(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_maxstate received a null pointer.";
        return false;
    }
    if (size > 32) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_maxstate);
    return true;
}
void cACTION_APMANAGER_STATE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_STATE_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_STATE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += 32 * sizeof(char); // curstate
    class_size += 32 * sizeof(char); // maxstate
    return class_size;
}

bool cACTION_APMANAGER_STATE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_curstate = reinterpret_cast<char*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(char) * (32))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (32) << ") Failed!";
        return false;
    }
    m_curstate_idx__  = 32;
    m_maxstate = reinterpret_cast<char*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(char) * (32))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (32) << ") Failed!";
        return false;
    }
    m_maxstate_idx__  = 32;
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::~cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST() {
}
sMacAddr& cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

uint16_t& cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::timeout() {
    return (uint16_t&)(*m_timeout);
}

void cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_sta_mac->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_timeout));
}

bool cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::finalize()
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

size_t cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // sta_mac
    class_size += sizeof(uint16_t); // timeout
    return class_size;
}

bool cACTION_APMANAGER_MULTI_CHAN_BEACON_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sta_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    m_timeout = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


