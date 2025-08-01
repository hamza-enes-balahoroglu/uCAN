#include <stdlib.h>

#include "ucan.h"
#include "ucan_debug.h"
#include "ucan_runtime.h"

static const CAN_FilterTypeDef defaultFilterConfig = {
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
	if(ucan == NULL || ucan->hcan == NULL || ucan->node.clients == NULL)
	{
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
	UCAN_CHECK_READY(ucan);


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

	if(uCAN_Debug_CheckUniquePackets(ucan) != UCAN_OK)
	{
		ucan->status = UCAN_ERROR_DUPLICATE_ID;
		return UCAN_ERROR_DUPLICATE_ID;
	}


	if (HAL_CAN_ConfigFilter(ucan->hcan, &ucan->filter) != HAL_OK)
	{
		ucan->status = UCAN_ERROR_FILTER_CONFIG;
		return UCAN_ERROR_FILTER_CONFIG;
	}

	if (HAL_CAN_Start(ucan->hcan) != HAL_OK)
	{
		ucan->status = UCAN_ERROR_CAN_START;
		return UCAN_ERROR_CAN_START;
	}

	if (HAL_CAN_ActivateNotification(ucan->hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		ucan->status = UCAN_ERROR_CAN_NOTIFICATION;
		return UCAN_ERROR_CAN_NOTIFICATION;
	}


	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_SendAll(UCAN_HandleTypeDef* ucan)
{
	UCAN_CHECK_READY(ucan);

	for(uint32_t i = 0; i< ucan->txHolder.count;i++)
	{
		if(uCAN_Runtime_SendPacket(ucan->hcan, &ucan->txHolder.packets[i]) != UCAN_OK)
		{
			return UCAN_ERROR;
		}
	}

	uCAN_Runtime_SendPing(ucan->hcan, &ucan->node);

	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Update(UCAN_HandleTypeDef* ucan)
{
	UCAN_CHECK_READY(ucan);

	CAN_RxHeaderTypeDef rxHeader;
	uint8_t data[8];

	if(HAL_CAN_GetRxMessage(ucan->hcan, CAN_RX_FIFO0, &rxHeader, data) != HAL_OK)
	{
		return UCAN_ERROR;
	}

	UCAN_StatusTypeDef packetStatus = uCAN_Runtime_UpdatePacket(&ucan->rxHolder, rxHeader.StdId, data);

	// eğer bilinmeyen paketse, belki handshake'tir diyip oraya gönderelim
	if (packetStatus == UCAN_ERROR_UNKNOWN_ID)
	{
	    UCAN_StatusTypeDef handshakeStatus = uCAN_Runtime_UpdateHandshake(&ucan->node, ucan->hcan, rxHeader.StdId, data);

	    if (handshakeStatus != UCAN_OK)
	    {
	        return handshakeStatus; // handshake de çözemedi, sıkıntı var
	    }
	}
	else if (packetStatus != UCAN_OK)
	{
	    return packetStatus; // bilinen paketti ama hata çıktı
	}

	// eğer buraya kadar geldiyse her şey yolunda
	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Handshake(UCAN_HandleTypeDef* ucan)
{
	UCAN_CHECK_READY(ucan);

	UCAN_StatusTypeDef connectionErrorFlag = UCAN_OK;

	for(uint32_t i = 0; i < ucan->node.clientCount; i++)
	{
	    if (ucan->node.clients[i].responseTick == 0)
	    {
	    	connectionErrorFlag = UCAN_ERROR;
	        continue;
	    }

	    UCAN_ConnectionStatusTypeDef status;

	    if (UCAN_HANDSHAKE_IS_TIMEOUT(ucan->node.sentTick, ucan->node.clients[i].responseTick))
	    {
	        status = UCAN_CONN_TIMEOUT;
	        connectionErrorFlag = UCAN_ERROR;
	    }
	    else if (UCAN_HANDSHAKE_IS_LOST(ucan->node.sentTick, ucan->node.clients[i].responseTick))
	    {
	        status = UCAN_CONN_LOST;
	        connectionErrorFlag = UCAN_ERROR;
	    }
	    else
	    {
	        status = UCAN_CONN_ACTIVE;
	    }

	    ucan->node.clients[i].status = status;
	}

	return connectionErrorFlag;
}
