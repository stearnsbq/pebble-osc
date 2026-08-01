module.exports = [
  {
    "type": "heading",
    "defaultValue": "Pulsoid Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Here you can configure some settings for your pulsoid tracking"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Heart Rate"
      },
      {
        "type": "slider",
        "messageKey": "UpdateRate",
        "defaultValue": 15,
        "min": 1,
        "max": 600,
        "label": "Update Rate (in Seconds)"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Pulsoid Connection"
      },
      {
        "type": "input",
        "messageKey": "APIToken",
        "label": "Token",
        "defaultValue": ""
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
