#include <WebServer.h>
#include "webSite.h"
#include "src/WebsiteVariables.h"

WebServer server(80);

WebsiteVariables variables;
extern CFlash application_information;

extern bool pump_override;

void webSiteTask(void* parameter)
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_POST, handleActivatePump);
    server.on("/settings", handleSettings);
    server.on("/pump_settings", HTTP_POST, handlePumpSettings);
    server.on("/irrigation_start_times", HTTP_POST, handleIrrigationStartTimes);
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

    // Replace variables in the HTML template
    temp_settings.replace("{{pump_power}}", String(application_information.GetPumpPower()));
    temp_settings.replace("{{activation_time}}", String(application_information.GetActivationTime()));

    String additionalEntries = "";
    uint8_t entryCount = 0;

    uint8_t active_time_stored = application_information.GetStoredActiveTimes();
    if(active_time_stored > 0)
    {
        for (int i = 0; i < active_time_stored; i++)
        {
            CTime temp_variable;

            temp_variable.UpdateEpoch(application_information.GetActiveTime(i));

            additionalEntries += "<div class=\"entry-group\">";
            additionalEntries += "<input type=\"text\" id=\"entry" + String(entryCount) + "\" name=\"entry" + String(entryCount) + "\" value=\"" + String(temp_variable.getTimeString()) + "\" placeholder=\"HH:MM:SS\">";
            additionalEntries += "<button type=\"button\" class=\"remove-btn\" onclick=\"removeEntry(this)\">Remove</button>";
            additionalEntries += "</div>";
            entryCount++;
        }
    }
    else
    {
        additionalEntries += "<div class=\"entry-group\">";
        additionalEntries += "<input type=\"text\" id=\"entry" + String(entryCount) + "\" name=\"entry" + String(entryCount) + "\" value=\"00:00:00\" placeholder=\"HH:MM:SS\">";
        additionalEntries += "<button type=\"button\" class=\"remove-btn\" onclick=\"removeEntry(this)\">Remove</button>";
        additionalEntries += "</div>";
        entryCount++;
    }

    temp_settings.replace("{{additionalEntries}}", additionalEntries);
    temp_settings.replace("{{entryCount}}", String(entryCount));

    // Send the HTML page as the response
    server.send(200, "text/html", temp_settings);
}

void handlePumpSettings()
{
    // Check for POST parameters and update variables
    if (server.hasArg("activation_time"))
    {
        int temp_act_time = 0;
        temp_act_time = server.arg("activation_time").toInt();
        if (temp_act_time >= 0)
        {
            log("Activation time updated: %d --> %d", application_information.GetActivationTime(), temp_act_time);
            application_information.SetActivationTime(temp_act_time);
        }
    }
    
    if (server.hasArg("pump_power"))
    {
        int temp_pump_power = 0;
        temp_pump_power = server.arg("pump_power").toInt();
        if (temp_pump_power <=100 && temp_pump_power >=0)
        {
            log("Pump power updated: %d --> %d", application_information.GetPumpPower(), temp_pump_power);
            application_information.SetPumpPower(temp_pump_power);
        }
    }

    // Redirect to about page after updating variables
    server.sendHeader("Location", "/settings");
    server.send(303); // HTTP 303 See Other
}

void handleIrrigationStartTimes()
{
    application_information.ResetStoredActiveTimes();
    for (int i = 0; i < 10; i++)
    {
        String varName = "entry" + String(i);
        if (server.hasArg(varName))
        {
            CTime temp_variable;

            temp_variable.UpdateEpoch(server.arg(varName).c_str());
            application_information.SetActiveTime(temp_variable.GetEpoch());
            SendSerialMessage("New time added: %s\n", temp_variable.getTimeString());
        }
    }

    server.sendHeader("Location", "/settings");
    server.send(303);
}

void handleActivatePump()
{
    pump_override = true;

    SendSerialMessage("Pump override triggered\n");

    // Redirect back to the root page
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found");
}