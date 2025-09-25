/*******************************************************************************
 *   Ontology Ledger App
 *   (c) 2025 Ontology
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

/*
An Ontology address is generated from the address script as follows:

The address script has 35 bytes which consists of:
| OPCODE_PUSHBYTES21 |compressed public key| OPCODE_CHECKSIG |
|    --1 byte--      |   --0x21 bytes--    |   --1 byte--    |

The script hash is a 20-byte value obtained by performing a SHA256 hash followed by a RIPEMD-160
hash on the address script.

Then, concatenate ADDRESS_VERSION, script hash, and CHECK_SUM to obtain a 25-byte pre-address.
| ADDRESS_VERSION |    script hash   |   CHECK_SUM   |
|   --1 byte--    |   --20 bytes--   |  --4 bytes--  |
where CHECK_SUM = SHA256(SHA256(ADDRESS_VERSION || address script))[0:3].

The Ontology address is a base58 encoding of the pre-address.
*/

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

#include "address.h"
#include "types.h"
#include "base58.h"
#include "lcx_common.h"
#include "lcx_sha256.h"
#include "lcx_ripemd160.h"
#include "crypto_helpers.h"
#include "../globals.h"
#include "address.h"

#define ADDRESS_VERSION 23  // 0x17
#define ADDRESS_SCRIPT_LEN       35
#define SCRIPT_HASH_CHECKSUM_LEN 4
#define ADDRESS_PRE_LEN          (1 + ADDRESS_SCRIPT_HASH_LEN + SCRIPT_HASH_CHECKSUM_LEN)
#define BASE58_ADDRESS_LEN       34
static bool covert_pk_to_address_script(const uint8_t uncompressed_key[static UNCOMPRESSED_KEY_LEN],
                                        uint8_t *out,
                                        size_t out_len) {
    LEDGER_ASSERT(uncompressed_key != NULL, "NULL uncompressed_key");
    LEDGER_ASSERT(out != NULL, "NULL out");

    if (out_len != ADDRESS_SCRIPT_LEN) {
        return false;
    }

    if (uncompressed_key[0] != 0x04) {
        return false;
    }
    const uint8_t *x = &uncompressed_key[1];
    const uint8_t *y = &uncompressed_key[33];
    uint8_t compressed_key[COMPRESSED_KEY_LEN];

    compressed_key[0] = (y[31] & 1) ? 0x03 : 0x02;
    memcpy(&compressed_key[1], x, 32);

    out[0] = OPCODE_PUSHBYTES21;
    memcpy(&out[1], compressed_key, sizeof(compressed_key));
    out[sizeof(compressed_key) + 1] = OPCODE_CHECKSIG;

    explicit_bzero(compressed_key, sizeof(compressed_key));

    return true;
}

static bool hash_script(const uint8_t *script, size_t script_len, uint8_t *output_hash) {
    LEDGER_ASSERT(script != NULL, "NULL verification_script");
    LEDGER_ASSERT(output_hash != NULL, "NULL output_hash");

    if (script_len != ADDRESS_SCRIPT_LEN || script[0] != OPCODE_PUSHBYTES21 ||
        script[34] != OPCODE_CHECKSIG) {
        return false;
    }

    uint8_t sha256_hash[CX_SHA256_SIZE];
    bool result = (cx_sha256_hash(script, script_len, sha256_hash) == CX_OK) &&
                  (cx_ripemd160_hash(sha256_hash, CX_SHA256_SIZE, output_hash) == CX_OK);

    explicit_bzero(sha256_hash, sizeof(sha256_hash));

    return result;
}

bool convert_script_hash_to_base58_address(char *out, size_t out_len, const uint8_t *script_hash) {
    LEDGER_ASSERT(out != NULL, "NULL out");
    LEDGER_ASSERT(script_hash != NULL, "NULL script_hash");

    if (out_len < BASE58_ADDRESS_LEN) {
        return false;
    }

    uint8_t data_hash_1[CX_SHA256_SIZE];
    uint8_t data_hash_2[CX_SHA256_SIZE];
    uint8_t address[ADDRESS_PRE_LEN];

    address[0] = ADDRESS_VERSION;
    memcpy(&address[1], script_hash, ADDRESS_SCRIPT_HASH_LEN);

    bool result = cx_sha256_hash(address, ADDRESS_SCRIPT_HASH_LEN + 1, data_hash_1) == CX_OK &&
                  cx_sha256_hash(data_hash_1, CX_SHA256_SIZE, data_hash_2) == CX_OK;

    memcpy(&address[1 + ADDRESS_SCRIPT_HASH_LEN], data_hash_2, SCRIPT_HASH_CHECKSUM_LEN);

    result = result && base58_encode(address, sizeof(address), out, out_len);

    explicit_bzero(data_hash_1, sizeof(data_hash_1));
    explicit_bzero(data_hash_2, sizeof(data_hash_2));
    explicit_bzero(address, sizeof(address));

    return result;
}

bool convert_uncompressed_pubkey_to_address(
    char *out,
    size_t out_len,
    const uint8_t uncompressed_key[static UNCOMPRESSED_KEY_LEN]) {
    LEDGER_ASSERT(uncompressed_key != NULL, "NULL uncompressed_key");
    LEDGER_ASSERT(out != NULL, "NULL out");

    if (out_len < BASE58_ADDRESS_LEN) {
        return false;
    }

    uint8_t script[ADDRESS_SCRIPT_LEN] = {0};
    uint8_t ripemd160_hash[ADDRESS_SCRIPT_HASH_LEN] = {0};

    bool result = covert_pk_to_address_script(uncompressed_key, script, sizeof(script)) &&
                  hash_script(script, sizeof(script), ripemd160_hash) &&
                  convert_script_hash_to_base58_address(out, out_len, ripemd160_hash);

    explicit_bzero(script, sizeof(script));
    explicit_bzero(ripemd160_hash, sizeof(ripemd160_hash));

    return result;
}

bool derive_address_from_bip32_path(char *out, size_t out_len) {
    LEDGER_ASSERT(out != NULL, "NULL out");

    uint8_t uncompressed_key[UNCOMPRESSED_KEY_LEN];
    uint8_t chain_code[CHAIN_CODE_LEN];

    bool result = (bip32_derive_get_pubkey_256(CX_CURVE_256R1,
                                               G_context.bip32_path,
                                               G_context.bip32_path_len,
                                               uncompressed_key,
                                               chain_code,
                                               CX_SHA256) == CX_OK) &&
                  convert_uncompressed_pubkey_to_address(out, out_len, uncompressed_key);

    explicit_bzero(uncompressed_key, sizeof(uncompressed_key));
    explicit_bzero(chain_code, sizeof(chain_code));

    return result;
}