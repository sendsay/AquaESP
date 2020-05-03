var xmlHttp = new XMLHttpRequest();


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
            document.getElementById("mqttserver").value = json.mqttserver;
            document.getElementById("mqttport").value = json.mqttport;
            document.getElementById("mqttUserName").value = json.mqttUserName;
            document.getElementById("mqttpass").value = json.mqttpass;
            document.getElementById("mqttid").value = json.mqttid;
            document.getElementById("mqttname2").value = json.mqttname2;
            document.getElementById("mqttswitchname21").value = json.mqttswitchname21;
            document.getElementById("mqttswitchname22").value = json.mqttswitchname22;
            document.getElementById("mqttname4").value = json.mqttname4;
            document.getElementById("mqttswitchname41").value = json.mqttswitchname41;
            document.getElementById("mqttswitchname42").value = json.mqttswitchname42;
            document.getElementById("mqttswitchname43").value = json.mqttswitchname43;
            document.getElementById("mqttswitchname44").value = json.mqttswitchname44;


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
                    "&upEdgeTemp=" + val('upEdgeTemp') + "&dnEdgeTemp=" + val('dnEdgeTemp') + "&mqttserver=" + val("mqttserver") +

                    "&mqttport=" + val('mqttport') + "&mqttUserName=" + val('mqttUserName') + "&mqttpass=" + val('mqttpass') +
                    "&mqttid=" + val('mqttid') + "&mqttname2=" + val('mqttname2') + "&mqttswitchname21=" + val('mqttswitchname21') +
                    "&mqttswitchname22" + val('mqttswitchname22') + "&mqttname4=" + val('mqttname4') +
                    "&mqttswitchname41" + val('mqttswitchname41') + "&mqttswitchname42" + val('mqttswitchname42') +
                    "&mqttswitchname43" + val('mqttswitchname43') + "&mqttswitchname44" + val('mqttswitchname44')

                    ;
    console.log("************* send to server ");
    console.log(content);

    xmlHttp.open('POST', content,true);
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
