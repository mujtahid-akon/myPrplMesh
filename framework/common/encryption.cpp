/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2017 Broadband Forum
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

/*
 * Parts of this file are adapted from Broadband Forum meshComms
 * https://github.com/BroadbandForum/meshComms
 * under the BSD-2-Clause-Patent license.
 *
 *  Copyright (c) 2017, Broadband Forum
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  Subject to the terms and conditions of this license, each copyright
 *  holder and contributor hereby grants to those receiving rights under
 *  this license a perpetual, worldwide, non-exclusive, no-charge,
 *  royalty-free, irrevocable (except for failure to satisfy the
 *  conditions of this license) patent license to make, have made, use,
 *  offer to sell, sell, import, and otherwise transfer this software,
 *  where such license applies only to those patent claims, already
 *  acquired or hereafter acquired, licensable by such copyright holder or
 *  contributor that are necessarily infringed by:
 *
 *  (a) their Contribution(s) (the licensed copyrights of copyright holders
 *      and non-copyrightable additions of contributors, in source or binary
 *      form) alone; or
 *
 *  (b) combination of their Contribution(s) with the work of authorship to
 *      which such Contribution(s) was added by such copyright holder or
 *      contributor, if, at the time the Contribution is added, such addition
 *      causes such combination to be necessarily infringed. The patent
 *      license shall not apply to any other combinations which include the
 *      Contribution.
 *
 *  Except as expressly stated above, no rights or licenses from any
 *  copyright holder or contributor is granted under this license, whether
 *  expressly, by implication, estoppel or otherwise.
 *
 *  DISCLAIMER
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 *  DAMAGE.
 */

#include <arpa/inet.h>
#include <cstddef>
#include <mapf/common/encryption.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>

#include "hmac_wrapper.h"

