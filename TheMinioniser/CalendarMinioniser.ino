
#include <WiFi.h>
#include <HTTPClient.h>
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/acces_token.hpp"
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp"
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/time_tools.hpp"
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/leds_tools.hpp"
#include "/home/gal/dev/TheMiniuniser/TheMinioniser/sound_tools.hpp"
// Your Domain name with URL path or IP address with path
String serverName = "https://www.googleapis.com/calendar/v3/calendars/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
// unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

const char *ssid = "Augury_Cellular";
const char *password = "augurysys1";

calendar::Event events[MAX_EVENTS] = {};
int today_num_of_events = 0;
bool manually_should_fetch_calendar = true;
const int DELAY_IN_SEC = 10;
void setup()
{

    Serial.begin(115200);
    delay(1000);

    pinMode(4, OUTPUT); // Flash setup
    ledstools::init_leds();
    soundtools::init_sound();
    
    WiFi.begin(ssid, password);
    int num_of_connections = 10;
    ledstools::show_color(ledstools::CONNECTING_TO_WIFI);
    while (WiFi.status() != WL_CONNECTED and num_of_connections-- > 0)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    if (num_of_connections < 0)
    {
        for (size_t i = 0; i < 3; i++)
        {
            Serial.println("Failed connecting to WiFi..");
            ledstools::show_color(ledstools::CONNECTING_TO_WIFI_FAILED);
            delay(1000);
        }
        exit(1);
    }

    Serial.println("Connected to the WiFi network");
    // init and get the time
    configTime(timetools::gmtOffset_sec, timetools::daylightOffset_sec, timetools::ntpServer);
    timetools::printAndUpdateLocalTime();
    ledstools::turn_off_leds();
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
                    ledstools::show_color(ledstools::GETTING_CALENDAR);
                    today_num_of_events = 0;
                    today_num_of_events = calendar::parse_calendar(http, events, MAX_EVENTS);
                    Serial.printf("\n**************************************** DONE PARSING %d events****************************************\n", today_num_of_events);
                }
                else if (httpCode == HTTP_CODE_UNAUTHORIZED)
                {
                    Serial.print("[HTTP] returned with HTTP_CODE_UNAUTHORIZED.\n");
                    Serial.print("Try to refresh tooken...\n");
                    token_data::token_expiration_time = -1;
                    ledstools::show_color(ledstools::GETTING_CALENDAR_FAILED);
                }

                else if (httpCode == HTTP_CODE_FORBIDDEN)
                {
                    Serial.print("[HTTP] returned with HTTP_CODE_FORBIDDEN.\n");
                    ledstools::show_color(ledstools::GETTING_CALENDAR_FAILED);
                }
                else if (httpCode == HTTP_CODE_NOT_FOUND)
                {
                    Serial.print("[HTTP] returned with HTTP_CODE_NOT_FOUND.\n");
                    ledstools::show_color(ledstools::GETTING_CALENDAR_FAILED);
                }
            }
            else
            {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
                ledstools::show_color(ledstools::GETTING_CALENDAR_FAILED);
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