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

#include "macros.h"

#include "parse.h"
#include "utils.h"
#include "address.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

static bool parse_amount(buffer_t *buf, tx_parameter_t *out) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(out != NULL, "NULL out");

    uint8_t amt = 0;
    if (!buffer_read_u8(buf, &amt)) {
        return false;
    }

    if ((amt > OPCODE_PUSH_NUMBER && amt <= OPCODE_PUSH_NUMBER + 16) || amt == 0) {
        out->len = 1;
    } else if (amt > 2 * sizeof(uint64_t)) {
        return false;
    } else {
        out->len = amt + 1;
    }

    out->data = (uint8_t *) (buf->ptr + buf->offset - 1);
    out->type = PARAM_AMOUNT;
    return buffer_seek_cur(buf, out->len - 1);
}

static bool parse_get_amount(buffer_t *buf, uint64_t *out) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(out != NULL, "NULL out");

    tx_parameter_t tmp;
    return parse_amount(buf, &tmp) && convert_param_to_uint64_le(&tmp, out);
}

static bool parse_uint128(buffer_t *buf, tx_parameter_t *out) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(out != NULL, "NULL out");

    const uint8_t size = 16;
    if (!buffer_can_read(buf, size)) return false;

    out->len = size;
    out->data = (uint8_t *) (buf->ptr + buf->offset);
    out->type = PARAM_UINT128;
    return buffer_seek_cur(buf, size);
}

static bool parse_pk(buffer_t *buf, tx_parameter_t *out) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(out != NULL, "NULL out");

    uint8_t size = 0;
    if (!buffer_read_u8(buf, &size) || size != 2 * COMPRESSED_KEY_LEN ||
        !buffer_can_read(buf, 2 * COMPRESSED_KEY_LEN)) {
        return false;
    }

    out->len = size;
    out->data = (uint8_t *) (buf->ptr + buf->offset);
    out->type = PARAM_PUBKEY;
    return buffer_seek_cur(buf, size);
}

static bool parse_ont_id(buffer_t *buf) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");

    uint8_t size = 0;
    return buffer_read_u8(buf, &size) && size != 0 && buffer_seek_cur(buf, size);
}

static bool parse_pk_amount_pairs(buffer_t *buf, tx_parameter_t *pairs, size_t *cur) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(pairs != NULL, "NULL pairs");
    LEDGER_ASSERT(cur != NULL, "NULL cur");

    uint64_t pks_num = 0;

    if (!parse_amount(buf, &pairs[0]) || !convert_param_to_uint64_le(&pairs[0], &pks_num) ||
        pks_num == 0 ||
        !parse_check_constant(buf, OPCODE_PARAM_END, ARRAY_LENGTH(OPCODE_PARAM_END))) {
        return false;
    }

    for (size_t i = 1; i <= pks_num; i++) {
        if (!parse_pk(buf, &pairs[i]) ||
            !parse_check_constant(buf, OPCODE_PARAM_END, ARRAY_LENGTH(OPCODE_PARAM_END))) {
            return false;
        }
    }

    if (!parse_check_amount(buf, pks_num) ||
        !parse_check_constant(buf, OPCODE_PARAM_END, ARRAY_LENGTH(OPCODE_PARAM_END))) {
        return false;
    }

    for (size_t i = 1; i <= pks_num; i++) {
        if (!parse_amount(buf, &pairs[pks_num + i]) ||
            (i != pks_num &&
             !parse_check_constant(buf, OPCODE_PARAM_END, ARRAY_LENGTH(OPCODE_PARAM_END)))) {
            return false;
        }
    }

    *cur += (pks_num * 2 + 1);
    return true;
}

bool parse_check_constant(buffer_t *buf, const uint8_t *str, size_t len) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(str != NULL, "NULL str");
    LEDGER_ASSERT(len > 0, "len is 0");

    return buffer_can_read(buf, len) && memcmp(buf->ptr + buf->offset, str, len) == 0 &&
           buffer_seek_cur(buf, len);
}

