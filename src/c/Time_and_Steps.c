#include "pebble.h"
#include "math.h"

Window *window;

TextLayer *text_tod_layer;
TextLayer *text_steps_layer;

TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_location_layer;
TextLayer *text_degrees_layer;

TextLayer *text_numsteps_layer;
TextLayer *text_meters_layer;
TextLayer *text_miles_layer;

Layer     *BatteryLineLayer;
Layer     *BTLayer;

GFont     fontMonaco13;
GFont     fontRobotoCondensed23;
GFont     fontRobotoBoldSubset30;

GPoint     Linepoint;

static int BTConnected = 1;
static int BTVibesDone = 0;

static int  batterychargepct;
static int  BatteryVibesDone = 0;
static int  batterycharging = 0;
static int  FirstTime = 0;

static int  int_degrees  = 0;
static int  wxcall     = 1;

static char time_text[]         = "00:00a ";
static char date_text[]         = "  Dec 26, 2015";
static char seconds_text[]      = "00";
static char date_format[]       = "  %a %m/%d/%y";
static char text_location[18]   = "N/A";
static char text_degrees[]      = "N/A ";

static char steps_str[13];
static char meters_str[13];
static char meters1_str[13];
static char meters_out[15];
static char miles_str[13];
static char miles_tenths_str[3];
static char miles_out[13];

static long  long_tenth_miles = 0;
static long  long_miles = 0;
static long  long_tot_miles = 0;

static int  PersistTimeBG      = 0;
static int  PersistTimeText    = 0;
static int  PersistStepsText     = 0;
static char PersistDateFormat[]  =  "0";   // 0 = US, 1 = Intl
static char PersistTempFormat[]  =  "0";   // 0 = F, 1 = C
static int  PersistBTLoss       = 0;
static int  PersistLow_Batt     = 0;
static int  PersistStepsBG      = 0;

GColor TextColorHold1;
GColor TextColorHold2;
GColor BGColorHold2;
GColor ColorHold;

void handle_battery(BatteryChargeState charge_state) {
  batterychargepct = charge_state.charge_percent;

  if (charge_state.is_charging) {
    batterycharging = 1;
  } else {
    batterycharging = 0;
  }

  // Reset if Battery > 20% ********************************
  if (batterychargepct > 20) {
     if (BatteryVibesDone == 1) {     //OK Reset to normal
         BatteryVibesDone = 0;
     }
  }

  if ((batterychargepct < 30) && (PersistLow_Batt == 1) &&  (BatteryVibesDone == 0)) {
         APP_LOG(APP_LOG_LEVEL_INFO, ". . . . Battery Vibes!");
         BatteryVibesDone = 1;
         vibes_long_pulse();
      }

    layer_mark_dirty(BatteryLineLayer);
}

void battery_line_layer_update_callback(Layer *BatteryLineLayer, GContext* batctx) {
     graphics_context_set_fill_color(batctx, GColorWhite);

     graphics_fill_rect(batctx, layer_get_bounds(BatteryLineLayer), 3, GCornersAll);

     if (batterycharging == 1) {
          #ifdef PBL_PLATFORM_APLITE
             graphics_context_set_fill_color(batctx, GColorBlack);
          #else
             graphics_context_set_fill_color(batctx, GColorBlue);
          #endif

          graphics_fill_rect(batctx, GRect(2, 1, 100, 4), 3, GCornersAll);
     } else if (batterychargepct > 20) {
          #ifdef PBL_PLATFORM_APLITE
             graphics_context_set_fill_color(batctx, GColorBlack);
          #else
             graphics_context_set_fill_color(batctx, GColorGreen);
          #endif

          graphics_fill_rect(batctx, GRect(2, 1, batterychargepct, 4), 3, GCornersAll);
     } else {   // Battery 20% or less
          #ifdef PBL_PLATFORM_APLITE
             graphics_context_set_fill_color(batctx, GColorDarkGray);
          #else
             graphics_context_set_fill_color(batctx, GColorRed);
          #endif

          graphics_fill_rect(batctx, GRect(2, 1, batterychargepct, 4),3, GCornersAll);
     }

  //Battery % Markers
      #ifdef PBL_PLATFORM_APLITE
         if(batterycharging == 1) {
            graphics_context_set_fill_color(batctx, GColorBlack);
         } else {
            graphics_context_set_fill_color(batctx, GColorWhite);
         }
      #else
         graphics_context_set_fill_color(batctx, GColorBlack);
      #endif

      graphics_fill_rect(batctx, GRect(89, 1, 3, 4), 3, GCornerNone);
      graphics_fill_rect(batctx, GRect(79, 1, 3, 4), 3, GCornerNone);
      graphics_fill_rect(batctx, GRect(69, 1, 3, 4), 3, GCornerNone);
      graphics_fill_rect(batctx, GRect(59, 1, 3, 4), 3, GCornerNone);
      graphics_fill_rect(batctx, GRect(49, 1, 3, 4), 3, GCornerNone);
      graphics_fill_rect(batctx, GRect(39, 1, 3, 4), 3, GCornerNone);
      graphics_fill_rect(batctx, GRect(29, 1, 3, 4), 3, GCornerNone);
      graphics_fill_rect(batctx, GRect(19, 1, 3, 4), 3, GCornerNone);
      graphics_fill_rect(batctx, GRect(9,  1, 3, 4), 3, GCornerNone);
}

