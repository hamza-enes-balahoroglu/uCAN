#ifndef YCAN_TYPES
#define YCAN_TYPES

#include "stm32f4xx_hal.h"

typedef enum {
    YCAN_U8,
    YCAN_U16,
    YCAN_U32
} YCAN_Dataype_t;

typedef struct {
    void* ptr;
    YCAN_Dataype_t type;
} YCAN_Data;

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t item_count;
    YCAN_Data items[8];
} YCAN_Packet_Init;

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t* bits[8];
} YCAN_Packet;

typedef struct {
	uint32_t count;
	YCAN_Packet* packets;
} YCAN_Packet_Holder;



typedef struct {
	CAN_HandleTypeDef* hcan;
	YCAN_Packet_Init* txPacketList;
	uint32_t txPacketCount;
	YCAN_Packet_Init* rxPacketList;
	uint32_t rxPacketCount;
} YCAN_InitTypeDef;

typedef enum
{
  YCAN_OK              = 0x00U, // Sorun yok veri gitti
  YCAN_ERROR           = 0x01U, // veri gönderilemedi genel hata
  YCAN_MISSING_VAL     = 0x02U, // veri yanlış ya da yazılan uzunluk uyumsuz
  YCAN_NO_CONNECTION   = 0x03U, // can hattı kopunca döner
  YCAN_NO_CHANGED_VAL  = 0x04U, // değişen değer yoksa göndermez
  YCAN_TIMEOUT         = 0x05U, // işlem zaman aşımına uğradı
  YCAN_INVALID_PARAM   = 0x06U, // fonksiyona hatalı parametre gönderildi
  YCAN_BUSY            = 0x07U, // can hattı meşgul, şu anda işleme alınamaz
  YCAN_NOT_INITIALIZED  = 0x08U, // yCAN modülü init edilmedi
} YCAN_StatusTypeDef;

#endif
