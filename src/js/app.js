Pebble.addEventListener("ready", function(){
  console.log("PebbleKit JS ready");
});

Pebble.addEventListener("showConfiguration", function(){
  var url = "http://briwestervelt.github.io/formal_config";
  
  console.log("Opening the config page: " + url);
  
  Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e){
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log("Config returned: " + JSON.stringify(configData));
  
  if(configData.backgroundColor){
    Pebble.sendAppMessage(
      {
        backgroundColor: parseInt(configData.backgroundColor, 16),
        tickColor: parseInt(configData.tickColor, 16),
        hourColor: parseInt(configData.hourColor, 16),
        minuteColor: parseInt(configData.minuteColor, 16),
        dotColor: parseInt(configData.dotColor, 16),
        dateColor: parseInt(configData.dateColor, 16),
        bluetoothVibes: configData.bluetoothVibes,
      },
      function(){
        console.log('data send success');
      },
      function(){
        console.log('data send failure');
      }
    );
  }
});