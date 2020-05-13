//////////////////////////////////////////////
// Aquarium controller 1.0                  //
// Shpakov Vladyslav aka SendSay            //
// 18.04.2020                               //
//                                          //
// Fish feeding = D2                        //
// Feeder limit = D0                        //
// Beeper       = D5                        //
// Ph meter     = D1                        //
// Water level  = D7                        //
//                                          //
// 5v for OpenAquarium      = VU            //
// 3.3v for Open Aquarium   = 3.3v          //
//                                          //
// sendsay34@gmail.com                      //
//                                          //
//////////////////////////////////////////////


//TODO: Add offline Jquery & bootstrap

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

#include <main.h>

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
void fileparams();                      // web params page
void filelight();                       // web light page
void style();                           // web style css
void styleaqua();                       // web style aqua css
void stylelight();                      // web style light css
void styleparams();                     // web style params css
void aquapng();                         // web aqua png
void lightpng();                        // web light png
void parampng();                        // web param png
void script();                          // web script js
void script_params();                   // web script  param js
void script_light();                    // web script  light js
void printTime();                       // print time for debug
void timeUpdateNTP();                   // update time from iNet
void getNTPtime();                      // get time from server
void printFile(const char *filename);   // print file for debug
void saveConfig(const char *filename, Config &config);  // load config from file
void loadConfiguration(const char *filename, Config &config);   // save config to file
void updateTime();                      // update time, work clock!
void sendData();                        // send data to web
void restart();                         // restart controller
void saveContent();                     // save web content
void getTemp();                         // get water temp timer func
void getSensorsData();                  // get and send sensors data
void feedFish();                        // manual feed fish
double avergearray(int* arr, int number);  //average array for pH meter
void shutOffSignal();                   // shut off alarm signal
void beepTime();                        // beep time when alarm
void beepDelayTime();                   // beep delay timer when alarm
void beepLongDelayTime();               // beep LONG delay timer when alarm
void delayCheckAlarmTime();             // delay after start check sensors
void beep(boolean beep);                // beep signal
void callback(char* topic, byte* payload, unsigned int length);     // call back MQTT
void switchRequestTime();               // request status switches
void sendSwStatusWeb();                 // web send status switches
void getSwitches();                     // get swithce data
void getSwitchesNames();                // get switches names

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
IPAddress apIP(192, 168, 10, 10);       // adress AP
WiFiUDP ntpUDP;                         // DP client for time
IPAddress timeServerIP;                 // server IP adress
Config config;                          // config structure
OneWire oneWire(ONE_WIRE_BUS);          // for aqua temp
DallasTemperature sensors(&oneWire);    // aqua temp probe
AlarmCodes alarm;                       // alarm codes;
Ticker beepTimer(beepTime, 15000, MILLIS);              // beep timer when alarm 15 sec
Ticker beepDelayTimer(beepDelayTime, 900000UL, MILLIS);    //beep delay timer when alarm 15 min
Ticker beepLongDelayTimer(beepLongDelayTime, 1080000UL, MILLIS);   //beep LONG delay timer when alarm 3 hrs
Ticker delayCheckAlarmTimer(delayCheckAlarmTime, 60000, MILLIS);   // delay chack alarm timer 1 min
Ticker switchRequestTimer(switchRequestTime, 60000);
WiFiClient ClientMqtt;                  //Wifi client for MQTT
PubSubClient clientMqtt(ClientMqtt);    // MQTT client
SwitchesMQTT switchMqtt;                // mqtt switches

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

    //FS
    SPIFFS.begin();
    //FS

    DEBUG("");
    DEBUG("START >>>");

    loadConfiguration(fileConfigName, config);  //loadconfig from file

    DEBUG("");

    wifiConnect();                      // try connect to Wifi

    localMillisAtUpdate = millis();
    localEpoc = (hour * 60 * 60 + minute * 60 + second);

    if(WiFi.status() == WL_CONNECTED) {
        ntpUDP.begin(localPort);            // Run UDP for take a time
        timeUpdateNTP();                    // update Time
    }

    //MQTT
    if (WiFi.status() == WL_CONNECTED) {
        clientMqtt.setServer(config.mqttserver, config.mqttport);
        clientMqtt.setCallback(callback);
        clientMqtt.connect(config.mqttid, config.mqttUserName, config.mqttpass);

        if (clientMqtt.connected()) {

            clientMqtt.subscribe("stat/sonoff2/RESULT");
            clientMqtt.subscribe("stat/sonoff4/RESULT");

            switchRequestTime();


        }
    }


    //WEB
    server.begin();
    server.on("/", fileindex);
    server.on("/index", fileindex);
    server.on("/aqua.html", fileaqua);
    server.on("/light.html", filelight);
    server.on("params.html", fileparams);
    server.on("/params.html", fileparams);
    server.on("style.css", style);
    server.on("/style.css", style);
    server.on("style-aqua.css", styleaqua);
    server.on("/style-aqua.css", styleaqua);
    server.on("style-light.css", stylelight);
    server.on("/style-light.css", stylelight);
    server.on("style-params.css", styleparams);
    server.on("/style-params.css", styleparams);
    server.on("aqua.png", aquapng);
    server.on("/aqua.png", aquapng);
    server.on("light.png", lightpng);
    server.on("/light.png", lightpng);
    server.on("params.png", parampng);
    server.on("/params.png", parampng);
    server.on("script.js", script);
    server.on("/script.js", script);
    server.on("script-params.js", script_params);
    server.on("/script-params.js", script_params);
    server.on("script-light.js", script_light);
    server.on("/script-light.js", script_light);

    server.on("/getData", sendData);
    server.on("/saveContent", saveContent);
    server.on("/restart", restart);
    server.on("/getSensorsData", getSensorsData);
    server.on("/feedFish", feedFish);
    server.on("/shutOffSignal", shutOffSignal);
    server.on("/getDataSwithes", sendSwStatusWeb);
    server.on("/switchesMode", getSwitches);
    server.on("/getSwitchesNames", getSwitchesNames);
    //WEB

    //PINS
    pinMode(PIN_FEEDING, OUTPUT);               // Motor feeder
    pinMode(PIN_FEEDLIMIT, INPUT);              // Feeder limit
    pinMode(PIN_BEEPER, OUTPUT);                // Buzzer
    //PINS

    sensors.begin();                            // start aqua temp probe

    switchRequestTimer.start();
    delayCheckAlarmTimer.start();               // delay check sensors

    DEBUG("RUNNING!!!");
}//SETUP

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

