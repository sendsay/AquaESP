var xmlHttp = new XMLHttpRequest();

var NO_ALARM = 0;
var WATER_LOW = 1;
var TEMP = 2;
var PH = 4;
var TDS = 8;

setInterval(getSensorsData, 1000);

function getData() {

    if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
        xmlHttp.open('GET','/getData',true);
        xmlHttp.send();
        xmlHttp.onload = function(e) {

            var str = xmlHttp.responseText;

            json=JSON.parse(str);

            //wifi
            document.getElementById("ssid").value = json.ssid;
            document.getElementById("password").value = json.password;
            document.getElementById("ssidAP").value = json.ssidAP;
            document.getElementById("passwordAP").value = json.passwordAP;
            //time
            document.getElementById("timezone").value = json.timezone;
            if (json.summertime == 1) {document.getElementById("summerTime").checked = true; }
            document.getElementById("ntpServerName").value = json.ntpServerName;

            //mqtt
            //TODO: Add MQTT for SonOFF

            //aqua
            document.getElementById("feedTime").value = json.feedTime;
            document.getElementById("offsetPh").value = json.offsetPh;
            document.getElementById("upEdgePh").value = json.upEdgePh;
            document.getElementById("dnEdgePh").value = json.dnEdgePh;
            document.getElementById("upEdgeTemp").value = json.upEdgeTemp;
            document.getElementById("dnEdgeTemp").value = json.dnEdgeTemp;

            console.log(str);
        }
    }
}

function val(id){
var v = document.getElementById(id).value;
return v;
}

function val_sw(nameSwitch) {
var switchOn = document.getElementById(nameSwitch);
if (switchOn.checked){
    return 1;
}
return 0;
}

function restartButton() {
    xmlHttp.open("GET", "/restart", true);
    xmlHttp.send();
}

function saveButton() {
    var content = "/saveContent?ssid=" + val('ssid') + "&password=" + val('password') + "&ssidAP=" + val('ssidAP') +
                    "&passwordAP=" + val('passwordAP') + "&timezone=" + val('timezone') + "&summertime=" + val_sw('summerTime') +
                    "&sigOn=" +"&ntpServerName=" + val('ntpServerName') + "&feedTime=" + val('feedTime') +
                    "&offsetPh=" + val('offsetPh') + "&upEdgePh=" + val('upEdgePh') + "&dnEdgePh=" + val('dnEdgePh') +
                    "&upEdgeTemp=" + val('upEdgeTemp') + "&dnEdgeTemp=" + val('dnEdgeTemp')
                    ;
    console.log("************* send to server ");
    console.log(content);

    xmlHttp.open('GET', content,true);
    xmlHttp.send();
}

function getSensorsData() {
    if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
        xmlHttp.open('GET','/getSensorsData',true);
        xmlHttp.send();
        xmlHttp.onload = function(e) {

            var str = xmlHttp.responseText;

            json=JSON.parse(str);

            // temperature
            temp = json.temp;
            calc = (440 - (440 * temp) / 30);
            $(".box:nth-child(1) svg circle:nth-child(2)").css("stroke-dashoffset", calc);
            document.getElementById("temp").innerHTML = temp + "<span> °C</span>";

            // pH
            pHValue = json.pHValue;
            calc = (440 - (440 * pHValue) / 10);
            $(".box:nth-child(2) svg circle:nth-child(2)").css("stroke-dashoffset", calc);
            document.getElementById("pH").innerHTML = pHValue + "<span> pH</span>";

            // EDC


            // ALARMS

            // temp
            if (json.alarmCode & TEMP) {
                $(".temp").addClass("alarm");
            } else {
                $(".temp").removeClass("alarm");
            }

            // ph
            if (json.alarmCode & PH) {
                $(".ph").addClass("alarm");
            } else {
                $(".ph").removeClass("alarm");
            }

            //tds
            if (json.alarmCode & TDS) {
                $(".tds").addClass("alarm");
            } else {
                $(".tds").removeClass("alarm");
            }
        }
    }
}

function feedButton() {
    xmlHttp.open("GET", "/feedFish", true);
    xmlHttp.send();
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
