#include <Preferences.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/acces_token.hpp"
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp"
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/time_tools.hpp"
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/leds_tools.hpp"
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/sound_tools.hpp"

Preferences preferences;

// Your Domain name with URL path or IP address with path
String serverName = "https://www.googleapis.com/calendar/v3/calendars/";

String hostname = "Minioniser";

calendar::Event events[MAX_EVENTS] = {};
int today_num_of_events = 0;
bool manually_should_fetch_calendar = true;
const int DELAY_IN_SEC = 10;

// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking
bool wm_nonblocking = false; // change to true to use non blocking

WiFiManager wm;                    // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )

void setup()
{

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname.c_str()); // define hostname
    Serial.begin(115200);
    delay(1000);

    ledstools::init_leds();
    soundtools::init_sound();
    preferences.begin(hostname.c_str(), false);
    auto stored = preferences.getString("USER_NAME", "USER_NAME_NOT_FOUND");
    if (stored == "USER_NAME_NOT_FOUND")
    {
        printf("Could not found user name in preferences, storing original username: %s\n", token_data::USER_NAME.c_str());
        preferences.putString("USER_NAME", token_data::USER_NAME);
    }
    else
        token_data::USER_NAME = stored;

    if (wm_nonblocking)
        wm.setConfigPortalBlocking(false);

    // wm.resetSettings();

    const char *augury_user_str = "<br><label for='auguryuserid'>Enter Augury user (prefix only, no '@augury.com')</label><br><input type='text' name='auguryuserid' <br>";
    new (&custom_field) WiFiManagerParameter(augury_user_str); // custom html input

    wm.addParameter(&custom_field);
    wm.setSaveParamsCallback(saveParamCallback);

    // set dark theme
    wm.setDarkMode(true);

    ledstools::communicate_status(ledstools::CONNECTING_TO_WIFI_CONTINUES);
    bool res;
    res = wm.autoConnect((hostname + String("AP")).c_str()); // password protected ap
    ledstools::turn_off_leds();
    if (!res)
    {
        Serial.println("Failed to connect or hit timeout");
        for (size_t i = 0; i < 3; i++)
        {
            Serial.println("Failed connecting to WiFi..");
            ledstools::communicate_status(ledstools::CONNECTING_TO_WIFI_FAILED);
            delay(1000);
        }
        ESP.restart();
    }
    else
    {
        // if you get here you have connected to the WiFi
        ledstools::communicate_status(ledstools::CONNECTED_TO_WIFI);
        Serial.println("connected...yeey :)");
        configTime(timetools::gmtOffset_sec, timetools::daylightOffset_sec, timetools::ntpServer);
        timetools::printAndUpdateLocalTime();
        ledstools::turn_off_leds();
    }
}

String getParam(String name)
{
    // read parameter from server, for customhmtl input
    String value;
    if (wm.server->hasArg(name))
    {
        value = wm.server->arg(name);
    }
    return value;
}

void saveParamCallback()
{
    Serial.println("[CALLBACK] saveParamCallback fired");
    auto user = getParam("auguryuserid");
    if (user.length() > 0)
    {
        if (user == "soundtest")
        {
            soundtools::jingle_bells();
            return;
        }

        token_data::USER_NAME = user;
        preferences.putString("USER_NAME", token_data::USER_NAME);
        Serial.println("PARAM augury_user = " + user);
    }
}

void loop()
{
    using namespace timetools;
    ledstools::turn_off_leds();
    if (manually_should_fetch_calendar || calendar::should_fetch_calendar())
    {
        manually_should_fetch_calendar = false;
        printAndUpdateLocalTime();

        if (WiFi.status() == WL_CONNECTED)
        {

            HTTPClient http;

            String timeMax = dateYear + "-" + dateMonth + "-" + dateDay + "T" + dayEndTime + "%3A00%3A00.000%2B02%3A00";   // Need to be able to change year, month,day
            String timeMin = dateYear + "-" + dateMonth + "-" + dateDay + "T" + dayStartTime + "%3A00%3A00.000%2B02%3A00"; // Need to be able to change year, month,day
            String serverPath = serverName + token_data::USER_NAME + token_data::USER_DOMAIN + "/events?q=" + token_data::USER_NAME + "&singleEvents=true&fields=items&timeMax=" + timeMax + "&timeMin=" + timeMin;
            Serial.print("serverPath: ");
            Serial.println(serverPath);

            // Your Domain name with URL path or IP address with path
            http.begin(serverPath.c_str());

            int httpCode{-1};

            // start connection and send HTTP header
            if (token_data::token_expiration_time > 0)
            {
                httpCode = http.GET();
                http.setAuthorization(token_data::token.c_str());
            }
            else
            {
                token_data::token_expiration_time = access_token::token_freshener(token_data::token);
                http.setAuthorization(token_data::token.c_str());
                httpCode = http.GET();
            }

            if (httpCode > 0)
            {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("\n\n\n\n[HTTP] GET... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK)
                {
                    Serial.printf("\n**************************************** STARTING PARSING ****************************************\n");
                    ledstools::communicate_status(ledstools::GETTING_CALENDAR);
                    today_num_of_events = 0;
                    today_num_of_events = calendar::parse_calendar(http, events, MAX_EVENTS);
                    Serial.printf("\n**************************************** DONE PARSING %d events****************************************\n", today_num_of_events);
                }
                else if (httpCode == HTTP_CODE_UNAUTHORIZED)
                {
                    Serial.print("[HTTP] returned with HTTP_CODE_UNAUTHORIZED.\n");
                    Serial.print("Try to refresh tooken...\n");
                    token_data::token_expiration_time = -1;
                    ledstools::communicate_status(ledstools::GETTING_CALENDAR_FAILED);
                }

                else if (httpCode == HTTP_CODE_FORBIDDEN)
                {
                    Serial.print("[HTTP] returned with HTTP_CODE_FORBIDDEN.\n");
                    ledstools::communicate_status(ledstools::GETTING_CALENDAR_FAILED);
                }
                else if (httpCode == HTTP_CODE_NOT_FOUND)
                {
                    Serial.print("[HTTP] returned with HTTP_CODE_NOT_FOUND.\n");
                    ledstools::communicate_status(ledstools::GETTING_CALENDAR_FAILED);
                }
            }
            else
            {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
                ledstools::communicate_status(ledstools::GETTING_CALENDAR_FAILED);
            }

            http.end();
        }
        else
        {
            Serial.println("WiFi Disconnected");
        }
    }
    else if (today_num_of_events > 0)
    {
        printf("Its not time to fetch calendar, check if Im in a meeting right Now\n");
        auto meeting_index = get_currently_ocuring_accepted_meeting(events, today_num_of_events);
        if (meeting_index > -1)
        {
            printf("Im inside a meeting, time to track meeting duration and light up leds accordingly :)\n");
            // ledstools::simple_handle_event(events[meeting_index]);
            ledstools::show_event_progress(events[meeting_index]);
            token_data::token_expiration_time -= events[meeting_index].duration;
            soundtools::jingle_bells();
            ledstools::turn_off_leds();
            printf("Meeting is over. Leds are turned off.\n");
            manually_should_fetch_calendar = true; // Meeting is blocking. So after a meeting, fetch new calendar
        }
    }
    else
        printf("Its not time to fetch calendar, event haven't fetched yet\n");
    delay(1000 * DELAY_IN_SEC);
    token_data::token_expiration_time -= DELAY_IN_SEC;
}