// WEB SERVER
    server.handleClient();

// BEEP TIMER
    beepTimer.update();
    beepDelayTimer.update();
    beepLongDelayTimer.update();

// MQTT
    clientMqtt.loop();

// DELAY CHECK SENSORS;
    delayCheckAlarmTimer.update();

//
    switchRequestTimer.update();

// WORK WITH TIME
    if(second != lastSecond) {
        lastSecond = second;
        secFr = 0;
    } else {
        secFr++;
    }

// WORK CLOCK
    updateTime();


// CHECK WIFI
    if ((second > 30 && second < 38) && (WiFi.status() != WL_CONNECTED || !WIFI_connected)) {
        WIFI_connected = false;

        if (secFr == 0) DEBUG("============> Check WIFI connect!!!");

        WiFi.disconnect();
        if(minute % 5 == 1) {
            wifiConnect();
            if(WiFi.status() == WL_CONNECTED) WIFI_connected = true;
        }
    }

// Get time every hour from server
    if ((minute == 59) and (second == 0) and (secFr == 0)) {
        if (WIFI_connected) {
           getNTPtime();               // ***** Получение времени из интернета
        }
    }

#ifdef DEBUG
    if ((minute == 0) and (second == 0) and (secFr == 0))
    {
        printTime();
    }
#endif

    //feeding
    if ((hour == config.feedTime) and (minute == 0) and (second == 0) and (secFr == 0))
    {
        currMode = FEEDFISH;            // time for fish feed
        waitFeedEnd = true;             // flag limit up

        printTime();
        DEBUG("Fish feeding");
    }



