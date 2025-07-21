#include "ycan.h"

ycan_packet_t *_txPacketList;

ycan_packet_t *_rxPacketList;

uint32_t _txPacketCount;

uint32_t _rxPacketCount;

YCAN_StatusTypeDef isInitOk = YCAN_NOT_INITIALIZED;

static uint8_t Calculate_DLC(ycan_packet_t* pkt) {
    uint8_t dlc = 0;
    for (int i=0; i < pkt->item_count; i++) {
        switch(pkt->items[i].type) {
            case YCAN_U8:
                dlc += 1;
                break;
            case YCAN_U16:
                dlc += 2;
                break;
            case YCAN_U32:
                dlc += 4;
                break;
        }
    }
    return dlc;
}

static YCAN_StatusTypeDef yCAN_Check_Init_Values(ycan_packet_t *packetList, uint32_t packetCount)
{
    if (packetList == NULL || packetCount == 0 || packetCount > YCAN_MAX_PACKET_COUNT) {
        return YCAN_INVALID_PARAM;
    }

	for(int i=0; i < packetCount; i++){
		ycan_packet_t *pkt = &packetList[i];

		if(pkt == NULL)
		{
			return YCAN_INVALID_PARAM;
		}

		uint8_t dlc = Calculate_DLC(pkt);
		if(dlc > 8 || dlc == 0)
		{
			return YCAN_MISSING_VAL;
		}
		else
		{
			pkt->dlc = dlc;
		}
	}

	if (packetList[packetCount] != NULL)
	{
		return YCAN_INVALID_PARAM;
	}
	return YCAN_OK;
}


YCAN_StatusTypeDef yCAN_Init(ycan_packet_t *txPacketList, uint32_t txPacketCount,
		ycan_packet_t *rxPacketList, uint32_t rxPacketCount)
{
	YCAN_StatusTypeDef txListCheck = yCAN_Check_Init_Values(txPacketList, txPacketCount);
	YCAN_StatusTypeDef rxListCheck = yCAN_Check_Init_Values(rxPacketList, rxPacketCount);

	if (txListCheck != YCAN_OK)
	{
		return txListCheck;
	}

	if (rxListCheck != YCAN_OK)
	{
		return rxListCheck;
	}

	_txPacketList = txPacketList;
	_txPacketCount = txPacketCount;

	_rxPacketList = rxPacketList;
	_rxPacketCount = rxPacketCount;

	isInitOk = YCAN_OK;
	return YCAN_OK;
}

YCAN_StatusTypeDef yCAN_SendAll(void)
{
	if(isInitOk == YCAN_NOT_INITIALIZED)
	{
		return YCAN_NOT_INITIALIZED;
	}

}

YCAN_StatusTypeDef yCAN_Update(void)
{
	if(isInitOk == YCAN_NOT_INITIALIZED)
	{
		return YCAN_NOT_INITIALIZED;
	}
}