void handle_bluetooth(bool connected) {
      if (connected) {
         BTConnected = 1;     // Connected
         BTVibesDone = 0;
    } else {
         BTConnected = 0;      // Not Connected
    }

    layer_mark_dirty(BTLayer);
}

//BT Logo Callback;
void BTLine_update_callback(Layer *BTLayer, GContext* BT1ctx) {

       GPoint BTLinePointStart;
       GPoint BTLinePointEnd;

      if ((BTConnected == 0) && (PersistBTLoss == 1) && (BTVibesDone == 0)) {
              BTVibesDone = 1;
              vibes_long_pulse();
      }

      if(BTConnected == 0) {
          graphics_context_set_stroke_color(BT1ctx, GColorRed);
          graphics_context_set_fill_color(BT1ctx, GColorWhite);
          graphics_fill_rect(BT1ctx, layer_get_bounds(BTLayer), 0, GCornerNone);

        // "X"" Line 1
          BTLinePointStart.x = 1;
          BTLinePointStart.y = 1;

          BTLinePointEnd.x = 20;
          BTLinePointEnd.y = 20;
          graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

         // "X"" Line 2
          BTLinePointStart.x = 1;
          BTLinePointStart.y =20;

          BTLinePointEnd.x = 20;
          BTLinePointEnd.y = 1;
          graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

      } else {
       BTVibesDone = 0;

       graphics_context_set_stroke_color(BT1ctx, GColorWhite);
       graphics_context_set_fill_color(BT1ctx, BGColorHold2);


       //Line 1
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 1;

       BTLinePointEnd.x = 10;
       BTLinePointEnd.y = 20;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 1a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 1;

       BTLinePointEnd.x = 11;
       BTLinePointEnd.y = 20;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 2
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 1;

       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 6;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 2a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 1;

       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 5;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 3
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 5;

       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 15;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 3a
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 6;

       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 16;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 4
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 15;

       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 5;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 4a
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 16;

       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 6;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 5
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 20;

       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 15;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 5a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 20;

       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 16;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
      }
}

void ProcessNoBTPersist() {
  if (BTConnected == 0) {
     if(persist_exists(MESSAGE_KEY_TIME_BG_COLOR_KEY)) {
        PersistTimeBG = persist_read_int(MESSAGE_KEY_TIME_BG_COLOR_KEY);

        APP_LOG(APP_LOG_LEVEL_INFO, "In ProcessNoBT, PersistTimeBG = %d", PersistTimeBG);

        ColorHold = GColorFromHEX(PersistTimeBG);

        text_layer_set_background_color(text_time_layer,    ColorHold);
        text_layer_set_background_color(text_location_layer, ColorHold);
        text_layer_set_background_color(text_degrees_layer,  ColorHold);
        text_layer_set_background_color(text_date_layer,     ColorHold);
        text_layer_set_background_color(text_time_layer,     ColorHold);
     }

    if(persist_exists(MESSAGE_KEY_STEPS_BG_COLOR_KEY)) {
       PersistTimeBG = persist_read_int(MESSAGE_KEY_STEPS_BG_COLOR_KEY);

        APP_LOG(APP_LOG_LEVEL_INFO, "                 PersistTZ2lBG = %d", PersistTimeBG);

        ColorHold = GColorFromHEX(PersistTimeBG);

       text_layer_set_background_color(text_steps_layer,       ColorHold);
      // text_layer_set_background_color(text_date2_layer,     ColorHold);
      // text_layer_set_background_color(text_time2_layer,     ColorHold);

    }

    if(persist_exists(MESSAGE_KEY_TIME_TEXT_COLOR_KEY)) {
       PersistTimeText = persist_read_int(MESSAGE_KEY_TIME_TEXT_COLOR_KEY);

       APP_LOG(APP_LOG_LEVEL_INFO, "                 PersistTimeText = %d", PersistTimeText);

       ColorHold = GColorFromHEX(PersistTimeText);

       text_layer_set_text_color(text_time_layer,    ColorHold);
       text_layer_set_text_color(text_location_layer, ColorHold);
       text_layer_set_text_color(text_degrees_layer,  ColorHold);
       text_layer_set_text_color(text_date_layer,     ColorHold);
       text_layer_set_text_color(text_time_layer,     ColorHold);
    }

    if(persist_exists(MESSAGE_KEY_STEPS_TEXT_COLOR_KEY)) {
       PersistStepsText = persist_read_int(MESSAGE_KEY_STEPS_TEXT_COLOR_KEY);

       APP_LOG(APP_LOG_LEVEL_INFO, "                 PersistStepsText = %d", PersistStepsText);

       ColorHold = GColorFromHEX(PersistStepsText);

       TextColorHold2 = ColorHold; //Save for BT callback
       text_layer_set_text_color(text_steps_layer,       ColorHold);
    }
    //***********

    if(persist_exists(MESSAGE_KEY_DATE_FORMAT_KEY)) {
       persist_read_string(MESSAGE_KEY_DATE_FORMAT_KEY, PersistDateFormat, sizeof(PersistDateFormat));

       #ifdef PBL_PLATFORM_CHALK
             if (strcmp(PersistDateFormat, "0") == 0 ) {    // US
               strcpy(date_format,   " %a  %D");
             } else {
               strcpy(date_format,   " %a  %d/%m/%y");   //Intl
             }
         #else
             if (strcmp(PersistDateFormat, "0") == 0 ) { // US
                 strcpy(date_format, "%a   %D");
             } else {
                 strcpy(date_format, "%a   %d/%m/%y");   //Intl
             }
         #endif

        text_layer_set_text(text_date_layer, date_text);
    }

    if(persist_exists(MESSAGE_KEY_LOW_BATTERY_KEY)) {
                 PersistLow_Batt = persist_read_int(MESSAGE_KEY_LOW_BATTERY_KEY);
    }
  }
}

