/**
  ******************************************************************************
  * @file    ucan_macros.h
  * @author  Hamza Enes Balahoroğlu
  * @brief   General-purpose macros for uCAN and application-level use.
  *
  * This header file contains essential macro definitions used throughout the
  * uCAN communication framework. These macros provide utility functions,
  * protocol timing logic, and validation tools to simplify and standardize
  * the development process.
  *
  * Key Categories:
  *  - **Handshake Logic:** Macros such as `UCAN_HANDSHAKE_TICK_DIFF`,
  *    `UCAN_HANDSHAKE_IS_TIMEOUT`, and `UCAN_HANDSHAKE_IS_LOST` define how
  *    handshake response timing is interpreted to detect delays or lost clients.
  *
  *  - **Array Utilities:** Macros like `UCAN_PACKET_COUNT()` and
  *    `UCAN_CLIENT_COUNT()` are used to safely determine the size of arrays at compile time.
  *
  *  - **Validation Macros:** Macros such as `IS_UCAN_STATUS_TYPE_DEF()` ensure
  *    that arguments passed to functions match expected types or limits, reducing bugs.
  *
  *  - **Status Check Macro:** `UCAN_CHECK_READY()` provides a compact and
  *    readable way to validate the state of a `uCAN` instance before proceeding.
  *
  * Usage of these macros increases code readability and consistency while
  * reducing repeated boilerplate checks.
  *
  * @note    While primarily intended for internal use within the uCAN library,
  *          many of these macros are generic enough for use in other modules.
  *
  *
  *                          _____          _   _
  *                         / ____|   /\   | \ | |
  *	  				  _   _| |       /  \  |  \| |
  *	  				 | | | | |      / /\ \ | . ` |
  *	  				 | |_| | |____ / ____ \| |\  |
  *	  				  \____|\_____/_/    \_\_| \_|
  *
  ******************************************************************************
  */

#ifndef UCAN_MACROS
#define UCAN_MACROS

#include "ucan_types.h"


#define UCAN_HANDSHAKE_REQUEST_VALUE   	0xA5U 	/*!< Value sent by the Master to initiate a handshake (ping) */

#define UCAN_HANDSHAKE_RESPONSE_VALUE  	0x5AU	/*!< Value sent back by the Client in response to a handshake request */

#define UCAN_HANDSHAKE_INTERVAL_MS    	500  	/*!< Interval (ms) at which the Master sends handshake pings */

#define UCAN_HANDSHAKE_TIMEOUT_MS     	700 	/*!< Max time (ms) to wait for a Client response before considering it "delayed" (with 200ms tolerance) */

#define UCAN_HANDSHAKE_LOST_MS       	2000  	/*!< If no response is received within this time (ms), the connection is considered lost */

/**
  * @brief Calculates the time difference between when the handshake was sent and when a response was received.
  *
  * @note  Handles timer overflow by wrapping around UINT32_MAX.
  */
#define UCAN_HANDSHAKE_TICK_DIFF(sentTick, responseTick) \
    (((sentTick) <= (responseTick)) ? ((responseTick) - (sentTick)) : (UINT32_MAX - (responseTick) + (sentTick) + 1U))

/**
  * @brief Determines if a client response is considered "timed out" but not yet fully "lost".
  *
  * @note  Used to detect slow or delayed responses before marking as disconnected.
  */
#define UCAN_HANDSHAKE_IS_TIMEOUT(sentTick, responseTick) \
    (UCAN_HANDSHAKE_TICK_DIFF((sentTick), (responseTick)) > UCAN_HANDSHAKE_TIMEOUT_MS && \
     UCAN_HANDSHAKE_TICK_DIFF((sentTick), (responseTick)) < UCAN_HANDSHAKE_LOST_MS)

/**
  * @brief Determines if the client is considered "lost" due to no response within allowed window.
  *
  * @note  Use this to flag the client as disconnected from the network.
  */
#define UCAN_HANDSHAKE_IS_LOST(sentTick, responseTick) \
    (UCAN_HANDSHAKE_TICK_DIFF((sentTick), (responseTick)) >= UCAN_HANDSHAKE_LOST_MS)

/**
  * @brief  Checks if the given UCAN handler is ready for operation.
  *
  * @note   This macro performs a series of sanity checks on the `ucan` pointer
  *         and its internal status. If any error or uninitialized condition is
  *         detected, it returns the corresponding UCAN_StatusTypeDef error code.
  *
  *         Typical use case is at the beginning of any UCAN function to avoid
  *         executing logic on an invalid or misconfigured instance.
  *
  *         Errors checked:
  *         - Null pointer
  *         - Not initialized
  *         - General error
  *         - Duplicate ID
  *         - Filter configuration failure
  *         - CAN peripheral start failure
  *         - Notification setup failure
  *         - Missing required values
  */
