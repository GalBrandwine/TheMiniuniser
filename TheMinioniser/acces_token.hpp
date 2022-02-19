#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/data.hpp>
namespace access_token
{
    /* refresh_token
    Accepts a expired_token as reference and update's it.
    Return its new expiration time or -1 if failed
     */
    const int token_freshener(String &token_to_update)
    {
        if (!token_to_update)
        {
            Serial.println("Can't refresh token. Given input is Null");
            return -1;
        }
        // Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;

            http.begin(token_data::refreshTokenUtl);
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");

            auto httpCode = http.POST("client_id=" + token_data::client_id + "&client_secret=" + token_data::client_secret + "&refresh_token=" + token_data::refresh_token + "&grant_type=refresh_token");
            if (httpCode > 0)
            {
                // file found at server
                if (httpCode == HTTP_CODE_OK)
                {

                    // get length of document (is -1 when Server sends no Content-Length header)
                    int len = http.getSize();
                    Serial.print("Got response HTTP_CODE_OK");

                    // create buffer for read
                    uint8_t buff[250] = {0}; // access_token is 166 bytes

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
                            if (responseChunk.indexOf("access_token") > -1)
                            {
                                Serial.println("\n*******************************************\n");
                                Serial.println("Found Access token!");
                                // Serial.println(responseChunk);

                                // Take access_token out of buffer
                                auto start = responseChunk.indexOf("access_token");
                                auto end = responseChunk.indexOf(",", start); // First comma is end of access_token
                                token_to_update = responseChunk.substring(start + sizeof("access_token") + 2, end);

                                // Take expires_in out of buffer
                                start = responseChunk.indexOf("expires_in");
                                end = responseChunk.indexOf(",", start); // First comma is end of access_token
                                auto expired_in_buf = responseChunk.substring(start + sizeof("expires_in") + 2, end);
                                return expired_in_buf.toInt();
                            }
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
            }
            else
            {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }

            http.end();
            return -1;
        }
    }

} // access_token