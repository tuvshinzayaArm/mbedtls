/**
 * \file sha3.h
 *
 * \brief This file contains SHA3 definitions and functions.
 *
 * The Secure Hash Algorithms cryptographic
 * hash functions are defined in <em>FIPS 202: SHA-3 Standard:
 * Permutation-Based Hash and Extendable-Output Functions </em>.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef MBEDTLS_SHA3_H
#define MBEDTLS_SHA3_H
#include "mbedtls/private_access.h"

#include "mbedtls/build_info.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** SHA3 input data was malformed. */
#define MBEDTLS_ERR_SHA3_BAD_INPUT_DATA                 -0x0076

/**
 * SHA-3 family id.
 *
 * It identifies the family (SHA3-256, SHA3-512, etc.)
 */

typedef enum
{
    MBEDTLS_SHA3_NONE = 0, /*!< Operation not defined. */
    MBEDTLS_SHA3_224, /*!< SHA3-224 */
    MBEDTLS_SHA3_256, /*!< SHA3-256 */
    MBEDTLS_SHA3_384, /*!< SHA3-384 */
    MBEDTLS_SHA3_512, /*!< SHA3-512 */
    MBEDTLS_SHA3_SHAKE128, /*!< SHA3-SHAKE128 */
    MBEDTLS_SHA3_SHAKE256, /*!< SHA3-SHAKE256 */
    MBEDTLS_SHA3_CSHAKE128, /*!< SHA3-CSHAKE128 */
    MBEDTLS_SHA3_CSHAKE256, /*!< SHA3-CSHAKE256 */
} mbedtls_sha3_id;

#if !defined(MBEDTLS_SHA3_ALT)
// Regular implementation
//

struct mbedtls_sha3_context;
typedef struct mbedtls_sha3_family_functions
{
    mbedtls_sha3_id id;

    uint16_t r;
    uint16_t olen;
    uint8_t xor_byte;
}
mbedtls_sha3_family_functions;

/**
 * \brief          The SHA-3 context structure.
 *
 *                 The structure is used SHA-3 checksum calculations.
 */
typedef struct mbedtls_sha3_context {
    uint64_t state[25];
    uint8_t index;
    uint8_t id;

    uint16_t r;
    uint16_t olen;
    uint8_t xor_byte;
    uint16_t max_block_size;
}
mbedtls_sha3_context;

#else  /* MBEDTLS_SHA3_ALT */
#include "sha3_alt.h"
#endif /* MBEDTLS_SHA3_ALT */

/**
 * \brief          This function initializes a SHA-3 context.
 *
 * \param ctx      The SHA-3 context to initialize. This must not be \c NULL.
 */
void mbedtls_sha3_init( mbedtls_sha3_context *ctx );

/**
 * \brief          This function clears a SHA-3 context.
 *
 * \param ctx      The SHA-3 context to clear. This may be \c NULL, in which
 *                 case this function returns immediately. If it is not \c NULL,
 *                 it must point to an initialized SHA-3 context.
 */
void mbedtls_sha3_free( mbedtls_sha3_context *ctx );

/**
 * \brief          This function clones the state of a SHA-3 context.
 *
 * \param dst      The destination context. This must be initialized.
 * \param src      The context to clone. This must be initialized.
 */
void mbedtls_sha3_clone( mbedtls_sha3_context *dst,
                           const mbedtls_sha3_context *src );

/**
 * \brief          This function starts a SHA-3 checksum
 *                 calculation.
 *
 * \param ctx      The context to use. This must be initialized.
 * \param id       The id of the SHA-3 family.
 *
 * \return         \c 0 on success.
 * \return         A negative error code on failure.
 */
int mbedtls_sha3_starts( mbedtls_sha3_context *ctx, mbedtls_sha3_id id );

/**
 * \brief          This function starts a SHA-3 checksum
 *                 calculation with function-name and customization strings.
 *
 * \param ctx      The context to use. This must be initialized.
 * \param id       The id of the SHA-3 family. It can be \c MBEDTLS_SHA3_CSHAKE128 or
 *                 \c MBEDTLS_SHA3_CSHAKE256.
 * \param name     The function-name string. It must be zero-terminated.
 * \param name_len The length of \c name in Bytes.
 * \param custom   The customization string. It must be zero-terminated.
 * \param custom_len The length of \c custom in Bytes.
 *
 * \return         \c 0 on success.
 * \return         A negative error code on failure.
 *
 * \note           If no name and no custom strings are provided (are \c NULL),
 *                 it is equivalent to mbedtls_sha3_starts().
 */
