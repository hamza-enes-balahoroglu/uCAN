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

UCAN_StatusTypeDef uCAN_Runtime_UpdatePacket(UCAN_HandleTypeDef* ucan)
{
	if(ucan == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

	CAN_RxHeaderTypeDef rxHeader;
	uint8_t data[8];

	if(HAL_CAN_GetRxMessage(ucan->hcan, CAN_RX_FIFO0, &rxHeader, data) != HAL_OK)
	{
		return UCAN_ERROR;
	}

	UCAN_Packet key = {.id = rxHeader.StdId};
	UCAN_Packet* found = bsearch(&key, ucan->rxHolder.packets, ucan->rxHolder.count, sizeof(UCAN_Packet), uCAN_Runtime_ComparePacketId);

	if(found == NULL)
	{
		return UCAN_ERROR_UNKNOWN_ID;
	}

	for(int i = 0; i < found->dlc; i++) {
		*(found->bits[i]) = data[i];
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
