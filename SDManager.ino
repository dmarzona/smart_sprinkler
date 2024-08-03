/*
Cosa deve fare:
- Accertarsi che ci sia l'SD
- Accertarsi della presenza della cartella log. Se non c'è la crea.
- Per ogni giorno, deve creare un file di log che ha come titolo la data
- analogamente al serial print, al file si appendono tutti i log
- Quando finisco di scrivere, chiuderei l'SD in modo che posso anche staccarla in caso.
così facendo ogni volta che scrivo qualcosa collego, scrivo, mi scollego. Se non posso scrivere,
vado avanti
- Verifico lo spazio: se comincio a scrivere troppa roba, allora meglio cancellare la roba vecchia

- Per il sito, mi aspetto che in questo caso ci sia una cartella web in cui 
dentro salvo l'html per fare il sito. Come fare, questo vedremo

- Per evitare confusione tra serial e SD, bisogna trovare un modo di fare una classe interfaccia
di log che in base ai parametri, sceglie se mandare su SD o su Seriale
*/
#include "src/CTime.h"

#define BUFFER_SIZE 100

static const int miso = 13;
static const int mosi = 11;
static const int clk  = 12;
static const int cs   = 10;

static QueueHandle_t log_msg_queue;
extern CTime current_epoch_time;

bool createDir(fs::FS &fs, const char *path)
{
    bool ret_value = false;
    // Check if dir already exist
    File dir = fs.open(path);
    if (!dir)
    {
        if(fs.mkdir(path))
        {
            SendSerialMessage("Dir created in %s\n", path);
            ret_value = true;
        }
        else
        {
            SendSerialMessage("Failed to create dir\n");
            ret_value = false;
        }
    }
    else
    {
        SendSerialMessage("Dir already created\n");
        ret_value = true;
    }
    return ret_value;
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    File file = fs.open(path, FILE_APPEND);
    
    if (file.print(message))
    {
        SendSerialMessage("Message appended\n");
    }
    else
    {
        SendSerialMessage("Append failed\n");
    }
    file.close();
}

void log(const char *format, ...)
{
  char buffer_SD[BUFFER_SIZE];
  
  va_list args;  // Declare a variable to hold the argument list
  va_start(args, format);  // Initialize the argument list

  // Use vsnprintf to format the string and store it in the buffer
  vsnprintf(buffer_SD, BUFFER_SIZE, format, args);
  xQueueSend(log_msg_queue, (void *)buffer_SD, 10);
}

void SDCardManager(void* parameter)
{ 
    bool start_task_loop = true;
    char rcv_msg[BUFFER_SIZE];

    log_msg_queue = xQueueCreate(5, BUFFER_SIZE*sizeof(char));

    SPI.begin(clk, miso, mosi, cs);
    if (!SD.begin(cs))
    {
        SendSerialMessage("Card Mount Failed");
        start_task_loop = false;
    }

    // Check for the presence of two folders
    // if not present, create them
    start_task_loop = createDir(SD, "/logs");
    start_task_loop = createDir(SD, "/website");

    while(start_task_loop)
    {
        if(xQueueReceive(log_msg_queue, (void *)&rcv_msg, 0) == pdTRUE)
        {
            char tmp_string[BUFFER_SIZE];
            char file_path[BUFFER_SIZE];
            snprintf(tmp_string, BUFFER_SIZE, "[%s]: %s\n", current_epoch_time.getDateString(), rcv_msg);
            snprintf(file_path, BUFFER_SIZE, "/logs/%s.log", current_epoch_time.getDateStringForFilename());
            appendFile(SD, file_path, tmp_string);
            memset(rcv_msg, 0, BUFFER_SIZE);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
