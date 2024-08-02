#include <stdarg.h>
#define BUFFER_SIZE 100

char rcv_msg[BUFFER_SIZE];
static QueueHandle_t msg_queue;

void SendSerialMessage(const char *format, ...)
{
  char buffer[BUFFER_SIZE];
  
  va_list args;  // Declare a variable to hold the argument list
  va_start(args, format);  // Initialize the argument list

  // Use vsnprintf to format the string and store it in the buffer
  vsnprintf(buffer, BUFFER_SIZE, format, args);
  xQueueSend(msg_queue, (void *)buffer, 10);
}

void SerialManagerTask(void* parameter)
{
    msg_queue = xQueueCreate(5, BUFFER_SIZE*sizeof(char));
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
