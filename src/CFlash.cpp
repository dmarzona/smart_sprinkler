#include "Arduino.h"
#include "CFlash.h"

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