//***************************************************************
static void health_handler(HealthEventType event, void *context) {
  int strlength;
  
  // Which type of event occurred?
  switch(event) {
    case HealthEventSignificantUpdate:
      APP_LOG(APP_LOG_LEVEL_ERROR, 
              "New HealthService HealthEventSignificantUpdate event");
    
    case HealthEventMovementUpdate:
      APP_LOG(APP_LOG_LEVEL_ERROR, 
              "New HealthService HealthEventMovementUpdate event");
        {      
            //Steps
            time_t start = time_start_of_today();
            time_t end = time(NULL);
            
            HealthMetric metric = HealthMetricStepCount;

            HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);

            if(mask & HealthServiceAccessibilityMaskAvailable) {                
                int steps = (int)health_service_sum_today(metric);
                APP_LOG(APP_LOG_LEVEL_INFO, "Steps from Health = %d", steps);
                //steps = 500000; // Test value
              
                if (steps > 999) {
                    snprintf(steps_str, 13, "%d,%03d Steps", steps/1000, steps%1000);            
                } else {
                    snprintf(steps_str, 13, "%d Steps", steps);
                }
            } else {
                strcpy(steps_str, "No Step Data");
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps Unavailable");
            }
          
          metric = HealthMetricWalkedDistanceMeters;
          if(mask & HealthServiceAccessibilityMaskAvailable) {      
                int meters = (int)health_service_sum_today(metric);                
                APP_LOG(APP_LOG_LEVEL_INFO, "Meters from Health = %d", meters);
               
                //meters = 500000; // TEST VALUE 3500 meters = 2.174 miles / 500 meters = 0.31 miles / 50,000 meters = 31.06 mi
            
            //Meters
            if (meters > 999) {
                    snprintf(meters_str,  5, "%d", meters / 1000);
                    snprintf(meters1_str, 3, "%d", meters % 1000);
                    strcpy(meters_out, meters_str);
                    strcat(meters_out, ".");
                    strncat(meters_out, meters1_str, 2);
                    strcat(meters_out, " K");                       
                } else {
                    snprintf(meters_out, 13, "%d Meters", meters);
                }
                APP_LOG(APP_LOG_LEVEL_INFO, "meters_str = %s", meters_str);
            
            //*************************************************************
            //Miles 
            
            if (meters > 160903) {       // > 100 miles
                    snprintf(miles_str, 4, "%d",   (meters * 100) / 160934);
                    long_tot_miles = (meters * 10000) / 160934;
                    APP_LOG(APP_LOG_LEVEL_INFO, "long_tot_miles = %ld", long_tot_miles);
                    
                    //tenth miles = total - unit miles
                    long_miles = long_tot_miles / 100;
                    APP_LOG(APP_LOG_LEVEL_INFO, "long miles = %ld", long_miles);
                    long_miles = long_miles * 100;
                    APP_LOG(APP_LOG_LEVEL_INFO, "long miles = %ld", long_miles);
              
                    long_tenth_miles = long_tot_miles  - long_miles; 
                    APP_LOG(APP_LOG_LEVEL_INFO, "long tenth miles = %ld", long_tenth_miles);
                    
                    snprintf(miles_tenths_str, 3, "%ld", long_tenth_miles);
              
                    strlength  = strlen(miles_tenths_str);
              
                    APP_LOG(APP_LOG_LEVEL_INFO, "long_tenth_miles = %ld, strlen = %d", long_tenth_miles, strlength);
                    
              
                    snprintf(miles_tenths_str, 3, "%ld", long_tenth_miles);
                    strcpy(miles_out, miles_str);
                    strcat(miles_out, ".");
                    if (strlength == 1) {
                       strcat(miles_out, "0");
                    }
              
                    strncat(miles_out, miles_tenths_str, 2);

                    strcat(miles_out, " Miles"); 
                         APP_LOG(APP_LOG_LEVEL_INFO, "Miles_out = %s", miles_out);
              
               } else if (meters > 1609) { // > 1 mile
                    snprintf(miles_str, 3, "%d",   (meters * 100) / 160934);
                    long_tot_miles = (meters * 10000) / 160934;
                    APP_LOG(APP_LOG_LEVEL_INFO, "long_tot_miles = %ld", long_tot_miles);
                    
                    //tenth miles = total - unit miles
                    long_miles = long_tot_miles / 100;
                    APP_LOG(APP_LOG_LEVEL_INFO, "long miles = %ld", long_miles);
                    long_miles = long_miles * 100;
                    APP_LOG(APP_LOG_LEVEL_INFO, "long miles = %ld", long_miles);
              
                    long_tenth_miles = long_tot_miles  - long_miles; 
                    APP_LOG(APP_LOG_LEVEL_INFO, "long tenth miles = %ld", long_tenth_miles);
                    
                    snprintf(miles_tenths_str, 3, "%ld", long_tenth_miles);
              
                    strlength  = strlen(miles_tenths_str);
              
                    APP_LOG(APP_LOG_LEVEL_INFO, "long_tenth_miles = %ld, strlen = %d", long_tenth_miles, strlength);
                    
              
                    snprintf(miles_tenths_str, 3, "%ld", long_tenth_miles);
                    strcpy(miles_out, miles_str);
                    strcat(miles_out, ".");
                    if (strlength == 1) {
                       strcat(miles_out, "0");
                    }
              
                    strncat(miles_out, miles_tenths_str, 2);

                    strcat(miles_out, " Miles"); 
                    APP_LOG(APP_LOG_LEVEL_INFO, "Miles_out = %s", miles_out);             
              
                } else {      // < 1 mile
                   long_tenth_miles = (meters * 1000)  / 160934;
                   snprintf(miles_tenths_str, 3, "%ld", long_tenth_miles);
                   
                   strcpy(miles_out, "0.");
                   strcat(miles_out, miles_tenths_str);
                   strcat(miles_out, " Miles");
                   APP_LOG(APP_LOG_LEVEL_ERROR, "Milesout = %s", miles_out);
                }
            
            } else {
                strcpy(meters_out, " ");
                strcpy(miles_out, "No Dist Data");
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Dist Unavailable");
            }
        }
    
    case HealthEventSleepUpdate:
      APP_LOG(APP_LOG_LEVEL_ERROR, 
              "New HealthService HealthEventSleepUpdate event");
      break;
    case HealthEventHeartRateUpdate:
      APP_LOG(APP_LOG_LEVEL_ERROR,
              "New HealthService HealthEventHeartRateUpdate event");
    break;
    case HealthEventMetricAlert:
      APP_LOG(APP_LOG_LEVEL_ERROR,
              "New HealthService New HealthEventMetricAlert");
      break;
  }
  
}

