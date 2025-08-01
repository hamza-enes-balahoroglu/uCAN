#include <stdlib.h>
#include "ucan_runtime.h"


UCAN_StatusTypeDef uCAN_Runtime_SendPacket(CAN_HandleTypeDef* hcan, UCAN_Packet* packet)
{
	if(hcan == NULL || packet == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

	CAN_TxHeaderTypeDef txHeader;
	uint32_t TxMailbox;
	uint8_t data[8];

	txHeader.StdId 	= packet->id;
	txHeader.DLC 	= packet->dlc;
	txHeader.IDE 	= CAN_ID_STD;
	txHeader.RTR 	= CAN_RTR_DATA;

	for(int i = 0; i < packet->dlc; i++) {
	    data[i] = *(packet->bits[i]);
	}

	if(HAL_CAN_AddTxMessage(hcan, &txHeader, data, &TxMailbox) != HAL_OK)
	{
		return UCAN_ERROR;
	}

	return UCAN_OK;
}
UCAN_StatusTypeDef uCAN_Runtime_SendPing(CAN_HandleTypeDef* hcan, UCAN_NodeInfo* node)
{
	if(hcan == NULL || node == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

	if(node->role != UCAN_ROLE_MASTER)
	{
		return UCAN_ERROR;
	}

	if(HAL_GetTick() - node->sentTick >= UCAN_HANDSHAKE_INTERVAL_MS)
	{
		UCAN_Packet packet;
		uint8_t request = UCAN_HANDSHAKE_REQUEST_VALUE;

		packet.id = node->selfId;
		packet.dlc = 1;

		packet.bits[0] = &request;

		node->sentTick = HAL_GetTick();
		return uCAN_Runtime_SendPacket(hcan, &packet);
	}
	return UCAN_BUSY;
}

UCAN_StatusTypeDef uCAN_Runtime_ReplyPing(CAN_HandleTypeDef* hcan, UCAN_NodeInfo* node)
{
	if(hcan == NULL || node == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

	if(node->role != UCAN_ROLE_CLIENT)
	{
		return UCAN_ERROR;
	}

	UCAN_Packet packet;
	uint8_t response = UCAN_HANDSHAKE_RESPONSE_VALUE;
	packet.id = node->selfId;
	packet.dlc = 1;

	packet.bits[0] = &response;

	return uCAN_Runtime_SendPacket(hcan, &packet);
}


UCAN_StatusTypeDef uCAN_Runtime_UpdatePacket(UCAN_PacketHolder* rxHolder, uint32_t StdId, uint8_t aData[])
{
	if(rxHolder == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

	UCAN_Packet packetKey = {.id = StdId};
	UCAN_Packet* packetFound = bsearch(&packetKey, rxHolder->packets, rxHolder->count, sizeof(UCAN_Packet), uCAN_Runtime_ComparePacketId);


	if(packetFound == NULL)
	{
		return UCAN_ERROR_UNKNOWN_ID;
	}

	for(int i = 0; i < packetFound->dlc; i++) {
		*(packetFound->bits[i]) = aData[i];
	}

	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Runtime_UpdateHandshake(UCAN_NodeInfo* node, CAN_HandleTypeDef* hcan, uint32_t StdId, uint8_t aData[])
{
	if(node == NULL || hcan == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

	switch (node->role) {
		case UCAN_ROLE_MASTER:

			UCAN_Client handshakeKey = {.id = StdId};
			UCAN_Client* handshakeFound = bsearch(&handshakeKey, node->clients, node->clientCount, sizeof(UCAN_Client), uCAN_Runtime_CompareClientId);

			if(handshakeFound == NULL)
			{
				return UCAN_ERROR_UNKNOWN_ID;
			}

			if(aData[0] != UCAN_HANDSHAKE_RESPONSE_VALUE)
			{
				return UCAN_ERROR;
			}

			handshakeFound->responseTick = HAL_GetTick();
			break;
		case UCAN_ROLE_CLIENT:

			if(StdId != node->masterId)
			{
				return UCAN_ERROR_UNKNOWN_ID;
			}

			if(aData[0] != UCAN_HANDSHAKE_REQUEST_VALUE)
			{
				return UCAN_ERROR;
			}

			node->sentTick = HAL_GetTick();
			uCAN_Runtime_ReplyPing(hcan, node);
			break;

		case UCAN_ROLE_NONE:

			break;
		default:
			break;
	}

	return UCAN_OK;

}

int uCAN_Runtime_ComparePacketId(const void* a, const void* b)
{
    const UCAN_Packet* p1 = (const UCAN_Packet*)a;
    const UCAN_Packet* p2 = (const UCAN_Packet*)b;

    if (p1->id < p2->id) return -1;
    if (p1->id > p2->id) return 1;
    return 0;
}

int uCAN_Runtime_CompareClientId(const void* a, const void* b)
{
    const UCAN_Client* p1 = (const UCAN_Client*)a;
    const UCAN_Client* p2 = (const UCAN_Client*)b;

    if (p1->id < p2->id) return -1;
    if (p1->id > p2->id) return 1;
    return 0;
}
