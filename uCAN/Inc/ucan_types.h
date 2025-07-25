#ifndef UCAN_TYPES
#define UCAN_TYPES

#include "stm32f4xx_hal.h"

typedef enum {
    UCAN_U8,
    UCAN_U16,
    UCAN_U32
} UCAN_DataType;

typedef enum {
	UCAN_HANDSHAKE_MASTER,
	UCAN_HANDSHAKE_LISTENER,
	UCAN_HANDSHAKE_NONE
} UCAN_HandshakeType;

typedef struct {
    void* ptr;
    UCAN_DataType type;
} UCAN_Data;

typedef struct {
    uint32_t id;
    uint8_t item_count;
    UCAN_Data items[8];
} UCAN_PacketInit;

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t* bits[8];
} UCAN_Packet;

typedef struct {
	uint32_t count;
	UCAN_Packet* packets;
} UCAN_PacketHolder;


typedef struct {
	CAN_HandleTypeDef* hcan;
	UCAN_HandshakeType handshakeType;
	UCAN_PacketInit* txPacketList;
	uint32_t txPacketCount;
	UCAN_PacketInit* rxPacketList;
	uint32_t rxPacketCount;
} UCAN_InitTypeDef;

typedef enum {
  UCAN_OK              = 0x00U, // Sorun yok veri gitti
  UCAN_ERROR           = 0x01U, // veri gönderilemedi genel hata
  UCAN_MISSING_VAL     = 0x02U, // veri yanlış ya da yazılan uzunluk uyumsuz
  UCAN_NO_CONNECTION   = 0x03U, // can hattı kopunca döner
  UCAN_NO_CHANGED_VAL  = 0x04U, // değişen değer yoksa göndermez
  UCAN_TIMEOUT         = 0x05U, // işlem zaman aşımına uğradı
  UCAN_INVALID_PARAM   = 0x06U, // fonksiyona hatalı parametre gönderildi
  UCAN_BUSY            = 0x07U, // can hattı meşgul, şu anda işleme alınamaz
  UCAN_NOT_INITIALIZED = 0x08U, // uCAN modülü init edilmedi
} UCAN_StatusTypeDef;

#endif
