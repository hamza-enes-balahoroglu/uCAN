#ifndef UCAN_TYPES
#define UCAN_TYPES

#include "stm32f4xx_hal.h"

typedef enum {
    UCAN_U8,
    UCAN_U16,
    UCAN_U32
} UCAN_DataType;

typedef enum {
	UCAN_ROLE_MASTER,
	UCAN_ROLE_CLIENT,
	UCAN_ROLE_NONE
} UCAN_NodeRole;

typedef enum {
	UCAN_NOT_INITIALIZED = 0x00U, // uCAN modülü init edilmedi
	UCAN_OK              = 0x01U, // Sorun yok veri gitti
	UCAN_ERROR           = 0x02U, // veri gönderilemedi genel hata
	UCAN_MISSING_VAL     = 0x03U, // veri yanlış ya da yazılan uzunluk uyumsuz
	UCAN_NO_CONNECTION   = 0x04U, // can hattı kopunca döner
	UCAN_NO_CHANGED_VAL  = 0x05U, // değişen değer yoksa göndermez
	UCAN_TIMEOUT         = 0x06U, // işlem zaman aşımına uğradı
	UCAN_INVALID_PARAM   = 0x07U, // fonksiyona hatalı parametre gönderildi
	UCAN_BUSY            = 0x08U, // can hattı meşgul, şu anda işleme alınamaz
} UCAN_StatusTypeDef;

typedef struct {
    void* ptr;
    UCAN_DataType type;
} UCAN_Data;

typedef struct {
    uint32_t id;
    uint8_t item_count;
    UCAN_Data items[8];
} UCAN_PacketConfig;

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
	UCAN_NodeRole role;
    uint32_t selfId;
    uint32_t masterId;
    uint32_t* clientIdList;
    uint32_t clientCount;
} UCAN_NodeInfo;


typedef struct {
	UCAN_PacketConfig* txPacketList;
	UCAN_PacketConfig* rxPacketList;
} UCAN_Config;

typedef struct {
    CAN_HandleTypeDef* hcan;
    UCAN_NodeInfo node;
    UCAN_PacketHolder txHolder;
    UCAN_PacketHolder rxHolder;
    UCAN_StatusTypeDef status;
} UCAN_HandleTypeDef;

#endif
