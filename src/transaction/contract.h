#pragma once

#include "tx_types.h"
#include "address.h"

#define MAX_METHOD_NUM 11
#define PREDEFINED_CONTRACT_NUM 6    //should be updated if there is new predefined items.

#define ONG_TICKER "ONG"                     //native, gas token
#define ONT_TICKER "ONT"                     //native, staking token
#define ONG_DECIMALS 9                       // 18 for transferV2, transferFromV2, approveV2
#define ONT_DECIMALS 0                       // 9 for transferV2, transferFromV2, approveV2
#define TOKEN_AMOUNT 1000000000000000000ULL  // the total supply of ONT, also the one of ONG


#define ONT_ADDR ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01")
#define ONG_ADDR ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02")
#define GOV_ADDR ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07")
#define WING_ADDR ("\x80\xef\x58\x6e\xf5\xff\xf2\xb1\xea\x83\x78\x39\xd6\x62\xa5\x27\xcd\x9f\xc5\x00")
#define STONT_ADDR ("\xcd\x84\x1f\xf2\x17\xc6\x8d\xc2\xfa\xf8\x10\x63\x68\xc5\x57\x7b\x12\x04\xec\xe9")
#define MBL_ADDR ("\x6d\x03\xa1\x68\x43\xe6\x1e\x5d\x96\xd3\x89\xe1\x78\x71\x7e\xd5\x7f\x9d\xa4\xe5")


// Common method name constants
#define METHOD_TRANSFER "transfer"
#define METHOD_TRANSFER_FROM "transferFrom"
#define METHOD_APPROVE "approve"
#define METHOD_TRANSFER_V2 "transferV2"
#define METHOD_TRANSFER_FROM_V2 "transferFromV2"
#define METHOD_APPROVE_V2 "approveV2"
#define METHOD_REGISTER_CANDIDATE "registerCandidate"
#define METHOD_QUIT_NODE "quitNode"
#define METHOD_ADD_INIT_POS "addInitPos"
#define METHOD_REDUCE_INIT_POS "reduceInitPos"
#define METHOD_CHANGE_MAX_AUTH "changeMaxAuthorization"
#define METHOD_SET_FEE_PERCENTAGE "setFeePercentage"
#define METHOD_AUTHORIZE_FOR_PEER "authorizeForPeer"
#define METHOD_UNAUTHORIZE_FOR_PEER "unAuthorizeForPeer"
#define METHOD_WITHDRAW "withdraw"
#define METHOD_WITHDRAW_FEE "withdrawFee"

/**
 * Structure for transaction method signature.
 */
typedef struct {
    const char *name;
    const tx_parameter_type_e *parameters;
} tx_method_signature_t;

/**
 * Structure for transaction payload.
 */
typedef struct {
    uint8_t token_decimals;
    const char *ticker;
    uint8_t contract_addr[ADDRESS_SCRIPT_HASH_LEN];
    tx_method_signature_t methods[MAX_METHOD_NUM];
} payload_t;



/**
 * Add the predefined contracts and their methods in this function
 * Don't forget to modify the `PREDEFINED_CONTRACT_NUM` if you add a new contract
 *
 * @param[out] storage
 *   Pointer to the pre-defined contracts and methods array.
 *
 * @return none.
 *
 */
void get_tx_payload(payload_t *storage);