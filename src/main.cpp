#include <Arduino.h>
#include <main.h>

#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

/*
..#######..########........##.########..######..########..######.
.##.....##.##.....##.......##.##.......##....##....##....##....##
.##.....##.##.....##.......##.##.......##..........##....##......
.##.....##.########........##.######...##..........##.....######.
.##.....##.##.....##.##....##.##.......##..........##..........##
.##.....##.##.....##.##....##.##.......##....##....##....##....##
..#######..########...######..########..######.....##.....######.
*/

ESP8266WebServer server(80);            // web server
IPAddress apIP(192, 168, 10, 10);       // Adress AP
WiFiUDP ntpUDP;                         // UDP client for time
IPAddress timeServerIP;                 // server IP adress
Config config;                          // config structure


/*
.########..########...#######...######..########.########..##.....##.########..########..######.
.##.....##.##.....##.##.....##.##....##.##.......##.....##.##.....##.##.....##.##.......##....##
.##.....##.##.....##.##.....##.##.......##.......##.....##.##.....##.##.....##.##.......##......
.########..########..##.....##.##.......######...##.....##.##.....##.########..######....######.
.##........##...##...##.....##.##.......##.......##.....##.##.....##.##...##...##.............##
.##........##....##..##.....##.##....##.##.......##.....##.##.....##.##....##..##.......##....##
.##........##.....##..#######...######..########.########...#######..##.....##.########..######.
*/

void wifiConnect();                     // try connect to Wifi
void fileindex();                       // web index page
void fileaqua();                        // web aqua page
void filelight();                        // web light page
void style();                           // web style css
void styleaqua();                       // web style aqua css
void stylelight();                      // web style light css
void aquapng();                         // web aqua png
void lightpng();                        // web light png
void parampng();                        // web param png
void printTime();                       // print time for debug
void timeUpdateNTP();                   // update time from iNet
void getNTPtime();                      // get time from server

/*
..######..########.########.##.....##.########.
.##....##.##..........##....##.....##.##.....##
.##.......##..........##....##.....##.##.....##
..######..######......##....##.....##.########.
.......##.##..........##....##.....##.##.......
.##....##.##..........##....##.....##.##.......
..######..########....##.....#######..##.......
*/
void setup() {
    #ifdef DEBUG_ENABLE
        Serial.begin(57600);
    #endif

    DEBUG("");
    DEBUG("START >>>");

    wifiConnect();                      // try connect to Wifi

    localMillisAtUpdate = millis();
    localEpoc = (hour * 60 * 60 + minute * 60 + second);

    if(WiFi.status() == WL_CONNECTED) {
        ntpUDP.begin(localPort);            // Run UDP for take a time
        timeUpdateNTP();                    // update Time
    }

    //WEB
    server.begin();
    server.on("/", fileindex);
    server.on("/index", fileindex);
    server.on("/aqua.html", fileaqua);
    server.on("/light.html", filelight);
    server.on("style.css", style);
    server.on("/style.css", style);
    server.on("style-aqua.css", styleaqua);
    server.on("/style-aqua.css", styleaqua);
    server.on("style-light.css", stylelight);
    server.on("/style-light.css", stylelight);
    server.on("aqua.png", aquapng);
    server.on("/aqua.png", aquapng);
    server.on("light.png", lightpng);
    server.on("/light.png", lightpng);
    server.on("params.png", parampng);
    server.on("/params.png", parampng);
    //WEB

    //FS
    SPIFFS.begin();
    //FS
}

/*
.##........#######...#######..########.
.##.......##.....##.##.....##.##.....##
.##.......##.....##.##.....##.##.....##
.##.......##.....##.##.....##.########.
.##.......##.....##.##.....##.##.......
.##.......##.....##.##.....##.##.......
.########..#######...#######..##.......
*/

void loop() {
    server.handleClient();      // web server
}

/*
.########.##.....##.##....##..######..########.####..#######..##....##..######.
.##.......##.....##.###...##.##....##....##.....##..##.....##.###...##.##....##
.##.......##.....##.####..##.##..........##.....##..##.....##.####..##.##......
.######...##.....##.##.##.##.##..........##.....##..##.....##.##.##.##..######.
.##.......##.....##.##..####.##..........##.....##..##.....##.##..####.......##
.##.......##.....##.##...###.##....##....##.....##..##.....##.##...###.##....##
.##........#######..##....##..######.....##....####..#######..##....##..######.
*/