//************************************************************************************************************
void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  char time_format[] = "%I:%M%p   ";
  char time_hold[]   = "09:02A";

  strftime(seconds_text, sizeof(seconds_text), "%S", tick_time);

  //******************************************************************
  if((strcmp(seconds_text,"00") == 0) || (FirstTime == 0)) {
     FirstTime = 1;

    if(BTConnected == 0) {
        ProcessNoBTPersist();
    }
    
     if (clock_is_24h_style()) {
        strcpy(time_format,"%R%"); //24 hour HH:MM
     } else {
        strcpy(time_format,"%I:%M%P"); //12 hour HH:MM   %p = AM/PM
     }

    //Process Local Time
     strftime(time_text, sizeof(time_text), time_format, tick_time);

     if(!clock_is_24h_style()) {
        strncpy(time_hold, time_text, 6);
        strcpy(time_text, time_hold);
     }

     text_layer_set_text(text_time_layer,  time_text);

     strftime(date_text,  sizeof(date_text), date_format, tick_time);
     text_layer_set_text(text_date_layer,  date_text);  
    
     //Health Data
     text_layer_set_text(text_numsteps_layer, steps_str);
     text_layer_set_text(text_miles_layer,    miles_out);
     text_layer_set_text(text_meters_layer,   meters_out);


  }  // End of First Time or 00 seconds


   if(((tick_time->tm_min % 15 == 0)  && (tick_time->tm_sec == 0)) || (wxcall == 1)){

      // =================== Get weather:
      // Only update wx/location every 15 minutes or when called

      APP_LOG(APP_LOG_LEVEL_ERROR, "In wx Processing %d:%d / wxcall = %d", tick_time->tm_min, tick_time->tm_sec, wxcall);

      // Begin dictionary
      DictionaryIterator *iter;
      app_message_outbox_begin(&iter);

      // Add a key-value pair
      dict_write_uint8(iter, 0, 0);

      // Send the message!
      APP_LOG(APP_LOG_LEVEL_INFO, "before outbox send in wx processing");
      app_message_outbox_send();
    }

  FirstTime = 1;
}

