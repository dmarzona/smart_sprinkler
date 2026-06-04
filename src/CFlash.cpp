#include "Arduino.h"
#include "CFlash.h"
#include "CTime.h"

static const int SPI_MISO = 13;
static const int SPI_MOSI = 11;
static const int SPI_SCLK = 12;
static const int SPI_CS   = 10;

CFlash::CFlash():
    start_address(0)
{
}

void CFlash::begin()
{
    spi_interface = new SPIClass(FSPI);
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

void CFlash::InitializeFlashInfoV1()
{
    info.version = V1;
    info.flashData.flashInfo1.pump_power = 128;
    info.flashData.flashInfo1.activation_time = 60;
    info.flashData.flashInfo1.stored_active_times = 0;
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
    flash_interface->writeAnything(start_address, info);
}

void CFlash::InitializeFlashInfoV2()
{
    info.version = V2;
    for(uint8_t i = 0; i < PUMP_MAX_NUMBER; i++)
    {
        info.flashData.flashInfo2.pump_power[i] = 128;
        info.flashData.flashInfo2.activation_time[i] = 60;
        info.flashData.flashInfo2.stored_active_times[i] = 0;
    }
    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
    flash_interface->writeAnything(start_address, info);
}

void CFlash::InitializeFlashInfo()
{
    if(IsFlashVirgin())
    {
        InitializeFlashInfoV2();
    }
    else if(info.version != V2)
    {
        InitializeFlashInfoV2();
    }
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

uint8_t CFlash::GetPumpPower(uint8_t pump_num)
{
    if(pump_num < PUMP_MAX_NUMBER)
    {
        return info.flashData.flashInfo2.pump_power[pump_num]*100/255;
    }
}

uint8_t CFlash::GetPumpPowerRaw(uint8_t pump_num)
{
    if(pump_num < PUMP_MAX_NUMBER)
    {
        return info.flashData.flashInfo2.pump_power[pump_num];
    }
}

uint16_t CFlash::GetActivationTime(uint8_t pump_num)
{
    if(pump_num < PUMP_MAX_NUMBER)
    {
        return info.flashData.flashInfo2.activation_time[pump_num];
    }
}

uint8_t CFlash::GetStoredActiveTimes(uint8_t pump_num)
{
    if(pump_num < PUMP_MAX_NUMBER)
    {
        return info.flashData.flashInfo2.stored_active_times[pump_num];
    }
}

time_t CFlash::GetActiveTime(uint8_t pump_num, uint8_t i)
{
    time_t active_time = NOT_A_TIME;
    
    if(i <= info.flashData.flashInfo2.stored_active_times[pump_num] && pump_num < PUMP_MAX_NUMBER)
    {
        active_time = info.flashData.flashInfo2.sprinkler_active_time[i][pump_num];
    }

    return active_time;
}

void CFlash::SetPumpPower(uint8_t pump_num, uint8_t new_pump_power)
{
    if(pump_num < PUMP_MAX_NUMBER)
    {
        info.flashData.flashInfo2.pump_power[pump_num] = new_pump_power*255/100;
        flash_interface->eraseSection(start_address, sizeof(FlashInfo));    
        flash_interface->writeAnything(start_address, info, true);
    }
}

void CFlash::SetActivationTime(uint8_t pump_num, uint16_t new_activation_time)
{
    // Check on maximum number of pump activation times is done by the website
    if(pump_num < PUMP_MAX_NUMBER)
    {
        info.flashData.flashInfo2.activation_time[pump_num] = new_activation_time;
        flash_interface->eraseSection(start_address, sizeof(FlashInfo));
        flash_interface->writeAnything(start_address, info, true);
    }
}

void CFlash::ResetStoredActiveTimes(uint8_t pump_num)
{
    info.flashData.flashInfo2.stored_active_times[pump_num] = 0;
}

void CFlash::SetActiveTime(uint8_t pump_num, time_t active_time)
{
    time_t swap = 0;

    // add new element
    info.flashData.flashInfo2.sprinkler_active_time[info.flashData.flashInfo2.stored_active_times[pump_num]][pump_num] = active_time;
    info.flashData.flashInfo2.stored_active_times[pump_num]++;

    // order the array
    for(uint8_t i = 0; i < info.flashData.flashInfo2.stored_active_times[pump_num]; i++)
    {
        for(uint8_t j = 0; j < info.flashData.flashInfo2.stored_active_times[pump_num]; j++)
        {
            if(info.flashData.flashInfo2.sprinkler_active_time[j][pump_num] > info.flashData.flashInfo2.sprinkler_active_time[i][pump_num])
            {
                swap = info.flashData.flashInfo2.sprinkler_active_time[i][pump_num];
                info.flashData.flashInfo2.sprinkler_active_time[i][pump_num] = info.flashData.flashInfo2.sprinkler_active_time[j][pump_num];
                info.flashData.flashInfo2.sprinkler_active_time[j][pump_num] = swap;
            }
        }
    }

    flash_interface->eraseSection(start_address, sizeof(FlashInfo));
    flash_interface->writeAnything(start_address, info, true);
}

FlashVrs CFlash::GetVersion(void)
{
    return (FlashVrs)info.version;
}
