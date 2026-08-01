// Import the Clay package
var Clay = require("@rebble/clay");
// Load our Clay configuration file
var clayConfig = require("./config");
// Initialize Clay
var clay = new Clay(clayConfig);

const events = {
  connected: 0,
  socket_closed: 1,
  connect: 2,
  heart_rate: 3,
};

var socket;

function socket_open() {
  Pebble.sendAppMessage(
    { Event: events["connected"] },
    () => {
      console.log("sent socket connected event");
    },
    (e) => {
      console.log("Message failed: " + JSON.stringify(e));
    }
  );
}

function socket_closed(data) {}

function socket_message(data) {
  console.log("Websocket closed");

  Pebble.sendAppMessage(
    { Event: events["socket_closed"] },
    () => {
      console.log("sent socket closed event");
    },
    (e) => {
      console.log("Message failed: " + JSON.stringify(e));
    }
  );
}

function connectToRelay() {
  if (socket) {
    socket.removeEventListener("open", socket_open);

    socket.removeEventListener("message", socket_message);

    socket.removeEventListener("close", socket_message);
    socket.close();
  }

  var claySettings = localStorage.getItem("clay-settings");

  if (claySettings) {
    const settings = JSON.parse(claySettings);
    const host = settings["RelayHost"];
    const port = settings["RelayPort"];
    socket = new WebSocket(`ws://${host}:${port}`);
  }

  socket.addEventListener("open", socket_open);

  socket.addEventListener("message", socket_message);

  socket.addEventListener("close", socket_message);
}

Pebble.addEventListener("appmessage", (req) => {
  const payload = req.payload;

  if ("Event" in payload) {
    switch (payload["Event"]) {
      case events["connect"]: {
        connectToRelay();
      }
    }
  } else if ("HeartRate" in payload) {
    const heartRate = payload["HeartRate"];

    socket.send(
      JSON.stringify({ event: "heart_rate", data: heartRate })
    );
  }
});

Pebble.addEventListener("ready", function () {
  connectToRelay();
});
