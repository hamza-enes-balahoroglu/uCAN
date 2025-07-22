#include "ycan.h"
#include <stdlib.h>

ycan_packet_holder_t _txMessage;

ycan_packet_holder_t _rxMessage;


YCAN_StatusTypeDef isInitOk = YCAN_NOT_INITIALIZED;

static uint8_t Calculate_DLC(ycan_packet_init_t* pkt) {
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
            default:
            	break;
        }
    }
    return dlc;
}

static YCAN_StatusTypeDef yCAN_Check_Init_Values(ycan_packet_init_t *packetList, uint32_t packetCount)
{
    if (packetList == NULL || packetCount == 0 || packetCount > YCAN_MAX_PACKET_COUNT) {
        return YCAN_INVALID_PARAM;
    }

	for(int i=0; i < packetCount; i++){
		ycan_packet_init_t *pkt = &packetList[i];

		if(pkt == NULL)
		{
			return YCAN_INVALID_PARAM;
		}

		uint8_t dlc = Calculate_DLC(pkt);
		if(dlc > 8 || dlc == 0)
		{
			return YCAN_MISSING_VAL;
		}
	}
	return YCAN_OK;
}


static ycan_packet_t* yCAN_Finalize_Packet(ycan_packet_init_t* init_pkt, uint32_t count)
{
	ycan_packet_t* packets = malloc(count * sizeof(ycan_packet_t));
	if (!packets) return NULL;

	for (uint32_t i = 0; i < count; ++i) {

		uint8_t j = 0;
		uint8_t byte_idx = 0;

		packets[i].id = init_pkt[i].id;
		packets[i].dlc = Calculate_DLC(init_pkt);


		while (j < init_pkt[i].item_count) {
			void* data_ptr = init_pkt[i].items[j].ptr;
			switch (init_pkt[i].items[j].type)
			{
				case YCAN_U8:
					packets[i].bits[byte_idx++] = (uint8_t*)data_ptr;
					break;

				case YCAN_U16:
					packets[i].bits[byte_idx++] = &((uint8_t*)data_ptr)[0];
					packets[i].bits[byte_idx++] = &((uint8_t*)data_ptr)[1];
					break;

				case YCAN_U32:
					packets[i].bits[byte_idx++] = &((uint8_t*)data_ptr)[0];
					packets[i].bits[byte_idx++] = &((uint8_t*)data_ptr)[1];
					packets[i].bits[byte_idx++] = &((uint8_t*)data_ptr)[2];
					packets[i].bits[byte_idx++] = &((uint8_t*)data_ptr)[3];
					break;
				default:
					break;
			}
			j++; // sıradaki item'a geçiyoruz
		}

	}


	return packets;

}


YCAN_StatusTypeDef yCAN_Init(ycan_packet_init_t* txInitPacketList, uint32_t txPacketCount,
		ycan_packet_init_t* rxInitPacketList, uint32_t rxPacketCount)
{
	YCAN_StatusTypeDef txListCheck = yCAN_Check_Init_Values(txInitPacketList, txPacketCount);
	YCAN_StatusTypeDef rxListCheck = yCAN_Check_Init_Values(rxInitPacketList, rxPacketCount);

	if (txListCheck != YCAN_OK)
	{
		return txListCheck;
	}

	if (rxListCheck != YCAN_OK)
	{
		return rxListCheck;
	}

	_txMessage.packets = yCAN_Finalize_Packet(txInitPacketList, txPacketCount);
	_txMessage.count = txPacketCount;

	_rxMessage.packets = yCAN_Finalize_Packet(rxInitPacketList, rxPacketCount);
	_rxMessage.count = rxPacketCount;

//	free(&txInitPacketList);
//	free(&rxInitPacketList);

	isInitOk = YCAN_OK;
	return YCAN_OK;
}

YCAN_StatusTypeDef yCAN_SendAll(void)
{
	if(isInitOk == YCAN_NOT_INITIALIZED)
	{
		return YCAN_NOT_INITIALIZED;
	}


	return YCAN_OK;
}

YCAN_StatusTypeDef yCAN_Update(void)
{
	if(isInitOk == YCAN_NOT_INITIALIZED)
	{
		return YCAN_NOT_INITIALIZED;
	}

	return YCAN_OK;
}