/*
.##.....##..#######..########..########..######.
.###...###.##.....##.##.....##.##.......##....##
.####.####.##.....##.##.....##.##.......##......
.##.###.##.##.....##.##.....##.######....######.
.##.....##.##.....##.##.....##.##.............##
.##.....##.##.....##.##.....##.##.......##....##
.##.....##..#######..########..########..######.
*/
    //Work modes
    switch (currMode) {

        case FEEDFISH:  //fishfeeding

            digitalWrite(PIN_FEEDING, HIGH);         // start feeder

            while ((digitalRead(PIN_FEEDLIMIT) == LOW) and (waitFeedEnd == true)) {}  // wait rotate

            waitFeedEnd = false;

            if (digitalRead(PIN_FEEDLIMIT) == LOW)      //end feeding
            {
                digitalWrite(PIN_FEEDING, LOW);
                waitFeedEnd = false;
                currMode = NOTHING;

                DEBUG("End feeding");
            }
        break;

    default:
        break;
    }


    // GET SENSORS
    // if ((second % 1 == 0) and (secFr == 0)) {
    if (secFr == 0) {
        // water temp
        sensors.requestTemperatures();
        waterTemp = sensors.getTempCByIndex(0);

        //Ph
        pHArray[pHArrayIndex++] = analogRead(A0);
        if(pHArrayIndex == 40) pHArrayIndex = 0;
        voltage = avergearray(pHArray, 40) * 3.3 / 1024;
        pHValue = 3.5 * voltage + config.offsetPh;


        //TDS

    }

    // CHECK LIMITS
    // water level
    if (delayCheckSensors == true) {

        if (digitalRead(PIN_WATERLOW) == HIGH) {
            alarmCode = alarmCode | alarm.WATER_LOW;
        } else {
            alarmCode = alarmCode &= ~alarm.WATER_LOW;
        }

        // temp
        if ((waterTemp < config.dnEdgeTemp) or (waterTemp > config.upEdgeTemp)) {
            alarmCode = alarmCode | alarm.TEMP;
        } else {
            alarmCode = alarmCode &= ~alarm.TEMP;
        }

        // pH
        if ((pHValue < config.dnEdgePh) or (pHValue > config.upEdgePh)) {
            alarmCode = alarmCode | alarm.PH;
        } else {
            alarmCode = alarmCode &= ~alarm.PH;
        }
    }

    // signal
    if (alarmCode != alarm.NO_ALARM) {
        while (alarmFlag2 == false) {   // doing once
            alarmFlag = true;
            beepTimer.start();
            alarmFlag2 = true;
        }

    } else {
        alarmFlag = false;
        alarmFlag2 = false;
        alarmFlag3 = false;
        beepTimer.stop();   // beep timer
        beepDelayTimer.stop();
        beepLongDelayTimer.stop();
    }

    if ((alarmFlag == true) and (alarmFlag3 == false) and (second %2 == 0)) {
        beep(true);
    } else {
        beep(false);
    }



}//LOOP

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
..######.....###....##.......##.......########.....###.....######..##....##
.##....##...##.##...##.......##.......##.....##...##.##...##....##.##...##.
.##........##...##..##.......##.......##.....##..##...##..##.......##..##..
.##.......##.....##.##.......##.......########..##.....##.##.......#####...
.##.......#########.##.......##.......##.....##.#########.##.......##..##..
.##....##.##.....##.##.......##.......##.....##.##.....##.##....##.##...##.
..######..##.....##.########.########.########..##.....##..######..##....##
*/