//<============================================================================================
//Receive Input from Config html page and location/temp:
void inbox_received_callback(DictionaryIterator *iterator, void *context) {
   FirstTime = 0;

  //****************
    APP_LOG(APP_LOG_LEVEL_WARNING, "In Inbox received callback*****************************************\n");

       #ifdef PBL_COLOR
          Tuple *TIME_BG_COLOR = dict_find(iterator, MESSAGE_KEY_TIME_BG_COLOR_KEY);

         if (TIME_BG_COLOR) { // Config Exists
             PersistTimeBG = TIME_BG_COLOR->value->int32;
             ColorHold = GColorFromHEX(PersistTimeBG);
             persist_write_int(MESSAGE_KEY_TIME_BG_COLOR_KEY,  PersistTimeBG );

             APP_LOG(APP_LOG_LEVEL_WARNING,    "    Added Config Time BG Color: %d", PersistTimeBG);
         } else { //Check for Persist
               if(persist_exists(MESSAGE_KEY_TIME_BG_COLOR_KEY)) {
                  PersistTimeBG = persist_read_int(MESSAGE_KEY_TIME_BG_COLOR_KEY);
                  ColorHold = GColorFromHEX(PersistTimeBG);
                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Persistant Time BG Color = %d", PersistTimeBG);
               }  else {   // Set Default
                  PersistTimeBG = 255;
                  ColorHold = GColorFromHEX(PersistTimeBG);

                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Default Time BG Color %d", PersistTimeBG);
               }
            }

         persist_write_int(MESSAGE_KEY_TIME_BG_COLOR_KEY,   PersistTimeBG);

       #else
         ColorHold = GColorBlack;
       #endif

         text_layer_set_background_color(text_tod_layer,     ColorHold);
         text_layer_set_background_color(text_time_layer,     ColorHold);
         text_layer_set_background_color(text_location_layer, ColorHold);
         text_layer_set_background_color(text_degrees_layer,  ColorHold);
         text_layer_set_background_color(text_date_layer,     ColorHold);
         text_layer_set_background_color(text_time_layer,     ColorHold);

  //****************

     #ifdef PBL_COLOR
        Tuple *STEPS_BG_COLOR =  dict_find(iterator, MESSAGE_KEY_STEPS_BG_COLOR_KEY);

         if (STEPS_BG_COLOR) { // Config Exists
             PersistStepsBG = STEPS_BG_COLOR->value->int32;
             ColorHold = GColorFromHEX(PersistStepsBG);
             persist_write_int(MESSAGE_KEY_STEPS_BG_COLOR_KEY,  PersistStepsBG);

             APP_LOG(APP_LOG_LEVEL_WARNING,    "    Added Config Steps BG Color: %d", PersistStepsBG);
         } else { //Check for Persist
               if(persist_exists(MESSAGE_KEY_STEPS_BG_COLOR_KEY)) {
                  PersistStepsBG = persist_read_int(MESSAGE_KEY_STEPS_BG_COLOR_KEY);
                  ColorHold = GColorFromHEX(PersistStepsBG);
                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Persistant Steps BG Color = %d", PersistStepsBG);
               }  else {   // Set Default
                  PersistStepsBG = 21760;
                  ColorHold = GColorFromHEX(PersistStepsBG);

                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Default Steps BG Color %d", PersistStepsBG);
               }
            }

         persist_write_int(MESSAGE_KEY_STEPS_BG_COLOR_KEY, PersistStepsBG);
       #else
         ColorHold = GColorWhite;
       #endif

         text_layer_set_background_color(text_steps_layer,       ColorHold);
         text_layer_set_background_color(text_numsteps_layer,    ColorHold);
         text_layer_set_background_color(text_meters_layer,    ColorHold);
         //text_layer_set_background_color(text_time2_layer,     ColorHold);

  //****************

    #ifdef PBL_COLOR
        Tuple *TIME_TEXT_COLOR =  dict_find(iterator, MESSAGE_KEY_TIME_TEXT_COLOR_KEY);

         if (TIME_TEXT_COLOR) { // Config Exists
             PersistTimeText = TIME_TEXT_COLOR->value->int32;
             ColorHold = GColorFromHEX(PersistTimeText);
             persist_write_int(MESSAGE_KEY_TIME_TEXT_COLOR_KEY, TIME_TEXT_COLOR->value->int32);

             APP_LOG(APP_LOG_LEVEL_WARNING,    "    Added Config Time Text Color: %d", PersistTimeText);
         } else { //Check for Persist
               if(persist_exists(MESSAGE_KEY_TIME_TEXT_COLOR_KEY)) {
                  PersistTimeText = persist_read_int(MESSAGE_KEY_TIME_TEXT_COLOR_KEY);
                  ColorHold = GColorFromHEX(PersistTimeText);
                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Persistant Time Text Color = %d", PersistTimeText);
               }  else {   // Set Default
                  PersistTimeText = 16777215;
                    ColorHold = GColorFromHEX(PersistTimeText);

                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Default Time Text %d", PersistTimeText);
               }
            }

         persist_write_int(MESSAGE_KEY_TIME_TEXT_COLOR_KEY, PersistTimeText);
       #else
         ColorHold = GColorWhite;
       #endif

         text_layer_set_text_color(text_time_layer,    ColorHold);
         text_layer_set_text_color(text_location_layer, ColorHold);
         text_layer_set_text_color(text_degrees_layer,  ColorHold);
         text_layer_set_text_color(text_date_layer,     ColorHold);
         text_layer_set_text_color(text_time_layer,     ColorHold);


      //****************
       #ifdef PBL_COLOR
        Tuple *STEPS_TEXT_COLOR =  dict_find(iterator, MESSAGE_KEY_STEPS_TEXT_COLOR_KEY);

         if (STEPS_TEXT_COLOR) { // Config Exists
             PersistStepsText = STEPS_TEXT_COLOR->value->int32;
             ColorHold = GColorFromHEX(PersistStepsText);
             persist_write_int(MESSAGE_KEY_STEPS_TEXT_COLOR_KEY,STEPS_TEXT_COLOR->value->int32);

             APP_LOG(APP_LOG_LEVEL_WARNING,    "    Added Config Steps Text Color: %d", PersistStepsText);
         } else { //Check for Persist
               if(persist_exists(MESSAGE_KEY_STEPS_TEXT_COLOR_KEY)) {
                  PersistStepsText = persist_read_int(MESSAGE_KEY_STEPS_TEXT_COLOR_KEY);
                  ColorHold = GColorFromHEX(PersistStepsText);
                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Persistant Steps Text Color = %d", PersistStepsText);
               }  else {   // Set Default
                  PersistStepsText =16777215;
                  ColorHold = GColorFromHEX(PersistStepsText);

                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Default Steps Text Color = %d", PersistStepsText);

               }
            }

         persist_write_int(MESSAGE_KEY_STEPS_TEXT_COLOR_KEY, PersistStepsText);
       #else
         ColorHold = GColorBlack;
       #endif


         TextColorHold2 = ColorHold; //Save for BT callback
         text_layer_set_text_color(text_numsteps_layer,    ColorHold);
         text_layer_set_text_color(text_meters_layer,    ColorHold);

  //****************

        Tuple *Date_Format = dict_find(iterator, MESSAGE_KEY_DATE_FORMAT_KEY);

        if(Date_Format && ((strcmp(Date_Format->value->cstring, "0") == 0) || (strcmp(Date_Format->value->cstring, "1") == 0))) {
             strcpy(PersistDateFormat,Date_Format->value->cstring);

            APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Config Value Date Format = %s, 0 = US, 1 = Intl", PersistDateFormat);
         } else {
            if(persist_exists(MESSAGE_KEY_DATE_FORMAT_KEY)) {
               persist_read_string(MESSAGE_KEY_DATE_FORMAT_KEY, PersistDateFormat, sizeof(PersistDateFormat));

               APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Persistant Date Format = %s, (0 = US, 1 = Intl)", PersistDateFormat);
            } else {
               strcpy(PersistDateFormat, "0"); // US Default
               APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Default Date Format (US) = %s", PersistDateFormat);
            }
         }

         persist_write_string(MESSAGE_KEY_DATE_FORMAT_KEY, PersistDateFormat);

         #ifdef PBL_PLATFORM_CHALK
             if (strcmp(PersistDateFormat, "0") == 0) {     // US
               strcpy(date_format,   " %a  %D");
             } else {
               strcpy(date_format,   " %a  %d/%m/%y");       //Intl
             }
         #else
             if (strcmp(PersistDateFormat, "0") == 0) {       // US
                 strcpy(date_format, "%a   %D");
             } else {
                 strcpy(date_format, "%a   %d/%m/%y");        //Intl
             }
         #endif

        text_layer_set_text(text_date_layer, date_text);

  //****************

        Tuple *BT_LossVib = dict_find(iterator, MESSAGE_KEY_BT_LOSS_KEY);

       if((BT_LossVib) && ((BT_LossVib->value->uint8 == 0) || (BT_LossVib->value->uint8 == 1))) {
           PersistBTLoss = BT_LossVib->value->uint8; //Vibe on loss
           APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Config BT Loss Key = %d (0=No Vib, 1=Vib)", PersistBTLoss);
       } else {
         if(persist_exists(MESSAGE_KEY_BT_LOSS_KEY)) {
               PersistBTLoss = persist_read_int(MESSAGE_KEY_BT_LOSS_KEY);
               APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Persistant BT Loss = %d, (0 = No Vib, 1 = Vib)", PersistBTLoss);
            } else {
               PersistBTLoss = 0; // No Vib on BT Loss
               APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Default BT Loss Key = %d (No Vibrate on Loss)", PersistBTLoss);
            }
       }
        persist_write_int(MESSAGE_KEY_BT_LOSS_KEY, PersistBTLoss);
 //******************

        Tuple *Low_Battery_Vib = dict_find(iterator, MESSAGE_KEY_LOW_BATTERY_KEY);

       if((Low_Battery_Vib) && ((Low_Battery_Vib->value->uint8 == 0) || (Low_Battery_Vib->value->uint8 == 1))) {
           PersistLow_Batt = BT_LossVib->value->uint8; //Vibe on loss
           APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Config Low Batt Key = %d (0=No Vib, 1=Vib)", PersistLow_Batt);
       } else {
         if(persist_exists(MESSAGE_KEY_LOW_BATTERY_KEY)) {
               PersistLow_Batt = persist_read_int(MESSAGE_KEY_LOW_BATTERY_KEY);
               APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Persistant Low Batt Key = %d, (0 = No Vib, 1 = Vib)", PersistLow_Batt);
            } else {
               PersistLow_Batt= 0; // No Vib on BT Loss
               APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Default Low Batt Key = %d (No Vibrate on Loss)", PersistLow_Batt);
            }
       }
        persist_write_int(MESSAGE_KEY_LOW_BATTERY_KEY, PersistLow_Batt);
  //******************

         wxcall = 0; //Reset


        Tuple *Temp_Type = dict_find(iterator, MESSAGE_KEY_TEMP_FORMAT_KEY);
        if(Temp_Type && ((strcmp(Temp_Type->value->cstring, "0") == 0) || (strcmp(Temp_Type->value->cstring, "1") == 0))) {
             strcpy(PersistTempFormat,Temp_Type->value->cstring);
             APP_LOG(APP_LOG_LEVEL_WARNING,     "    Added Config Temp Type = %s, (1 = F. 0 = C)\n", PersistTempFormat);
       } else {
         if(persist_exists(MESSAGE_KEY_TEMP_FORMAT_KEY)) {
               persist_read_string(MESSAGE_KEY_TEMP_FORMAT_KEY, PersistTempFormat, sizeof(PersistTempFormat));
               APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Persistant Temp Type = %s, (1 = F, 0 = C)\n", PersistTempFormat);
            } else {
               strcpy(PersistTempFormat,"0"); //Default F
               APP_LOG(APP_LOG_LEVEL_WARNING, "    Added Default Temp Type = %s, (Degrees F)\n", PersistTempFormat);
            }
       }
        persist_write_string(MESSAGE_KEY_TEMP_FORMAT_KEY, PersistTempFormat);
  //******************

            strcpy(text_degrees, "===");  // Reset

            APP_LOG(APP_LOG_LEVEL_WARNING, "Processing Temp & Location...");

            Tuple *Temperature = dict_find(iterator, MESSAGE_KEY_WEATHER_TEMPERATURE_KEY);

            if(Temperature) {
                int_degrees = Temperature->value->int32;
            } else {
                strcpy(text_degrees, "N/A");
            }
             if (strcmp((text_degrees), "N/A") != 0) {
                APP_LOG(APP_LOG_LEVEL_WARNING,   "    PersistTempFormat = %s, (0 = F, 1 = C)", PersistTempFormat);
                APP_LOG(APP_LOG_LEVEL_WARNING,   "    Input Degrees C = %d", int_degrees);

               if(strcmp(PersistTempFormat, "0") == 0)    { // Degrees F
                   APP_LOG(APP_LOG_LEVEL_WARNING, "       Converting C to F");
                   int_degrees = ((int_degrees * 9) / 5) + 32;

                 // Assemble full string and display
                  snprintf(text_degrees, 5, "%dF ", int_degrees);
                  APP_LOG(APP_LOG_LEVEL_WARNING, "    Output Degrees F = %s", text_degrees);
                } else {
                  snprintf(text_degrees, 5, "%dC ", int_degrees); //Temp Degrees C
                }
           }

           text_layer_set_text(text_degrees_layer, text_degrees);
           APP_LOG(APP_LOG_LEVEL_WARNING, "    Temp Output: %s\n", text_degrees);

            //******************
          APP_LOG(APP_LOG_LEVEL_WARNING, "Processing Wx Location Name...");

          Tuple *Wx_City = dict_find(iterator, MESSAGE_KEY_WEATHER_CITY_KEY);

          strcpy(text_location,Wx_City->value->cstring) ;
          text_layer_set_text(text_location_layer, text_location);
          APP_LOG(APP_LOG_LEVEL_WARNING, "    Location = %s\n", text_location);
}

