# uCAN Library

uCAN is a lightweight, modular, and robust CAN bus communication library for STM32-based systems.  
It is built on top of STM32 HAL CAN drivers and provides an abstraction layer for managing multiple CAN clients with handshake and connection monitoring functionality.

## Features

- **Hardware-independent design:** works with any STM32 device supported by HAL CAN drivers.  
- **Multiple clients support:** allows multiple nodes with unique IDs to communicate on the same CAN bus.  
- **Handshake mechanism:** monitors the connection status of clients to detect lost or unresponsive nodes.  
- **Efficient RX handling:** uses interrupt-driven RX updates via `uCAN_Update()`; no busy polling needed.  
- **TX packet management:** queued packet transmission with automatic node presence ping.  
- **Flexible integration:** simple to add to STM32CubeIDE projects and main loop designs.

## Key Concepts

1. **UCAN_HandleTypeDef**  
   - Main handle for the uCAN instance.  
   - Holds CAN peripheral handle, TX/RX packet holders, node info, and client statuses.  
   - Must be initialized with `uCAN_Init()` before starting communication.

2. **Packet Management**  
   - **TX packets:** define which application variables to transmit.  
   - **RX packets:** define which CAN IDs to listen to and update corresponding variables.  
   - Packets are configured using `UCAN_PacketConfig` and finalized in `uCAN_Start()`.  
   - Duplicate packet IDs are detected during startup to avoid collisions.

3. **Handshake Mechanism**  
   - Tracks connection status for each client.  
   - Each client has `sentTick` and `responseTick`.  
   - Status can be:
     - `UCAN_CONN_ACTIVE` – client responded on time.  
     - `UCAN_CONN_TIMEOUT` – client did not respond in the expected timeframe.  
     - `UCAN_CONN_LOST` – client has been disconnected.  
   - Must call `uCAN_Handshake()` periodically (main loop or timer) to update client statuses.  
   - Safe to call very frequently; internal logic prevents bus flooding.

4. **Interrupt-driven RX Handling**  
   - Incoming messages must be processed using `uCAN_Update()` inside the CAN RX0 interrupt handler:  

```c
    void CAN1_RX0_IRQHandler(void)
    {
        HAL_CAN_IRQHandler(&hcan1);
        uCAN_Update(&ucan1);
    }
```

   - This ensures RX packets are updated only when new data arrives, avoiding CPU waste.  
   - Unknown packet IDs are treated as potential handshake messages.  

5. **Initialization and Startup**  
   - **`uCAN_Init()`** – validates the handle, assigns default CAN filter if none provided, prepares internal state.  
   - **`uCAN_Start()`** – validates TX/RX packet lists, finalizes packet holders, checks for duplicates, configures CAN filters, starts the CAN peripheral, and activates RX FIFO0 interrupt.  
   - Both functions are required in order; initialization alone is insufficient for communication.  

6. **TX Packet Transmission**  
   - `uCAN_SendAll()` iterates over all TX packets and sends them sequentially.  
   - After transmitting, a ping is sent to announce node presence.  
   - Assumes the CAN peripheral is started and ready.  

## Example Variables and Packet Setup

```c
    // Application variables
    uint16_t motorSpeed;    // TX
    uint8_t  ledStatus;     // TX
    uint16_t temperature;   // RX
    uint8_t  buttonPressed; // RX

    UCAN_Client clients[2] = {
        {.id = 0x100U},
        {.id = 0x200U}
    };
```

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/hamza-enes-balahoroglu/uCAN.git
    ```

2. Copy the uCAN/Inc/ and uCAN/Src/ directories into your STM32 project.

3. Include the header:

```c
    #include "ucan.h"
```
---

## Usage Example

- Below is a minimal working example with TX and RX packet configuration, client setup, and handshake logic.
Note the use of CAN RX0 interrupt to update internal RX states.

```c
#include "ucan.h"

