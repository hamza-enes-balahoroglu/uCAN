/**
  ******************************************************************************
  * @file    ucan_debug.c
  * @author  Hamza Enes Balahoroğlu
  * @brief   [INTERNAL] Debug and validation utilities for the UCAN communication module.
  *
  * This file holds core internal functions responsible for validating
  * UCAN configurations, detecting logical errors, and verifying runtime parameters.
  *
  * Main features:
  * - Null pointer and invalid parameter checks to prevent crashes.
  * - Duplicate ID detection for clients and packets to avoid CAN bus conflicts.
  * - Packet item type validation and Data Length Code (DLC) computation.
  * - Structural integrity checks of UCAN_HandleTypeDef prior to operation.
  *
  * These functions are intended for development and testing phases to catch
  * misconfigurations and misuse early. Not optimized for runtime performance.
  * It’s recommended to disable or strip these in production builds.
  *
  * @note    All functions here are marked as [INTERNAL] and not exposed publicly.
  *
  *
  *                          _____          _   _
  *                         / ____|   /\   | \ | |
  *                   _   _| |       /  \  |  \| |
  *                  | | | | |      / /\ \ | . ` |
  *                  | |_| | |____ / ____ \| |\  |
  *                   \____|\_____/_/    \_\_| \_|
  *
  ******************************************************************************
  */

#include <stdlib.h>
#include "ucan_runtime.h"
#include "ucan_debug.h"

/**
  * @brief [INTERNAL] Calculate total Data Length Code (DLC) for a CAN packet config.
  *
  * This function iterates through all data items in the provided UCAN_PacketConfig,
  * summing the byte size of each item according to its type. The total DLC represents
  * the complete payload size of the CAN frame.
  *
  * The function supports these types with corresponding byte sizes:
  * - UCAN_U8  : 1 byte
  * - UCAN_U16 : 2 bytes
  * - UCAN_U32 : 4 bytes
  *
  * If an unknown or unsupported type is encountered, it is skipped silently.
  *
  * @param pkt Pointer to the UCAN_PacketConfig structure to calculate DLC for.
  * @retval uint8_t Total DLC value (number of bytes).
  */
uint8_t uCAN_Debug_Calculate_DLC(UCAN_PacketConfig* pkt)
{
    uint8_t dlc = 0;

    // loop through all items to accumulate byte sizes
    for (int i = 0; i < pkt->item_count; i++) {
        switch (pkt->items[i].type) {
            case UCAN_U8:
                dlc += 1;  // add 1 byte for 8-bit data
                break;
            case UCAN_U16:
                dlc += 2;  // add 2 bytes for 16-bit data
                break;
            case UCAN_U32:
                dlc += 4;  // add 4 bytes for 32-bit data
                break;
            default:
                // unknown type, ignore safely
                break;
        }
    }

    return dlc;
}

/**
  * @brief  [INTERNAL] Validates the integrity of a given packet configuration list.
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
  * @brief  [INTERNAL] Converts high-level packet configuration into finalized UCAN_Packet format.
  *
  * @note   Mainly used in debug or simulation contexts where byte-level pointer mapping
  *         is needed without actual CAN hardware interaction. This function traverses all
  *         configured packets, calculates their DLC, and fills the bits[] array with
  *         byte pointers pointing to the actual data.
  *
  *         Additionally, it sorts the finalized packets by their CAN IDs to ensure
  *         consistent packet order during runtime operations.
  *
  * @param  configPackets Pointer to array of UCAN_PacketConfig structures.
  * @param  packetHolder  Pointer to a UCAN_PacketHolder that will be populated with finalized packets.
  *
  * @retval UCAN_StatusTypeDef Returns UCAN_OK if the operation is successful, UCAN_INVALID_PARAM if input is NULL.
  *
  * @warning This function assumes packetHolder->count is already set and matches configPackets.
  *          No boundary or overflow checks are performed beyond basic NULL checks.
  */
