/**
  ******************************************************************************
  * @file    ucan_debug.c
  * @author  Hamza Enes Balahoroğlu
  * @brief   Debug and validation utilities for the uCAN communication module.
  *
  * This file contains internal functions that verify configuration consistency,
  * detect logical errors, and validate runtime parameters in the uCAN module.
  *
  * Core functionalities include:
  * - Checking for null pointers and invalid parameters.
  * - Ensuring that client IDs and packet IDs are unique.
  * - Validating packet item types and DLC calculations.
  * - Asserting the integrity of UCAN_HandleTypeDef before operation.
  *
  * These checks are designed to be used during development, testing, or
  * system bring-up phases. They help catch misconfigurations or API misuse
  * early in the application lifecycle.
  *
  * @note    These functions are not optimized for runtime efficiency and
  *          should be disabled or removed in production builds if not required.
  *
  *
  *	  						 _____          _   _
  *	  						/ ____|   /\   | \ | |
  *	  				  _   _| |       /  \  |  \| |
  *	  				 | | | | |      / /\ \ | . ` |
  *	  				 | |_| | |____ / ____ \| |\  |
  *	  				  \__,_|\_____/_/    \_\_| \_|
  *
  ******************************************************************************
  */


#include "ucan_debug.h"

/**
  * @brief  Calculates the total Data Length Code (DLC) of a CAN packet.
  * @note   Loops through each item in the packet and adds its byte size
  *         to compute total data size for the CAN frame.
  *         - UCAN_U8   → 1 byte
  *         - UCAN_U16  → 2 bytes
  *         - UCAN_U32  → 4 bytes
  *
  * @param  pkt: Pointer to a UCAN_PacketConfig structure.
  * @retval uint8_t: Computed DLC value (total number of bytes).
  */
uint8_t uCAN_Debug_Calculate_DLC(UCAN_PacketConfig* pkt)
{
    uint8_t dlc = 0;

    // iterate through each data item in the packet
    for (int i = 0; i < pkt->item_count; i++) {
        switch (pkt->items[i].type) {
            case UCAN_U8:
                dlc += 1;  // 8-bit data → 1 byte
                break;

            case UCAN_U16:
                dlc += 2;  // 16-bit data → 2 bytes
                break;

            case UCAN_U32:
                dlc += 4;  // 32-bit data → 4 bytes
                break;

            default:
                break;  // if type is invalid or unknown, skip it
        }
    }

    return dlc;
}

/**
  * @brief  Validates the integrity of a given packet configuration list.
  * @note   This function checks if the config list is non-null and has valid entries.
  *         For each packet:
  *           - Ensures pointer is valid
  *           - Validates item types via uCAN_Debug_CheckIsDataType()
  *           - Calculates and verifies DLC is within valid CAN frame size (1 to 8 bytes)
  *
  * @param  configList: Pointer to an array of UCAN_PacketConfig structures.
  * @param  packetHolder: Pointer to a UCAN_PacketHolder which includes the packet count.
  * @retval UCAN_OK: All configurations are valid
  * @retval UCAN_INVALID_PARAM: NULL pointer or invalid packet pointer
  * @retval UCAN_MISSING_VAL: DLC is 0 or exceeds 8 bytes
  *
  * @warning Item types in each packet must be correctly set before calling this function.
  *          Invalid or unsupported types may not be caught directly here.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckPacketConfig(UCAN_PacketConfig* configList, UCAN_PacketHolder* packetHolder)
{
	// Null pointer check to prevent invalid memory access
    if (configList == NULL || packetHolder->count == 0) {
        return UCAN_INVALID_PARAM;
    }

    // iterate through all packets in the holder
	for(int i=0; i < packetHolder->count; i++){
		UCAN_PacketConfig *pkt = &configList[i];

		// validate packet pointer (technically redundant if array is valid)
		if(pkt == NULL)
		{
			return UCAN_INVALID_PARAM;
		}

		// verify each item inside the packet has a valid data type
		uCAN_Debug_CheckIsDataType(pkt);

		// calculate total DLC for current packet
		uint8_t dlc = uCAN_Debug_Calculate_DLC(pkt);

		// validate DLC range: must be between 1 and 8 for standard CAN
		if(dlc > 8 || dlc == 0)
		{
			return UCAN_MISSING_VAL;
		}
	}

	// All checks passed successfully
	return UCAN_OK;
}

/**
 * @brief  Finalizes the internal packet representation by assigning pointer addresses
 *         for each byte of data according to its type (U8, U16, U32).
 *
 * @note   This function is mainly used for debugging or simulation environments
 *         where actual byte-wise pointer mapping is required without real CAN transmission.
 *         It iterates through all configured packets, calculates their DLC,
 *         and fills the bits[] array with appropriate byte pointers.
 *
 * @param  configPackets Pointer to array of UCAN_PacketConfig structs.
 * @param  packetHolder  Pointer to UCAN_PacketHolder struct that will be populated.
 *
 * @retval UCAN_StatusTypeDef Returns UCAN_OK on success.
 *
 * @warning This function assumes both configPackets and packetHolder are properly initialized.
 *          No runtime safety checks (e.g. NULL pointers or overflow guards) are performed.
 */
