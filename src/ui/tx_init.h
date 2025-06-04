#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../transaction/tx_types.h"

// Configuration for parameter parsing
typedef struct {
    const char *item;
    uint8_t pos;
} param_config_t;


typedef struct {
    const char *title;
    const char *finish_title;
    param_config_t *configs;
    uint8_t config_count;
} method_display_t;

/**
 * @brief Initializes the display position and item based on the given transaction.
 *
 * @param tx Pointer to the transaction structure containing the data to initialize the display.
 * @return Pointer to a method_display_t structure representing the initialized display position and item.
 */
const method_display_t *init_dipslay_pos_and_item(const transaction_t *tx);

/**
 * @brief Converts a parameter from the transaction into a character string.
 *        It supports simple type parameters.
 *
 * @param tx Pointer to the transaction structure containing the parameter to convert.
 * @param param_idx Index of the parameter to convert.
 * @param buffer Pointer to the buffer where the converted string will be stored.
 * @param buffer_len Length of the buffer to ensure no overflow occurs.
 * @return true if the conversion is successful, false otherwise.
 */
bool convert_param_to_chars(transaction_t *tx, uint8_t param_idx, char *buffer, size_t buffer_len);