#include <Arduino.h>
#include <main.h>

#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/*
..#######..########........##.########..######..########..######.
.##.....##.##.....##.......##.##.......##....##....##....##....##
.##.....##.##.....##.......##.##.......##..........##....##......
.##.....##.########........##.######...##..........##.....######.
.##.....##.##.....##.##....##.##.......##..........##..........##
.##.....##.##.....##.##....##.##.......##....##....##....##....##
..#######..########...######..########..######.....##.....######.
*/

ESP8266WebServer server(80);          // web server
IPAddress apIP(192, 168, 10, 10);       // Adress AP

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
void style();                           // web style css
void styleaqua();                       // web style aqua css
void aquapng();                         // web aqua png
void lightpng();                        // web light png
void parampng();                        // web param png

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

    // localMillisAtUpdate = millis();
    // localEpoc = (hour * 60 * 60 + minute * 60 + second);

    // if(WiFi.status() == WL_CONNECTED) {
    //     ntpUDP.begin(localPort);            // Запуск UDP для получения времени
    //     timeUpdateNTP();                    // Обновление времени
    // }

    server.begin();
    server.on("/", fileindex);
    server.on("/index", fileindex);
    server.on("/aqua.html", fileaqua);
    server.on("style.css", style);
    server.on("/style.css", style);

    server.on("style-aqua.css", styleaqua);
    server.on("/style-aqua.css", styleaqua);
    server.on("aqua.png", aquapng);
    server.on("/aqua.png", aquapng);
    server.on("light.png", lightpng);
    server.on("/light.png", lightpng);
    server.on("params.png", parampng);
    server.on("/params.png", parampng);



    SPIFFS.begin();


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

void wifiConnect() {
    // printTime();
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

        // printTime();

        DEBUG("Start AP mode!!!");
        DEBUG("Wifi AP IP : ");
        DEBUG(WiFi.softAPIP());

        // updateTime();

        firstStart = 1;
    }
}

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
