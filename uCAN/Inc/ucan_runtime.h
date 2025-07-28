#ifndef UCAN_RUNTIME
#define UCAN_RUNTIME

#include "ucan_macros.h"
#include "ucan_types.h"

UCAN_StatusTypeDef uCAN_Runtime_SendPacket(CAN_HandleTypeDef* hcan, UCAN_Packet* packet);

int uCAN_Runtime_ComparePacketId(const void* a, const void* b);
#endif
