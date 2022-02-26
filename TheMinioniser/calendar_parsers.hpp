#pragma once
#include <HTTPClient.h>

#define MAX_EVENTS 15

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
            Serial.printf("Event name: %s\n Start time: %s\n End time: %s\naccepted: %d\n", name.c_str(), start_str.c_str(), end_str.c_str(), accepted);
        };
    };

    void get_event_summary_str(WiFiClient *stream, String &str_out)
    {
        // Serial.println(__PRETTY_FUNCTION__);

        if (stream->find("summary\": "))
        {
            Serial.printf("Found 'summary'\n");
            str_out = stream->readStringUntil(',');
        }
    }
    /*
    return number of bytes readden from stream
    */
    void get_event_start_str(WiFiClient *stream, String &str_out)
    {
        // Serial.println(__PRETTY_FUNCTION__);
        if (stream->find("start\": "))
        {
            Serial.printf("Found 'start'\n");
            stream->find("dateTime\": ");
            str_out = stream->readStringUntil(',');
        }
    }

    /*
    return number of bytes readden from stream
    */
    void get_event_end_str(WiFiClient *stream, String &str_out)
    {
        // Serial.println(__PRETTY_FUNCTION__);
        if (stream->find("end\": "))
        {
            Serial.printf("Found 'end'\n");
            stream->find("dateTime\": ");
            str_out = stream->readStringUntil(',');
        }
    }

    void get_event_acceptance_status(WiFiClient *stream, bool &is_accepted_out)
    {
        // Serial.println(__PRETTY_FUNCTION__);
        if (stream->find("attendees"))
        {
            if (stream->find("gbrandwine@augury.com")) // TODO change to variable
            {
                Serial.printf("Found 'attendee'\n");
                stream->find("responseStatus\": \"");
                auto response_status = stream->readStringUntil('"');
                Serial.println(response_status);
                if (response_status.compareTo("accepted") == 0)
                    is_accepted_out = true;
                else
                    is_accepted_out = false;
            }
        }
    }

    Event parse_calendar(HTTPClient &http)
    {
        // get length of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();

        // create buffer for read
        // uint8_t buff[256] = {0};

        // get tcp stream
        WiFiClient *stream = http.getStreamPtr();

        Event events[MAX_EVENTS] = {};
        // read all data from server
        while (http.connected() && (len > 0 || len == -1))
        {
            // get available data size
            size_t size = stream->available();

            if (size)
            {
                Event newEvent;

                get_event_summary_str(stream, newEvent.name);
                get_event_start_str(stream, newEvent.start_str);
                get_event_end_str(stream, newEvent.end_str);
                get_event_acceptance_status(stream, newEvent.accepted);

                Serial.println("\n*******************************************\n");
                Serial.println("Found todays event: ");
                newEvent.Print();
                Serial.println("\n*******************************************\n");
            }
            else
            {
                Serial.println("Done reading todays event.");
                return;
            }

            delay(1);
        }

        Serial.println();
        Serial.print("[HTTP] connection closed or file end.\n");
        return events;
    }
} // namespace calendar
