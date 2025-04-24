#pragma once

/**
 * Instruction class of the Boilerplate application.
 */
#define CLA 0x80

/**
 * Length of APPNAME variable in the Makefile.
 */
#define APPNAME_LEN (sizeof(APPNAME) - 1)

/**
 * Maximum length of MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION.
 */
#define APPVERSION_LEN 3

/**
 * Maximum length of application name.
 */
#define MAX_APPNAME_LEN 64

/**
 * Maximum transaction length (bytes).
 */
#if defined(TARGET_STAX) || defined(TARGET_FLEX)
#define MAX_TRANSACTION_LEN (1024 * 6 + 700)
#else
#define MAX_TRANSACTION_LEN (1024 * 4)
#endif
/**
 * Maximum personal message length (bytes).
 * Must be less than (PARAMETERS_MAX_NUM + 1) * MAX_BUFFER_LEN / 4
 */
#define MAX_MESSAGE_LEN 1024
/**
 * Maximum signature length (bytes).
 */
#define MAX_SIGNATURE_LEN 72