UCAN_StatusTypeDef uCAN_Debug_FinalizePacket(UCAN_PacketConfig* configPackets, UCAN_PacketHolder* packetHolder)
{
	// Null pointer check to prevent invalid memory access
	if(configPackets == NULL || packetHolder == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

    UCAN_Packet* packets = packetHolder->packets;

    // loop through each packet in the holder
    for (uint32_t i = 0; i < packetHolder->count; ++i) {

        uint8_t j = 0;
        uint8_t byte_idx = 0;

        // set packet ID and calculate DLC
        packets[i].id = configPackets[i].id;
        packets[i].dlc = uCAN_Debug_Calculate_DLC(configPackets);

        // go through each data item in this config
        while (j < configPackets[i].item_count) {

            void* data_ptr = configPackets[i].items[j].ptr;

            // map each data type into individual byte pointers
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

    // All checks passed successfully
    return UCAN_OK;
}


/**
 * @brief  Validates the UCAN_NodeInfo structure for correctness.
 * @param  node Pointer to the UCAN_NodeInfo to validate.
 * @retval UCAN_OK if validation passes.
 * @retval UCAN_INVALID_PARAM if node or clientIdList pointer is NULL.
 * @retval UCAN_DUPLICATE_ID if duplicate client IDs are detected.
 *
 * @note   Checks that the node role is valid via assert_param.
 *         Also scans clientIdList for duplicate IDs to prevent address conflicts on the CAN bus.
 */
UCAN_StatusTypeDef uCAN_Debug_CheckNodeInfo(UCAN_NodeInfo* node)
{
	// Null pointer check to prevent invalid memory access
    if (node == NULL || node->clientIdList == NULL) {
        return UCAN_INVALID_PARAM;
    }

    // Validate that the node's role is one of the allowed enumerations
    assert_param(IS_UCAN_NODE_ROLE(node->role));

    // Nested loop to check for duplicate client IDs
    for (uint32_t outerIndex = 0; outerIndex < node->clientCount; outerIndex++)
    {
        for (uint32_t innerIndex = outerIndex + 1; innerIndex < node->clientCount; innerIndex++)
        {
            // If duplicate ID found, return error code
            if (node->clientIdList[outerIndex] == node->clientIdList[innerIndex])
            {
                return UCAN_DUPLICATE_ID;
            }
        }
    }

    // All checks passed successfully
    return UCAN_OK;
}


/**
 * @brief  Validates that each item in the packet has a valid data type.
 * @param  pkt Pointer to the UCAN_PacketConfig to check.
 * @retval UCAN_OK if all data types are valid.
 * @retval UCAN_INVALID_PARAM if pkt is NULL.
 *
 * @note   Uses assert_param to ensure each item’s type is one of the allowed UCAN_DataType enums.
 */
UCAN_StatusTypeDef uCAN_Debug_CheckIsDataType(UCAN_PacketConfig* pkt)
{
	// Check for null pointer to avoid dereferencing invalid memory
    if (pkt == NULL) {
        return UCAN_INVALID_PARAM;
    }

    // Iterate over all items and validate their data types
    for (uint32_t i = 0; i < pkt->item_count; i++)
    {
        // Assert that item type is within valid UCAN_DataType enum
        assert_param(IS_UCAN_DATA_TYPE(pkt->items[i].type));
    }

    // All checks passed successfully
    return UCAN_OK;
}


/**
  * @brief  Checks whether all packet IDs in the TX and RX holders are unique across both lists.
  * @note   This function iterates over both TX and RX packet holders and verifies that
  *         no packet ID is duplicated. If any duplicate is found, the corresponding status
  *         is returned and saved in the main UCAN handle.
  * @param  ucan: Pointer to the UCAN handle structure.
  * @retval UCAN_OK if all packet IDs are unique,
  *         error status otherwise.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckUniquePackets(UCAN_HandleTypeDef* ucan)
{
    // Check for null pointer to avoid dereferencing invalid memory
	if (ucan == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

    // Iterate through all TX packets and check if their IDs are unique
	for (uint32_t i = 0; i < ucan->txHolder.count; i++)
	{
		UCAN_StatusTypeDef status = uCAN_Debug_CheckUniqueID(
			ucan->txHolder.packets[i].id,
			&ucan->txHolder,
			&ucan->rxHolder
		);

        // If a duplicate is found, save the error status and return it
		if (status != UCAN_OK)
		{
			ucan->status = status;
			return status;
		}
	}

    // Iterate through all RX packets and perform the same check
	for (uint32_t i = 0; i < ucan->rxHolder.count; i++)
	{
		UCAN_StatusTypeDef status = uCAN_Debug_CheckUniqueID(
			ucan->rxHolder.packets[i].id,
			&ucan->txHolder,
			&ucan->rxHolder
		);

        // Again, if any duplicate found, update handle status and return
		if (status != UCAN_OK)
		{
			ucan->status = status;
			return status;
		}
	}

    // All checks passed, IDs are unique
	return UCAN_OK;
}


/**
  * @brief  Checks if the given packet ID exists more than once across TX and RX packet holders.
  * @note   Counts how many times the specified ID appears in both TX and RX lists combined.
  *         If it occurs more than once, it's considered a duplicate.
  * @param  id: The packet ID to check.
  * @param  txHolder: Pointer to the TX packet holder structure.
  * @param  rxHolder: Pointer to the RX packet holder structure.
  * @retval UCAN_OK if the ID is unique,
  *         UCAN_DUPLICATE_ID if the ID appears more than once.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckUniqueID(uint32_t id, UCAN_PacketHolder* txHolder, UCAN_PacketHolder* rxHolder)
{
	uint32_t idCounter = 0;

    // Count occurrences in the TX list
    for (uint32_t i = 0; i < txHolder->count; i++)
    {
        if (txHolder->packets[i].id == id)
        {
        	idCounter++;
        }
    }

    // Count occurrences in the RX list
    for (uint32_t i = 0; i < rxHolder->count; i++)
    {
        if (rxHolder->packets[i].id == id)
        {
        	idCounter++;
        }
    }

    // If ID appears more than once, return duplicate error
    if (idCounter > 1) {
    	return UCAN_DUPLICATE_ID;
    }

    // Otherwise, the ID is unique
    return UCAN_OK;
}
