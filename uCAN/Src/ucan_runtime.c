/**
  ******************************************************************************
  * @file    ucan_runtime.c
  * @author  Hamza Enes Balahoroğlu
  * @brief   [INTERNAL] Core runtime functions for UCAN protocol handling.
  *
  * This file contains the internal implementation of UCAN runtime operations including:
  * - Packet transmission and reception helpers
  * - Handshake (ping/pong) processing
  * - Packet data updating based on CAN messages
  * - Client and packet ID comparison utilities for searching/sorting
  *
  * All functions in this file are intended for internal use within the UCAN library and
  * are not exposed in the public API.
  *
  * The code assumes proper initialization of CAN peripheral and UCAN structures before use.
  *
  * @note    This module forms the low-level backbone of the UCAN stack, enabling
  *          communication and synchronization between master and client nodes.
  *
  * @see     uCAN GitHub repository: https://github.com/hamza-enes-balahoroglu/uCAN.git
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

/**
  * @brief [INTERNAL] Sends a single CAN packet using the HAL CAN interface.
  *
  * Used internally by the UCAN core to transmit a constructed UCAN_Packet over the CAN bus.
  * This function should not be called directly from user application code. It assumes that
  * the CAN peripheral (`hcan`) is already initialized and started.
  *
  * It builds a standard CAN frame using the packet’s ID and data length (`dlc`), and
  * populates the TX data buffer by dereferencing the individual pointers in the
  * `packet->bits[]` array. The actual transmission is handled via `HAL_CAN_AddTxMessage()`.
  *
  * @param hcan    Pointer to the HAL CAN handle.
  * @param packet  Pointer to the UCAN packet to be transmitted.
  *
  * @retval UCAN_OK              Packet sent successfully.
  * @retval UCAN_INVALID_PARAM   Provided pointer is NULL.
  * @retval UCAN_ERROR           HAL CAN transmission failed.
  */
UCAN_StatusTypeDef uCAN_Runtime_SendPacket(CAN_HandleTypeDef* hcan, UCAN_Packet* packet)
{
    if (hcan == NULL || packet == NULL)
    {
        return UCAN_INVALID_PARAM;
    }

    CAN_TxHeaderTypeDef txHeader;
    uint32_t TxMailbox;
    uint8_t data[8];

    // Construct standard data frame header
    txHeader.StdId = packet->id;
    txHeader.DLC   = packet->dlc;
    txHeader.IDE   = CAN_ID_STD;
    txHeader.RTR   = CAN_RTR_DATA;

    // Copy data bytes from packet pointers
    for (int i = 0; i < packet->dlc; i++) {
        data[i] = *(packet->bits[i]);
    }

    // Transmit the CAN message
    if (HAL_CAN_AddTxMessage(hcan, &txHeader, data, &TxMailbox) != HAL_OK)
    {
        return UCAN_ERROR;
    }

    return UCAN_OK;
}

/**
  * @brief [INTERNAL] Sends a handshake request ("ping") from the master node to all clients.
  *
  * This function is used internally by the UCAN core to periodically send handshake requests
  * over the CAN bus. It should only be called by a master node. The function ensures that
  * handshake pings are only sent if the defined interval (`UCAN_HANDSHAKE_INTERVAL_MS`) has passed
  * since the last ping.
  *
  * The handshake packet is a 1-byte CAN message containing a predefined constant
  * (`UCAN_HANDSHAKE_REQUEST_VALUE`) and is sent with the master's own CAN ID (`node->selfId`).
  *
  * If the interval condition is met, it creates a UCAN_Packet and transmits it via
  * `uCAN_Runtime_SendPacket()`. It also updates `node->sentTick` to record the last ping time.
  *
  * @param hcan Pointer to the CAN peripheral handle.
  * @param node Pointer to the UCAN node information structure.
  *
  * @retval UCAN_OK              Ping was sent successfully.
  * @retval UCAN_BUSY            Ping was not sent because the interval hasn't passed.
  * @retval UCAN_INVALID_PARAM   One or more parameters are NULL.
  * @retval UCAN_ERROR           Called on a node that is not configured as master.
  */
