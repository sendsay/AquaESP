var xmlHttp = new XMLHttpRequest();

var json;

var ALL_OFF = 0;
var SW2_1 = 1;
var SW2_2 = 2;
var SW4_1 = 4;
var SW4_2 = 8;
var SW4_3 = 16;
var SW4_4 = 32;

setInterval(getSwitches, 10000);

function clickSw(id, sw) {

    // if (json.alarmCode & sw) {
    //     document.getElementById(id).checked = true;
    // } else {
    //     document.getElementById(id).checked = false;
    // }

    console.log("OK");
}

function getSwitches() {
    if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
        xmlHttp.open('GET','/getDataSwithes',true);
        xmlHttp.send();
        xmlHttp.onload = function(e) {

            var str = xmlHttp.responseText;

            json=JSON.parse(str);

            console.log(json.switches);

            // sonoff 2
            if (json.switches & SW2_1) {
                document.getElementById("sw21").checked = true;
            } else {
                document.getElementById("sw21").checked = false;
            }
            if (json.switches & SW2_2) {
                document.getElementById("sw22").checked = true;
            } else {
                document.getElementById("sw22").checked = false;
            }

            // sonoff 4
            if (json.switches & SW4_1) {
                document.getElementById("sw41").checked = true;
            } else {
                document.getElementById("sw41").checked = false;
            }
            if (json.switches & SW4_2) {
                document.getElementById("sw42").checked = true;
            } else {
                document.getElementById("sw42").checked = false;
            }
            if (json.switches & SW4_3) {
                document.getElementById("sw43").checked = true;
            } else {
                document.getElementById("sw43").checked = false;
            }
            if (json.switches & SW4_4) {
                document.getElementById("sw44").checked = true;
            } else {
                document.getElementById("sw44").checked = false;
            }
        }
    }
}
