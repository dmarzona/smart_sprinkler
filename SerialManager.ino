char rcv_msg[50];

void SendSerialMessage(const char* string_message)
{
  xQueueSend(msg_queue, (void *)string_message, 10);
}

void SerialManagerTask(void* parameter)
{
    Serial.begin(115200);
    while(true)
    {
        if(xQueueReceive(msg_queue, (void *)&rcv_msg, 0) == pdTRUE)
        {
            Serial.print(rcv_msg);
            memset(rcv_msg, 0, 50);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    Serial.print("Task died");
    vTaskDelete(NULL);
}