UCAN_StatusTypeDef uCAN_Runtime_SendPing(CAN_HandleTypeDef* hcan, UCAN_NodeInfo* node)
{
    if(hcan == NULL || node == NULL)
    {
        // check for null pointers to avoid crash
        return UCAN_INVALID_PARAM;
    }

    if(node->role != UCAN_ROLE_MASTER)
    {
        // only master nodes can send handshake pings
        return UCAN_ERROR;
    }

    // check if handshake interval has elapsed since last ping
    if(HAL_GetTick() - node->sentTick >= UCAN_HANDSHAKE_INTERVAL_MS)
    {
        UCAN_Packet packet;
        uint8_t request = UCAN_HANDSHAKE_REQUEST_VALUE;

        packet.id = node->selfId;    // set CAN ID to master's own ID
        packet.dlc = 1;              // data length = 1 byte

        packet.bits[0] = &request;   // point to handshake request value

        node->sentTick = HAL_GetTick();  // update last sent timestamp

        return uCAN_Runtime_SendPacket(hcan, &packet);  // transmit handshake ping
    }

    // interval not yet reached, skip sending
    return UCAN_BUSY;
}

/**
  * @brief [INTERNAL] Sends a handshake response ("pong") from a client node to the master.
  *
  * This function is intended for internal use within the UCAN core. It transmits a 1-byte
  * handshake response message from a client node indicating active presence to the master.
  * Only nodes configured as clients should call this function.
  *
  * @param hcan Pointer to the HAL CAN handle.
  * @param node Pointer to the UCAN node structure.
  *
  * @retval UCAN_OK              Response packet sent successfully.
  * @retval UCAN_INVALID_PARAM   Null pointer provided.
  * @retval UCAN_ERROR           Node is not configured as a client.
  */
UCAN_StatusTypeDef uCAN_Runtime_SendPong(CAN_HandleTypeDef* hcan, UCAN_NodeInfo* node)
{
    if(hcan == NULL || node == NULL)
    {
        // Validate input pointers to prevent null dereference
        return UCAN_INVALID_PARAM;
    }

    if(node->role != UCAN_ROLE_CLIENT)
    {
        // Only client nodes are allowed to send handshake responses
        return UCAN_ERROR;
    }

    UCAN_Packet packet;
    uint8_t response = UCAN_HANDSHAKE_RESPONSE_VALUE;

    packet.id = node->selfId;    // Use the client's own CAN ID
    packet.dlc = 1;              // Set data length to 1 byte

    packet.bits[0] = &response;  // Point to handshake response constant

    // Send the handshake response packet over CAN bus
    return uCAN_Runtime_SendPacket(hcan, &packet);
}

/**
  * @brief [INTERNAL] Updates RX packet data matching the received CAN ID.
  *
  * Searches the RX packet list for a packet with the given standard CAN ID (`StdId`).
  * If found, copies the received data bytes into the packet's data pointers.
  *
  * @param rxHolder Pointer to the RX packet holder containing packet array.
  * @param StdId    Standard CAN ID of the received message.
  * @param aData    Array of received data bytes.
  *
  * @retval UCAN_OK              Packet updated successfully.
  * @retval UCAN_INVALID_PARAM   rxHolder is NULL.
  * @retval UCAN_ERROR_UNKNOWN_ID No matching packet found for StdId.
  */
UCAN_StatusTypeDef uCAN_Runtime_UpdatePacket(UCAN_PacketHolder* rxHolder, uint32_t StdId, uint8_t aData[])
{
    if(rxHolder == NULL)
    {
        // Null pointer guard
        return UCAN_INVALID_PARAM;
    }

    // Create search key with received StdId
    UCAN_Packet packetKey = {.id = StdId};

    // Search packet array by ID
    UCAN_Packet* packetFound = bsearch(&packetKey, rxHolder->packets, rxHolder->count, sizeof(UCAN_Packet), uCAN_Runtime_ComparePacketId);

    if(packetFound == NULL)
    {
        // Packet with StdId not found
        return UCAN_ERROR_UNKNOWN_ID;
    }

    // Update packet data bytes from received data
    for(int i = 0; i < packetFound->dlc; i++) {
        *(packetFound->bits[i]) = aData[i];
    }

    return UCAN_OK;
}

