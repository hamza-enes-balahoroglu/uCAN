#ifndef UCAN
#define UCAN

#include "ucan_macros.h"
#include "ucan_types.h"

// TODO: implement Init - initialize the internal packet buffers and clear state flags
UCAN_StatusTypeDef uCAN_Init(UCAN_HandleTypeDef* ucan);

UCAN_StatusTypeDef uCAN_Start(UCAN_HandleTypeDef* ucan, UCAN_Config* config);

// TODO: implement SendAll - loop through txPacketList and send all valid packets over CAN
UCAN_StatusTypeDef uCAN_SendAll(UCAN_HandleTypeDef* ucan);

// TODO: implement Update - check for incoming CAN packets and update rxPacketList accordingly
UCAN_StatusTypeDef uCAN_Update(UCAN_HandleTypeDef* ucan);

// TODO: implement Handshake - perform a basic identification or sync routine with other nodes
UCAN_StatusTypeDef uCAN_Handshake(UCAN_HandleTypeDef* ucan);

#endif
