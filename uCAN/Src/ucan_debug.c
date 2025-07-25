#include "ucan_debug.h"

uint8_t Calculate_DLC(UCAN_PacketConfig* pkt)
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

void uCAN_CheckIsDataType(UCAN_PacketConfig* pkt)
{
    for (int i=0; i < pkt->item_count; i++) {
    	assert_param(IS_UCAN_DATA_TYPE(pkt->item[i].type));
    }
}

UCAN_StatusTypeDef uCAN_CheckPacketConfig(UCAN_PacketConfig* configList, UCAN_PacketHolder* packetHolder)
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

		uCAN_CheckIsDataType(pkt);

		uint8_t dlc = Calculate_DLC(pkt);
		if(dlc > 8 || dlc == 0)
		{
			return UCAN_MISSING_VAL;
		}
	}
	return UCAN_OK;
}


UCAN_Packet* uCAN_Finalize_Packet(UCAN_PacketConfig* configPackets, UCAN_PacketHolder* packetHolder)
{
	UCAN_Packet* packets = packetHolder->packets;

	if (!packets) return NULL;

	for (uint32_t i = 0; i < packetHolder->count; ++i) {

		uint8_t j = 0;
		uint8_t byte_idx = 0;

		packets[i].id = configPackets[i].id;
		packets[i].dlc = Calculate_DLC(configPackets);


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
			j++; // sıradaki item'a geçiyoruz
		}

	}


	return packets;
}

UCAN_StatusTypeDef uCAN_CheckNodeInfo(UCAN_NodeInfo* node)
{
	if (node->clientIdList == NULL) {
		return UCAN_INVALID_PARAM;
	}

	assert_param(IS_UCAN_NODE_ROLE(node->role));

	return UCAN_OK;
}
