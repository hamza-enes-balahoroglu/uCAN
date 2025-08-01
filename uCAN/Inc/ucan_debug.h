/**
  ******************************************************************************
  * @file    ucan_debug.h
  * @author  Hamza Enes Balahoroğlu
  * @brief   This header provides internal debugging and validation functions
  *          for the UCAN protocol stack. It includes helper utilities to check
  *          packet configurations, node information, data types, and ensure
  *          unique packet identifiers across TX and RX lists.
  *
  *          These functions are mainly intended for development-time checks and
  *          debugging purposes. They help identify configuration issues early
  *          before runtime, improving system robustness and consistency.
  *
  * @note    All functions here are designed for internal use and are not meant
  *          to be called during normal operation unless explicitly needed for
  *          diagnostics or assertions.
  *
  *
  *	  						 _____          _   _
  *	  						/ ____|   /\   | \ | |
  *	  				  _   _| |       /  \  |  \| |
  *	  				 | | | | |      / /\ \ | . ` |
  *	  				 | |_| | |____ / ____ \| |\  |
  *	  				  \____|\_____/_/    \_\_| \_|
  *
  ******************************************************************************
  */


#ifndef UCAN_DEBUG
#define UCAN_DEBUG

#include "ucan_types.h"
#include "ucan_macros.h"

/**
  * @brief  Calculates the appropriate DLC based on item count and types.
  */
uint8_t uCAN_Debug_Calculate_DLC(UCAN_PacketConfig* pkt);

/**
  * @brief  Validates a packet configuration list against a packet holder.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckPacketConfig(UCAN_PacketConfig* configList, UCAN_PacketHolder* packetHolder);

/**
  * @brief  Converts packet configuration into finalized packet format.
  */
UCAN_StatusTypeDef uCAN_Debug_FinalizePacket(UCAN_PacketConfig* configPackets, UCAN_PacketHolder* packetHolder);

/**
  * @brief  Validates node info and checks for duplicate client IDs.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckNodeInfo(UCAN_NodeInfo* node);

/**
  * @brief  Validates each item's data type in a packet config.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckIsDataType(UCAN_PacketConfig* pkt);

/**
  * @brief  Checks all packets in TX and RX holders for unique IDs.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckUniquePackets(UCAN_HandleTypeDef* ucan);

/**
  * @brief  Checks if a given packet ID is unique across TX and RX holders.
  */
UCAN_StatusTypeDef uCAN_Debug_CheckUniqueID(uint32_t id, UCAN_PacketHolder* txHolder, UCAN_PacketHolder* rxHolder);

#endif