/*
.##......##.####.########.####.....######...#######..##....##.##....##.########..######..########
.##..##..##..##..##........##.....##....##.##.....##.###...##.###...##.##.......##....##....##...
.##..##..##..##..##........##.....##.......##.....##.####..##.####..##.##.......##..........##...
.##..##..##..##..######....##.....##.......##.....##.##.##.##.##.##.##.######...##..........##...
.##..##..##..##..##........##.....##.......##.....##.##..####.##..####.##.......##..........##...
.##..##..##..##..##........##.....##....##.##.....##.##...###.##...###.##.......##....##....##...
..###..###..####.##.......####.....######...#######..##....##.##....##.########..######.....##...
*/
void wifiConnect() {
    printTime();
    DEBUG("Connecting WiFi (ssid=" + String(ssid) + "  pass=" + String(password) + ") ");

#ifdef DEBUG_ENABLE
    if (!firstStart) Serial.println("Trying connecting to Wifi");
#endif

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    for (int i = 1; i < 21; i++)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            WIFI_connected = true;
            DEBUG("Wifi connected, IP adress : ");
            DEBUG(WiFi.localIP());
            // if (!firstStart)
            // {
            //     String msg = WiFi.localIP().toString();
            //     DEBUG(" IP: ");
            //     DEBUG(msg);
            // }
            firstStart = 1;
            amountNotStarts = 0;
            return;
        }
        DEBUG(".");
        if (!firstStart)
        {
            int j = 0;
            while (j < 500)
            {
                if (j % 10 == 0)
                    delay(5);
                j++;
                delay(2);
            }
        }
    }

    WiFi.disconnect();
    DEBUG(" Not connected!!!");
    amountNotStarts++;
    DEBUG("Amount of the unsuccessful connecting = ");
    DEBUG(amountNotStarts);
    if (amountNotStarts > 21)
        ESP.reset();
    if (!firstStart)
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(ssidAP, passwordAP);
        printTime();
        DEBUG("Start AP mode!!!");
        DEBUG("Wifi AP IP : ");
        DEBUG(WiFi.softAPIP());

        // updateTime();

        firstStart = 1;
    }
}

/*
.##......##.########.########.
.##..##..##.##.......##.....##
.##..##..##.##.......##.....##
.##..##..##.######...########.
.##..##..##.##.......##.....##
.##..##..##.##.......##.....##
..###..###..########.########.
*/

void fileindex() {
    File file = SPIFFS.open("/index.html.gz", "r");
    size_t sent = server.streamFile(file, "text/html");
    DEBUG("load index page");
}

void fileaqua() {
    File file = SPIFFS.open("/aqua.html.gz", "r");
    size_t sent = server.streamFile(file, "text/html");
    DEBUG("load aqua page");
}

void filelight() {
    File file = SPIFFS.open("/light.html.gz", "r");
    size_t sent = server.streamFile(file, "text/html");
    DEBUG("load light page");
}

void style() {
    File file = SPIFFS.open("/style.css.gz", "r");
    size_t sent = server.streamFile(file, "text/css");
    DEBUG("load style css");
}

void styleaqua() {
    File file = SPIFFS.open("/style-aqua.css.gz", "r");
    size_t sent = server.streamFile(file, "text/css");
    DEBUG("load aqua style css");
}
void stylelight() {
    File file = SPIFFS.open("/style-light.css.gz", "r");
    size_t sent = server.streamFile(file, "text/css");
    DEBUG("load light style css");
}

void aquapng() {
    File file = SPIFFS.open("/aqua.png", "r");
    size_t sent = server.streamFile(file, "image/png");
    DEBUG("load aqua png");
}
void lightpng() {
    File file = SPIFFS.open("/light.png", "r");
    size_t sent = server.streamFile(file, "image/png");
    DEBUG("load light png");
}
void parampng() {
    File file = SPIFFS.open("/params.png", "r");
    size_t sent = server.streamFile(file, "image/png");
    DEBUG("load param png");
}

