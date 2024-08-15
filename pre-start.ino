#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

extern SemaphoreHandle_t wait_for_connection_semaphore;

// Replace with your network credentials
const char *ssidAP = "ESP32_AP";
const char *passwordAP = "12345678";

// Create an instance of the web connection_server on port 80
WebServer connection_server(80);

bool connection_established = false;

void PreStartApplication(void* parameter)
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  
  // Start as Access Point
  WiFi.softAP(ssidAP, passwordAP);
  SendSerialMessage("AP Started. Connect to %s with password: %s\n", ssidAP, passwordAP);

  // Initialize mDNS
  if (!MDNS.begin("smart-sprinkler"))
  {
    // Set the hostname to "esp32.local"
    SendSerialMessage("Error setting up MDNS responder!");
    while(1)
    {
    }
  }
  SendSerialMessage("mDNS responder started");

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
  SendSerialMessage("Connection established, closing pre-start server and stop task\n");

  xSemaphoreGive(wait_for_connection_semaphore);

  vTaskDelete(NULL);
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
  html += "<p>Select a network and enter the password to connect your ESP32 to Wi-Fi.</p>";
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
  WiFi.softAPdisconnect(true);  // Turn off AP mode
  WiFi.begin(ssid.c_str(), password.c_str());  // Connect to selected WiFi network

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(1000/portTICK_PERIOD_MS);
    SendSerialMessage(".");
  }
  
  SendSerialMessage("\nConnected!\n");
  connection_server.send(200, "text/plain", "Connected to " + ssid);
  connection_established = true;
}

