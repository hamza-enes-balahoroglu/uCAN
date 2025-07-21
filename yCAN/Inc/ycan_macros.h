#ifndef YCAN_MACROS
#define YCAN_MACROS

#define YCAN_MAX_PACKET_COUNT 128

#define YCAN_PACKET_COUNT(list) (sizeof(list) / sizeof((list)[0]))

#define YCAN_INIT_AUTO(txList, rxList) \
    yCAN_Init(txList, YCAN_PACKET_COUNT(txList), rxList, YCAN_PACKET_COUNT(rxList))

#endif
