#ifndef UCAN_DEBUG
#define UCAN_DEBUG

#include "ucan_types.h"
#include "ucan_macros.h"

uint8_t Calculate_DLC(UCAN_PacketConfig* pkt);
UCAN_StatusTypeDef uCAN_CheckPacketConfig(UCAN_PacketConfig* configList, UCAN_PacketHolder* packetHolder);
UCAN_Packet* uCAN_Finalize_Packet(UCAN_PacketConfig* configList, UCAN_PacketHolder* packetHolder);
UCAN_StatusTypeDef uCAN_CheckNodeInfo(UCAN_NodeInfo* node);
void uCAN_CheckIsDataType(UCAN_PacketConfig* pkt);

#endif