void handle_appfocus(bool in_focus){
    if (in_focus) {
        FirstTime = 0;
    }
}
void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Inbox Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  app_focus_service_unsubscribe();
  app_message_deregister_callbacks();
  health_service_events_unsubscribe();

  text_layer_destroy(text_tod_layer);
  text_layer_destroy(text_date_layer);
  text_layer_destroy(text_location_layer);
  text_layer_destroy(text_degrees_layer);
  
  text_layer_destroy(text_numsteps_layer);
  text_layer_destroy(text_meters_layer);
  text_layer_destroy(text_miles_layer);

  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_steps_layer);

  layer_destroy(BatteryLineLayer);
  layer_destroy(BTLayer);

  fonts_unload_custom_font(fontMonaco13);
  fonts_unload_custom_font(fontRobotoCondensed23);
  fonts_unload_custom_font(fontRobotoBoldSubset30);

}

void handle_init(void) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Enterng INIT************************************************************");
  APP_LOG(APP_LOG_LEVEL_ERROR, "*************************************************************************");

  FirstTime = 0;

  setlocale(LC_TIME, "");

 #ifdef PBL_BW
     GColor BGCOLOR1 = GColorBlack;
     GColor BGCOLOR2 = GColorWhite;
     BGColorHold2    = GColorWhite;

     GColor TEXTCOLOR1 = GColorWhite;
     GColor TEXTCOLOR2 = GColorBlack;
     TextColorHold1   = GColorWhite;
     TextColorHold2   = GColorBlack;
  #else
     GColor BGCOLOR1 = GColorBlue;
     GColor BGCOLOR2 = GColorDarkGreen;
     BGColorHold2 = GColorDarkGreen;

     GColor TEXTCOLOR1 = GColorWhite;
     GColor TEXTCOLOR2 = GColorWhite;
     TextColorHold1   = GColorWhite;
     TextColorHold2   = GColorWhite;
 #endif

  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, BGCOLOR1);

  fontMonaco13           = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONACO_13));
  fontRobotoCondensed23  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_23));

  fontRobotoBoldSubset30 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_30));

  Layer *window_layer = window_get_root_layer(window);

  
  #if defined(PBL_HEALTH)
     // Attempt to subscribe 
     if(!health_service_events_subscribe(health_handler, NULL)) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available in Init!");
     }
  #else
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available in Init!");
  #endif

  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(256, 256);

  //TOD Layer
  #ifdef PBL_PLATFORM_CHALK
    text_tod_layer = text_layer_create(GRect(1, 1, 180, 90));
  #else
    text_tod_layer = text_layer_create(GRect(1, 1, 144, 84));
  #endif

  text_layer_set_background_color(text_tod_layer, BGCOLOR1);
  layer_add_child(window_layer, text_layer_get_layer(text_tod_layer));

  //Steps Layer
  #ifdef PBL_PLATFORM_CHALK
    text_steps_layer = text_layer_create(GRect(1, 90, 180, 90));
  #else
    text_steps_layer = text_layer_create(GRect(1, 84, 144, 84));
  #endif

  text_layer_set_background_color(text_steps_layer, BGCOLOR2);
  #ifdef PBL_HEALTH
     layer_add_child(window_layer, text_layer_get_layer(text_steps_layer));
  #endif
  
  //Location - Local
  #ifdef PBL_PLATFORM_CHALK
    text_location_layer = text_layer_create(GRect(1, 44, 180, 17));
  #else
    text_location_layer = text_layer_create(GRect(1, 1, 144, 17));
  #endif

  text_layer_set_text_alignment(text_location_layer, GTextAlignmentCenter);
  text_layer_set_text(text_location_layer, text_location);
  text_layer_set_font(text_location_layer, fontMonaco13);
  text_layer_set_background_color(text_location_layer, BGCOLOR1);
  text_layer_set_text_color(text_location_layer, TEXTCOLOR1);
  layer_add_child(window_layer, text_layer_get_layer(text_location_layer));

  // Date 
  #ifdef PBL_PLATFORM_CHALK
    text_date_layer = text_layer_create(GRect(3, 57, 135, 22));
    text_layer_set_text_alignment(text_date_layer, GTextAlignmentLeft);;
  #else
    text_date_layer = text_layer_create(GRect(1, 18, 144, 22));
    text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);;
  #endif

  text_layer_set_text_color(text_date_layer, TEXTCOLOR1);
  text_layer_set_background_color(text_date_layer, BGCOLOR1);
  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));


  // Time of Day 
  #ifdef PBL_PLATFORM_CHALK
     text_time_layer = text_layer_create(GRect(1, 6, 180, 40));
  #else
     text_time_layer = text_layer_create(GRect(5, 40, 100, 40));
  #endif

  text_layer_set_font(text_time_layer, fontRobotoBoldSubset30);
  text_layer_set_text_color(text_time_layer, TEXTCOLOR1);
  text_layer_set_background_color(text_time_layer, BGCOLOR1);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

   //Temperature
  #ifdef PBL_PLATFORM_CHALK
    text_degrees_layer = text_layer_create(GRect(141, 63, 39, 17));
    text_layer_set_text_alignment(text_degrees_layer, GTextAlignmentLeft);
  #else
     text_degrees_layer = text_layer_create(GRect(100, 55, 40, 17));
     text_layer_set_text_alignment(text_degrees_layer, GTextAlignmentRight);
  #endif

  text_layer_set_text(text_degrees_layer, text_degrees);
  text_layer_set_font(text_degrees_layer, fontMonaco13);
  text_layer_set_background_color(text_degrees_layer, BGCOLOR1);
  text_layer_set_text_color(text_degrees_layer, TEXTCOLOR1);
  layer_add_child(window_layer, text_layer_get_layer(text_degrees_layer));

    // Battery Line
  #ifdef PBL_PLATFORM_CHALK
      GRect battery_line_frame = GRect(38, 87, 104, 6);
  #else
      GRect battery_line_frame = GRect(22, 81, 104, 6);
  #endif

  BatteryLineLayer = layer_create(battery_line_frame);
  layer_set_update_proc(BatteryLineLayer, battery_line_layer_update_callback);
  layer_add_child(window_layer, BatteryLineLayer);

  // Steps
  #ifdef PBL_PLATFORM_CHALK
    text_numsteps_layer = text_layer_create(GRect(1, 100, 180, 40));
  #else
    text_numsteps_layer = text_layer_create(GRect(1, 85, 144, 28));
  #endif

  text_layer_set_font(text_numsteps_layer, fontRobotoCondensed23);
  text_layer_set_text_color(text_numsteps_layer, TEXTCOLOR2);
  text_layer_set_background_color(text_numsteps_layer, BGCOLOR2);
  text_layer_set_text_alignment(text_numsteps_layer, GTextAlignmentCenter);
  #ifdef PBL_HEALTH
     layer_add_child(window_layer, text_layer_get_layer(text_numsteps_layer)); 
  #endif 
  
