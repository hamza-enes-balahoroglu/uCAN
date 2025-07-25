#include <stdlib.h>

#include "ucan.h"
#include "ucan_debug.h"

UCAN_StatusTypeDef uCAN_Init(UCAN_HandleTypeDef* ucan)
{
	if(ucan->hcan == NULL)
	{
		ucan->status = UCAN_INVALID_PARAM;
		return UCAN_INVALID_PARAM;
	}

	ucan->status = UCAN_OK;
	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Start(UCAN_HandleTypeDef* ucan, UCAN_Config* config)
{
	UCAN_StatusTypeDef txListCheck = uCAN_CheckPacketConfig(config->txPacketList, &ucan->txHolder);
	UCAN_StatusTypeDef rxListCheck = uCAN_CheckPacketConfig(config->rxPacketList, &ucan->rxHolder);

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

	uCAN_Finalize_Packet(config->txPacketList, &ucan->txHolder);

	uCAN_Finalize_Packet(config->rxPacketList, &ucan->rxHolder);


	//HAL_CAN_Start(ucan->hcan);

	ucan->status = UCAN_OK;
	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_SendAll(UCAN_HandleTypeDef* ucan)
{
	if(ucan->status == UCAN_NOT_INITIALIZED)
	{
		return UCAN_NOT_INITIALIZED;
	}

	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Update(UCAN_HandleTypeDef* ucan)
{
	if(ucan->status == UCAN_NOT_INITIALIZED)
	{
		return UCAN_NOT_INITIALIZED;
	}

	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Handshake(UCAN_HandleTypeDef* ucan)
{
	if(ucan->status == UCAN_NOT_INITIALIZED)
	{
		return UCAN_NOT_INITIALIZED;
	}

	return UCAN_OK;
}
