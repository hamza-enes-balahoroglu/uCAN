#ifndef UCAN
#define UCAN

#include "stm32f4xx_hal.h"
#include "ucan_macros.h"
#include "ucan_types.h"


UCAN_StatusTypeDef uCAN_Init(UCAN_HandleTypeDef* ucan);

UCAN_StatusTypeDef uCAN_Start(UCAN_HandleTypeDef* ucan, UCAN_Config* config);

UCAN_StatusTypeDef uCAN_SendAll(UCAN_HandleTypeDef* ucan);

UCAN_StatusTypeDef uCAN_Update(UCAN_HandleTypeDef* ucan);

UCAN_StatusTypeDef uCAN_Handshake(UCAN_HandleTypeDef* ucan);

#endif
