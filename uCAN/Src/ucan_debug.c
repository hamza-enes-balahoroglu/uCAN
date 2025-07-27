#include "ucan_debug.h"

uint8_t uCAN_Debug_Calculate_DLC(UCAN_PacketConfig* pkt)
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

UCAN_StatusTypeDef uCAN_Debug_CheckPacketConfig(UCAN_PacketConfig* configList, UCAN_PacketHolder* packetHolder)
{
    if (configList == NULL || packetHolder->count == 0) {
        return UCAN_INVALID_PARAM;
    }

	for(int i=0; i < packetHolder->count; i++){
		UCAN_PacketConfig *pkt = &configList[i];

		if(pkt == NULL)
		{
			return UCAN_INVALID_PARAM;
		}

		uCAN_Debug_CheckIsDataType(pkt);

		uint8_t dlc = uCAN_Debug_Calculate_DLC(pkt);
		if(dlc > 8 || dlc == 0)
		{
			return UCAN_MISSING_VAL;
		}
	}
	return UCAN_OK;
}


UCAN_StatusTypeDef uCAN_Debug_FinalizePacket(UCAN_PacketConfig* configPackets, UCAN_PacketHolder* packetHolder)
{
	UCAN_Packet* packets = packetHolder->packets;

	for (uint32_t i = 0; i < packetHolder->count; ++i) {

		uint8_t j = 0;
		uint8_t byte_idx = 0;

		packets[i].id = configPackets[i].id;
		packets[i].dlc = uCAN_Debug_Calculate_DLC(configPackets);


		while (j < configPackets[i].item_count) {
			void* data_ptr = configPackets[i].items[j].ptr;
			switch (configPackets[i].items[j].type)
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
			j++;
		}
	}
	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Debug_CheckNodeInfo(UCAN_NodeInfo* node)
{
	if (node->clientIdList == NULL) {
		return UCAN_INVALID_PARAM;
	}

	assert_param(IS_UCAN_NODE_ROLE(node->role));

	return UCAN_OK;
}
UCAN_StatusTypeDef uCAN_Debug_CheckIsDataType(UCAN_PacketConfig* pkt)
{
	if (pkt == NULL) {
		return UCAN_INVALID_PARAM;
	}
    for (int i=0; i < pkt->item_count; i++) {
    	assert_param(IS_UCAN_DATA_TYPE(pkt->item[i].type));
    }
    return UCAN_OK;
}
UCAN_StatusTypeDef uCAN_Debug_CheckUniquePackets(UCAN_HandleTypeDef* ucan)
{
	if(ucan == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

	for(int i = 0; i< ucan->txHolder.count; i++)
	{
		UCAN_StatusTypeDef status = uCAN_Debug_CheckUniqueID(ucan->txHolder.packets[i].id, &ucan->txHolder, &ucan->rxHolder);
		if(status != UCAN_OK)
		{
			ucan->status = status;
			return status;
		}
	}

	for(int i = 0; i< ucan->rxHolder.count; i++)
	{
		UCAN_StatusTypeDef status = uCAN_Debug_CheckUniqueID(ucan->rxHolder.packets[i].id, &ucan->txHolder, &ucan->rxHolder);
		if (status != UCAN_OK)
		{
			ucan->status = status;
			return status;
		}
	}

	return UCAN_OK;
}

UCAN_StatusTypeDef uCAN_Debug_CheckUniqueID(uint32_t id, UCAN_PacketHolder* txHolder, UCAN_PacketHolder* rxHolder)
{
    for (uint8_t i = 0; i < txHolder->count; i++)
    {
        if (txHolder->packets[i].id == id)
        {
        	return UCAN_MISSING_VAL;
        }
    }

    for (uint8_t i = 0; i < rxHolder->count; i++)
    {
        if (rxHolder->packets[i].id == id)
        {
        	return UCAN_MISSING_VAL;
        }
    }

    return UCAN_OK;
}
