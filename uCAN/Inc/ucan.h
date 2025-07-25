#ifndef UCAN
#define UCAN

#include "stm32f4xx_hal.h"
#include "ucan_macros.h"
#include "ucan_types.h"


UCAN_StatusTypeDef uCAN_Init(UCAN_InitTypeDef UCAN_InitStruct);

UCAN_StatusTypeDef uCAN_SendAll(void);

UCAN_StatusTypeDef uCAN_Update(void);

UCAN_StatusTypeDef uCAN_Handshake(void);

#endif
