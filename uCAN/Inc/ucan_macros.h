#ifndef UCAN_MACROS
#define UCAN_MACROS

#define UCAN_MAX_PACKET_COUNT 128

#define UCAN_PACKET_COUNT(list) (sizeof(list) / sizeof((list)[0]))

#define UCAN_INIT_AUTO(txList, rxList) \
    uCAN_Init(txList, UCAN_PACKET_COUNT(txList), rxList, UCAN_PACKET_COUNT(rxList))

#endif
