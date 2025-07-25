#ifndef UCAN_MACROS
#define UCAN_MACROS

#define UCAN_MAX_PACKET_COUNT 128

#define UCAN_PACKET_COUNT(list) (sizeof(list) / sizeof((list)[0]))

#define UCAN_CLIENT_COUNT(client) (sizeof(client) / sizeof((client)[0]))

#endif
