#include <Arduino.h>


#define DEBUG_ENABLE        //DEBUG MODE

#ifdef DEBUG_ENABLE
#define DEBUG(x) Serial.println(x)
#else
#define DEBUG(x)
#endif

const char* ssid = "PUTIN UTELE";
const char* password = "0674788273";
String ssidAP      = "Aquarium-AP";      // Назва точки доступу
String passwordAP  = "" ;               // Пароль точки доступу

boolean firstStart = false;                  // Первый старт
boolean WIFI_connected = false;           // Флаг подкючекния к ВАЙФАЙ
byte amountNotStarts = 0;                 // Счет НЕ подключения