bool parse_address(buffer_t *buf, bool has_length, tx_parameter_t *out) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(out != NULL, "NULL out");

    size_t prefix_len = has_length ? 1 : 0;
    uint8_t size = 0;

    if (!buffer_can_read(buf, ADDRESS_SCRIPT_HASH_LEN + prefix_len) ||
        (has_length && (!buffer_read_u8(buf, &size) || size != ADDRESS_SCRIPT_HASH_LEN))) {
        return false;
    }

    out->len = ADDRESS_SCRIPT_HASH_LEN;
    out->data = (uint8_t *) (buf->ptr + buf->offset);
    out->type = PARAM_ADDR;
    return buffer_seek_cur(buf, ADDRESS_SCRIPT_HASH_LEN);
}

bool parse_check_amount(buffer_t *buf, uint64_t num) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");

    uint64_t out = 0;
    return parse_get_amount(buf, &out) && out == num;
}

bool parse_method_name(buffer_t *buf, tx_parameter_t *out) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(out != NULL, "NULL out");

    uint8_t size = 0;
    if (!buffer_read_u8(buf, &size) || size == 0 || !buffer_can_read(buf, size)) {
        return false;
    }

    out->len = size;
    out->data = (uint8_t *) (buf->ptr + buf->offset);
    return buffer_seek_cur(buf, size);
}

bool parse_trasfer_state(buffer_t *buf, tx_parameter_t *transfer_state, size_t *cur) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(transfer_state != NULL, "NULL transfer_state");
    LEDGER_ASSERT(cur != NULL, "NULL cur");

    if (!parse_check_constant(buf, OPCODE_ST_BEGIN, ARRAY_LENGTH(OPCODE_ST_BEGIN)) ||
        !parse_address(buf, true, &transfer_state[0]) ||
        !parse_check_constant(buf, OPCODE_PARAM_END, ARRAY_LENGTH(OPCODE_PARAM_END)) ||
        !parse_address(buf, true, &transfer_state[1]) ||
        !parse_check_constant(buf, OPCODE_PARAM_END, ARRAY_LENGTH(OPCODE_PARAM_END)) ||
        !parse_amount(buf, &transfer_state[2]) ||
        !parse_check_constant(buf, OPCODE_PARAM_END, ARRAY_LENGTH(OPCODE_PARAM_END)) ||
        !parse_check_constant(buf, OPCODE_ST_END, ARRAY_LENGTH(OPCODE_ST_END))) {
        return false;
    }

    *cur += 3;
    return true;
}

bool parse_method_params(buffer_t *buf,
                         transaction_t *tx,
                         const tx_parameter_type_e *params,
                         size_t *params_num) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");
    LEDGER_ASSERT(params != NULL, "NULL params");
    LEDGER_ASSERT(params_num != NULL, "NULL params_num");

    size_t cur = 0;
    *params_num = 0;

    for (; *params != PARAM_END; ++params) {
        (*params_num)++;
        switch (*params) {
            case PARAM_ADDR:
                if (!parse_address(buf,
                                   tx->contract.type != WASMVM_CONTRACT,
                                   &tx->method.parameters[cur++])) {
                    return false;
                }
                break;
            case PARAM_AMOUNT:
                if (!parse_amount(buf, &tx->method.parameters[cur++])) {
                    return false;
                }
                break;
            case PARAM_UINT128:
                if (!parse_uint128(buf, &tx->method.parameters[cur++])) {
                    return false;
                }
                break;
            case PARAM_PUBKEY:
                if (!parse_pk(buf, &tx->method.parameters[cur++])) {
                    return false;
                }
                break;
            case PARAM_ONTID:
                if (!parse_ont_id(buf)) {
                    return false;
                }
                break;
            case PARAM_PK_AMOUNT_PAIRS:
                if (!parse_pk_amount_pairs(buf, &tx->method.parameters[cur], &cur)) {
                    return false;
                }
                break;
            case PARAM_TRANSFER_STATE:
                if (!parse_trasfer_state(buf, &tx->method.parameters[cur], &cur)) {
                    return false;
                }
                break;

            default:
                return false;
        }

        if (tx->contract.type == NATIVE_CONTRACT &&
            !parse_check_constant(buf, OPCODE_PARAM_END, ARRAY_LENGTH(OPCODE_PARAM_END))) {
            return false;
        }
    }

    return true;
}
