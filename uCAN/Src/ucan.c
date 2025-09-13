/**
  ******************************************************************************
  * @file    ucan.c
  * @author  Hamza Enes Balahoroğlu
  * @brief   uCAN Protocol Core Source File
  *
  * @details This file contains the core implementation of the uCAN communication
  *          stack for STM32 microcontrollers using the HAL CAN interface.
  *
  *          It includes internal logic for:
  *            - Initialization and startup of the protocol layer
  *            - TX/RX packet management and handshake finalization
  *            - Dynamic connection tracking and client status evaluation
  *            - Runtime message handling and update routines
  *
  * @note    Before using the uCAN system, both @ref uCAN_Init() and
  *          @ref uCAN_Start() must be called in order. Initialization alone
  *          is not sufficient for operational communication.
  *
  *          Message reception should be handled in the CAN RX interrupt
  *          handler (e.g., @ref CANx_RX0_IRQHandler) by calling @ref uCAN_Update().
  *
  *          Handshake tracking (if enabled) requires calling @ref uCAN_Handshake()
  *          periodically from a main loop or timer context to keep connection
  *          statuses updated.
  *
  * @see     uCAN GitHub repository: https://github.com/hamza-enes-balahoroglu/uCAN.git
  * @link    https://github.com/hamza-enes-balahoroglu/uCAN.git uCAN GitHub Link @endlink
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
#include "ucan.h"
#include "ucan_debug.h"
#include "ucan_runtime.h"

/**
  * @brief  Default CAN filter configuration structure.
  *
  * @note   This filter uses ID mask mode with 32-bit scale,
  *         assigned to FIFO 0, and is enabled by default.
  *         All filter ID and mask fields are zero, so it
  *         accepts all CAN messages (no filtering).
  */
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

/**
  * @brief  Initialize the uCAN peripheral handle and its parameters.
  * @param  ucan Pointer to the UCAN handle structure.
  * @retval UCAN_StatusTypeDef Status of the initialization:
  *         - UCAN_OK: Initialization successful
  *         - UCAN_INVALID_PARAM: Invalid input parameters (null pointers)
  *
  * @note   If CAN filter is disabled in the handle, default filter
  *         configuration is assigned automatically.
  *         This function does not start CAN hardware; it only prepares
  *         the internal state.
  */
UCAN_StatusTypeDef uCAN_Init(UCAN_HandleTypeDef* ucan)
{
    // Validate input pointers to prevent null dereference
    if (ucan == NULL || ucan->hcan == NULL || ucan->node.clients == NULL)
    {
        return UCAN_INVALID_PARAM;
    }

    // Assign default filter config if filter is disabled
    if (ucan->filter.FilterActivation == CAN_FILTER_DISABLE)
    {
        ucan->filter = defaultFilterConfig;
    }

    // Mark status as OK, init done
    ucan->status = UCAN_OK;

    return UCAN_OK;
}

/**
  * @brief  Start the uCAN peripheral with specified configuration.
  * @param  ucan   Pointer to the initialized UCAN handle.
  * @param  config Pointer to the UCAN configuration containing TX/RX packet lists.
  * @retval UCAN_StatusTypeDef Status of the start operation:
  *         - UCAN_OK: Started successfully
  *         - UCAN_INVALID_PARAM: Handle not ready or invalid
  *         - UCAN_ERROR_DUPLICATE_ID: Duplicate packet IDs detected
  *         - UCAN_ERROR_FILTER_CONFIG: CAN filter configuration failed
  *         - UCAN_ERROR_CAN_START: CAN peripheral start failed
  *         - UCAN_ERROR_CAN_NOTIFICATION: Activation of CAN notifications failed
  *
  * @note   This function performs:
  *         - Validation of TX/RX packet configurations
  *         - Finalization of packet holders
  *         - Duplicate packet ID check
  *         - CAN filter configuration and peripheral start
  *         - Activation of RX FIFO 0 message pending interrupt
  *
  *         @b Important: Calling @ref uCAN_Init() alone is not sufficient to start
  *         the communication system. The @ref uCAN_Start() function must be called
  *         at least once after initialization to validate configuration and
  *         prepare internal packet pointers. Without it, packet transmission and
  *         reception will not function correctly.
  */
