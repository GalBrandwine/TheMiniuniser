
#include <WiFi.h>
#include <HTTPClient.h>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/acces_token.hpp>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/time_tools.hpp>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/leds_tools.hpp>

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
void setup()
{

    Serial.begin(115200);
    delay(1000);

    pinMode(4, OUTPUT); // Flash setup
    ledstools::init_leds();

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    Serial.println("Connected to the WiFi network");
    // init and get the time
    configTime(timetools::gmtOffset_sec, timetools::daylightOffset_sec, timetools::ntpServer);
    timetools::printAndUpdateLocalTime();
}

void loop()
{

    if (manually_should_fetch_calendar || calendar::should_fetch_calendar())
    {
        manually_should_fetch_calendar = false;

        if (WiFi.status() == WL_CONNECTED)
        {
            using namespace timetools;
            HTTPClient http;

            String timeMax = dateYear + "-" + dateMonth + "-" + dateDay + "T" + dayEndTime + "%3A00%3A00.000%2B02%3A00";   // Need to be able to change year, month,day
            String timeMin = dateYear + "-" + dateMonth + "-" + dateDay + "T" + dayStartTime + "%3A00%3A00.000%2B02%3A00"; // Need to be able to change year, month,day
            String serverPath = serverName + token_data::USER_NAME + token_data::USER_DOMAIN + "/events?q=" + token_data::USER_NAME + "&singleEvents=true&fields=items&timeMax=" + timeMax + "&timeMin=" + timeMin;
            Serial.print("serverPath: ");
            Serial.println(serverPath);

            // Your Domain name with URL path or IP address with path
            http.begin(serverPath.c_str());
            http.setAuthorization(token_data::token.c_str());

            // start connection and send HTTP header
            int httpCode = http.GET();
            if (httpCode > 0)
            {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("\n\n\n\n[HTTP] GET... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK)
                {
                    Serial.printf("\n**************************************** STARTING PARSING ****************************************\n");
                    today_num_of_events = 0;
                    today_num_of_events = calendar::parse_calendar(http, events, MAX_EVENTS);
                    Serial.printf("\n**************************************** DONE PARSING %d events****************************************\n", today_num_of_events);
                }
                else if (httpCode == HTTP_CODE_UNAUTHORIZED)
                {
                    Serial.print("[HTTP] returned with HTTP_CODE_UNAUTHORIZED.\n");
                    Serial.print("Try to refresh tooken...\n");
                    token_data::token_expiration_time = access_token::token_freshener(token_data::token);
                    if (token_data::token_expiration_time > 0)
                    {
                        Serial.print("Got new token with expiration time: ");
                        Serial.println(token_data::token_expiration_time);
                        manually_should_fetch_calendar = true;
                    }
                }
                else if (httpCode == HTTP_CODE_NOT_FOUND)
                {
                    Serial.print("[HTTP] returned with HTTP_CODE_NOT_FOUND.\n");
                }
            }
            else
            {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
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
        auto meeting_index = timetools::get_currently_ocuring_accepted_meeting(events, today_num_of_events);
        if (meeting_index > -1)
        {
            printf("Im inside a meeting, time to track meeting duration and light up leds accordingly :)\n");
            // ledstools::simple_handle_event(events[meeting_index]);
            ledstools::show_event_progress(events[meeting_index]);
            ledstools::turn_off_leds();
            printf("Meeting is over. Leds are turned off.\n");
            manually_should_fetch_calendar = true; // Meeting is blocking. So after a meeting, fetch new calendar
        }
    }
    else
        printf("Its not time to fetch calendar, event haven't fetched yet\n");
    delay(1000 * 30);
}