/*
.##.....##.########..########.....###....########.########.########.####.##.....##.########
.##.....##.##.....##.##.....##...##.##......##....##..........##.....##..###...###.##......
.##.....##.##.....##.##.....##..##...##.....##....##..........##.....##..####.####.##......
.##.....##.########..##.....##.##.....##....##....######......##.....##..##.###.##.######..
.##.....##.##........##.....##.#########....##....##..........##.....##..##.....##.##......
.##.....##.##........##.....##.##.....##....##....##..........##.....##..##.....##.##......
..#######..##........########..##.....##....##....########....##....####.##.....##.########
*/
void timeUpdateNTP() {
    if(!WIFI_connected) return;
    printTime();
    statusUpdateNtpTime = 1;
    for(int timeTest = 0; timeTest < 3; timeTest++) {
        getNTPtime();

        DEBUG("Proba #"+String(timeTest+1)+"   "+String(g_hour)+":"+((g_minute<10)?"0":"")+String(g_minute)+":"+((g_second<10)?"0":"")+String(g_second)+":"+String(g_dayOfWeek)+":"+String(g_day)+":"+String(g_month)+":"+String(g_year));

        hourTest[timeTest] = g_hour;
        minuteTest[timeTest] = g_minute;
        if(statusUpdateNtpTime == 0) {
            printTime();
            DEBUG("ERROR TIME!!!\r\n");
                return;
        }
        if(timeTest > 0) {
            if((hourTest[timeTest] != hourTest[timeTest - 1]||minuteTest[timeTest] != minuteTest[timeTest - 1])) {
                statusUpdateNtpTime = 0;
                printTime();
                DEBUG("ERROR TIME!!!\r\n");
                return;
            }
        }
    }

    hour=g_hour;
    minute=g_minute;
    second=g_second;
    day=g_day;
    dayOfWeek=g_dayOfWeek;
    month=g_month;
    year=g_year;

    localMillisAtUpdate = millis();
    localEpoc = (hour * 60 * 60 + minute * 60 + second);

    printTime();

    DEBUG((day < 10 ? "0" : "") + String(day) + "." + (month < 10 ? "0" : "") + String(month) + "." + String(year) + " DW = " + String(dayOfWeek));
    DEBUG("Time update OK.");
}

/*
.########..########..####.##....##.########.########.####.##.....##.########
.##.....##.##.....##..##..###...##....##.......##.....##..###...###.##......
.##.....##.##.....##..##..####..##....##.......##.....##..####.####.##......
.########..########...##..##.##.##....##.......##.....##..##.###.##.######..
.##........##...##....##..##..####....##.......##.....##..##.....##.##......
.##........##....##...##..##...###....##.......##.....##..##.....##.##......
.##........##.....##.####.##....##....##.......##....####.##.....##.########
*/
void printTime() {
    DEBUG((hour < 10 ? "0" : "") + String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute) + ":" + (second < 10 ? "0" : "") + String(second) + "  ");
}

