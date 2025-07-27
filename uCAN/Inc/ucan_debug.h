#ifndef UCAN_DEBUG
#define UCAN_DEBUG

#include "ucan_types.h"
#include "ucan_macros.h"

uint8_t uCAN_Debug_Calculate_DLC(UCAN_PacketConfig* pkt);

UCAN_StatusTypeDef uCAN_Debug_CheckPacketConfig(UCAN_PacketConfig* configList, UCAN_PacketHolder* packetHolder);

UCAN_Packet uCAN_Debug_FinalizePacket(UCAN_PacketConfig* configPackets, UCAN_PacketHolder* packetHolder);

UCAN_StatusTypeDef uCAN_Debug_CheckNodeInfo(UCAN_NodeInfo* node);

UCAN_StatusTypeDef uCAN_Debug_CheckIsDataType(UCAN_PacketConfig* pkt);


#endif
