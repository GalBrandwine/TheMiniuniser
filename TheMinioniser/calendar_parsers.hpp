#pragma once
#include <HTTPClient.h>
namespace calendar
{
    struct Event
    {
        String name{};
        String start_str{};
        time_t start{};
        String end_str{};
        time_t end{};
        // Minutes
        int duration{0};
        bool accepted{false};
        void Print()
        {
            Serial.printf("Event name: %s\n Start time: %s\n End time: %s\n", name.c_str(), start_str.c_str(), end_str.c_str());
        };
    };

    void parse_calendar(HTTPClient &http)
    {
        // get length of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();

        // create buffer for read
        uint8_t buff[256] = {0};

        // get tcp stream
        WiFiClient *stream = http.getStreamPtr();

        // read all data from server
        while (http.connected() && (len > 0 || len == -1))
        {
            // get available data size
            size_t size = stream->available();

            if (size)
            {
                Event newEvent;

                // read up to 128 byte
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                auto responseChunk = String((char *)buff);
                Serial.println("Printing responseChunk: ");
                Serial.println(responseChunk);
                Serial.println("");
                // The begginning of an Event
                if (responseChunk.indexOf("summary") > -1)
                {
                    Serial.println("\n*******************************************\n");
                    Serial.println("Found todays event: ");
                    auto summary_start = responseChunk.indexOf("summary");
                    auto summary_end = responseChunk.indexOf("\",", summary_start);

                    newEvent.name = responseChunk.substring(summary_start + sizeof("summary\": "), summary_end);

                    // auto eventDescidx = responseChunk.indexOf("description");
                    // if (eventDescidx > -1) // Skip description
                    // {
                    //     c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                    // }

                    auto eventStartidx = responseChunk.indexOf("start");
                    Serial.printf("eventStartidx: %d\n", eventStartidx);
                    while (eventStartidx == -1 && (len > 0)) // i.e. Not found, seek for it.
                    {
                        buff[256] = {0};
                        c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        // Serial.write(buff, c);
                        if (len > 0)
                        {
                            len -= c;
                        }
                        responseChunk = String((char *)buff);
                        eventStartidx = responseChunk.indexOf("start");
                    }
                    Serial.printf("After eventStartidx: %d\n", eventStartidx);
                    if (eventStartidx > -1)
                    {
                        auto date_start = responseChunk.indexOf("dateTime", eventStartidx);
                        newEvent.start_str = responseChunk.substring(date_start + sizeof("dateTime: "), date_start + sizeof("dateTime: ") + 27);
                    }

                    auto eventEndidx = responseChunk.indexOf("end");
                    while (eventEndidx == -1 && (len > 0)) // i.e. Not found, seek for it.
                    {
                        buff[256] = {0};
                        c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        // Serial.write(buff, c);
                        if (len > 0)
                        {
                            len -= c;
                        }
                        responseChunk = String((char *)buff);
                        eventEndidx = responseChunk.indexOf("end");
                    }
                    Serial.printf("After eventEndidx: %d\n", eventStartidx);
                    if (eventEndidx > -1)
                    {
                        auto date_start = responseChunk.indexOf("dateTime", eventEndidx);
                        newEvent.end_str = responseChunk.substring(date_start + sizeof("dateTime: "), date_start + sizeof("dateTime: ") + 27);
                    }

                    //     if (len > 0)
                    //     {
                    //         len -= c;
                    //     }
                    // }
                    newEvent.Print();
                    Serial.println("\n*******************************************\n");
                }

                // Serial.println(responseChunk);
                // write it to Serial
                // Serial.write(buff, c);

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
} // namespace calendar