UCAN_StatusTypeDef uCAN_Debug_FinalizePacket(UCAN_PacketConfig* configPackets, UCAN_PacketHolder* packetHolder)
{
	// Null pointer check to prevent invalid memory access
	if(configPackets == NULL || packetHolder == NULL)
	{
		return UCAN_INVALID_PARAM;
	}

    UCAN_Packet *packets = packetHolder->packets;

    // loop through each packet in the holder
    for (uint32_t i = 0; i < packetHolder->count; ++i) {

        uint8_t j = 0;
        uint8_t byte_idx = 0;

        // set packet ID and calculate DLC
        packets[i].id = configPackets[i].id;
        packets[i].dlc = uCAN_Debug_Calculate_DLC(&configPackets[i]);

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


    qsort(packetHolder->packets, packetHolder->count, sizeof(UCAN_Packet), uCAN_Runtime_ComparePacketId);


    // All checks passed successfully
    return UCAN_OK;
}

/**
  * @brief [INTERNAL] Validates the UCAN_NodeInfo structure integrity and correctness.
  *
  * Performs sanity checks on the provided UCAN_NodeInfo pointer, including:
  * - Null pointer validation to avoid invalid memory access.
  * - Role verification via assert_param to ensure node role is within allowed range.
  * - Duplicate client ID detection by scanning the client array to prevent
  *   CAN bus address conflicts and communication errors.
  *
  * This validation helps catch configuration errors early and maintains
  * network consistency within the UCAN protocol.
  *
  * @param node Pointer to the UCAN_NodeInfo instance to validate.
  *
  * @retval UCAN_OK              Validation succeeded, node info is consistent.
  * @retval UCAN_INVALID_PARAM   Node pointer or clients list pointer is NULL.
  * @retval UCAN_ERROR_DUPLICATE_ID Duplicate client IDs found in client list.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckNodeInfo(UCAN_NodeInfo* node)
{
	// Null pointer check to prevent invalid memory access
    if (node == NULL || node->clients == NULL) {
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
            if (node->clients[outerIndex].id == node->clients[innerIndex].id)
            {
                return UCAN_ERROR_DUPLICATE_ID;
            }
        }
    }

    // All checks passed successfully
    return UCAN_OK;
}

/**
  * @brief [INTERNAL] Finalizes the UCAN_NodeInfo client list by sorting clients by ID.
  *
  * This function validates the input pointer and sorts the array of UCAN_Client
  * structures based on their client IDs using the standard qsort function.
  * Sorting the client list improves lookup efficiency and guarantees a consistent
  * order for operations like searching and handshake management.
  *
  * @param node Pointer to the UCAN_NodeInfo containing the client array to sort.
  *
  * @retval UCAN_OK               Sorting completed without errors.
  * @retval UCAN_INVALID_PARAM    Null pointer provided as input.
  */
UCAN_StatusTypeDef uCAN_Debug_FinalizeNodeInfo(UCAN_NodeInfo* node)
{
	// Null pointer check to prevent invalid memory access
    if (node == NULL || node->clients == NULL) {
        return UCAN_INVALID_PARAM;
    }

    // Sort clients array by client ID for deterministic behavior
    qsort(node->clients, node->clientCount, sizeof(UCAN_Client), uCAN_Runtime_CompareClientId);

    return UCAN_OK;
}

/**
  * @brief  [INTERNAL] Validates that each item in the packet has a valid data type.
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
  * @brief  [INTERNAL] Checks whether all packet IDs in the TX and RX holders are unique across both lists.
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
  * @brief  [INTERNAL] Checks if the given packet ID exists more than once across TX and RX packet holders.
  * @note   Counts how many times the specified ID appears in both TX and RX lists combined.
  *         If it occurs more than once, it's considered a duplicate.
  * @param  id: The packet ID to check.
  * @param  txHolder: Pointer to the TX packet holder structure.
  * @param  rxHolder: Pointer to the RX packet holder structure.
  * @retval UCAN_OK if the ID is unique,
  *         UCAN_ERROR_DUPLICATE_ID if the ID appears more than once.
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
    	return UCAN_ERROR_DUPLICATE_ID;
    }

    // Otherwise, the ID is unique
    return UCAN_OK;
}
