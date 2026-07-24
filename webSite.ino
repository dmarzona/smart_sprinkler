#include <WebServer.h>
#include <Update.h>
#include "webSite.h"
#include "src/WebsiteVariables.h"

WebServer server(80);

WebsiteVariables variables;
extern CFlash application_information;

extern CPump pump;

void webSiteTask(void* parameter)
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_POST, handleActivatePump);
    server.on("/settings", handleSettings);
    server.on("/pump_settings", HTTP_POST, handlePumpSettings);
    server.on("/irrigation_start_times", HTTP_POST, handleIrrigationStartTimes);
    server.on("/updatePage", HTTP_GET, handleUpdatePage);
    server.on("/update", HTTP_POST, handleUpdateComplete, handleUpdateUpload);
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
    temp_home.replace("{{pressure}}", String(variables.GetHumidity()));
    temp_home.replace("{{current_sense}}", String(variables.GetCurrentSense()));
    server.send(200, "text/html", temp_home);
}

void handleSettings() 
{
    // Read the contents of the file into a string and close it
    String temp_settings(settings);
    for(uint8_t j = 0; j < PUMP_MAX_NUMBER; j++)
    {
        // Place holders
        String additional_entries_string = "";
        String entries_string = "";
        String pump_power_string = "";
        String activation_time_string = "";

        // Replace variables in the HTML template
        pump_power_string += "{{pump_power" + String(j) +"}}";
        activation_time_string += "{{activation_time" + String(j) +"}}";
        temp_settings.replace(pump_power_string, String(application_information.GetPumpPower(j)));
        temp_settings.replace(activation_time_string, String(application_information.GetActivationTime(j)));

        String additionalEntries = "";
        uint8_t entryCount = 0;

        uint8_t active_time_stored = application_information.GetStoredActiveTimes(j);
        if(active_time_stored > 0)
        {
            for (int i = 0; i < active_time_stored; i++)
            {
                CTime temp_variable;

                temp_variable.UpdateEpoch(application_information.GetActiveTime(j, i));

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

        additional_entries_string += "{{additionalEntries" + String(j) + "}}";
        entries_string += "{{entryCount" + String(j) + "}}";

        temp_settings.replace(additional_entries_string, additionalEntries);
        temp_settings.replace(entries_string, String(entryCount));
    }

    // Send the HTML page as the response
    server.send(200, "text/html", temp_settings);
}

void handlePumpSettings()
{
    // Check for POST parameters and update variables
    for(uint8_t i = 0; i < PUMP_MAX_NUMBER; i++)
    {
        const String activation_time_string = "activation_time" + String(i+1);
        const String pump_power_string = "pump_power" + String(i+1);
        if (server.hasArg(activation_time_string))
        {
            int temp_act_time = 0;
            temp_act_time = server.arg(activation_time_string).toInt();
            if (temp_act_time >= 0)
            {
                SendSerialMessage("Activation time updated: %d --> %d\n", application_information.GetActivationTime(i), temp_act_time);
                application_information.SetActivationTime(i, temp_act_time);
            }
        }
        
        if (server.hasArg(pump_power_string))
        {
            int temp_pump_power = 0;
            temp_pump_power = server.arg(pump_power_string).toInt();
            if (temp_pump_power <=100 && temp_pump_power >=0)
            {
                SendSerialMessage("Pump power updated: %d --> %d\n", application_information.GetPumpPower(i), temp_pump_power);
                application_information.SetPumpPower(i, temp_pump_power);
                switch(i)
                {
                    case 0: pump.setPwmDirection1(application_information.GetPumpPowerRaw(0)); break;
                    case 1: pump.setPwmDirection2(application_information.GetPumpPowerRaw(1)); break;
                }
            }
        }
    }

    // Redirect to about page after updating variables
    server.sendHeader("Location", "/settings");
    server.send(303); // HTTP 303 See Other
}

void handleIrrigationStartTimes()
{
    int8_t pump = -1;
    if (server.hasArg("pump_id"))
    {
        pump = server.arg("pump_id").toInt();
    }

    if(pump > 0)
    {
        pump--;
        application_information.ResetStoredActiveTimes(pump);
        for (int i = 0; i < 10; i++)
        {
            String varName = "entry" + String(i);
            if (server.hasArg(varName))
            {
                CTime temp_variable;

                temp_variable.UpdateEpoch(server.arg(varName).c_str());
                application_information.SetActiveTime(pump, temp_variable.GetEpoch());
                SendSerialMessage("New time added for pump %d: %s\n", pump, temp_variable.getTimeString());
            }
        }
    }

    server.sendHeader("Location", "/settings");
    server.send(303);
}

void handleActivatePump()
{
    int8_t pump_id = -1;
    if (server.hasArg("pump"))
    {
        pump_id = server.arg("pump").toInt();
        pump.sendEvent((CPump::PumpEntries)pump_id, application_information.GetActivationTime(0));
        SendSerialMessage("Pump %d override triggered\n", pump_id);
    }

    // Redirect back to the root page
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found");
}

void handleUpdatePage()
{
    server.send(200, "text/html", updatePage);
}

void handleUpdateComplete() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", Update.hasError() ? "FAIL" : "OK");
    ESP.restart();
}

void handleUpdateUpload() {
    HTTPUpload& upload = server.upload();

    switch (upload.status) {
        case UPLOAD_FILE_START:
            Serial.printf("Update: %s\n", upload.filename.c_str());

            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
            break;

        case UPLOAD_FILE_WRITE:
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
            break;

        case UPLOAD_FILE_END:
            if (Update.end(true)) {
                Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
            break;

        default:
            break;
    }
}