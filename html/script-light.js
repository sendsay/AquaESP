var xmlHttp = new XMLHttpRequest();

console.log("OK");

if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
    xmlHttp.open('GET','/getData',true);
    xmlHttp.send();
    xmlHttp.onload = function(e) {

        var str = xmlHttp.responseText;

        json=JSON.parse(str);


        console.log(json);
    
    }
}