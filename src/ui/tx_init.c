/*****************************************************************************
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
 *****************************************************************************/

#include "tx_init.h"
#include "types.h"
#include "../transaction/contract.h"
#include "../transaction/utils.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

#define MAX_CONFIGS 7  // Max number of param_config_t entries per method

const method_display_t *init_dipslay_pos_and_item(const transaction_t *tx) {
    static method_display_t method;
    static param_config_t configs[MAX_CONFIGS];

    if (tx == NULL) {
        return NULL;
    }

    const uint8_t *method_data = tx->method.name.data;
    size_t method_len = tx->method.name.len;

    if (method_data == NULL || method_len == 0) {
        return NULL;
    }   

    if (methodcmp(&tx->method.name, METHOD_TRANSFER)) {
        method.title = TRANSFER_TITLE;    
        method.finish_title = TRANSFER_CONTENT;
        if (tx->contract.type != NEOVM_CONTRACT) {
            configs[0] = (param_config_t) {FROM, 0};
            configs[1] = (param_config_t) {TO, 2};
            configs[2] = (param_config_t) {AMOUNT, 1};
        } else {
            configs[0] = (param_config_t) {AMOUNT, 1};
            configs[1] = (param_config_t) {TO, 2};
            configs[2] = (param_config_t) {FROM, 0};
        }
        method.configs = configs;
        method.config_count = 3;
    } else if (methodcmp(&tx->method.name, METHOD_TRANSFER_V2)) {
        method.title = TRANSFER_TITLE;
        method.finish_title = TRANSFER_CONTENT;
        configs[0] = (param_config_t) {FROM, 0};
        configs[1] = (param_config_t) {TO, 2};
        configs[2] = (param_config_t) {AMOUNT, 1};
        method.configs = configs;
        method.config_count = 3;
    } else if (methodcmp(&tx->method.name, METHOD_TRANSFER_FROM)) {
        method.title = TRANSFER_FROM_TITLE;
        method.finish_title = TRANSFER_FROM_CONTENT;
        if (tx->contract.type != NEOVM_CONTRACT) {
            configs[0] = (param_config_t) {SPENDER, 0};
            configs[1] = (param_config_t) {FROM, 1};
            configs[2] = (param_config_t) {TO, 3};
            configs[3] = (param_config_t) {AMOUNT, 2};
        } else {
            configs[0] = (param_config_t) {AMOUNT, 2};
            configs[1] = (param_config_t) {TO, 3};
            configs[2] = (param_config_t) {FROM, 1};
            configs[3] = (param_config_t) {SPENDER, 0};
        }
        method.configs = configs;
        method.config_count = 4;
    } else if (methodcmp(&tx->method.name, METHOD_TRANSFER_FROM_V2)) {
        method.title = TRANSFER_FROM_TITLE;
        method.finish_title = TRANSFER_FROM_CONTENT;
        configs[0] = (param_config_t) {SPENDER, 0};
        configs[1] = (param_config_t) {FROM, 1};
        configs[2] = (param_config_t) {TO, 3};
        configs[3] = (param_config_t) {AMOUNT, 2};
        method.configs = configs;
        method.config_count = 4;
    } else if (methodcmp(&tx->method.name, METHOD_APPROVE)) {
        method.title = APPROVE_TITLE;
        method.finish_title = APPROVE_CONTENT;
        if (tx->contract.type != NEOVM_CONTRACT) {
            configs[0] = (param_config_t) {FROM, 0};
            configs[1] = (param_config_t) {TO, 2};
            configs[2] = (param_config_t) {AMOUNT, 1};
        } else {
            configs[0] = (param_config_t) {AMOUNT, 1};
            configs[1] = (param_config_t) {TO, 2};
            configs[2] = (param_config_t) {FROM, 0};
        }
        method.configs = configs;
        method.config_count = 3;
    } else if (methodcmp(&tx->method.name, METHOD_APPROVE_V2)) {
        method.title = APPROVE_TITLE;
        method.finish_title = APPROVE_CONTENT;
        configs[0] = (param_config_t) {FROM, 0};
        configs[1] = (param_config_t) {TO, 2};
        configs[2] = (param_config_t) {AMOUNT, 1};
        method.configs = configs;
        method.config_count = 3;
    } else if (methodcmp(&tx->method.name, METHOD_REGISTER_CANDIDATE)) {
        method.title = REGISTER_CANDIDATE_TITLE;
        method.finish_title = REGISTER_CANDIDATE_CONTENT;
        configs[0] = (param_config_t) {PEER_PUBKEY, 1};
        configs[1] = (param_config_t) {STAKE_ADDRESS, 0};
        configs[2] = (param_config_t) {STAKE_AMOUNT, 2};
        method.configs = configs;
        method.config_count = 3;
    } else if (methodcmp(&tx->method.name, METHOD_QUIT_NODE)) {
        method.title = QUIT_NODE_TITLE;
        method.finish_title = QUIT_NODE_CONTENT;
        configs[0] = (param_config_t) {PEER_PUBKEY, 1};
        configs[1] = (param_config_t) {STAKE_ADDRESS, 0};
        method.configs = configs;
        method.config_count = 2;
    } else if (methodcmp(&tx->method.name, METHOD_ADD_INIT_POS)) {
        method.title = ADD_INIT_POS_TITLE;
        method.finish_title = ADD_INIT_POS_CONTENT;
        configs[0] = (param_config_t) {PEER_PUBKEY, 1};
        configs[1] = (param_config_t) {STAKE_ADDRESS, 0};
        configs[2] = (param_config_t) {STAKE_AMOUNT, 2};
        method.configs = configs;
        method.config_count = 3;
    } else if (methodcmp(&tx->method.name, METHOD_REDUCE_INIT_POS)) {
        method.title = REDUCE_INIT_POS_TITLE;
        method.finish_title = REDUCE_INIT_POS_CONTENT;
        configs[0] = (param_config_t) {PEER_PUBKEY, 1};
        configs[1] = (param_config_t) {STAKE_ADDRESS, 0};
        configs[2] = (param_config_t) {AMOUNT, 2};
        method.configs = configs;
        method.config_count = 3;
    } else if (methodcmp(&tx->method.name, METHOD_CHANGE_MAX_AUTH)) {
        method.title = CHANGE_MAX_AUTHORIZATION_TITLE;
        method.finish_title = CHANGE_MAX_AUTHORIZATION_CONTENT;
        configs[0] = (param_config_t) {PEER_PUBKEY, 1};
        configs[1] = (param_config_t) {STAKE_ADDRESS, 0};
        configs[2] = (param_config_t) {MAX_AUTHORIZE, 2};
        method.configs = configs;
        method.config_count = 3;
    } else if (methodcmp(&tx->method.name, METHOD_SET_FEE_PERCENTAGE)) {
        method.title = SET_FEE_PERCENTAGE_TITLE;
        method.finish_title = SET_FEE_PERCENTAGE_CONTENT;
        configs[0] = (param_config_t) {PEER_PUBKEY, 1};
        configs[1] = (param_config_t) {STAKE_ADDRESS, 0};
        configs[2] = (param_config_t) {PEER_INCENTIVE, 2};
        configs[3] = (param_config_t) {USER_INCENTIVE, 3};
        method.configs = configs;
        method.config_count = 4;
    } else if (methodcmp(&tx->method.name, METHOD_AUTHORIZE_FOR_PEER)) {
        method.title = AUTHORIZE_FOR_PEER_TITLE;
        method.finish_title = AUTHORIZE_FOR_PEER_CONTENT;
        configs[0] = (param_config_t) {STAKE_ADDRESS, 0};
        method.configs = configs;
        method.config_count = 1;
    } else if (methodcmp(&tx->method.name, METHOD_UNAUTHORIZE_FOR_PEER)) {
        method.title = UN_AUTHORIZE_FOR_PEER_TITLE;
        method.finish_title = UN_AUTHORIZE_FOR_PEER_CONTENT;
        configs[0] = (param_config_t) {STAKE_ADDRESS, 0};
        method.configs = configs;
        method.config_count = 1;
    } else if (methodcmp(&tx->method.name, METHOD_WITHDRAW)) {
        method.title = WITHDRAW_TITLE;
        method.finish_title = WITHDRAW_CONTENT;
        configs[0] = (param_config_t) {STAKE_ADDRESS, 0};
        method.configs = configs;
        method.config_count = 1;
    } else if (methodcmp(&tx->method.name, METHOD_WITHDRAW_FEE)) {
        method.title = WITHDRAW_FEE_TITLE;
        method.finish_title = WITHDRAW_FEE_CONTENT;
        configs[0] = (param_config_t) {STAKE_ADDRESS, 0};
        method.configs = configs;
        method.config_count = 1;
    } else {
        return NULL;
    }

    return &method;
}