// Meters
  #ifdef PBL_PLATFORM_CHALK
     text_meters_layer = text_layer_create(GRect(3, 135, 177, 29));
     text_layer_set_text_alignment(text_meters_layer, GTextAlignmentCenter);;
  #else
     text_meters_layer = text_layer_create(GRect(1, 140, 144, 28));
     text_layer_set_text_alignment(text_meters_layer, GTextAlignmentCenter);;
  #endif

  text_layer_set_text_color(text_meters_layer, TEXTCOLOR2);
  text_layer_set_background_color(text_meters_layer, BGCOLOR2);
  text_layer_set_font(text_meters_layer, fontRobotoCondensed23);
 
  #ifdef PBL_HEALTH
     layer_add_child(window_layer, text_layer_get_layer(text_meters_layer)); 
  #endif 

  // Miles
  #ifdef PBL_PLATFORM_CHALK
     text_miles_layer = text_layer_create(GRect(3, 135, 177, 29));
     text_layer_set_text_alignment(text_miles_layer, GTextAlignmentCenter);;
  #else
     text_miles_layer = text_layer_create(GRect(1, 112, 144, 28));
     text_layer_set_text_alignment(text_miles_layer, GTextAlignmentCenter);;
  #endif
  
  text_layer_set_text_color(text_miles_layer, TEXTCOLOR2);
  text_layer_set_background_color(text_miles_layer, BGCOLOR2);
  text_layer_set_font(text_miles_layer, fontRobotoCondensed23);
 
  #ifdef PBL_HEALTH
     layer_add_child(window_layer, text_layer_get_layer(text_miles_layer)); 
  #endif
  
  //Bluetooth Logo Setup area
  #ifdef PBL_PLATFORM_CHALK
     GRect BTArea = GRect(139, 103, 20, 20);
  #else
     GRect BTArea = GRect(120, 138, 20, 20);
  #endif

  BTLayer = layer_create(BTArea);

  layer_add_child(window_layer, BTLayer);
  layer_set_update_proc(BTLayer, BTLine_update_callback);

  bluetooth_connection_service_subscribe(&handle_bluetooth);

  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  app_focus_service_subscribe(&handle_appfocus);

  handle_battery(battery_state_service_peek());
  handle_bluetooth(bluetooth_connection_service_peek());
  
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);

  ProcessNoBTPersist();

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_tick(current_time, SECOND_UNIT);



}
int main(void) {
   handle_init();

   app_event_loop();

   handle_deinit();
}
