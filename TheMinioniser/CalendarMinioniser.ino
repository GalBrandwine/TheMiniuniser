
#include <WiFi.h>
#include <HTTPClient.h>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/acces_token.hpp>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp>

String dayEndTime = "18";
String dayStartTime = "09";
String dateDay = "23";
String dateMonth = "02";
String dateYear = "2022";
String timeMax = dateYear + "-" + dateMonth + "-" + dateDay + "T" + dayEndTime + "%3A00%3A00.000%2B02%3A00";   // Need to be able to change year, month,day
String timeMin = dateYear + "-" + dateMonth + "-" + dateDay + "T" + dayStartTime + "%3A00%3A00.000%2B02%3A00"; // Need to be able to change year, month,day

// Your Domain name with URL path or IP address with path
String serverName = "https://www.googleapis.com/calendar/v3/calendars/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
// unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

const char *ssid = "gozal_2.4";
const char *password = "asdffdsa";

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