int mbedtls_sha3_starts_cshake( mbedtls_sha3_context *ctx,
                            mbedtls_sha3_id id,
                            const uint8_t *name, size_t name_len,
                            const uint8_t *custom, size_t custom_len );

/**
 * \brief          This function feeds an input buffer into an ongoing
 *                 SHA-3 checksum calculation.
 *
 * \param ctx      The SHA-3 context. This must be initialized
 *                 and have a hash operation started.
 * \param input    The buffer holding the data. This must be a readable
 *                 buffer of length \p ilen Bytes.
 * \param ilen     The length of the input data in Bytes.
 *
 * \return         \c 0 on success.
 * \return         A negative error code on failure.
 */
int mbedtls_sha3_update( mbedtls_sha3_context *ctx,
                           const uint8_t *input,
                           size_t ilen );

/**
 * \brief          This function finishes the SHA-3 operation, and writes
 *                 the result to the output buffer.
 *
 * \param ctx      The SHA-3 context. This must be initialized
 *                 and have a hash operation started.
 * \param output   The SHA-3 checksum result.
 *                 This must be a writable buffer of length \c olen bytes.
 * \param olen     Defines a variable output length (in bytes). \c output must be
 *                 \c olen bytes length. For SHA-3 224, SHA-3 256, SHA-3 384 and
 *                 SHA-3 512 must equal to 28, 32, 48 and 64, respectively.
 *                 For SHAKE128 and SHAKE256 it can be an arbitrary number.
 *
 * \return         \c 0 on success.
 * \return         A negative error code on failure.
 */
int mbedtls_sha3_finish( mbedtls_sha3_context *ctx,
                           uint8_t *output, size_t olen );

/**
 * \brief          This function calculates the SHA-3
 *                 checksum of a buffer.
 *
 *                 The function allocates the context, performs the
 *                 calculation, and frees the context.
 *
 *                 The SHA-3 result is calculated as
 *                 output = SHA-3(id, input buffer, d).
 *
 * \param id       The id of the SHA-3 family.
 * \param input    The buffer holding the data. This must be a readable
 *                 buffer of length \p ilen Bytes.
 * \param ilen     The length of the input data in Bytes.
 * \param output   The SHA-3 checksum result.
 *                 This must be a writable buffer of length \c olen bytes.
 * \param olen     Determines the length (in bytes) of the output. \c output
 *                 must be \c olen bytes length.
 *
 * \return         \c 0 on success.
 * \return         A negative error code on failure.
 */
int mbedtls_sha3( mbedtls_sha3_id id, const uint8_t *input,
                    size_t ilen,
                    uint8_t *output,
                    size_t olen );

/**
 * \brief          This function calculates the SHA-3
 *                 checksum of a buffer with function-name and customization
 *                 strings.
 *
 *                 The function allocates the context, performs the
 *                 calculation, and frees the context.
 *
 *                 The SHA-3 result is calculated as
 *                 output = SHA-3(id, name || customization || input buffer, d).
 *
 * \param id       The id of the SHA-3 family. It can be \c MBEDTLS_SHA3_CSHAKE128 or
 *                 \c MBEDTLS_SHA3_CSHAKE256.
 * \param input    The buffer holding the data. This must be a readable
 *                 buffer of length \p ilen Bytes.
 * \param ilen     The length of the input data in Bytes.
 * \param name     The function-name string. It must be zero-terminated.
 * \param name_len The length of \c name in Bytes.
 * \param custom   The customization string. It must be zero-terminated.
 * \param custom_len The length of \c custom in Bytes.
 * \param output   The SHA-3 checksum result.
 *                 This must be a writable buffer of length \c olen bytes.
 * \param olen     Determines the length (in bytes) of the output. \c output
 *                 must be \c olen bytes length.
 *
 * \return         \c 0 on success.
 * \return         A negative error code on failure.
 *
 * \note           If no name and no custom strings are provided (are \c NULL),
 *                 it is equivalent to mbedtls_sha3().
 */
int mbedtls_sha3_cshake( mbedtls_sha3_id id, const uint8_t *input,
                    size_t ilen, const uint8_t *name, size_t name_len,
                    const uint8_t *custom, size_t custom_len,
                    uint8_t *output, size_t olen );

#ifdef __cplusplus
}
#endif

#endif /* mbedtls_sha3.h */

