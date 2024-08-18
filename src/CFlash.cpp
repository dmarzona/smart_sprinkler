#include "Arduino.h"
#include "CFlash.h"
#include "CTime.h"

static const int SPI_MISO = 37;
static const int SPI_MOSI = 35;
static const int SPI_SCLK = 36;
static const int SPI_CS   = 38;

CFlash::CFlash():
    start_address(0)
{
}

void CFlash::begin()
{
    spi_interface = new SPIClass(HSPI);
    spi_interface->begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_CS);
    flash_interface = new SPIFlash(SPI_CS, spi_interface);
    flash_interface->begin();
    flash_interface->readAnything(start_address, info);
}

bool CFlash::IsFlashVirgin()
{
    bool ret_value = false;
    
    if (0xFF == info.version)
    {
        ret_value = true;
    }

    return ret_value;
}

void CFlash::InitializeFlashInfo()
{
    info.version = V1;
    info.pump_power = 128;
    info.activation_time = 60;
    info.stored_active_times = 0;
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
    flash_interface->writeAnything(start_address, info);
}
void CFlash::ClearWiFiInfo()
{
    memset(info.ssid, 0xFF, WIFI_INFO_LENGTH);
    memset(info.password, 0xFF, WIFI_INFO_LENGTH);
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
    flash_interface->writeAnything(start_address, info);
}

void CFlash::ClearAllInfo()
{
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
}

const char* CFlash::GetWifiSSID()
{
    return info.ssid;
}

const char* CFlash::GetWifiPassword()
{
    return info.password;
}

void CFlash::SetWiFiSSID(const char* ssid)
{
    strcpy(info.ssid, ssid);
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
    flash_interface->writeAnything(start_address, info, true);
}

void CFlash::SetWiFiPassword(const char* password)
{
    strcpy(info.password, password);
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));    
    flash_interface->writeAnything(start_address, info, true);
}

bool CFlash::IsSSIDSet()
{
    bool ret_value = true;
    if(info.ssid[0] == 0xFF)
    {
        ret_value = false;
    }

    return ret_value;
}

uint8_t CFlash::GetPumpPower()
{
    return info.pump_power*100/255;
}

uint8_t CFlash::GetPumpPowerRaw()
{
    return info.pump_power;
}

uint16_t CFlash::GetActivationTime()
{
    return info.activation_time;
}

uint8_t CFlash::GetStoredActiveTimes()
{
    return info.stored_active_times;
}

time_t CFlash::GetActiveTime(uint8_t i)
{
    time_t active_time = NOT_A_TIME;
    
    if(i <= info.stored_active_times)
    {
        active_time = info.sprinkler_active_time[i];
    }

    return active_time;
}

void CFlash::SetPumpPower(uint8_t new_pump_power)
{
    info.pump_power = new_pump_power*255/100;
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));    
    flash_interface->writeAnything(start_address, info, true);
}

void CFlash::SetActivationTime(uint16_t new_activation_time)
{
    info.activation_time = new_activation_time;
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
    flash_interface->writeAnything(start_address, info, true);
}

void CFlash::ResetStoredActiveTimes()
{
    info.stored_active_times = 0;
}

void CFlash::SetActiveTime(time_t active_time)
{
    time_t swap = 0;

    // add new element
    info.sprinkler_active_time[info.stored_active_times] = active_time;
    info.stored_active_times++;

    // order the array
    for(uint8_t i = 0; i < info.stored_active_times; i++)
    {
        for(uint8_t j = 0; j < info.stored_active_times; j++)
        {
            if(info.sprinkler_active_time[j] > info.sprinkler_active_time[i])
            {
                swap = info.sprinkler_active_time[i];
                info.sprinkler_active_time[i] = info.sprinkler_active_time[j];
                info.sprinkler_active_time[j] = swap;
            }
        }
    }

    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
    flash_interface->writeAnything(start_address, info, true);
}
