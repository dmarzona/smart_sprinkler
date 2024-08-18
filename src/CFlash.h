#ifndef __CFLASH_H__
#define __CFLASH_H__
#include <stdio.h>
#include <SPIMemory.h>

#define WIFI_INFO_LENGTH 20

enum FlashVrs
{
    V1 = 1,
};

typedef struct
{
    uint8_t version;
    char ssid[WIFI_INFO_LENGTH];
    char password[WIFI_INFO_LENGTH];
    time_t sprinkler_active_time[10];
    uint8_t stored_active_times;
    uint8_t pump_power;
    uint16_t activation_time;
} __attribute__((packed, aligned(1))) FlashInfo;

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
        uint8_t GetPumpPower();
        uint8_t GetPumpPowerRaw();
        uint16_t GetActivationTime();
        uint8_t GetStoredActiveTimes();
        time_t GetActiveTime(uint8_t i);
        void SetPumpPower(uint8_t new_pump_power);
        void SetActivationTime(uint16_t new_activation_time);
        void SetActiveTime(time_t active_time);
        void ResetStoredActiveTimes();
    
    private:
        FlashInfo info;
        SPIClass* spi_interface;
        SPIFlash* flash_interface;
        uint32_t start_address;
};
#endif
