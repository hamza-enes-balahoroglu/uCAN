/**
  ******************************************************************************
  * @file    ucan.h
  * @author  Hamza Enes Balahoroğlu
  * @brief   uCAN Protocol API Header File
  *
  * @details This file provides the public interface for the uCAN protocol,
  *          a lightweight communication stack over CAN for STM32 using HAL.
  *
  *          The API enables:
  *            - Initialization of the uCAN context
  *            - Configuration and startup of the protocol
  *            - Periodic update and message handling
  *            - Handshake and connection tracking
  *            - Sending of all registered TX packets
  *
  * @note    This module is designed to be initialized once via @ref uCAN_Init(),
  *          and started with @ref uCAN_Start() before use.
  *
  *          All other functions depend on successful startup.
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

#ifndef UCAN
#define UCAN

#include "ucan_macros.h"
#include "ucan_types.h"

/**
  * @brief  Initialize the uCAN handle.
  * @param  ucan Pointer to the UCAN handle structure.
  * @retval UCAN_OK if initialization was successful.
  */
UCAN_StatusTypeDef uCAN_Init(UCAN_HandleTypeDef* ucan);

/**
  * @brief  Starts uCAN communication with provided configuration.
  * @param  ucan   Pointer to the uCAN handle.
  * @param  config Pointer to the configuration structure.
  * @retval Status of the operation.
  */
UCAN_StatusTypeDef uCAN_Start(UCAN_HandleTypeDef* ucan, UCAN_Config* config);

/**
  * @brief  Sends all packets in the transmission list.
  * @param  ucan Pointer to the uCAN handle.
  * @retval Status of the operation.
  */
UCAN_StatusTypeDef uCAN_SendAll(UCAN_HandleTypeDef* ucan);

/**
  * @brief  Updates internal RX packet states and flags.
  * @param  ucan Pointer to the uCAN handle.
  * @retval Status of the operation.
  */
UCAN_StatusTypeDef uCAN_Update(UCAN_HandleTypeDef* ucan);

/**
  * @brief  Performs handshake evaluation with all clients.
  * @param  ucan Pointer to the uCAN handle.
  * @retval Status of the handshake operation.
  */
UCAN_StatusTypeDef uCAN_Handshake(UCAN_HandleTypeDef* ucan);

#endif