/*
..######...########.########.##....##.########.########..########.####.##.....##.########
.##....##..##..........##....###...##....##....##.....##....##.....##..###...###.##......
.##........##..........##....####..##....##....##.....##....##.....##..####.####.##......
.##...####.######......##....##.##.##....##....########.....##.....##..##.###.##.######..
.##....##..##..........##....##..####....##....##...........##.....##..##.....##.##......
.##....##..##..........##....##...###....##....##...........##.....##..##.....##.##......
..######...########....##....##....##....##....##...........##....####.##.....##.########
*/
void getNTPtime() {

    WiFi.hostByName(ntpServerName, timeServerIP);

    int cb;
    for(int i = 0; i < 3; i++){
        memset(packetBuffer, 0, NTP_PACKET_SIZE);
        packetBuffer[0] = 0b11100011;
        packetBuffer[1] = 0;
        packetBuffer[2] = 6;
        packetBuffer[3] = 0xEC;
        packetBuffer[12] = 49;
        packetBuffer[13] = 0x4E;
        packetBuffer[14] = 49;
        packetBuffer[15] = 52;
        ntpUDP.beginPacket(timeServerIP, 123);                       //NTP порт 123
        ntpUDP.write(packetBuffer, NTP_PACKET_SIZE);
        ntpUDP.endPacket();
        delay(800);                                                  // чекаємо пів секуни
        cb = ntpUDP.parsePacket();
        if(!cb) Serial.println("          no packet yet..." + String (i + 1));
        if(!cb && i == 2) {                                          // якщо час не отримано
        statusUpdateNtpTime = 0;
        return;                                                    // вихіз з getNTPtime()
        }
        if(cb) i = 3;
    }

    if(cb) {                                                      // якщо отримали пакет з серверу
        ntpUDP.read(packetBuffer, NTP_PACKET_SIZE);
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        const unsigned long seventyYears = 2208988800UL;        // Unix час станом на 1 січня 1970. в секундах, то 2208988800:
        unsigned long epoch = secsSince1900 - seventyYears;
        boolean summerTime;

        if(month < 3 || month > 10) summerTime = false;             // не переходимо на літній час в січні, лютому, листопаді і грудню
        if(month > 3 && month < 10) summerTime = true;              // Sommerzeit лічимо в квіні, травні, червені, липні, серпені, вересені
        if(((month == 3) && (hour + 24 * day)) >= (3 + 24 * (31 - (5 * year / 4 + 4) % 7)) || ((month == 10) && (hour + 24 * day)) < (3 + 24 * (31 - (5 * year / 4 + 1) % 7))) summerTime = true;
        epoch += (int)(timeZone*3600 + (3600*(summertime   /*isDayLightSaving*/ && summerTime)));

        g_year = 0;
        int days = 0;
        uint32_t time;
        time = epoch/86400;
        g_hour = (epoch % 86400L) / 3600;
        g_minute = (epoch % 3600) / 60;
        g_second = epoch % 60;
        g_dayOfWeek = (((time) + 4) % 7) + 1;
        while((unsigned)(days += (LEAP_YEAR(g_year) ? 366 : 365)) <= time) {    // Счет года
        g_year++;
        }
        days -= LEAP_YEAR(g_year) ? 366 : 365;
        time -= days;
        days = 0;
        g_month = 0;
        uint8_t monthLength = 0;
        for(g_month = 0; g_month < 12; g_month++){                      // Счет месяца
        if(g_month == 1){
            if(LEAP_YEAR(g_year)) monthLength = 29;
            else monthLength = 28;
        }
        else monthLength = monthDays[g_month];
        if(time >= monthLength) time -= monthLength;
        else break;
        }
        g_month++;
        g_day = time + 1;
        g_year += 1970;
        return;
    }
    DEBUG("Nie ma czasu(((");
}

/*
.##........#######.....###....########...######...#######..##....##.########.####..######..
.##.......##.....##...##.##...##.....##.##....##.##.....##.###...##.##........##..##....##.
.##.......##.....##..##...##..##.....##.##.......##.....##.####..##.##........##..##.......
.##.......##.....##.##.....##.##.....##.##.......##.....##.##.##.##.######....##..##...####
.##.......##.....##.#########.##.....##.##.......##.....##.##..####.##........##..##....##.
.##.......##.....##.##.....##.##.....##.##....##.##.....##.##...###.##........##..##....##.
.########..#######..##.....##.########...######...#######..##....##.##.......####..######..
*/
void loadConfiguration(const char *filename, Config &config) {
    File file = SPIFFS.open(filename, "r");

    const size_t capacity = JSON_OBJECT_SIZE(26) + 720;
    DynamicJsonDocument doc(capacity);

    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        DEBUG(F("Failed to read file, using default configuration"));
        DEBUG("Error is :");
        DEBUG(error.c_str());
    }

    //Wifi

#ifdef HOME
    strlcpy(config.ssid, doc["ssid"] | "PUTIN UTELE", sizeof(config.ssid));
    strlcpy(config.password, doc["password"] | "0674788273", sizeof(config.password));
#else
    strlcpy(config.ssid, doc["ssid"] | "SUERTEKSA CNC", sizeof(config.ssid));
    strlcpy(config.password, doc["password"] | "61347400", sizeof(config.password));
#endif

    strlcpy(config.ssidAP, doc["ssidAP"] | "AQUA_ROOM_AP", sizeof(config.ssidAP));
    strlcpy(config.passwordAP, doc["passwordAP"] | "", sizeof(config.passwordAP));
    //Time
    config.timeZone = doc["timezone"] | 2.0;
    config.summertime = doc["summertime"] | 1;
    strlcpy(config.ntpServerName, doc["ntpServerName"] | "ntp3.time.in.ua", sizeof(config.ntpServerName));

    file.close();
}
