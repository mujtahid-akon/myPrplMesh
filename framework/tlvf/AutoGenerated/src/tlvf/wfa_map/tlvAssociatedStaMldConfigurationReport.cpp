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

#include <tlvf/wfa_map/tlvAssociatedStaMldConfigurationReport.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvAssociatedStaMldConfigurationReport::tlvAssociatedStaMldConfigurationReport(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvAssociatedStaMldConfigurationReport::tlvAssociatedStaMldConfigurationReport(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvAssociatedStaMldConfigurationReport::~tlvAssociatedStaMldConfigurationReport() {
}
const eTlvTypeMap& tlvAssociatedStaMldConfigurationReport::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvAssociatedStaMldConfigurationReport::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvAssociatedStaMldConfigurationReport::sta_mld_mac_addr() {
    return (sMacAddr&)(*m_sta_mld_mac_addr);
}

sMacAddr& tlvAssociatedStaMldConfigurationReport::ap_mld_mac_addr() {
    return (sMacAddr&)(*m_ap_mld_mac_addr);
}

tlvAssociatedStaMldConfigurationReport::sModes& tlvAssociatedStaMldConfigurationReport::modes() {
    return (sModes&)(*m_modes);
}

uint8_t* tlvAssociatedStaMldConfigurationReport::reserved(size_t idx) {
    if ( (m_reserved_idx__ == 0) || (m_reserved_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_reserved[idx]);
}

bool tlvAssociatedStaMldConfigurationReport::set_reserved(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_reserved received a null pointer.";
        return false;
    }
    if (size > 18) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than buffer length";
        return false;
    }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_reserved);
    return true;
}
uint8_t& tlvAssociatedStaMldConfigurationReport::num_affiliated_sta() {
    return (uint8_t&)(*m_num_affiliated_sta);
}

std::tuple<bool, cAffiliatedSta&> tlvAssociatedStaMldConfigurationReport::affiliated_sta(size_t idx) {
    bool ret_success = ( (m_affiliated_sta_idx__ > 0) && (m_affiliated_sta_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_affiliated_sta_vector[ret_idx]));
}

std::shared_ptr<cAffiliatedSta> tlvAssociatedStaMldConfigurationReport::create_affiliated_sta() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list affiliated_sta, abort!";
        return nullptr;
    }
    size_t len = cAffiliatedSta::get_initial_size();
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
    uint8_t *src = (uint8_t *)m_affiliated_sta;
    if (m_affiliated_sta_idx__ > 0) {
        src = (uint8_t *)m_affiliated_sta_vector[m_affiliated_sta_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cAffiliatedSta>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvAssociatedStaMldConfigurationReport::add_affiliated_sta(std::shared_ptr<cAffiliatedSta> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_affiliated_sta was called before add_affiliated_sta";
        return false;
    }
    uint8_t *src = (uint8_t *)m_affiliated_sta;
    if (m_affiliated_sta_idx__ > 0) {
        src = (uint8_t *)m_affiliated_sta_vector[m_affiliated_sta_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_affiliated_sta_idx__++;
    if (!m_parse__) { (*m_num_affiliated_sta)++; }
    size_t len = ptr->getLen();
    m_affiliated_sta_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvAssociatedStaMldConfigurationReport::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_sta_mld_mac_addr->struct_swap();
    m_ap_mld_mac_addr->struct_swap();
    m_modes->struct_swap();
    for (size_t i = 0; i < m_affiliated_sta_idx__; i++){
        std::get<1>(affiliated_sta(i)).class_swap();
    }
}

bool tlvAssociatedStaMldConfigurationReport::finalize()
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
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t tlvAssociatedStaMldConfigurationReport::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // sta_mld_mac_addr
    class_size += sizeof(sMacAddr); // ap_mld_mac_addr
    class_size += sizeof(sModes); // modes
    class_size += 18 * sizeof(uint8_t); // reserved
    class_size += sizeof(uint8_t); // num_affiliated_sta
    return class_size;
}

bool tlvAssociatedStaMldConfigurationReport::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = reinterpret_cast<eTlvTypeMap*>(m_buff_ptr__);
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_ASSOCIATED_STA_MLD_CONFIGURATION_REPORT;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_sta_mld_mac_addr = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_sta_mld_mac_addr->struct_init(); }
    m_ap_mld_mac_addr = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_ap_mld_mac_addr->struct_init(); }
    m_modes = reinterpret_cast<sModes*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sModes))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sModes) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sModes); }
    if (!m_parse__) { m_modes->struct_init(); }
    m_reserved = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (18))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (18) << ") Failed!";
        return false;
    }
    m_reserved_idx__  = 18;
    if (!m_parse__) {
        if (m_length) { (*m_length) += (sizeof(uint8_t) * 18); }
    }
    m_num_affiliated_sta = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_num_affiliated_sta = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_affiliated_sta = reinterpret_cast<cAffiliatedSta*>(m_buff_ptr__);
    uint8_t num_affiliated_sta = *m_num_affiliated_sta;
    m_affiliated_sta_idx__ = 0;
    for (size_t i = 0; i < num_affiliated_sta; i++) {
        auto affiliated_sta = create_affiliated_sta();
        if (!affiliated_sta || !affiliated_sta->isInitialized()) {
            TLVF_LOG(ERROR) << "create_affiliated_sta() failed";
            return false;
        }
        if (!add_affiliated_sta(affiliated_sta)) {
            TLVF_LOG(ERROR) << "add_affiliated_sta() failed";
            return false;
        }
        // swap back since affiliated_sta will be swapped as part of the whole class swap
        affiliated_sta->class_swap();
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_ASSOCIATED_STA_MLD_CONFIGURATION_REPORT) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_ASSOCIATED_STA_MLD_CONFIGURATION_REPORT) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cAffiliatedSta::cAffiliatedSta(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cAffiliatedSta::cAffiliatedSta(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cAffiliatedSta::~cAffiliatedSta() {
}
sMacAddr& cAffiliatedSta::bssid() {
    return (sMacAddr&)(*m_bssid);
}

sMacAddr& cAffiliatedSta::affiliated_sta_mac_addr() {
    return (sMacAddr&)(*m_affiliated_sta_mac_addr);
}

uint8_t* cAffiliatedSta::reserved(size_t idx) {
    if ( (m_reserved_idx__ == 0) || (m_reserved_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_reserved[idx]);
}

bool cAffiliatedSta::set_reserved(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_reserved received a null pointer.";
        return false;
    }
    if (size > 19) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than buffer length";
        return false;
    }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_reserved);
    return true;
}
void cAffiliatedSta::class_swap()
{
    m_bssid->struct_swap();
    m_affiliated_sta_mac_addr->struct_swap();
}

bool cAffiliatedSta::finalize()
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

size_t cAffiliatedSta::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(sMacAddr); // affiliated_sta_mac_addr
    class_size += 19 * sizeof(uint8_t); // reserved
    return class_size;
}

bool cAffiliatedSta::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_affiliated_sta_mac_addr = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_affiliated_sta_mac_addr->struct_init(); }
    m_reserved = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (19))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (19) << ") Failed!";
        return false;
    }
    m_reserved_idx__  = 19;
    if (m_parse__) { class_swap(); }
    return true;
}


