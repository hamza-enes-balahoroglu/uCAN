/**
  ******************************************************************************
  * @file    ucan_macros.h
  * @author  Hamza Enes Balahoroğlu
  * @brief   General-purpose macros for uCAN and application-level use.
  *
  * This file provides a collection of helper macros commonly used in the uCAN
  * communication layer, but also useful in application code.
  *
  * Macros such as `UCAN_PACKET_COUNT()` and `UCAN_CLIENT_COUNT()` allow
  * fast and safe calculation of array lengths. Validation macros like
  * `IS_UCAN_STATUS_TYPE_DEF()` help ensure correct parameter usage when
  * working with the uCAN API.
  *
  * @note    While primarily designed for the uCAN library, many of these macros
  *          can be reused freely in external modules where applicable.
  *
  *
  *	  						 _____          _   _
  *	  						/ ____|   /\   | \ | |
  *	  				  _   _| |       /  \  |  \| |
  *	  				 | | | | |      / /\ \ | . ` |
  *	  				 | |_| | |____ / ____ \| |\  |
  *	  				  \__,_|\_____/_/    \_\_| \_|
  *
  ******************************************************************************
  */

#ifndef UCAN_MACROS
#define UCAN_MACROS

#include "ucan_types.h"

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
							((STATUS) == UCAN_DUPLICATE_ID) || \
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