/**
  * @brief [INTERNAL] Process incoming handshake messages based on node role.
  *
  * For master nodes, updates the responseTick of the client matching the received StdId
  * if the handshake response value matches.
  *
  * For client nodes, verifies the message is from the master and the handshake request value,
  * then updates sentTick and sends a handshake reply.
  *
  * @param node  Pointer to UCAN node info structure.
  * @param hcan  Pointer to HAL CAN handle.
  * @param StdId Standard CAN ID of the received message.
  * @param aData Pointer to received data bytes.
  *
  * @retval UCAN_OK              Handshake processed successfully.
  * @retval UCAN_INVALID_PARAM   Null pointer input.
  * @retval UCAN_ERROR_UNKNOWN_ID Received StdId not found or unexpected sender.
  * @retval UCAN_ERROR           Handshake data value mismatch.
  */
UCAN_StatusTypeDef uCAN_Runtime_UpdateHandshake(UCAN_NodeInfo* node, CAN_HandleTypeDef* hcan, uint32_t StdId, uint8_t aData[])
{
    if(node == NULL || hcan == NULL)
    {
        // Validate pointers
        return UCAN_INVALID_PARAM;
    }

    switch (node->role) {
        case UCAN_ROLE_MASTER:
        {
            // Master expects handshake responses from clients
            UCAN_Client handshakeKey = {.id = StdId};
            UCAN_Client* handshakeFound = bsearch(&handshakeKey, node->clients, node->clientCount, sizeof(UCAN_Client), uCAN_Runtime_CompareClientId);

            if(handshakeFound == NULL)
            {
                // Unknown client ID
                return UCAN_ERROR_UNKNOWN_ID;
            }

            if(aData[0] != UCAN_HANDSHAKE_RESPONSE_VALUE)
            {
                // Invalid handshake response data
                return UCAN_ERROR;
            }

            // Update client's last response timestamp
            handshakeFound->responseTick = HAL_GetTick();
            break;
        }

        case UCAN_ROLE_CLIENT:
        {
            // Client expects handshake requests from master
            if(StdId != node->masterId)
            {
                // Message not from master
                return UCAN_ERROR_UNKNOWN_ID;
            }

            if(aData[0] != UCAN_HANDSHAKE_REQUEST_VALUE)
            {
                // Invalid handshake request data
                return UCAN_ERROR;
            }

            // Update last sent tick before replying
            node->sentTick = HAL_GetTick();

            // Send handshake reply (pong)
            uCAN_Runtime_SendPong(hcan, node);
            break;
        }

        case UCAN_ROLE_NONE:
        default:
            // No handshake processing for undefined roles
            break;
    }

    return UCAN_OK;
}

/**
  * @brief [INTERNAL] Compare two UCAN_Packet structs by their CAN ID.
  *
  * Used as a comparison callback for bsearch and sorting functions.
  *
  * @param a Pointer to first UCAN_Packet.
  * @param b Pointer to second UCAN_Packet.
  * @retval int Negative if a < b, positive if a > b, zero if equal.
  */
int uCAN_Runtime_ComparePacketId(const void* a, const void* b)
{
    const UCAN_Packet* p1 = (const UCAN_Packet*)a;
    const UCAN_Packet* p2 = (const UCAN_Packet*)b;

    if (p1->id < p2->id) return -1;
    if (p1->id > p2->id) return 1;
    return 0;
}

/**
  * @brief [INTERNAL] Compare two UCAN_Client structs by their client ID.
  *
  * Comparator function used for searching and sorting client arrays.
  *
  * @param a Pointer to first UCAN_Client.
  * @param b Pointer to second UCAN_Client.
  * @retval int Negative if a < b, positive if a > b, zero if equal.
  */
int uCAN_Runtime_CompareClientId(const void* a, const void* b)
{
    const UCAN_Client* p1 = (const UCAN_Client*)a;
    const UCAN_Client* p2 = (const UCAN_Client*)b;

    if (p1->id < p2->id) return -1;
    if (p1->id > p2->id) return 1;
    return 0;
}
