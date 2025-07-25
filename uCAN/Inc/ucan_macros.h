#ifndef UCAN_MACROS
#define UCAN_MACROS

#include "ucan_types.h"

#define UCAN_MAX_PACKET_COUNT 128

#define UCAN_PACKET_COUNT(list) (sizeof(list) / sizeof((list)[0]))

#define UCAN_CLIENT_COUNT(client) (sizeof(client) / sizeof((client)[0]))

#define IS_UCAN_CONNECTION_STATUS_TYPE_DEF(STATUS) (((STATUS) == UCAN_CONN_ACTIVE) || \
                                              ((STATUS) == UCAN_CONN_LOST) || \
                                              ((STATUS) == UCAN_CONN_WAITING) || \
                                              ((STATUS) == UCAN_CONN_TIMEOUT)

#define IS_UCAN_NODE_ROLE(ROLE) (((ROLE) == UCAN_ROLE_MASTER) || \
                         ((ROLE) == UCAN_ROLE_CLIENT) || \
                         ((ROLE) == UCAN_ROLE_NONE)

#define IS_UCAN_STATUS_TYPE_DEF(STATUS) (((STATUS) == UCAN_NOT_INITIALIZED) || \
                         	 	 	 ((STATUS) == UCAN_OK) || \
									 ((STATUS) == UCAN_ERROR) || \
									 ((STATUS) == UCAN_MISSING_VAL) || \
									 ((STATUS) == UCAN_NO_CONNECTION) || \
									 ((STATUS) == UCAN_NO_CHANGED_VAL) || \
									 ((STATUS) == UCAN_TIMEOUT) || \
									 ((STATUS) == UCAN_INVALID_PARAM) || \
								 	 ((STATUS) == UCAN_BUSY)


#define IS_UCAN_DATA_TYPE(TYPE) (((TYPE) == UCAN_U8) || \
                         	 ((TYPE) == UCAN_U16) || \
							 ((TYPE) == UCAN_U32)


typedef enum {
    UCAN_U8,
    UCAN_U16,
    UCAN_U32
} UCAN_DataType;

#endif
