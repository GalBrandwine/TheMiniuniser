#pragma once
#include <HTTPClient.h>
namespace calendar
{
    struct Event
    {
        String name{};
        time_t start{};
        time_t end{};
        int duration{0};
        bool accepted{false};
        void Print()
        {
            Serial.printf("Event name: %s\n", name.c_str());
        };
    };

    void parse_calendar(HTTPClient &http)
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
                Event newEvent;

                // read up to 128 byte
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                auto responseChunk = String((char *)buff);
                Serial.print("Printing responseChunk: ");
                Serial.println(responseChunk);
                // The begginning of an Event
                if (responseChunk.indexOf("summary") > -1)
                {
                    Serial.println("\n*******************************************\n");
                    Serial.println("Found todays event: ");
                    auto summary_start = responseChunk.indexOf("summary");
                    auto summary_end = responseChunk.indexOf("\",", summary_start);

                    newEvent.name = responseChunk.substring(summary_start + sizeof("summary\": "), summary_end); // summary": "IoT Sync @ Mon"

                    // Continue reading through all the event.
                    // "eventType" is the last attribute of an event
                    String eventChunk{"INIT"};
                    // create buffer for read
                    uint8_t eventBuff[128] = {0};
                    while (eventChunk.indexOf("eventType") == -1)
                    {
                        c = stream->readBytes(eventBuff, ((size > sizeof(eventBuff)) ? sizeof(eventBuff) : size));
                        eventChunk = String((char *)eventBuff);
                        auto eventStartidx = eventChunk.indexOf("start");
                        if (eventStartidx > -1)
                        {
                            // Serial.println(eventStartidx);
                            // Serial.println(eventChunk);
                            auto date_start = eventChunk.indexOf("dateTime", eventStartidx);
                            // auto start_end = eventChunk.indexOf("\",", date_start);
                            Serial.printf("Start idx: %d\n", date_start);
                            // Serial.printf("End idx: %d\n", start_end);
                            auto dateStr = eventChunk.substring(date_start + sizeof("dateTime: "), date_start + sizeof("dateTime: ") + 27);
                            Serial.print("Date start: ");
                            Serial.println(dateStr);

                            // Found what I'm looking for, now look at the next chunk
                            // eventChunk = eventChunk.substring(eventChunk.indexOf("start") + sizeof("start"));
                        }
                        auto eventEndidx = eventChunk.indexOf("end");
                        if (eventEndidx > -1)
                        {
                            auto date_start = eventChunk.indexOf("dateTime", eventEndidx);
                            // auto start_end = eventChunk.indexOf("\",", date_start);
                            auto dateStr = eventChunk.substring(date_start + sizeof("dateTime: "), date_start + sizeof("dateTime: ") + 27);
                            Serial.print("Date end: ");
                            Serial.println(dateStr);
                            // Found what I'm looking for, now look at the next chunk
                            // eventChunk = eventChunk.substring(eventChunk.indexOf("end") + sizeof("end"));
                            break;
                        }

                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    newEvent.Print();
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
} // namespace calendar
