#include <stdlib.h>

#include "ucan.h"
#include "ucan_debug.h"

const CAN_FilterTypeDef defaultFilterConfig = {
    .FilterMode = CAN_FILTERMODE_IDMASK,
    .FilterFIFOAssignment = CAN_FILTER_FIFO0,
    .FilterIdHigh = 0x0000,
    .FilterIdLow = 0x0000,
    .FilterMaskIdHigh = 0x0000,
    .FilterMaskIdLow = 0x0000,
    .FilterScale = CAN_FILTERSCALE_32BIT,
    .FilterActivation = CAN_FILTER_ENABLE
};

UCAN_StatusTypeDef uCAN_Init(UCAN_HandleTypeDef* ucan)
{
	if(ucan->hcan == NULL)
	{
		ucan->status = UCAN_INVALID_PARAM;
		return UCAN_INVALID_PARAM;
	}

	if(ucan->filter.FilterActivation == CAN_FILTER_DISABLE)
	{
		ucan->filter = defaultFilterConfig;
	}


	ucan->status = UCAN_OK;
	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Start(UCAN_HandleTypeDef* ucan, UCAN_Config* config)
{
	if(ucan->status == UCAN_NOT_INITIALIZED)
	{
		return UCAN_NOT_INITIALIZED;
	}

	UCAN_StatusTypeDef txListCheck = uCAN_Debug_CheckPacketConfig(config->txPacketList, &ucan->txHolder);
	UCAN_StatusTypeDef rxListCheck = uCAN_Debug_CheckPacketConfig(config->rxPacketList, &ucan->rxHolder);

	if (txListCheck != UCAN_OK)
	{
		ucan->status = txListCheck;
		return txListCheck;
	}

	if (rxListCheck != UCAN_OK)
	{
		ucan->status = rxListCheck;
		return rxListCheck;
	}

	uCAN_Debug_FinalizePacket(config->txPacketList, &ucan->txHolder);

	uCAN_Debug_FinalizePacket(config->rxPacketList, &ucan->rxHolder);




	ucan->status = UCAN_OK;
	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_SendAll(UCAN_HandleTypeDef* ucan)
{
	if(ucan->status == UCAN_NOT_INITIALIZED)
	{
		return UCAN_NOT_INITIALIZED;
	}

	//TODO

	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Update(UCAN_HandleTypeDef* ucan)
{
	if(ucan->status == UCAN_NOT_INITIALIZED)
	{
		return UCAN_NOT_INITIALIZED;
	}


	//TODO

	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Handshake(UCAN_HandleTypeDef* ucan)
{
	if(ucan->status == UCAN_NOT_INITIALIZED)
	{
		return UCAN_NOT_INITIALIZED;
	}

	//TODO

	return UCAN_OK;
}
