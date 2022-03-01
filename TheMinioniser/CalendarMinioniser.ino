
#include <WiFi.h>
#include <HTTPClient.h>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/acces_token.hpp>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp>

String dayEndTime = "20";
String dayStartTime = "08";
String dateDay = "2";
String dateMonth = "03";
String dateYear = "2022";

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

// Ntp information
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 2 * 60 * 60;
const int daylightOffset_sec = 3600;

void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    dateDay = String(timeinfo.tm_mday);
    dateMonth = String(1 + timeinfo.tm_mon); // 0-11
    dateYear = String(1900 + timeinfo.tm_year);
    printf("Updated query day\\month\\year: %s\\%s\\%s\n", dateDay.c_str(), dateMonth.c_str(), dateYear.c_str());
}

bool should_get_calendar()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return false;
    }
    switch (timeinfo.tm_hour)
    {
    case 0:
    case 6:
    case 9:
    case 12:
    case 16:
    case 19:
        printf("\nHo, its time for fetching the calendar\n");
        return true;
    default:
        return false;
    }
}

calendar::Event events[MAX_EVENTS] = {};

void setup()
{

    Serial.begin(115200);
    delay(1000);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    Serial.println("Connected to the WiFi network");
    // init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();
}

void loop()
{

    // Send an HTTP POST request every 10 minutes
    if ((millis() - lastTime) > timerDelay)
    {
        // Check WiFi connection status
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
                    calendar::parse_calendar(http, events, MAX_EVENTS);
                    Serial.printf("\n**************************************** DONE PARSING ****************************************\n");
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
        lastTime = millis();
    }
}