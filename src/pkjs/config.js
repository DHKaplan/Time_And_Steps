'use strict';
/* eslint-disable quotes */

module.exports = [
  {
    "type": "heading",
    "id": "main-heading",
    "defaultValue": "Time and Steps Configuration",
    "size": 1
  },
  
  {
    "type": "text",
    "defaultValue": "Use this configuration page to choose the settings you would like applied to Time and Steps!" 
  },
  
      {
      "type": "section",
      "capabilities": ["COLOR"],
      "items": [
      {
        "type": "heading",
        "defaultValue": "COLOR SELECTION"
      },
        
     {
        "type": "color",
        "messageKey": "TIME_BG_COLOR_KEY",
        "defaultValue": "0000FF",
        "label": "Time Background Color",
        "sunlight": false
      },
      {
        "type": "color",
        "messageKey": "STEPS_BG_COLOR_KEY",
        "defaultValue": "005500",
        "label": "Steps Background Color",
        "sunlight": false
      },
      {
        "type": "color",
        "messageKey": "TIME_TEXT_COLOR_KEY",
        "defaultValue": "FFFFFF",
        "label": "Time Text Color",
        "sunlight": false
      },
      {
        "type": "color",
        "messageKey": "STEPS_TEXT_COLOR_KEY",
        "defaultValue": "FFFFFF",
        "label": "Steps Text Color",
        "sunlight": false
      } 
       ]
      }, 
       
         //****************
      {
        "type": "section",
        "items": [
      {
        "type": "heading",
         "defaultValue": "DATE FORMAT:"
      },
      {
        "type": "radiogroup",
        "messageKey": "DATE_FORMAT_KEY",
        "label": "",
        "defaultValue": "0",
        "options": [
          { "label": "Jun 5, 2016", "value": "0" },
          { "label": "5 Jun 2016",  "value": "1" },
        ]
      }  
        ]
      },
          
      {
        "type": "section",
        "items": [
      {
        "type": "heading",
         "defaultValue": "FAHRENHEIT/CELSIUS:"
      },
      {
        "type": "radiogroup",
        "messageKey": "TEMP_FORMAT_KEY",
        "label": "",
        "defaultValue": "0",
        "options": [
          { "label": "Fahrenheit", "value": "0" },
          { "label": "Celsius",    "value": "1" },
       ]
      }  
      ]
      } , 
       //**********************
          
           {
        "type": "section",
        "items": [
      {
        "type": "heading",
         "defaultValue": "BLUETOOTH:"
      },
      {
        "type": "toggle",
        "messageKey": "BT_LOSS_KEY",
        "label": "Vibrate on Loss of Bluetooth",
        "defaultValue": false
      },
      ]
      },  
        //******************   
      {
        "type": "section",
        "items": [
      {
        "type": "heading",
         "defaultValue": "LOW BATTERY"
      },
      {
        "type": "toggle",
        "messageKey": "LOW_BATTERY_KEY",
        "label": "Vibrate on Low Battery",
        "defaultValue": false
      },
      ]
      },  
      
       //*****************************
      
    {
    "type": "submit",
    "defaultValue": "Save"
    },
    {
    "type": "text",
    "defaultValue": "Time and Steps By WA1OUI V1.0",
          },
    
];   