bool convert_param_to_chars(transaction_t *tx, uint8_t param_idx, char *buffer, size_t buffer_len) {
    if (tx == NULL || buffer == NULL) {
        PRINTF("Error: Null pointer in convert_param_to_chars\n");
        return false;
    }

    tx_parameter_t *param = &tx->method.parameters[param_idx];
    if (param_idx >= PARAMETERS_MAX_NUM || param == NULL || param->data == NULL) {
        PRINTF("Error: param, idx %u, %p\n", param_idx, param->data);
        return false;
    }
    explicit_bzero(buffer, buffer_len);

    switch (param->type) {
        case PARAM_ADDR:
            if (!convert_script_hash_to_base58_address(buffer, buffer_len, param->data)) {
                return false;
            }
            break;
        case PARAM_UINT128:
        case PARAM_AMOUNT:
            if (!convert_param_amount_to_chars(param,
                                               tx->contract.token_decimals,
                                               tx->contract.type != WASMVM_CONTRACT,
                                               buffer,
                                               buffer_len)) {
                return false;
            }
            if (methodcmp(&tx->method.name, METHOD_SET_FEE_PERCENTAGE)) {
                strlcat(buffer, PERCENTAGE, buffer_len);
            } else {
                strlcat(buffer, " ", buffer_len);
                strlcat(buffer, tx->contract.ticker, buffer_len);
            }
            return true;
            break;
        case PARAM_PUBKEY:
            if (param->len <= buffer_len) {
                memcpy(buffer, param->data, param->len);
                buffer[param->len] = '\0';
                return true;
            } else {
                PRINTF("Error: PUBKEY length %u exceeds buffer_len %u\n", param->len, buffer_len);
                return false;
            }
            break;
        default:
            PRINTF("Warning: Unknown param type %d\n", param->type);
            return false;
    }
    return true;
}