/**
  ******************************************************************************
  * @file    ucan_runtime.h
  * @author  Hamza Enes Balahoroğlu
  * @brief   [INTERNAL] Header for UCAN core runtime functions.
  *
  * Declares internal functions used by the UCAN protocol stack for runtime operations.
  * These include packet transmission, handshake management, packet updates, and utility
  * comparison functions.
  *
  * All functions declared here are meant for internal use within the UCAN library and
  * should not be called directly by user applications.
  *
  * This header must be included only in UCAN source files that implement or use runtime features.
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

#ifndef UCAN_RUNTIME
#define UCAN_RUNTIME

#include "ucan_macros.h"
#include "ucan_types.h"

/**
  * @brief [INTERNAL] Sends a single UCAN packet over CAN bus.
  * @param hcan Pointer to the HAL CAN handle.
  * @param packet Pointer to the packet to send.
  * @retval UCAN_StatusTypeDef Status of the transmission operation.
  */
UCAN_StatusTypeDef uCAN_Runtime_SendPacket(CAN_HandleTypeDef* hcan, UCAN_Packet* packet);

/**
  * @brief [INTERNAL] Sends a handshake request ("ping") from the master node.
  * @param hcan Pointer to the HAL CAN handle.
  * @param node Pointer to the UCAN node structure.
  * @retval UCAN_StatusTypeDef Status of the ping transmission.
  */
UCAN_StatusTypeDef uCAN_Runtime_SendPing(CAN_HandleTypeDef* hcan, UCAN_NodeInfo* node);

/**
  * @brief [INTERNAL] Sends a handshake response ("pong") from a client node.
  * @param hcan Pointer to the HAL CAN handle.
  * @param node Pointer to the UCAN node structure.
  * @retval UCAN_StatusTypeDef Status of the reply transmission.
  */
UCAN_StatusTypeDef uCAN_Runtime_SendPong(CAN_HandleTypeDef* hcan, UCAN_NodeInfo* node);

/**
  * @brief [INTERNAL] Updates received packet data based on CAN ID.
  * @param rxHolder Pointer to the RX packet holder.
  * @param StdId Standard CAN ID of the received message.
  * @param aData Pointer to the received data bytes.
  * @retval UCAN_StatusTypeDef Status of the update operation.
  */
UCAN_StatusTypeDef uCAN_Runtime_UpdatePacket(UCAN_PacketHolder* rxHolder, uint32_t StdId, uint8_t aData[]);

/**
  * @brief [INTERNAL] Processes handshake messages based on node role.
  * @param node Pointer to the UCAN node info.
  * @param hcan Pointer to the HAL CAN handle.
  * @param StdId Standard CAN ID of the received handshake message.
  * @param aData Pointer to the received data bytes.
  * @retval UCAN_StatusTypeDef Status of the handshake processing.
  */
UCAN_StatusTypeDef uCAN_Runtime_UpdateHandshake(UCAN_NodeInfo* node, CAN_HandleTypeDef* hcan, uint32_t StdId, uint8_t aData[]);

/**
  * @brief [INTERNAL] Compare two UCAN_Packet structures by their CAN IDs.
  * @param a Pointer to first UCAN_Packet.
  * @param b Pointer to second UCAN_Packet.
  * @retval int Comparison result for sorting/searching.
  */
int uCAN_Runtime_ComparePacketId(const void* a, const void* b);

/**
  * @brief [INTERNAL] Compare two UCAN_Client structures by their client IDs.
  * @param a Pointer to first UCAN_Client.
  * @param b Pointer to second UCAN_Client.
  * @retval int Comparison result for sorting/searching.
  */
int uCAN_Runtime_CompareClientId(const void* a, const void* b);

#endif
