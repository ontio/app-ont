#pragma once

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t

#define UNCOMPRESSED_KEY_LEN     65
#define COMPRESSED_KEY_LEN       33
#define ADDRESS_SCRIPT_HASH_LEN  20

#define CHAIN_CODE_LEN 32

/**
 * @brief Converts a address script to a base58 address.
 *
 * @param[out] out
 *   Pointer to the structure where the converted value will be stored.
 * @param[in] out_len
 *   Length of the output buffer to store the uint64_t value.
 * @param[in] script_hash
 *   Pointer to the input data to be converted.
 *
 * @return true if the conversion is successful, false otherwise.
 */
bool convert_script_hash_to_base58_address(char* out, size_t out_len, const uint8_t* script_hash);

/**
 * @brief Converts an uncompressed public key to a corresponding address.
 *
 * @param[out] out A pointer to the output buffer where the resulting address will be stored.
 * @param[in] out_len The size of the output buffer. It must be large enough to hold the address.
 *  * @param[in] uncompressed_key A pointer to the uncompressed public key array.
 *                             The array must have a size of at least UNCOMPRESSED_KEY_LEN.
 *
 * @return true if the conversion is successful, false otherwise.
 */
bool convert_uncompressed_pubkey_to_address(
    char* out,
    size_t out_len,
    const uint8_t uncompressed_key[static UNCOMPRESSED_KEY_LEN]);

/**
 * @brief Derives an Ontology address from a given BIP32 path.
 *
 * This function generates an Ontology address based on the provided
 * BIP32 path and writes it to the output buffer.
 *
 * @param[out] out      A pointer to the buffer where the derived address will be stored.
 * @param[in]  out_len  The size of the output buffer in bytes.
 *
 * @return true if the address was successfully derived and written to the buffer,
 *         false if an error occurred (e.g., insufficient buffer size).
 */
bool derive_address_from_bip32_path(char* out, size_t out_len);
