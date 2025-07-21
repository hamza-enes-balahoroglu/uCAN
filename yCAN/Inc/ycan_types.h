#ifndef YCAN_TYPES
#define YCAN_TYPES

typedef enum {
    YCAN_U8,
    YCAN_U16,
    YCAN_U32
} ycan_data_type_t;

typedef struct {
    void* ptr;
    ycan_data_type_t type;
} ycan_data_t;

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t item_count;
    ycan_data_t items[8];
} ycan_packet_t;

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
