#ifndef UCAN
#define UCAN

#include "stm32f4xx_hal.h"
#include "ucan_macros.h"
#include "ucan_types.h"


UCAN_StatusTypeDef uCAN_Init(UCAN_Packet_Init* txPacketList, uint32_t txPacketCount,
		UCAN_Packet_Init* rxPacketList, uint32_t rxPacketCount);

UCAN_StatusTypeDef uCAN_SendAll(void);

UCAN_StatusTypeDef uCAN_Update(void);

UCAN_StatusTypeDef uCAN_Handshake(void);

#endif
