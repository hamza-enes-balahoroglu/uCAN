#ifndef YCAN
#define YCAN

#include "ycan_types.h"
#include "ycan_macros.h"
#include "stm32f4xx_hal.h"


YCAN_StatusTypeDef yCAN_Init(YCAN_Packet_Init* txPacketList, uint32_t txPacketCount,
		YCAN_Packet_Init* rxPacketList, uint32_t rxPacketCount);

YCAN_StatusTypeDef yCAN_SendAll(void);

YCAN_StatusTypeDef yCAN_Update(void);

YCAN_StatusTypeDef yCAN_Handshake(void);

#endif