#define UCAN_CHECK_READY(ucan)                                      \
    do {                                                            \
        if ((ucan) == NULL) {                                       \
            return UCAN_INVALID_PARAM;                              \
        }                                                           \
        if ((ucan)->status == UCAN_NOT_INITIALIZED) {               \
            return UCAN_NOT_INITIALIZED;                            \
        }                                                           \
        if ((ucan)->status == UCAN_ERROR) {                         \
            return UCAN_ERROR;                                      \
        }                                                           \
        if ((ucan)->status == UCAN_ERROR_DUPLICATE_ID) {            \
            return UCAN_ERROR_DUPLICATE_ID;                         \
        }                                                           \
        if ((ucan)->status == UCAN_ERROR_FILTER_CONFIG) {           \
            return UCAN_ERROR_FILTER_CONFIG;                        \
        }                                                           \
		if ((ucan)->status == UCAN_ERROR_CAN_START) {               \
            return UCAN_ERROR_CAN_START;                            \
        }                                                           \
		if ((ucan)->status == UCAN_ERROR_CAN_NOTIFICATION) {        \
            return UCAN_ERROR_CAN_NOTIFICATION;                     \
        }                                                           \
		if ((ucan)->status == UCAN_MISSING_VAL) {                   \
            return UCAN_MISSING_VAL;                                \
        }                                                           \
    } while (0)

/**
  * @brief  Calculates the number of packets in a static UCAN_PacketConfig list.
  * @param  list: Array of UCAN_PacketConfig elements.
  * @retval Number of elements in the list.
  * @note   Should only be used with statically allocated arrays, not pointers.
  */
#define UCAN_PACKET_COUNT(list) (sizeof(list) / sizeof((list)[0]))

/**
  * @brief  Calculates the number of client IDs in a static list.
  * @param  client: Array of client IDs (uint32_t).
  * @retval Number of client IDs.
  * @note   Should only be used with statically allocated arrays, not pointers.
  */
#define UCAN_CLIENT_COUNT(client) (sizeof(client) / sizeof((client)[0]))

/**
  * @brief  Checks if the given status is a valid UCAN connection status.
  * @param  STATUS: Value to check.
  * @retval 1 if valid, 0 otherwise.
  */
#define IS_UCAN_CONNECTION_STATUS_TYPE_DEF(STATUS) ( \
							((STATUS) == UCAN_CONN_ACTIVE) || \
							((STATUS) == UCAN_CONN_LOST) || \
							((STATUS) == UCAN_CONN_WAITING) || \
							((STATUS) == UCAN_CONN_TIMEOUT)

/**
  * @brief  Checks if the given role is a valid UCAN node role.
  * @param  ROLE: Value to check.
  * @retval 1 if valid, 0 otherwise.
  */
#define IS_UCAN_NODE_ROLE(ROLE) ( \
							((ROLE) == UCAN_ROLE_MASTER) || \
							((ROLE) == UCAN_ROLE_CLIENT) || \
							((ROLE) == UCAN_ROLE_NONE)

/**
  * @brief  Checks if the given status is a valid UCAN status code.
  * @param  STATUS: Value to check.
  * @retval 1 if valid, 0 otherwise.
  */
#define IS_UCAN_STATUS_TYPE_DEF(STATUS) ( \
							((STATUS) == UCAN_NOT_INITIALIZED) || \
							((STATUS) == UCAN_OK) || \
							((STATUS) == UCAN_ERROR) || \
							((STATUS) == UCAN_MISSING_VAL) || \
							((STATUS) == UCAN_NO_CONNECTION) || \
							((STATUS) == UCAN_NO_CHANGED_VAL) || \
							((STATUS) == UCAN_TIMEOUT) || \
							((STATUS) == UCAN_INVALID_PARAM) || \
							((STATUS) == UCAN_BUSY) || \
							((STATUS) == UCAN_ERROR_DUPLICATE_ID) || \
							((STATUS) == UCAN_ERROR_FILTER_CONFIG) || \
							((STATUS) == UCAN_ERROR_CAN_START) || \
							((STATUS) == UCAN_ERROR_CAN_NOTIFICATION))


/**
  * @brief  Checks if the given type is a valid UCAN data type.
  * @param  TYPE: Value to check.
  * @retval 1 if valid, 0 otherwise.
  */
#define IS_UCAN_DATA_TYPE(TYPE) ( \
							((TYPE) == UCAN_U8) || \
							((TYPE) == UCAN_U16) || \
							((TYPE) == UCAN_U32)

#endif