void callback(char* topic, byte* payload, unsigned int length) {

    String payLoad = "";
    String topiC = topic;


    for (int i = 0; i < length; i++) {
        payLoad += ((char)payload[i]);
    }

    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc, payLoad);

    if (error) {
        DEBUG(F("deserializeJson() failed: "));
        DEBUG(error.c_str());
        return;
    }

    // sonoff 2
    if (topiC.equals("stat/sonoff2/RESULT")) {
        String sw1 = doc["POWER1"];
        String sw2 = doc["POWER2"];

        // sw 1
        if (sw1.equals("ON")) {
            switchesMqtt = switchesMqtt | switchMqtt.SW2_1;
        } else if (sw1.equals("OFF")) {
            switchesMqtt = switchesMqtt &= ~switchMqtt.SW2_1;
        }

        //sw 2
        if (sw2.equals("ON")) {
            switchesMqtt = switchesMqtt | switchMqtt.SW2_2;
        } else if (sw2.equals("OFF")) {
            switchesMqtt = switchesMqtt &= ~switchMqtt.SW2_2;
        }
    }

    // sonoff 4
    if (topiC.equals("stat/sonoff4/RESULT")) {
        String sw1 = doc["POWER1"];
        String sw2 = doc["POWER2"];
        String sw3 = doc["POWER3"];
        String sw4 = doc["POWER4"];

        // sw 1
        if (sw1.equals("ON")) {
            switchesMqtt = switchesMqtt | switchMqtt.SW4_1;
        } else if (sw1.equals("OFF")) {
            switchesMqtt = switchesMqtt &= ~switchMqtt.SW4_1;
        }

        //sw 2
        if (sw2.equals("ON")) {
            switchesMqtt = switchesMqtt | switchMqtt.SW4_2;
        } else if (sw2.equals("OFF")) {
            switchesMqtt = switchesMqtt &= ~switchMqtt.SW4_2;
        }

        // sw 3
        if (sw3.equals("ON")) {
            switchesMqtt = switchesMqtt | switchMqtt.SW4_3;
        } else if (sw3.equals("OFF")) {
            switchesMqtt = switchesMqtt &= ~switchMqtt.SW4_3;
        }

        //sw 4
        if (sw4.equals("ON")) {
            switchesMqtt = switchesMqtt | switchMqtt.SW4_4;
        } else if (sw4.equals("OFF")) {
            switchesMqtt = switchesMqtt &= ~switchMqtt.SW4_4;
        }
    }

    sendSwStatusWeb();

}

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
    DEBUG("Connecting WiFi (ssid=" + String(config.ssid) + "  pass=" + String(config.password) + ") ");

#ifdef DEBUG_ENABLE
    if (!firstStart) Serial.println("Trying connecting to Wifi");
#endif

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.ssid, config.password);
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
        WiFi.softAP(config.ssidAP, config.passwordAP);
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

