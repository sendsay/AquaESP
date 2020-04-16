#include <Arduino.h>


#define DEBUG_ENABLE        //DEBUG MODE

#define HOME                // switch to home work

#ifdef DEBUG_ENABLE
#define DEBUG(x) Serial.println(x)
#else
#define DEBUG(x)
#endif

#define PIN_FEEDING D2                      // Pin motor feeding
#define PIN_FEEDLIMIT D0                    // Pin feed mechanism limit
#define PIN_BEEPER D5                       // Pin buzzer
#define ONE_WIRE_BUS D1                     // Aqua temp port adress

boolean firstStart = false;                 // Первый старт
boolean WIFI_connected = false;             // Флаг подкючекния к ВАЙФАЙ
byte amountNotStarts = 0;                   // Счет НЕ подключения

//time
long localEpoc = 0;
long localMillisAtUpdate = 0;
int hour=22, minute=40, second=42, month=4, day=6, dayOfWeek=6, year=2018;
unsigned int localPort = 2390;
bool statusUpdateNtpTime = 0;             // Если не 0, то обновление было удачным
// float timeZone = 2.0;                     // Временная зона для часов
long timeUpdate = 60000;                  // Период обновления времени
uint8_t hourTest[3], minuteTest[3];       // ??
int g_hour, g_minute, g_second, g_month=1, g_day, g_dayOfWeek, g_year;  // ??
// const char* ntpServerName = "ntp3.time.in.ua";   // Сервер обновления времени
const int NTP_PACKET_SIZE = 48;           // Размер пакета от сервера времени
byte packetBuffer[NTP_PACKET_SIZE];       // Буфер пакета времени
// float timeZone;                                     // Временная зона для часов
int summertime = 1;                                     // летнее время
#define LEAP_YEAR(Y) (((1970+Y)>0) && !((1970+Y)%4) && (((1970+Y)%100)||!((1970+Y)%400)))   // Високосні літа
static const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};        // Кількість днів у місяцях
int secFr, lastSecond, lastMinute;                    // Работа с временем

// Config structure
struct Config {
#ifdef HOME
    char ssid[50] = "PUTIN UTELE";
    char password[50] = "0674788273";
#else
    char ssid[50] = "SUERTEKSA CNC";
    char password[50] = "61347400";
#endif
    char ssidAP[50] = "Aquarium-AP";
    char passwordAP[50] = "" ;
    float timeZone = 2.0;
    char ntpServerName[50] = "ntp3.time.in.ua";
    int summerTime = 0;

    int feedTime = 7;       // hour for fish feeding
    float offsetPh = 1.4;   // offset pH sensor data
    float upEdgePh = 8.5;   // up edge pH sensor 
    float dnEdgePh = 6.8;   // down edge pH sensor 
    int upEdgeTemp = 28;    // up edge temp sensor 
    int dnEdgeTemp = 22;    // down edge temp sensor 
};

const char* fileConfigName = "/config.txt";
// Config structure

// Modes
enum Modes {NOTHING, CHECKSENSORS, FEEDFISH, ALARM};
int currMode = 0;

// Errors
enum Errors {NOERRORS, WATERLEVEL, TEMPERATURE, PH, EDC};

boolean waitFeedEnd = false;                // for fish feed flag
int waterTemp = 85;                       // water temp

int pHArray[40];   //Store the average value of the sensor feedback
int pHArrayIndex=0; 
static float pHValue, voltage;
float Offset = 0;         //deviation compensate

boolean alarmSignal = false;


//END.
