#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include "bip32.h"
#include "lcx_sha256.h"

#include "constants.h"
#include "tx_types.h"
#include "address.h"

/**
 * Enumeration with expected INS of APDU commands.
 */
typedef enum {
    GET_VERSION = 0x03,     /// version of the application
    GET_APP_NAME = 0x04,    /// name of the application
    GET_PUBLIC_KEY = 0x05,  /// public key of corresponding BIP32 path
    SIGN_TX = 0x02,         /// sign transaction with BIP32 path
    SIGN_MESSAGE = 0x07,    /// sign personal message
} command_e;
/**
 * Enumeration with parsing state.
 */
typedef enum {
    STATE_NONE,     /// No state
    STATE_PARSED,   /// Transaction data parsed
    STATE_APPROVED  /// Transaction data approved
} state_e;

/**
 * Enumeration with user request type.
 */
typedef enum {
    CONFIRM_ADDRESS,      /// confirm address derived from public key
    CONFIRM_TRANSACTION,  /// confirm transaction information
    CONFIRM_MESSAGE       /// confirm message information
} request_type_e;

/**
 * Structure for public key context information.
 */
typedef struct {
    uint8_t
        raw_public_key[UNCOMPRESSED_KEY_LEN];  /// format (1), x-coordinate (32), y-coodinate (32)
    uint8_t chain_code[CHAIN_CODE_LEN];        /// for public key derivation
} pubkey_ctx_t;

/**
 * Structure for transaction information context.
 */
typedef struct {
    uint8_t raw_tx[MAX_TRANSACTION_LEN];   /// raw transaction serialized
    size_t raw_tx_len;                     /// length of raw transaction
    transaction_t transaction;             /// structured transaction
    uint8_t m_hash[CX_SHA256_SIZE];        /// message hash digest
    uint8_t signature[MAX_SIGNATURE_LEN];  /// transaction signature encoded in DER
    uint8_t signature_len;                 /// length of transaction signature
    uint8_t v;                             /// parity of y-coordinate of R in ECDSA signature
} transaction_ctx_t;

/**
 * Structure for personal message context information.
 */
typedef struct {
    uint8_t raw_msg[MAX_MESSAGE_LEN];      /// raw message serialized
    size_t raw_msg_len;                    /// length of raw message
    uint8_t m_hash[CX_SHA256_SIZE];        /// message hash digest
    uint8_t signature[MAX_SIGNATURE_LEN];  /// message signature encoded in DER
    uint8_t signature_len;                 /// length of message signature
    uint8_t v;                             /// parity of y-coordinate of R in ECDSA signature
} message_ctx_t;

/**
 * Structure for global context.
 */
typedef struct {
    state_e state;  /// state of the context
    union {
        pubkey_ctx_t pk_info;       /// public key context
        transaction_ctx_t tx_info;  /// transaction context
        message_ctx_t msg_info;     /// personal msg context
    };
    request_type_e req_type;              /// user request
    uint32_t bip32_path[MAX_BIP32_PATH];  /// BIP32 path
    uint8_t bip32_path_len;               /// length of BIP32 path
} global_ctx_t;
