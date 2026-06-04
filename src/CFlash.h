#ifndef __CFLASH_H__
#define __CFLASH_H__
#include <stdio.h>
#include <SPIMemory.h>

#define WIFI_INFO_LENGTH 20
#define PUMP_MAX_NUMBER   2

enum FlashVrs
{
    V1 = 1,
    V2
};

typedef struct
{
    time_t sprinkler_active_time[10];
    uint8_t stored_active_times;
    uint8_t pump_power;
    uint16_t activation_time;
} __attribute__((packed, aligned(1))) FlashInfo1;

typedef struct
{
    time_t sprinkler_active_time[10][PUMP_MAX_NUMBER];
    uint8_t stored_active_times[PUMP_MAX_NUMBER];
    uint8_t pump_power[PUMP_MAX_NUMBER];
    uint16_t activation_time[PUMP_MAX_NUMBER];
} __attribute__((packed, aligned(1))) FlashInfo2;

typedef union
{
    FlashInfo1 flashInfo1;
    FlashInfo2 flashInfo2;
} FlashContent;

typedef struct
{
    uint8_t version;
    char ssid[WIFI_INFO_LENGTH];
    char password[WIFI_INFO_LENGTH];
    FlashContent flashData;
} FlashInfo;

class CFlash
{
    public:
        CFlash();
        void begin();
        bool IsFlashVirgin();
        void InitializeFlashInfo();
        void ClearWiFiInfo();
        void ClearAllInfo();
        const char* GetWifiSSID();
        const char* GetWifiPassword();
        void SetWiFiSSID(const char* ssid);
        void SetWiFiPassword(const char* password);
        bool IsSSIDSet();
        uint8_t GetPumpPower(uint8_t pump_num);
        uint8_t GetPumpPowerRaw(uint8_t pump_num);
        uint16_t GetActivationTime(uint8_t pump_num);
        uint8_t GetStoredActiveTimes(uint8_t pump_num);
        time_t GetActiveTime(uint8_t pump_num, uint8_t i);
        void SetPumpPower(uint8_t pump_num,uint8_t new_pump_power);
        void SetActivationTime(uint8_t pump_num,uint16_t new_activation_time);
        void SetActiveTime(uint8_t pump_num,time_t active_time);
        void ResetStoredActiveTimes(uint8_t pump_num);
        FlashVrs GetVersion(void);
    
    private:
        void InitializeFlashInfoV1();
        void InitializeFlashInfoV2();
        FlashInfo info;
        SPIClass* spi_interface;
        SPIFlash* flash_interface;
        uint32_t start_address;
};
#endif
