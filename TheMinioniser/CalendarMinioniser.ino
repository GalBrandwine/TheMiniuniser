
#include <WiFi.h>
#include <HTTPClient.h>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/acces_token.hpp>
// Your Domain name with URL path or IP address with path
String serverName = "https://www.googleapis.com/calendar/v3/calendars/gbrandwine@augury.com/events?q=gbrandwine&orderBy=updated&fields=items&timeMax=2022-02-17T18%3A00%3A00.000%2B02%3A00&timeMin=2022-02-17T09%3A00%3A00.000%2B02%3A00";

/*
https://www.googleapis.com/calendar/v3/calendars/gbrandwine@augury.com/events/?timeMax%3D2022-01-24T00%3A00%3A00%2B02%3A00&q=gbrandwine&timeMin%3D2022-01-23T00%3A00%3A00%2B02%3A00&fields=items
*/

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
// unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

bool need_to_refresh_token = false;
const char *ssid = "gozal_2.4";
const char *password = "asdffdsa";
#define LED_BUILTIN 4

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

// void sendHeader(const char *aHeaderName, const char *aHeaderValue);

void loop()
{

    // Send an HTTP POST request every 10 minutes
    if ((millis() - lastTime) > timerDelay)
    {
        // Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;

            String serverPath = serverName;

            // Your Domain name with URL path or IP address with path
            http.begin(serverPath.c_str());
            http.setAuthorization(token_data::token.c_str());

            // Send HTTP GET request
            // int httpResponseCode = http.GET();
            // start connection and send HTTP header
            int httpCode = http.GET();
            if (httpCode > 0)
            {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("\n\n\n\n[HTTP] GET... code: %d\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK)
                {

                    // get length of document (is -1 when Server sends no Content-Length header)
                    int len = http.getSize();

                    // create buffer for read
                    uint8_t buff[128] = {0};

                    // get tcp stream
                    WiFiClient *stream = http.getStreamPtr();

                    // read all data from server
                    while (http.connected() && (len > 0 || len == -1))
                    {
                        // get available data size
                        size_t size = stream->available();

                        if (size)
                        {
                            // read up to 128 byte
                            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                            auto responseChunk = String((char *)buff);
                            if (responseChunk.indexOf("2022-02-12") > -1)
                            {
                                Serial.println("\n*******************************************\n");
                                Serial.println("Found todays event");
                                digitalWrite(LED_BUILTIN, HIGH);
                                Serial.println("\n*******************************************\n");
                            }
                            // Serial.println(responseChunk);
                            // write it to Serial
                            Serial.write(buff, c);

                            if (len > 0)
                            {
                                len -= c;
                            }
                        }
                        delay(1);
                    }

                    Serial.println();
                    Serial.print("[HTTP] connection closed or file end.\n");
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