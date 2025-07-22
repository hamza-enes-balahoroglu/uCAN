#ifndef YCAN
#define YCAN

#include "ycan_types.h"
#include "ycan_macros.h"
#include "stm32f4xx_hal.h"


YCAN_StatusTypeDef yCAN_Init(ycan_packet_init_t* txPacketList, uint32_t txPacketCount,
		ycan_packet_init_t* rxPacketList, uint32_t rxPacketCount);

YCAN_StatusTypeDef yCAN_SendAll(void);

YCAN_StatusTypeDef yCAN_Update(void);

#endif
