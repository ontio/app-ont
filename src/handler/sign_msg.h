#pragma once


#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool

#include "buffer.h"

/**
 * Handler for SIGN_MESSAGE command. If successfully parse BIP32 path
 * and message, sign message and send APDU response.
 *
 * @see G_context.bip32_path, G_context.tx_info.raw_msg,
 * G_context.tx_info.signature and G_context.tx_info.v.
 *
 * @param[in,out] cdata
 *   Command data with BIP32 path and raw message serialized.
 * @param[in]     chunk
 *   Index number of the APDU chunk.
 * @param[in]       more
 *   Whether more APDU chunk to be received or not.
 *
 * @return zero or positive integer if success, negative integer otherwise.
 *
 */
int handler_sign_message(buffer_t *cdata, uint8_t chunk, bool more);
