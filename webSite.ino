#include <WebServer.h>
#include "webSite.h"
#include "src/WebsiteVariables.h"

WebServer server(80);

WebsiteVariables variables;

void webSiteTask(void* parameter)
{
    server.on("/", handleRoot);
    server.on("/settings.html", HTTP_GET, handleSettings);
    server.on("/settings.html", HTTP_POST, handleSettingsSubmit);
    server.onNotFound(handleNotFound);

    // Start the server
    server.begin();
    SendSerialMessage("HTTP server started\n");

    while(true)
    {
        server.handleClient();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void handleRoot()
{
    String temp_home(home);
    // Replace placeholders with actual variable values
    temp_home.replace("{{temperature}}", String(variables.GetTemperature()));
    temp_home.replace("{{pressure}}", String(variables.GetPressure()));
    temp_home.replace("{{current_sense}}", String(variables.GetCurrentSense()));
    temp_home.replace("{{log_line_1}}", String(variables.getString(0)));
    temp_home.replace("{{log_line_2}}", String(variables.getString(1)));
    temp_home.replace("{{log_line_3}}", String(variables.getString(2)));
    temp_home.replace("{{log_line_4}}", String(variables.getString(3)));
    temp_home.replace("{{log_line_5}}", String(variables.getString(4)));
    server.send(200, "text/html", temp_home);
}

void handleSettings() 
{
    // Read the contents of the file into a string and close it
    String temp_settings(settings);

    // Send the HTML page as the response
    server.send(200, "text/html", temp_settings);
}

void handleSettingsSubmit()
{
    // Check for POST parameters and update variables
    if (server.hasArg("activationTime"))
    {
        int temp_act_time = 0;
        temp_act_time = server.arg("activationTime").toInt();
        if (temp_act_time >= 0)
        {
            log("Activation time updated: %d --> %d", variables.GetActivationTime(), temp_act_time);
            variables.UpdateActivationTime(temp_act_time);
        }
    }
    
    if (server.hasArg("pumpPower"))
    {
        int temp_pump_power = 0;
        temp_pump_power = server.arg("pumpPower").toInt();
        if (temp_pump_power <=100 && temp_pump_power >=0)
        {
            log("Pump power updated: %d --> %d", variables.GetPumpPower(), temp_pump_power);
            variables.UpdatePumpPower(temp_pump_power);
        }
    }

    // Redirect to about page after updating variables
    server.sendHeader("Location", "/settings.html");
    server.send(303); // HTTP 303 See Other
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found");
}