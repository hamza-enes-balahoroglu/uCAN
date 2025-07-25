#include <stdlib.h>

#include "ucan.h"

UCAN_PacketHolder _txMessage;

UCAN_PacketHolder _rxMessage;


UCAN_StatusTypeDef isInitOk = UCAN_NOT_INITIALIZED;

static uint8_t Calculate_DLC(UCAN_PacketInit* pkt);
static UCAN_StatusTypeDef uCAN_Check_Init_Values(UCAN_PacketInit *packetList, uint32_t packetCount);
static UCAN_Packet* uCAN_Finalize_Packet(UCAN_PacketInit* init_pkt, uint32_t count);


UCAN_StatusTypeDef uCAN_Init(UCAN_PacketInit* txInitPacketList, uint32_t txPacketCount,
		UCAN_PacketInit* rxInitPacketList, uint32_t rxPacketCount)
{
	UCAN_StatusTypeDef txListCheck = uCAN_Check_Init_Values(txInitPacketList, txPacketCount);
	UCAN_StatusTypeDef rxListCheck = uCAN_Check_Init_Values(rxInitPacketList, rxPacketCount);

	if (txListCheck != UCAN_OK)
	{
		isInitOk = txListCheck;
		return txListCheck;
	}

	if (rxListCheck != UCAN_OK)
	{
		isInitOk = txListCheck;
		return rxListCheck;
	}

	_txMessage.packets = uCAN_Finalize_Packet(txInitPacketList, txPacketCount);
	_txMessage.count = txPacketCount;

	_rxMessage.packets = uCAN_Finalize_Packet(rxInitPacketList, rxPacketCount);
	_rxMessage.count = rxPacketCount;

	txInitPacketList[0] = (UCAN_PacketInit){0};
	rxInitPacketList[0] = (UCAN_PacketInit){0};

	isInitOk = UCAN_OK;
	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_SendAll(void)
{
	if(isInitOk != UCAN_OK)
	{
		return isInitOk;
	}


	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Update(void)
{
	if(isInitOk != UCAN_OK)
	{
		return isInitOk;
	}

	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Handshake(void)
{
	if(isInitOk != UCAN_OK)
	{
		return isInitOk;
	}

	return UCAN_OK;
}

static uint8_t Calculate_DLC(UCAN_PacketInit* pkt)
{
    uint8_t dlc = 0;
    for (int i=0; i < pkt->item_count; i++) {
        switch(pkt->items[i].type) {
            case UCAN_U8:
                dlc += 1;
                break;
            case UCAN_U16:
                dlc += 2;
                break;
            case UCAN_U32:
                dlc += 4;
                break;
            default:
            	break;
        }
    }
    return dlc;
}

static UCAN_StatusTypeDef uCAN_Check_Init_Values(UCAN_PacketInit *packetList, uint32_t packetCount)
{
    if (packetList == NULL || packetCount == 0 || packetCount > UCAN_MAX_PACKET_COUNT) {
        return UCAN_INVALID_PARAM;
    }

	for(int i=0; i < packetCount; i++){
		UCAN_PacketInit *pkt = &packetList[i];

		if(pkt == NULL)
		{
			return UCAN_INVALID_PARAM;
		}

		uint8_t dlc = Calculate_DLC(pkt);
		if(dlc > 8 || dlc == 0)
		{
			return UCAN_MISSING_VAL;
		}
	}
	return UCAN_OK;
}


static UCAN_Packet* uCAN_Finalize_Packet(UCAN_PacketInit* init_pkt, uint32_t count)
{
	UCAN_Packet* packets = malloc(count * sizeof(UCAN_Packet));
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
				case UCAN_U8:
					packets[i].bits[byte_idx++] = (uint8_t*)data_ptr;
					break;

				case UCAN_U16:
					packets[i].bits[byte_idx++] = &((uint8_t*)data_ptr)[0];
					packets[i].bits[byte_idx++] = &((uint8_t*)data_ptr)[1];
					break;

				case UCAN_U32:
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
