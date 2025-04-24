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

#ifdef HAVE_NBGL

#include <stdbool.h>  // bool
#include <ctype.h>
#include <string.h>  // memset

#include "os.h"
#include "glyphs.h"
#include "nbgl_use_case.h"
#include "io.h"
#include "bip32.h"
#include "format.h"

#include "display.h"
#include "../globals.h"
#include "../sw.h"
#include "action/validate.h"
#include "../menu.h"
#include "utils.h"
#include "types.h"
#include "../address.h"

static void personal_msg_review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    validate_personal_msg(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_MESSAGE_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_MESSAGE_REJECTED, ui_menu_main);
    }
}

/**
 * @brief Converts a message into a human-readable format and stores it in the output buffer.
 *
 * This function processes a given message and attempts to make it visible in a readable format.
 * If the message is 20 bytes long and contains non-printable characters, it is assumed to be
 * an Ethereum address and is formatted as a hexadecimal string prefixed with "0x".
 * Otherwise, the function processes the message character by character, replacing non-printable
 * characters with their hexadecimal escape sequences (e.g., "\xNN").
 *
 * @param[in]  msg      Pointer to the input message buffer.
 * @param[in]  msg_len  Length of the input message buffer.
 * @param[out] out      Pointer to the output buffer where the formatted message will be stored.
 * @param[in]  out_len  Length of the output buffer.
 *
 * @return true if the entire message was successfully processed and stored in the output buffer,
 *         false otherwise (e.g., if the output buffer is too small or invalid input is provided).
 *
 * @note The output buffer must be large enough to accommodate the formatted message, including
 *       any escape sequences and the null-terminator.
 */
static bool make_message_visible(const uint8_t *msg,
                                 const size_t msg_len,
                                 char *out,
                                 size_t out_len) {
    if (msg == NULL || msg_len == 0 || out == NULL || out_len == 0) {
        return false;
    }

    size_t out_pos = 0;
    if (msg_len == 20 && out_len >= 43) {
        for (size_t i = 0; i < msg_len; i++) {
            if (!isprint(msg[i])) {
                out[out_pos++] = '0';
                out[out_pos++] = 'x';
                return format_hex(msg, 20, &out[out_pos], 41) == 41;
            }
        }
    }

    const char hex_digits[] = "0123456789ABCDEF";
    size_t msg_pos = 0;
    while (msg_pos < msg_len && out_pos < out_len - 1) {  // Reserve space for \0
        uint8_t c = msg[msg_pos];
        if (isspace(c)) {
            c = ' ';
        }
        if (isprint(c)) {
            out[out_pos++] = (char) c;
            msg_pos++;
        } else {
            if (out_pos + 4 > out_len - 1) {
                break;
            }
            out[out_pos++] = '\\';
            out[out_pos++] = 'x';
            out[out_pos++] = hex_digits[(c >> 4) & 0x0F];
            out[out_pos++] = hex_digits[c & 0x0F];
            msg_pos++;
        }
    }
    out[out_pos] = '\0';
    return msg_pos == msg_len;
}

// Flow used to display a clear-signed personal msg
// Public function to start the personal msg review
// - Check if the app is in the right state for personal msg review
// - Display the first screen of the personal msg review
int ui_display_message() {
    if (G_context.req_type != CONFIRM_MESSAGE || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    explicit_bzero(g_pairs, sizeof(g_pairs));
    explicit_bzero(g_buffers, sizeof(g_buffers));

    const size_t pos = sizeof(g_buffers) - MAX_BUFFER_LEN;
    if (!derive_address_from_bip32_path(&g_buffers[pos], MAX_BUFFER_LEN)) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    const size_t msg_len = G_context.msg_info.raw_msg_len;
    const uint8_t *msg = G_context.msg_info.raw_msg;
    if (!make_message_visible(msg, msg_len, g_buffers, pos)) {
        return io_send_sw(SW_PERSONAL_MSG_PARSING_FAIL);
    }

    g_pairList.nbMaxLinesForValue = 0;
    g_pairList.nbPairs = 0;
    g_pairList.pairs = g_pairs;

    g_pairs[g_pairList.nbPairs].item = NBGL_MSG;
    g_pairs[g_pairList.nbPairs++].value = g_buffers;

    g_pairs[g_pairList.nbPairs].item = SIGNER;
    g_pairs[g_pairList.nbPairs++].value = &g_buffers[(PARAMETERS_MAX_NUM + 1) * MAX_BUFFER_LEN];

    nbgl_useCaseReview(TYPE_MESSAGE,
                       &g_pairList,
                       &ICON_APP_ONTOLOGY,
                       PERSONAL_MSG_TITLE,
                       NULL,
                       PERSONAL_MSG_CONTENT,
                       personal_msg_review_choice);

    return 0;
}

#endif
