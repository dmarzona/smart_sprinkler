#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "src/CFlash.h"

extern SemaphoreHandle_t wait_for_connection_semaphore;

extern CFlash application_information;

// Replace with your network credentials
const char *ssidAP = "SMART_SPRINKLER";
const char *passwordAP = "12345678";

// Create an instance of the web connection_server on port 80
WebServer connection_server(80);

bool connection_established = false;

void PreStartApplication(void* parameter)
{
  bool needs_configurarion = true;

  WiFi.mode(WIFI_AP_STA);

  // Initialize mDNS
  if (!MDNS.begin("smart-sprinkler"))
  {
    SendSerialMessage("Error setting up MDNS responder!\n");
  }
  else
  {
    SendSerialMessage("mDNS responder started\n");
  }

  // Init communication with the external flash
  application_information.begin();
  
  // Check if it's the first time that the system has been turned on
  if(application_information.IsFlashVirgin())
  {
    application_information.InitializeFlashInfo();
    SendSerialMessage("Flash Info init\n");
  }

  // Check if there's already an SSID stored
  if(application_information.IsSSIDSet())
  {
    // Try to connect
    SendSerialMessage("Connection attempt to %s\n", application_information.GetWifiSSID());
    if(Connect(application_information.GetWifiSSID(), application_information.GetWifiPassword()))
    {
      needs_configurarion = false;
    }
    else
    {
      application_information.ClearWiFiInfo();
      ESP.restart();
    }
  }
  
  if(needs_configurarion)
  {
    // Start as Access Point
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssidAP, passwordAP);
    SendSerialMessage("AP Started. Connect to %s with password: %s\n", ssidAP, passwordAP);

    IPAddress IP = WiFi.softAPIP();
    SendSerialMessage("AP IP address: ");
    SendSerialMessage("%s\n", IP.toString());

    // Start Web Server
    connection_server.on("/", handleRootPreStart);
    connection_server.on("/connect", handleConnectPreStart);
    connection_server.begin();

    while(!connection_established)
    {
      connection_server.handleClient();
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    connection_server.stop();
  
  }

  SendSerialMessage("Connection established, closing pre-start server and stop task\n");

  xSemaphoreGive(wait_for_connection_semaphore);

  vTaskDelete(NULL);
}

bool Connect(const char* ssid, const char* password)
{
  bool ret_value = false;
  uint8_t tries = 0;
  static uint8_t MAX_TRIES = 30;
  
  WiFi.begin(ssid, password);  // Connect to selected WiFi network

  while (WiFi.status() != WL_CONNECTED && tries < MAX_TRIES)
  {
    vTaskDelay(500/portTICK_PERIOD_MS);
    SendSerialMessage("%d\n", WiFi.status());
    tries++;
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    ret_value = true;
  }

  return ret_value;
}

void handleRootPreStart() {
  String html = "<html><head><style>";
  html += "body { background-color: lightgray; font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  html += "form { background-color: white; padding: 20px; border-radius: 10px; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1); max-width: 400px; text-align: center; }";
  html += "h1 { text-align: center; }";
  html += "p { font-size: 14px; color: #333; }";
  html += "</style></head><body>";
  html += "<form action=\"/connect\" method=\"POST\">";
  html += "<h1>Wi-Fi Networks</h1>";
  html += "<p>Select a network and enter the password to connect your smart sprinkler to Wi-Fi.</p>";
  html += "<select name='ssid'>";

  int n = WiFi.scanNetworks();  
  
  for (int i = 0; i < n; ++i) {
    html += "<option value='" + String(WiFi.SSID(i)) + "'>";
    html += WiFi.SSID(i);
    html += " (" + String(WiFi.RSSI(i)) + " dBm)";
    html += "</option>";
  }
  html += "</select>";
  html += "<br>Password: <input type='password' name='password'><br><br>";
  html += "<input type='submit' value='Connect'></form></body></html>";
  
  connection_server.send(200, "text/html", html);

}

void handleConnectPreStart() {
  String ssid = connection_server.arg("ssid");
  String password = connection_server.arg("password");

  SendSerialMessage("Connecting to %s\n", ssid);

  if(Connect(ssid.c_str(), password.c_str()))  // Connect to selected WiFi network
  {
    SendSerialMessage("\nConnected!\n");
    connection_server.send(200, "text/plain", "Connected to " + ssid);
    application_information.SetWiFiSSID(ssid.c_str());
    application_information.SetWiFiPassword(password.c_str());
    connection_established = true;
  }
  else
  {
    connection_server.send(200, "text/plain", "Connection to " + ssid + " failed");
    SendSerialMessage("\nConnection to %s failed!\n", ssid.c_str());
    vTaskDelay(100/portTICK_PERIOD_MS);
    ESP.restart();
  }
  
  WiFi.softAPdisconnect(true);  // Turn off AP mode
}