CAN_HandleTypeDef hcan1;
UCAN_HandleTypeDef ucan1;
UCAN_Packet txPackets[2];
UCAN_Packet rxPackets[2];

// Example application variables
uint16_t motorSpeed;        // to be transmitted
uint8_t  ledStatus;         // to be transmitted

uint16_t temperature;       // to be received
uint8_t  buttonPressed;     // to be received

// Example client list
UCAN_Client clients[2] = {
    { .id = 0x100U },
    { .id = 0x200U },
};

static void UCAN1_Init(void) {
    ucan1.hcan = &hcan1;
    ucan1.node.masterId = 0x050;
    ucan1.node.selfId   = 0x050;
    ucan1.node.role     = UCAN_ROLE_MASTER;
    ucan1.node.clients  = clients;
    ucan1.node.clientCount = UCAN_CLIENT_COUNT(clients);

    ucan1.txHolder.packets = txPackets;
    ucan1.txHolder.count   = UCAN_PACKET_COUNT(txPackets);
    ucan1.rxHolder.packets = rxPackets;
    ucan1.rxHolder.count   = UCAN_PACKET_COUNT(rxPackets);

    if (uCAN_Init(&ucan1) != UCAN_OK) {
        Error_Handler();
    }
}

static void UCAN1_Start(void) {
    // Define TX packets (motor speed + LED status)
    UCAN_PacketConfig txConfigPacket[2] = {
        {
            .id = 0x120U,
            .item_count = 2,
            .items = {
                { .type = UCAN_U16, .ptr = &motorSpeed },
                { .type = UCAN_U8,  .ptr = &ledStatus  },
            }
        },
    };

    // Define RX packets (temperature + button status)
    UCAN_PacketConfig rxConfigPacket[2] = {
        {
            .id = 0x130U,
            .item_count = 2,
            .items = {
                { .type = UCAN_U16, .ptr = &temperature   },
                { .type = UCAN_U8,  .ptr = &buttonPressed },
            }
        },
    };

    UCAN_Config ucanConfig;
    ucanConfig.txPacketList = txConfigPacket;
    ucanConfig.rxPacketList = rxConfigPacket;

    if (uCAN_Start(&ucan1, &ucanConfig) != UCAN_OK) {
        Error_Handler();
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_CAN1_Init();

    UCAN1_Init();
    UCAN1_Start();

    while (1) {
        // Update handshake regularly (safe to call often)
        uCAN_Handshake(&ucan1);

        // Example: update application variables
        motorSpeed += 10;
        if (motorSpeed > 5000) motorSpeed = 0;

        ledStatus ^= 1;  // toggle LED status

        // Transmit queued packets
        uCAN_SendAll(&ucan1);

        HAL_Delay(100);

        // Example: read received values
        if (buttonPressed) {
            // button event detected
        }
        uint16_t currentTemp = temperature;
    }
}

// RX0 interrupt handler must call uCAN_Update
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
    uCAN_Update(&ucan1);
}

