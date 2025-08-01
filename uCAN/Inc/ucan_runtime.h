#ifndef UCAN_RUNTIME
#define UCAN_RUNTIME

#include "ucan_macros.h"
#include "ucan_types.h"

UCAN_StatusTypeDef uCAN_Runtime_SendPacket(CAN_HandleTypeDef* hcan, UCAN_Packet* packet);

UCAN_StatusTypeDef uCAN_Runtime_SendPing(CAN_HandleTypeDef* hcan, UCAN_NodeInfo* node);

UCAN_StatusTypeDef uCAN_Runtime_ReplyPing(CAN_HandleTypeDef* hcan, UCAN_NodeInfo* node);

UCAN_StatusTypeDef uCAN_Runtime_UpdatePacket(UCAN_PacketHolder* rxHolder, uint32_t StdId, uint8_t aData[]);

UCAN_StatusTypeDef uCAN_Runtime_UpdateHandshake(UCAN_NodeInfo* node, CAN_HandleTypeDef* hcan, uint32_t StdId, uint8_t aData[]);

int uCAN_Runtime_ComparePacketId(const void* a, const void* b);

int uCAN_Runtime_CompareClientId(const void* a, const void* b);

#endif
