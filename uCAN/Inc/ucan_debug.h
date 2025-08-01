/**
  ******************************************************************************
  * @file    ucan_debug.h
  * @author  Hamza Enes Balahoroğlu
  * @brief   [INTERNAL] Internal debug and validation utilities for the UCAN protocol stack.
  *
  * This header declares a set of helper functions focused on configuration validation,
  * consistency checks, and uniqueness enforcement of packet and node parameters within UCAN.
  *
  * These utilities are intended primarily for development and debugging phases to catch
  * misconfigurations, detect logical errors, and improve overall system robustness.
  *
  * Functions here are not meant for regular runtime use in production and should be
  * invoked only during system bring-up, testing, or explicit diagnostics.
  *
  * @note    All declarations are marked [INTERNAL] indicating intended usage
  *          is restricted to UCAN library internal modules and not exposed publicly.
  *
  *
  *                          _____          _   _
  *                         / ____|   /\   | \ | |
  *	  				  _   _| |       /  \  |  \| |
  *                  | | | | |      / /\ \ | . ` |
  *                  | |_| | |____ / ____ \| |\  |
  *                   \____|\_____/_/    \_\_| \_|
  *
  ******************************************************************************
  */


#ifndef UCAN_DEBUG
#define UCAN_DEBUG

#include "ucan_types.h"
#include "ucan_macros.h"

/**
  * @brief [INTERNAL] Calculate total Data Length Code (DLC) for a packet configuration.
  * @param pkt Pointer to the UCAN_PacketConfig to analyze.
  * @retval uint8_t Total DLC (byte count) for the CAN frame.
  */
uint8_t uCAN_Debug_Calculate_DLC(UCAN_PacketConfig* pkt);

/**
  * @brief [INTERNAL] Validate packet configuration list for correctness.
  * @param configList Pointer to array of UCAN_PacketConfig to check.
  * @param packetHolder Pointer to UCAN_PacketHolder to populate metadata.
  * @retval UCAN_StatusTypeDef UCAN_OK if valid, error code otherwise.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckPacketConfig(UCAN_PacketConfig* configList, UCAN_PacketHolder* packetHolder);

/**
  * @brief [INTERNAL] Finalize packet metadata and prepare for runtime use.
  * @param configPackets Pointer to UCAN_PacketConfig array to finalize.
  * @param packetHolder Pointer to UCAN_PacketHolder to store finalized data.
  * @retval UCAN_StatusTypeDef UCAN_OK if successful, error otherwise.
  */
UCAN_StatusTypeDef uCAN_Debug_FinalizePacket(UCAN_PacketConfig* configPackets, UCAN_PacketHolder* packetHolder);

/**
  * @brief [INTERNAL] Sort and finalize UCAN node information client list.
  * @param node Pointer to UCAN_NodeInfo to finalize.
  * @retval UCAN_StatusTypeDef UCAN_OK if success, error if invalid input.
  */
UCAN_StatusTypeDef uCAN_Debug_FinalizeNodeInfo(UCAN_NodeInfo* node);

/**
 * @brief [INTERNAL] Validate UCAN node information for correctness and duplicates.
 * @param node Pointer to UCAN_NodeInfo to check.
 * @retval UCAN_StatusTypeDef UCAN_OK if valid, error code otherwise.
 */
UCAN_StatusTypeDef uCAN_Debug_CheckNodeInfo(UCAN_NodeInfo* node);

/**
  * @brief [INTERNAL] Verify that all packet items are valid data types.
  * @param pkt Pointer to UCAN_PacketConfig to verify.
  * @retval UCAN_StatusTypeDef UCAN_OK if all types valid, error otherwise.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckIsDataType(UCAN_PacketConfig* pkt);

/**
 * @brief [INTERNAL] Ensure that packet IDs in TX and RX lists are unique.
 * @param ucan Pointer to UCAN_HandleTypeDef to check.
 * @retval UCAN_StatusTypeDef UCAN_OK if unique, error if duplicates found.
 */
UCAN_StatusTypeDef uCAN_Debug_CheckUniquePackets(UCAN_HandleTypeDef* ucan);

/**
  * @brief [INTERNAL] Check if a given packet ID is unique across TX and RX holders.
  * @param id Packet ID to check.
  * @param txHolder Pointer to UCAN_PacketHolder holding TX packets.
  * @param rxHolder Pointer to UCAN_PacketHolder holding RX packets.
  * @retval UCAN_StatusTypeDef UCAN_OK if unique, error if duplicate found.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckUniqueID(uint32_t id, UCAN_PacketHolder* txHolder, UCAN_PacketHolder* rxHolder);

#endif