UCAN_StatusTypeDef uCAN_Start(UCAN_HandleTypeDef* ucan, UCAN_Config* config)
{
    // Check if uCAN handle is ready for start
    UCAN_CHECK_READY(ucan);

    // Validate TX packet list config
    UCAN_StatusTypeDef txListCheck = uCAN_Debug_CheckPacketConfig(config->txPacketList, &ucan->txHolder);
    // Validate RX packet list config
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

    // Finalize TX packet holder setup
    uCAN_Debug_FinalizePacket(config->txPacketList, &ucan->txHolder);
    // Finalize RX packet holder setup
    uCAN_Debug_FinalizePacket(config->rxPacketList, &ucan->rxHolder);

    // Check for duplicate packet IDs across holders
    if (uCAN_Debug_CheckUniquePackets(ucan) != UCAN_OK)
    {
        ucan->status = UCAN_ERROR_DUPLICATE_ID;
        return UCAN_ERROR_DUPLICATE_ID;
    }

    // Configure CAN hardware filter with current filter settings
    if (HAL_CAN_ConfigFilter(ucan->hcan, &ucan->filter) != HAL_OK)
    {
        ucan->status = UCAN_ERROR_FILTER_CONFIG;
        return UCAN_ERROR_FILTER_CONFIG;
    }

    // Start CAN peripheral operation
    if (HAL_CAN_Start(ucan->hcan) != HAL_OK)
    {
        ucan->status = UCAN_ERROR_CAN_START;
        return UCAN_ERROR_CAN_START;
    }

    // Activate CAN RX FIFO 0 message pending interrupt notification
    if (HAL_CAN_ActivateNotification(ucan->hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        ucan->status = UCAN_ERROR_CAN_NOTIFICATION;
        return UCAN_ERROR_CAN_NOTIFICATION;
    }

    // All init steps succeeded
    return UCAN_OK;
}

/**
  * @brief  Send all queued TX packets over the CAN bus and transmit a ping.
  * @param  ucan Pointer to the initialized UCAN handle.
  * @retval UCAN_StatusTypeDef Status of the send operation:
  *         - UCAN_OK: All packets and ping sent successfully
  *         - UCAN_ERROR: Failed to send one or more packets
  *
  * @note   This function iterates over all packets in the TX holder and sends
  *         them sequentially. Afterward, it sends a ping message to announce
  *         node presence.
  *         The function assumes the CAN peripheral is started and ready.
  */
UCAN_StatusTypeDef uCAN_SendAll(UCAN_HandleTypeDef* ucan)
{
    // Verify that the UCAN handle is ready
    UCAN_CHECK_READY(ucan);

    // Loop through all TX packets and send them
    for (uint32_t i = 0; i < ucan->txHolder.count; i++)
    {
        if (uCAN_Runtime_SendPacket(ucan->hcan, &ucan->txHolder.packets[i]) != UCAN_OK)
        {
            // Stop and return error on first failure
            return UCAN_ERROR;
        }
    }

    // Send node presence ping after all packets are sent
    uCAN_Runtime_SendPing(ucan->hcan, &ucan->node);

    return UCAN_OK;
}

/**
  * @brief  Process incoming CAN message, update RX packets or handle handshake.
  * @param  ucan Pointer to the initialized UCAN handle.
  * @retval UCAN_StatusTypeDef Status of the update operation:
  *         - UCAN_OK: Message processed successfully
  *         - UCAN_ERROR: CAN receive failure or unknown error
  *         - UCAN_ERROR_UNKNOWN_ID: Received unknown packet ID (may trigger handshake)
  *         - Other handshake related error codes if handshake update fails
  *
  * @note   This function reads one message from CAN RX FIFO0,
  *         attempts to update RX packet data, and if the packet ID
  *         is unknown, tries to process it as a handshake message.
  *
  *         It expects the CAN peripheral to be started and interrupts enabled.
  *
  *         Recommended usage: Call this function inside the CAN RX FIFO0 interrupt
  *         handler (e.g., in CANx_RX0_IRQHandler) to handle received messages
  *         immediately upon arrival.
  */
UCAN_StatusTypeDef uCAN_Update(UCAN_HandleTypeDef* ucan)
{
    // Ensure handle and CAN peripheral are ready
    UCAN_CHECK_READY(ucan);

    CAN_RxHeaderTypeDef rxHeader;
    uint8_t data[8];

    // Receive one CAN message from RX FIFO 0
    if (HAL_CAN_GetRxMessage(ucan->hcan, CAN_RX_FIFO0, &rxHeader, data) != HAL_OK)
    {
        return UCAN_ERROR;
    }

    // Update RX packet data based on received CAN ID
    UCAN_StatusTypeDef packetStatus = uCAN_Runtime_UpdatePacket(&ucan->rxHolder, rxHeader.StdId, data);

    // If packet ID unknown, try to handle as handshake message
    if (packetStatus == UCAN_ERROR_UNKNOWN_ID)
    {
        UCAN_StatusTypeDef handshakeStatus = uCAN_Runtime_UpdateHandshake(&ucan->node, ucan->hcan, rxHeader.StdId, data);

        if (handshakeStatus != UCAN_OK)
        {
            // Handshake processing failed, propagate error
            return handshakeStatus;
        }
    }
    else if (packetStatus != UCAN_OK)
    {
        // Known packet, but error occurred during update
        return packetStatus;
    }

    // Message processed successfully
    return UCAN_OK;
}

/**
  * @brief  Evaluate handshake responses from all clients and update connection status.
  * @param  ucan Pointer to the initialized UCAN handle.
  * @retval UCAN_StatusTypeDef
  *         - UCAN_OK: All clients responded within valid time
  *         - UCAN_ERROR: One or more clients failed handshake or timed out
  *
  * @note   Iterates through all clients in the node. For each client:
  *         - Checks if responseTick is zero (no response)
  *         - Compares sentTick and responseTick to determine timeout or lost status
  *         - Updates client's connection status accordingly
  *         - Accumulates error flag if any client is not active
  *
  *         If handshake mechanism is enabled, this function must be called
  *         periodically (e.g., inside a main loop or timer callback).
  *         Otherwise, client connection status fields will not be updated,
  *         and connection loss or timeout conditions will not be detected.
  */
UCAN_StatusTypeDef uCAN_Handshake(UCAN_HandleTypeDef* ucan)
{
    // Ensure handle is ready
    UCAN_CHECK_READY(ucan);

    UCAN_StatusTypeDef connectionErrorFlag = UCAN_OK;

    // Iterate through clients to check handshake status
    for (uint32_t i = 0; i < ucan->node.clientCount; i++)
    {
        // No response received from client yet
        if (ucan->node.clients[i].responseTick == 0)
        {
            connectionErrorFlag = UCAN_ERROR;
            continue;
        }

        UCAN_ConnectionStatusTypeDef status;

        // Check if response timed out compared to sentTick
        if (UCAN_HANDSHAKE_IS_TIMEOUT(ucan->node.sentTick, ucan->node.clients[i].responseTick))
        {
            status = UCAN_CONN_TIMEOUT;
            connectionErrorFlag = UCAN_ERROR;
        }
        // Check if connection is lost
        else if (UCAN_HANDSHAKE_IS_LOST(ucan->node.sentTick, ucan->node.clients[i].responseTick))
        {
            status = UCAN_CONN_LOST;
            connectionErrorFlag = UCAN_ERROR;
        }
        else
        {
            status = UCAN_CONN_ACTIVE;
        }

        // Update client status
        ucan->node.clients[i].status = status;
    }

    return connectionErrorFlag;
}