```

---

## Important Notes

### RX Handling:
- uCAN_Update() must be called inside the CAN1 RX0 interrupt handler.
- This ensures RX packets are processed only when new data arrives, avoiding unnecessary CPU usage.

### Handshake:
- uCAN_Handshake() must be called regularly (e.g., in the main loop or via a timer).

- The function internally checks the timestamp of the last handshake and decides if a new message should be sent.

- If you do not call it, connection states will not be updated.

---

## API Reference

### `UCAN_StatusTypeDef uCAN_Init(UCAN_HandleTypeDef* ucan)`
Initializes the uCAN peripheral handle and prepares its internal state.

**Parameters:**  
- `ucan`: Pointer to the UCAN handle structure. Must not be `NULL` and must contain valid CAN handle and client list.

**Returns:**  
- `UCAN_OK` – Initialization successful.  
- `UCAN_INVALID_PARAM` – Invalid input parameters (null pointers).  

**Notes:**  
- Does **not** start the CAN hardware.  
- Assigns a default CAN filter if none is configured.  
- Must be called before `uCAN_Start()`.  

---

### `UCAN_StatusTypeDef uCAN_Start(UCAN_HandleTypeDef* ucan, UCAN_Config* config)`
Starts the uCAN protocol with the specified configuration, including TX/RX packet lists.

**Parameters:**  
- `ucan`: Pointer to an initialized UCAN handle.  
- `config`: Pointer to a configuration structure containing TX/RX packet lists.

**Returns:**  
- `UCAN_OK` – Started successfully.  
- `UCAN_INVALID_PARAM` – Handle not ready or invalid.  
- `UCAN_ERROR_DUPLICATE_ID` – Duplicate packet IDs detected.  
- `UCAN_ERROR_FILTER_CONFIG` – CAN filter configuration failed.  
- `UCAN_ERROR_CAN_START` – CAN peripheral start failed.  
- `UCAN_ERROR_CAN_NOTIFICATION` – Activation of CAN RX FIFO0 interrupt failed.

**Notes:**  
- Validates TX/RX packet configurations and finalizes internal packet holders.  
- Checks for duplicate packet IDs across all holders.  
- Configures CAN hardware filter and starts CAN peripheral.  
- Activates RX FIFO0 message pending interrupt.  
- Must be called **after** `uCAN_Init()` for proper operation.  

---

### `UCAN_StatusTypeDef uCAN_SendAll(UCAN_HandleTypeDef* ucan)`
Sends all queued TX packets over the CAN bus and sends a node presence ping.

**Parameters:**  
- `ucan`: Pointer to an initialized UCAN handle.

**Returns:**  
- `UCAN_OK` – All packets and ping sent successfully.  
- `UCAN_ERROR` – Failed to send one or more packets.

**Notes:**  
- Iterates through all packets in the TX holder and sends them sequentially.  
- Sends a ping message after all packets to announce node presence.  
- Assumes CAN peripheral is already started.  

---

### `UCAN_StatusTypeDef uCAN_Update(UCAN_HandleTypeDef* ucan)`
Processes incoming CAN messages, updates RX packet data, and handles handshake messages if necessary.

**Parameters:**  
- `ucan`: Pointer to an initialized UCAN handle.

**Returns:**  
- `UCAN_OK` – Message processed successfully.  
- `UCAN_ERROR` – CAN receive failure or unknown error.  
- `UCAN_ERROR_UNKNOWN_ID` – Received unknown packet ID; may trigger handshake handling.  
- Other handshake-related errors if handshake update fails.

**Notes:**  
- Must be called **inside the CAN RX0 interrupt handler** for immediate message processing.  
- Reads one message from CAN RX FIFO0 at a time.  
- Updates RX packet data if ID is known; otherwise, tries to handle it as a handshake message.  
- Ensures that connection status is updated only when messages arrive.  

---

### `UCAN_StatusTypeDef uCAN_Handshake(UCAN_HandleTypeDef* ucan)`
Evaluates handshake responses from all clients and updates connection statuses.

**Parameters:**  
- `ucan`: Pointer to an initialized UCAN handle.

**Returns:**  
- `UCAN_OK` – All clients responded within valid time.  
- `UCAN_ERROR` – One or more clients failed handshake or timed out.

**Notes:**  
- Iterates through all clients and checks `responseTick` against `sentTick`.  
- Updates `status` for each client:
  - `UCAN_CONN_ACTIVE` – Client responded in time.  
  - `UCAN_CONN_TIMEOUT` – Client response timed out.  
  - `UCAN_CONN_LOST` – Client connection lost.  
- Must be called **regularly**, either in the main loop or a periodic timer.  
- Safe to call very frequently; internal logic ensures no bus flooding.  
- If not called, client connection statuses will not update, and lost or timeout conditions may be missed.  