namespace mapf {
namespace encryption {

static bool generate_random_bytestream(uint8_t *buf, unsigned len)
{
    std::ifstream urandom("/dev/urandom");
    urandom.read(reinterpret_cast<char *>(buf), len);
    return urandom.good();
}

/**
  Diffie-Hellman group 5, see RFC3526
*/
const uint8_t diffie_hellman::dh1536_p[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
    0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
    0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
    0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
    0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
    0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
    0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
    0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
    0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
    0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
    0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
    0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x23, 0x73, 0x27, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

const uint8_t diffie_hellman::dh1536_g[] = {0x02};

#if OPENSSL_VERSION_NUMBER < 0x30000000L
diffie_hellman::diffie_hellman() : m_dh(nullptr), m_pubkey(nullptr)
{
    MAPF_DBG("Generating DH keypair");

    m_dh = DH_new();
    if (!m_dh) {
        MAPF_ERR("Failed to allocate DH");
        return;
    }

    if (!generate_random_bytestream(m_nonce, sizeof(m_nonce))) {
        MAPF_ERR("Failed to generate nonce");
        return;
    }

    // Convert binary to BIGNUM format
    if (0 == DH_set0_pqg(m_dh, BN_bin2bn(dh1536_p, sizeof(dh1536_p), nullptr), NULL,
                         BN_bin2bn(dh1536_g, sizeof(dh1536_g), nullptr))) {
        MAPF_ERR("Failed to set DH pqg");
        return;
    }

    // Obtain key pair
    if (0 == DH_generate_key(m_dh)) {
        MAPF_ERR("Failed to generate DH key");
        return;
    }

    const BIGNUM *pub_key;
    DH_get0_key(m_dh, &pub_key, nullptr);

    m_pubkey_length = BN_num_bytes(pub_key);
    m_pubkey        = new uint8_t[m_pubkey_length];
    BN_bn2bin(pub_key, m_pubkey);
}

diffie_hellman::~diffie_hellman()
{
    delete[] m_pubkey;
    DH_free(m_dh);
}

bool diffie_hellman::compute_key(uint8_t *key, size_t &key_length, const uint8_t *remote_pubkey,
                                 size_t remote_pubkey_length) const
{
    if (!m_pubkey) {
        return false;
    }

    MAPF_DBG("Computing DH shared key");

    BIGNUM *pub_key = BN_bin2bn(remote_pubkey, remote_pubkey_length, NULL);
    if (!pub_key) {
        MAPF_ERR("Failed to set DH remote_pub_key");
        return 0;
    }

    // Compute the shared secret and save it in the output buffer
    if ((int)key_length < DH_size(m_dh)) {
        MAPF_ERR("Output buffer for DH shared key to small: ")
            << key_length << " < " << DH_size(m_dh);
        BN_clear_free(pub_key);
        return false;
    }
    int ret = DH_compute_key(key, pub_key, m_dh);
    BN_clear_free(pub_key);
    if (ret < 0) {
        MAPF_ERR("Failed to compute DH shared key");
        return false;
    }
    key_length = (size_t)ret;
    return true;
}
#else
diffie_hellman::diffie_hellman() : m_evp(nullptr), m_pubkey(nullptr)
{
    MAPF_DBG("Generating EVP keypair");

    std::unique_ptr<BIGNUM, decltype(&BN_free)> p(BN_bin2bn(dh1536_p, sizeof(dh1536_p), nullptr),
                                                  &BN_free);
    std::unique_ptr<BIGNUM, decltype(&BN_free)> g(BN_bin2bn(dh1536_g, sizeof(dh1536_g), nullptr),
                                                  &BN_free);

    // Create the OSSL_PARAM_BLD
    std::unique_ptr<OSSL_PARAM_BLD, decltype(&OSSL_PARAM_BLD_free)> paramBuild(
        OSSL_PARAM_BLD_new(), &OSSL_PARAM_BLD_free);
    if (!paramBuild.get()) {
        MAPF_ERR("Failed to create a new OSSL_PARAM_BLD");
        return;
    }

    // Set the prime and generator
    if (!OSSL_PARAM_BLD_push_BN(paramBuild.get(), OSSL_PKEY_PARAM_FFC_P, p.get()) ||
        !OSSL_PARAM_BLD_push_BN(paramBuild.get(), OSSL_PKEY_PARAM_FFC_G, g.get())) {
        MAPF_ERR("Failed to set prime and generator in OSSL_PARAM_BLD");
        return;
    }

    // Convert to OSSL_PARAM
    std::unique_ptr<OSSL_PARAM, decltype(&OSSL_PARAM_free)> param(
        OSSL_PARAM_BLD_to_param(paramBuild.get()), &OSSL_PARAM_free);
    if (!param.get()) {
        MAPF_ERR("Failed to convert OSSL_PARAM_BLD to OSSL_PARAM");
        return;
    }

    // Create the key context
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> keyCtx(
        EVP_PKEY_CTX_new_from_name(nullptr, "DHX", nullptr), &EVP_PKEY_CTX_free);
    if (!keyCtx.get()) {
        MAPF_ERR("Failed to create EVP_PKEY context");
        return;
    }

    // Initialize the context.
    if (EVP_PKEY_fromdata_init(keyCtx.get()) <= 0) {
        MAPF_ERR("Failed to initialize EVP_PKEY context");
        return;
    }
    // Create the domain parameter key.
    EVP_PKEY *domainParamKey = nullptr;
    if (EVP_PKEY_fromdata(keyCtx.get(), &domainParamKey, EVP_PKEY_KEY_PARAMETERS, param.get()) <=
        0) {
        MAPF_ERR("Failed to create EVP_PKEY");
        return;
    }

    // Create keypair
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> keyGenerationCtx(
        EVP_PKEY_CTX_new_from_pkey(nullptr, domainParamKey, nullptr), &EVP_PKEY_CTX_free);
    if (!keyGenerationCtx.get()) {
        MAPF_ERR("Failed to create keyGeneration context");
        return;
    }
    if (EVP_PKEY_keygen_init(keyGenerationCtx.get()) <= 0) {
        MAPF_ERR("Failed to initialize key generation");
        return;
    }
    if (EVP_PKEY_generate(keyGenerationCtx.get(), &m_evp) <= 0) {
        MAPF_ERR("Failed to generate DH key pair");
        return;
    }

    BIGNUM *pub_key = nullptr;
    if (EVP_PKEY_get_bn_param(m_evp, OSSL_PKEY_PARAM_PUB_KEY, &pub_key) != 1) {
        MAPF_ERR("Failed to get the public key");
        return;
    }

    m_pubkey_length = BN_num_bytes(pub_key);
    m_pubkey        = new uint8_t[m_pubkey_length];
    if (0 == BN_bn2bin(pub_key, m_pubkey)) {
        MAPF_ERR("Failed to convert public key");
        return;
    }
    BN_free(pub_key);

    if (RAND_bytes(m_nonce, sizeof(m_nonce)) != 1) {
        MAPF_ERR("Failed to generate nonce");
        return;
    }

    MAPF_INFO("EVP keypair generated");
}

diffie_hellman::~diffie_hellman()
{
    delete[] m_pubkey;
    if (m_evp != nullptr) {
        EVP_PKEY_free(m_evp);
    }
}

bool diffie_hellman::create_peer_EVPKey(
    const uint8_t *remote_pubkey, size_t remote_pubkey_length,
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> *peerPubKey) const
{

    std::unique_ptr<BIGNUM, decltype(&BN_free)> p(BN_bin2bn(dh1536_p, sizeof(dh1536_p), nullptr),
                                                  &BN_free);
    std::unique_ptr<BIGNUM, decltype(&BN_free)> g(BN_bin2bn(dh1536_g, sizeof(dh1536_g), nullptr),
                                                  &BN_free);

    std::unique_ptr<BIGNUM, decltype(&BN_clear_free)> bigNumPeerPublicKey(
        BN_bin2bn(remote_pubkey, remote_pubkey_length, nullptr), &BN_clear_free);
    if (!bigNumPeerPublicKey.get()) {
        MAPF_ERR("Failed to convert remote_pubkey to bignum");
        return false;
    }

    // Create the OSSL_PARAM_BLD.
    std::unique_ptr<OSSL_PARAM_BLD, decltype(&OSSL_PARAM_BLD_free)> paramBuild{
        OSSL_PARAM_BLD_new(), &OSSL_PARAM_BLD_free};
    if (!paramBuild.get()) {
        MAPF_ERR("Failed to allocate OSSL_PARAM builder");
        return false;
    }

    // Set public key, prime and generator
    if (!OSSL_PARAM_BLD_push_BN(paramBuild.get(), OSSL_PKEY_PARAM_PUB_KEY,
                                bigNumPeerPublicKey.get())) {
        MAPF_ERR("Failed to set public key to OSSL_PARAM builder");
        return false;
    }
    if (!OSSL_PARAM_BLD_push_BN(paramBuild.get(), OSSL_PKEY_PARAM_FFC_P, p.get()) ||
        !OSSL_PARAM_BLD_push_BN(paramBuild.get(), OSSL_PKEY_PARAM_FFC_G, g.get())) {
        MAPF_ERR("Failed to set p or g value in OSSL_PARAM builder");
        return false;
    }

    std::unique_ptr<OSSL_PARAM, decltype(&OSSL_PARAM_free)> param{
        OSSL_PARAM_BLD_to_param(paramBuild.get()), &OSSL_PARAM_free};
    if (!param.get()) {
        MAPF_ERR("Failed to create public key, OSSL_PARAM generation failed");
        return false;
    }

    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> peerPublicKeyCtx{
        EVP_PKEY_CTX_new_from_name(nullptr, "DHX", nullptr), &EVP_PKEY_CTX_free};
    if (!peerPublicKeyCtx.get()) {
        MAPF_ERR("Failed to create public key, EVP_PKEY context generation failed");
        return false;
    }

    if (EVP_PKEY_fromdata_init(peerPublicKeyCtx.get()) <= 0) {
        MAPF_ERR("Failed to create public key, EVP_PKEY_fromdata_init failed");
        return false;
    }

    EVP_PKEY *tmp = nullptr;
    if (EVP_PKEY_fromdata(peerPublicKeyCtx.get(), &tmp, EVP_PKEY_PUBLIC_KEY, param.get()) <= 0) {
        MAPF_ERR("Failed to create public key, EVP_PKEY_fromdata failed");
        return false;
    }

    peerPubKey->reset(tmp);

    return true;
}

bool diffie_hellman::compute_key(uint8_t *key, size_t &key_length, const uint8_t *remote_pubkey,
                                 size_t remote_pubkey_length) const
{

    MAPF_DBG("Computing DH shared key");

    // Create remote EVP key from remote_pubkey
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> remoteEVPKey(EVP_PKEY_new(),
                                                                     &EVP_PKEY_free);
    if (!create_peer_EVPKey(remote_pubkey, remote_pubkey_length, &remoteEVPKey)) {
        MAPF_ERR("Failed to create public EVP key");
        return false;
    }

    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> derivationCtx(
        EVP_PKEY_CTX_new(m_evp, nullptr), &EVP_PKEY_CTX_free);
    if (!derivationCtx) {
        MAPF_ERR("Failed to allocate key derivation context");
        return false;
    }

    if (EVP_PKEY_derive_init(derivationCtx.get()) <= 0) {
        MAPF_ERR("Failed to initialize key derivation context");
        return false;
    }

    if (EVP_PKEY_derive_set_peer(derivationCtx.get(), remoteEVPKey.get()) <= 0) {
        MAPF_ERR("Failed to set public key in EVP context");
        return false;
    }

    if (EVP_PKEY_derive(derivationCtx.get(), key, &key_length) <= 0) {
        MAPF_ERR("Failed to derrive shared key");
        return false;
    }
    if (key_length == 0) {
        MAPF_ERR("Failed to compute DH shared key, keylength is zero");
        return false;
    }

    return true;
}
#endif

bool create_iv(uint8_t *iv, unsigned iv_length)
{
    return generate_random_bytestream(iv, iv_length);
}

sha256::sha256() : m_ctx(EVP_MD_CTX_new())
{
    if (!EVP_DigestInit_ex(m_ctx, EVP_sha256(), NULL)) {
        MAPF_ERR("Failed to create sha256");
        EVP_MD_CTX_free(m_ctx);
        m_ctx = nullptr;
    }
}

sha256::~sha256() { EVP_MD_CTX_free(m_ctx); }

bool sha256::update(const uint8_t *message, size_t message_length)
{
    if (!m_ctx) {
        return false;
    }
    return EVP_DigestUpdate(m_ctx, message, message_length);
}

bool sha256::digest(uint8_t *digest)
{
    if (!m_ctx) {
        return false;
    }
    unsigned int digest_length = 32;
    return EVP_DigestFinal(m_ctx, digest, &digest_length);
}

bool aes_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *plaintext, int plen,
                 uint8_t *ciphertext, int &clen)
{
    EVP_CIPHER_CTX *ctx;
    int len;

    /* Verify that the ciphertext buffer has enough storage room
     * for block size alignment padding which will be added
     * during encryption, which is up to plen + cipher_block_size -1
     * for the update, and another cipher_block_size for the final one.
     */
    int padlen = 16 - (plen % 16);
    if (clen < plen + padlen) {
        LOG(ERROR) << "Insufficient room for padding in ciphertext buffer" << std::endl
                   << "plaintext len: " << plen << std::endl
                   << "ciphertext len: " << clen << std::endl
                   << "padlen: " << padlen;
        return false;
    }

    ctx = EVP_CIPHER_CTX_new();
    if (NULL == ctx) {
        MAPF_ERR("Failed to create context");
        return false;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
        MAPF_ERR("EVP_EncryptInit_ex Failed");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate() encrypts inl bytes from the buffer in and writes
     * the encrypted version to out. The amount of data written depends on
     * the block alignment of the encrypted data: as a result the amount of
     * data written may be anything from zero bytes to
     * (inl + cipher_block_size - 1) so out should contain sufficient room.
     */
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plen) != 1) {
        MAPF_ERR("EVP_EncryptUpdate Failed");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &clen) != 1) {
        MAPF_ERR("EVP_EncryptFinal_ex Failed. clen=" << clen);
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    clen += len;

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool aes_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *ciphertext, int clen,
                 uint8_t *plaintext, int &plen)
{
    EVP_CIPHER_CTX *ctx;
    int len;

    /*
     * The decrypt operation will fail if  the final block is not correctly formatted.
     * This check requires the output buffer to have sufficient room for this check
     * which is (inl + cipher_block_size), which is clen + 16 in this case (aes128).
     */
    if (plen < clen + 16) {
        LOG(ERROR) << "Insufficient room for final block (padding) format check";
        return false;
    }

    ctx = EVP_CIPHER_CTX_new();
    if (NULL == ctx) {
        MAPF_ERR("Failed to create context");
        return false;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
        MAPF_ERR("EVP_DecryptInit_ex Failed");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, clen) != 1) {
        MAPF_ERR("EVP_DecryptUpdate Failed");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plen = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (EVP_DecryptFinal_ex(ctx, plaintext + plen, &len) != 1) {
        MAPF_ERR("EVP_DecryptFinal_ex Failed");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plen += len;

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

void copy_pubkey(const diffie_hellman &dh, uint8_t *dest)
{
    int padding = dh.max_pubkey_length - dh.pubkey_length();
    mapf_assert(padding >= 0);
    std::fill_n(dest, padding, 0);
    std::copy_n(dh.pubkey(), dh.pubkey_length(), dest + padding);
}

void wps_calculate_keys(const diffie_hellman &dh, const uint8_t *remote_pubkey,
                        size_t remote_pubkey_length, const uint8_t *m1_nonce, const uint8_t *mac,
                        const uint8_t *m2_nonce, uint8_t *authkey, uint8_t *keywrapkey)
{
    uint8_t shared_secret[192];
    size_t shared_secret_length = sizeof(shared_secret);

    dh.compute_key(shared_secret, shared_secret_length, remote_pubkey, remote_pubkey_length);
    // Zero pad the remaining part
    std::fill(shared_secret + shared_secret_length, shared_secret + sizeof(shared_secret), 0);

    sha256 sha;
    sha.update(shared_secret, shared_secret_length);

    uint8_t key[32];
    sha.digest(key);

    hmac_wrapper hmac(key, sizeof(key));
    hmac.update(m1_nonce, 16);
    hmac.update(mac, 6);
    hmac.update(m2_nonce, 16);

    uint8_t kdk[32];
    hmac.digest(kdk, 32);

    // Finally, take "kdk" and using a function provided in the "Wi-Fi
    // simple configuration" standard, obtain THREE KEYS that we will use
    // later ("authkey", "keywrapkey" and "emsk")
    union {
        struct {
            uint8_t authkey[32];
            uint8_t keywrapkey[16];
            // cppcheck-suppress unusedStructMember
            uint8_t emsk[32];
        } keys;
        uint8_t buf[3][32];
    } keys;

    // This is the key derivation function used in the WPS standard to obtain a
    // final hash that is later used for encryption.
    //
    // The output is stored in the memory buffer pointed by 'res', which must be
    // "SHA256_MAC_LEN" bytes long (ie. 're_len' must always be "SHA256_MAC_LEN",
    // even if it is an input argument)

    uint32_t kdf_key_length = htonl(sizeof(keys.keys) * 8);

    std::string personalization_string("Wi-Fi Easy and Secure Key Derivation");
    for (unsigned iter = 1; iter < sizeof(keys) / 32; iter++) {
        uint32_t kdf_iter = htonl(iter);

        hmac_wrapper hmac_iter(kdk, sizeof(kdk));
        hmac_iter.update(reinterpret_cast<const uint8_t *>(&kdf_iter), sizeof(kdf_iter));
        hmac_iter.update(reinterpret_cast<const uint8_t *>(personalization_string.data()),
                         personalization_string.length());
        hmac_iter.update(reinterpret_cast<const uint8_t *>(&kdf_key_length),
                         sizeof(kdf_key_length));
        static_assert(sizeof(keys.buf[1]) == 32, "Correct size");
        hmac_iter.digest(keys.buf[iter - 1], 32);
    }
    std::copy(keys.keys.authkey, keys.keys.authkey + sizeof(keys.keys.authkey), authkey);
    std::copy(keys.keys.keywrapkey, keys.keys.keywrapkey + sizeof(keys.keys.keywrapkey),
              keywrapkey);
}

bool kwa_compute(const uint8_t *authkey, uint8_t *data, uint32_t data_len, uint8_t *kwa)
{
    uint8_t hmac_[32];
    hmac_wrapper hmac_kwa(authkey, 32);
    if (!hmac_kwa.update(data, data_len))
        return false;
    if (!hmac_kwa.digest(hmac_, 32))
        return false;
    std::copy_n(hmac_, 8, kwa);
    return true;
}

} // namespace encryption
} // namespace mapf