void fileparams() {
    File file = SPIFFS.open("/params.html.gz", "r");
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

void styleparams() {
    File file = SPIFFS.open("/style-params.css.gz", "r");
    size_t sent = server.streamFile(file, "text/css");
    DEBUG("load params style css");
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

void script() {
    File file = SPIFFS.open("/script.js.gz", "r");
    size_t sent = server.streamFile(file, "application/javascript");
}

void script_params() {
    File file = SPIFFS.open("/script-params.js.gz", "r");
    size_t sent = server.streamFile(file, "application/javascript");
}

void script_light() {
    File file = SPIFFS.open("/script-light.js.gz", "r");
    size_t sent = server.streamFile(file, "application/javascript");
}

void getSwitchesNames() {
    String json = "{";
    //wifi
    json += "\"switchname21\":\"";
    json += config.mqttswitchname21;   
    json += "\",\"switchname22\":\"";
    json += config.mqttswitchname22;   
    json += "\",\"switchname41\":\"";
    json += config.mqttswitchname41;   
    json += "\",\"switchname42\":\"";
    json += config.mqttswitchname42;   
    json += "\",\"switchname43\":\"";
    json += config.mqttswitchname43;   
    json += "\",\"switchname44\":\"";
    json += config.mqttswitchname44;   

    json += "\"}";

    server.send (200, "text/json", json);
    DEBUG("****send switches names");
    DEBUG(json); 
}

void sendData() {
    DEBUG("Send data in WEB");

    String json = "{";
    //wifi
    json += "\"ssid\":\"";
    json += config.ssid;
    json += "\",\"password\":\"";
    json += config.password;
    json += "\",\"ssidAP\":\"";
    json += config.ssidAP;
    json += "\",\"passwordAP\":\"";
    json += config.passwordAP;
    //Time
    json += "\",\"timezone\":\"";
    json += config.timeZone;
    json += "\",\"summertime\":\"";
    json += config.summerTime;
    json += "\",\"ntpServerName\":\"";
    json += config.ntpServerName;

    //MQTT
    json += "\",\"mqttserver\":\"";
    json += config.mqttserver;
    json += "\",\"mqttport\":\"";
    json += config.mqttport;
    json += "\",\"mqttUserName\":\"";
    json += config.mqttUserName;
    json += "\",\"mqttpass\":\"";
    json += config.mqttpass;
    json += "\",\"mqttid\":\"";
    json += config.mqttid;
    json += "\",\"mqttname2\":\"";
    json += config.mqttname2;
    json += "\",\"mqttswitchname21\":\"";
    json += config.mqttswitchname21;
    json += "\",\"mqttswitchname22\":\"";
    json += config.mqttswitchname22;
    json += "\",\"mqttname4\":\"";
    json += config.mqttname4;
    json += "\",\"mqttswitchname41\":\"";
    json += config.mqttswitchname41;
    json += "\",\"mqttswitchname42\":\"";
    json += config.mqttswitchname42;
    json += "\",\"mqttswitchname43\":\"";
    json += config.mqttswitchname43;
    json += "\",\"mqttswitchname44\":\"";
    json += config.mqttswitchname44;



    //Aqua
    json += "\",\"feedTime\":\"";
    json += config.feedTime;
    json += "\",\"offsetPh\":\"";
    json += config.offsetPh;
    json += "\",\"upEdgePh\":\"";
    json += config.upEdgePh;
    json += "\",\"dnEdgePh\":\"";
    json += config.dnEdgePh;
    json += "\",\"upEdgeTemp\":\"";
    json += config.upEdgeTemp;
    json += "\",\"dnEdgeTemp\":\"";
    json += config.dnEdgeTemp;


    //TODO: Add sending data to web here!

    json += "\"}";

    server.send (200, "text/json", json);
    DEBUG(json);
}

 void saveContent() {
    DEBUG("save web content!");

    //Wifi
    server.arg("ssid").toCharArray(config.ssid, 50) ;
    server.arg("password").toCharArray(config.password, 50) ;
    server.arg("ssidAP").toCharArray(config.ssidAP, 50) ;
    server.arg("passwordAP").toCharArray(config.passwordAP, 50) ;

    //Time
    config.timeZone = server.arg("timezone").toFloat();
    config.summerTime = server.arg("summertime").toInt();
    server.arg("ntpServerName").toCharArray(config.ntpServerName, 50) ;

    //MQTT
    //TODO: Add MQTT for sonOFF

    //Aqua
    config.feedTime = server.arg("feedTime").toInt();
    config.offsetPh = server.arg("offsetPh").toFloat();
    config.upEdgePh = server.arg("upEdgePh").toFloat();
    config.dnEdgePh = server.arg("dnEdgePh").toFloat();
    config.upEdgeTemp = server.arg("upEdgeTemp").toInt();
    config.dnEdgeTemp = server.arg("dnEdgeTemp").toInt();

    server.arg("mqttserver").toCharArray(config.mqttserver, 50);
    config.mqttport = server.arg("mqttport").toInt();
    server.arg("mqttUserName").toCharArray(config.mqttUserName, 50);
    server.arg("mqttpass").toCharArray(config.mqttpass, 50);
    server.arg("mqttid").toCharArray(config.mqttid, 50);
    server.arg("mqttname2").toCharArray(config.mqttname2, 50);
    server.arg("mqttswitchname21").toCharArray(config.mqttswitchname21, 50);
    server.arg("mqttswitchname22").toCharArray(config.mqttswitchname22, 50);
    server.arg("mqttname4").toCharArray(config.mqttname4, 50);
    server.arg("mqttswitchname41").toCharArray(config.mqttswitchname41, 50);
    server.arg("mqttswitchname42").toCharArray(config.mqttswitchname42, 50);
    server.arg("mqttswitchname43").toCharArray(config.mqttswitchname43, 50);
    server.arg("mqttswitchname44").toCharArray(config.mqttswitchname44, 50);



    // server.send(200, "text/json", "{\"Result\":\"OK\"}");
    saveConfig(fileConfigName, config);


}

void restart() {
    // server.send(200, "text/json", "OK");
    server.send(200, "text/json", "{\"Response\":\"OK\"}");
    delay(100);
    ESP.restart();
}

void getSensorsData() {
    String json = "{";
    //alarm code
    json += "\"alarmCode\":\"";
    json += alarmCode;
    //temp
    json += "\",\"temp\":\"";
    json += waterTemp;
    //pH
    json += "\",\"pHValue\":\"";
    json += pHValue;

    //TDS



    json += "\"}";
    server.send (200, "text/json", json);
}

void feedFish() {
    currMode = FEEDFISH;            // time for fish feed
    waitFeedEnd = true;             // flag limit up
}

void shutOffSignal() {
    beepTimer.stop();               // beep timer
    beepDelayTimer.stop();          // beep delay timer
    alarmFlag3 = true;
    beepLongDelayTimer.start();
    alarmFlag2 = true;

    DEBUG("SHUT OFF BUTTON");
    server.send(200, "text/json", "{\"Response\":\"OK\"}");
}

void getSwitches() {

    if (server.arg("id").equals("sw21")) {
        clientMqtt.publish( "cmnd/sonoff2/POWER1", server.arg("mode").c_str());
    }
    if (server.arg("id").equals("sw22")) {
        clientMqtt.publish( "cmnd/sonoff2/POWER2", server.arg("mode").c_str());
    }

    if (server.arg("id").equals("sw41")) {
        clientMqtt.publish( "cmnd/sonoff4/POWER1", server.arg("mode").c_str());
    }
    if (server.arg("id").equals("sw42")) {
        clientMqtt.publish( "cmnd/sonoff4/POWER2", server.arg("mode").c_str());
    }
    if (server.arg("id").equals("sw43")) {
        clientMqtt.publish( "cmnd/sonoff4/POWER3", server.arg("mode").c_str());
    }
    if (server.arg("id").equals("sw44")) {
        clientMqtt.publish( "cmnd/sonoff4/POWER4", server.arg("mode").c_str());
    }


    server.send(200, "text/json", "{\"Response\":\"OK\"}");
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

    WiFi.hostByName(config.ntpServerName, timeServerIP);

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
        epoch += (int)(config.timeZone*3600 + (3600*(summertime   /*isDayLightSaving*/ && summerTime)));

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
.##........#######.....###....########...........######...#######..##....##.########.####..######..
.##.......##.....##...##.##...##.....##.........##....##.##.....##.###...##.##........##..##....##.
.##.......##.....##..##...##..##.....##.........##.......##.....##.####..##.##........##..##.......
.##.......##.....##.##.....##.##.....##.........##.......##.....##.##.##.##.######....##..##...####
.##.......##.....##.#########.##.....##.........##.......##.....##.##..####.##........##..##....##.
.##.......##.....##.##.....##.##.....##.........##....##.##.....##.##...###.##........##..##....##.
.########..#######..##.....##.########..#######..######...#######..##....##.##.......####..######..
*/
void loadConfiguration(const char *filename, Config &config) {

    File file = SPIFFS.open("/config.txt", "r");


    const size_t capacity = JSON_OBJECT_SIZE(26) + 1200;
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
    strlcpy(config.ssid, doc["ssid"] | "4G-Gateway-792B", sizeof(config.ssid));
    strlcpy(config.password, doc["password"] | "Su61347400!", sizeof(config.password));
#endif
    strlcpy(config.ssidAP, doc["ssidAP"] | "AQUA_ROOM_AP", sizeof(config.ssidAP));
    strlcpy(config.passwordAP, doc["passwordAP"] | "", sizeof(config.passwordAP));

    //Time
    config.timeZone = doc["timezone"] | 2.0;
    config.summerTime = doc["summertime"] | 1;
    strlcpy(config.ntpServerName, doc["ntpServerName"] | "ntp3.time.in.ua", sizeof(config.ntpServerName));

    //MQTT


    //Aqua
    config.feedTime = doc["feedTime"] | 7;
    config.offsetPh = doc["offsetPh"] | -0.40;
    config.upEdgePh = doc["upEdgePh"] | 8.2;
    config.dnEdgePh = doc["dnEdgePh"] | 6.8;
    config.upEdgeTemp = doc["upEdgeTemp"] | 28;
    config.dnEdgeTemp = doc["dnEdgeTemp"] | 20;



    strlcpy(config.mqttserver, doc["mqttserver"] | "m24.cloudmqtt.com", sizeof(config.mqttserver));
    config.mqttport = doc["mqttport"] | 18654;
    strlcpy(config.mqttUserName, doc["mqttUserName"] | "vrE5bb", sizeof(config.mqttUserName));
    strlcpy(config.mqttpass, doc["mqttpass"] | "vfnKkvfo", sizeof(config.mqttpass));
    strlcpy(config.mqttid, doc["mqttid"] | "MyHome", sizeof(config.mqttid));
    strlcpy(config.mqttname2, doc["mqttname2"] | "MyHome", sizeof(config.mqttname2));
    strlcpy(config.mqttswitchname21, doc["mqttswitchname21"] | "MyHome", sizeof(config.mqttswitchname21));
    strlcpy(config.mqttswitchname22, doc["mqttswitchname22"] | "MyHome", sizeof(config.mqttswitchname22));
    strlcpy(config.mqttname4, doc["mqttname4"] | "MyHome", sizeof(config.mqttname4));
    strlcpy(config.mqttswitchname41, doc["mqttswitchname41"] | "MyHome", sizeof(config.mqttswitchname41));
    strlcpy(config.mqttswitchname42, doc["mqttswitchname42"] | "MyHome", sizeof(config.mqttswitchname42));
    strlcpy(config.mqttswitchname43, doc["mqttswitchname43"] | "MyHome", sizeof(config.mqttswitchname43));
    strlcpy(config.mqttswitchname44, doc["mqttswitchname44"] | "MyHome", sizeof(config.mqttswitchname44));



    // TODO: Add all parameters for loading


#ifdef DEBUG
    DEBUG("****** LOAD FILE ******");
    printFile(fileConfigName);
#endif

    file.close();
}

/*
..######.....###....##.....##.########..........######...#######..##....##.########.####..######..
.##....##...##.##...##.....##.##...............##....##.##.....##.###...##.##........##..##....##.
.##........##...##..##.....##.##...............##.......##.....##.####..##.##........##..##.......
..######..##.....##.##.....##.######...........##.......##.....##.##.##.##.######....##..##...####
.......##.#########..##...##..##...............##.......##.....##.##..####.##........##..##....##.
.##....##.##.....##...##.##...##...............##....##.##.....##.##...###.##........##..##....##.
..######..##.....##....###....########.#######..######...#######..##....##.##.......####..######..
*/
void saveConfig(const char *filename, Config &config) {

    SPIFFS.remove(filename);

   // Open file for writing
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        Serial.println(F("Failed to create file"));
        return;
    }

    const size_t capacity = JSON_OBJECT_SIZE(26) + 1200;
    DynamicJsonDocument doc(capacity);

    doc["ssid"] = config.ssid;
    doc["password"] = config.password;
    doc["ssidAP"] = config.ssidAP;
    doc["passwordAP"] = config.passwordAP;
    doc["timezone"] = config.timeZone;
    doc["summertime"] = config.summerTime;
    doc["ntpServerName"] = config.ntpServerName;
    doc["feedTime"] = config.feedTime;
    doc["offsetPh"] = config.offsetPh;
    doc["upEdgePh"] = config.upEdgePh;
    doc["dnEdgePh"] = config.dnEdgePh;
    doc["upEdgeTemp"] = config.upEdgeTemp;
    doc["dnEdgeTemp"] = config.dnEdgeTemp;
    doc["mqttserver"] = config.mqttserver;
    doc["mqttport"] = config.mqttport;
    doc["mqttUserName"] = config.mqttUserName;
    doc["mqttpass"] = config.mqttpass;
    doc["mqttid"] = config.mqttid;
    doc["mqttname2"] = config.mqttname2;
    doc["mqttswitchname21"] = config.mqttswitchname21;
    doc["mqttswitchname22"] = config.mqttswitchname22;
    doc["mqttname4"] = config.mqttname4;
    doc["mqttswitchname41"] = config.mqttswitchname41;
    doc["mqttswitchname42"] = config.mqttswitchname42;
    doc["mqttswitchname43"] = config.mqttswitchname43;
    doc["mqttswitchname44"] = config.mqttswitchname44;


    //TODO: Add all params for saving

    if (serializeJson(doc, file) == 0) {
        Serial.println(F("Failed to write to file"));
    }

#ifdef DEBUG
    DEBUG("****** SAVE FILE ******");
    printFile(fileConfigName);
#endif

    file.close();
}

/*
.########..########..####.##....##.########.........########.####.##.......########
.##.....##.##.....##..##..###...##....##............##........##..##.......##......
.##.....##.##.....##..##..####..##....##............##........##..##.......##......
.########..########...##..##.##.##....##............######....##..##.......######..
.##........##...##....##..##..####....##............##........##..##.......##......
.##........##....##...##..##...###....##............##........##..##.......##......
.##........##.....##.####.##....##....##....#######.##.......####.########.########
*/
void printFile(const char *filename) {
  // Open file for reading
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    DEBUG(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }

  // Close the file
  file.close();
}

/*
.##.....##.########..########.....###....########.########.........########.####.##.....##.########
.##.....##.##.....##.##.....##...##.##......##....##..................##.....##..###...###.##......
.##.....##.##.....##.##.....##..##...##.....##....##..................##.....##..####.####.##......
.##.....##.########..##.....##.##.....##....##....######..............##.....##..##.###.##.######..
.##.....##.##........##.....##.#########....##....##..................##.....##..##.....##.##......
.##.....##.##........##.....##.##.....##....##....##..................##.....##..##.....##.##......
..#######..##........########..##.....##....##....########.#######....##....####.##.....##.########
*/
void updateTime() {
  long curEpoch = localEpoc + ((millis() - localMillisAtUpdate) / 1000);
  long epoch = round(double((curEpoch + 86400L) % 86400L));
  hour = ((epoch % 86400L) / 3600) % 24;

  minute = (epoch % 3600) / 60;
  second = epoch % 60;
}


/*
....###....##.....##.########.########.....###.....######...########
...##.##...##.....##.##.......##.....##...##.##...##....##..##......
..##...##..##.....##.##.......##.....##..##...##..##........##......
.##.....##.##.....##.######...########..##.....##.##...####.######..
.#########..##...##..##.......##...##...#########.##....##..##......
.##.....##...##.##...##.......##....##..##.....##.##....##..##......
.##.....##....###....########.##.....##.##.....##..######...########
*/
double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}

void beepTime() {;
    alarmFlag = false;
    DEBUG("BEEP TIMER");
    printTime();
    beepDelayTimer.start();
}

void beepDelayTime() {
    alarmFlag2 = false;
    printTime();
    DEBUG("DELAY TIMER");
}

void beepLongDelayTime() {
    alarmFlag2 = false;
    alarmFlag3 = false;
    DEBUG("LONG DELAY TIMER");
}

void delayCheckAlarmTime() {
    delayCheckSensors = true;
    DEBUG("START CHECK SENSORS");
}

void beep(boolean beep) {
    if (beep)
    {
        tone(PIN_BEEPER, 3000);
    } else {
        noTone(PIN_BEEPER);
    }
}

void switchRequestTime() {

    clientMqtt.publish("cmnd/sonoff2/POWER1", "");
    clientMqtt.publish("cmnd/sonoff2/POWER2", "");
    clientMqtt.publish("cmnd/sonoff4/POWER1", "");
    clientMqtt.publish("cmnd/sonoff4/POWER2", "");
    clientMqtt.publish("cmnd/sonoff4/POWER3", "");
    clientMqtt.publish("cmnd/sonoff4/POWER4", "");
}

void sendSwStatusWeb() {

    String json = "{";
    //wifi
    json += "\"switches\":\"";
    json += switchesMqtt;
    json += "\"}";

    server.send (200, "text/json", json);
}



/*
.########.##....##.########.
.##.......###...##.##.....##
.##.......####..##.##.....##
.######...##.##.##.##.....##
.##.......##..####.##.....##
.##.......##...###.##.....##
.########.##....##.########.
*/
