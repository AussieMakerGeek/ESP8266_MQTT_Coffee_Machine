

//
//  HTML PAGE
//
const char PAGE_Logging[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>MQTT Settings</strong>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td colspan="2"><hr></td></tr>
<tr><td align="right">Server:</td><td><input type="text" id="mqttserver" name="mqttserver" value=""></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td colspan="2"><hr></td></tr>
<tr><td align="right">Device ID:</td><td><div id="id"></div></td></tr>
<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>

<script>

function GetState()
{
  setValues("/admin/logging");
}

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
          setValues("/admin/logging");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>


)=====";

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_logging_html()
{
  
  if (server.args() > 0 )  // Save Settings
  {
    //String temp = "";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "mqttserver") config.mqttserver = urldecode(server.arg(i));
      WriteConfig();
    }  
  }
  server.send ( 200, "text/html", PAGE_Logging ); 
  Serial.println(__FUNCTION__); 
}



//
//   FILL THE PAGE WITH VALUES
//

void send_logging_values_html()
{

  String values ="";

  values += "mqttserver|" + (String) config.mqttserver + "|input\n";
  values += "id|" + (String) id + "|div\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
  
}
