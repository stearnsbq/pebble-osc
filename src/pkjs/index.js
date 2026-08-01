// Import the Clay package
var Clay = require("@rebble/clay");
// Load our Clay configuration file
var clayConfig = require("./config");
// Initialize Clay
var clay = new Clay(clayConfig);

// 59e1a644-9857-453c-b3b3-2cf8ea9f3a44

function sendHeartRate(token, heartRate) {
  const result = fetch("https://dev.pulsoid.net/api/v1/data", {
    method: "POST",
    headers: { "content-type": "application/json", "Authorization": `Bearer ${token}` },
    body: JSON.stringify({ measured_at: Date.now(), data: { heart_rate: heartRate } }),
  }).then(resp => resp.json()).then(console.log).catch(console.log);
}


function sendValidate(token, heartRate) {
  const result = fetch("https://dev.pulsoid.net/api/v1/token/validate", {
    method: "GET",
    headers: { "content-type": "application/json", "Authorization": `Bearer ${token}` },
  }).then(resp => resp.json()).then(console.log).catch(console.log);
}


Pebble.addEventListener("ready", function () {
  var claySettings = localStorage.getItem("clay-settings");

  if (claySettings) {

    token = "375d41c6-05bc-4d66-a4f4-7f10e3377539"
    console.log("sending heart rate")
    sendValidate(token, 50)
    sendHeartRate(token, 50)
  }
});
