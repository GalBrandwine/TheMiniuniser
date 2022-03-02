#pragma once
#include <HTTPClient.h>
#include </home/gal/dev/TheMiniuniser/TheMinioniser/data.hpp>
#define MAX_EVENTS 15

namespace calendar
{
    struct Event
    {
        String name{};
        String start_str{};
        struct tm start
        {
        };
        String end_str{};
        struct tm end
        {
        };
        // Seconds
        double duration{0};
        bool accepted{false};
        void Print() const
        {
            char buffer[80];
            Serial.printf("Event name: %s\naccepted: %d\n", name.c_str(), accepted);
            strftime(buffer, 80, "%x - %I:%M%p", &start);
            printf("start time : |%s|\n", buffer);
            printf("start time_str: %s\n", start_str.c_str());
            strftime(buffer, 80, "%x - %I:%M%p", &end);
            printf("end time : |%s|\n", buffer);
            printf("end time_str: %s\n", end_str.c_str());
            printf("Duration %lf[secs]\n", duration);
        };
    };

    void get_event_summary_str(WiFiClient *stream, String &str_out)
    {

        if (stream->find("summary\": "))
        {
            str_out = stream->readStringUntil(',');
        }
    }

    /*
    return number of bytes readden from stream
    */
    void get_event_start_str(WiFiClient *stream, Event &event_out)
    {
        if (stream->find("start\": "))
        {
            stream->find("dateTime\": ");

            event_out.start_str = stream->readStringUntil('+'); // Cut the +02:00 GMT offset

            // I want to replace the "T" that is stuck between date and time, with a space
            // the ASCII code for a blank space is the decimal number 32, or the binary number 0010 00002.
            event_out.start_str.setCharAt(11, 32);

            memset(&event_out.start, 0, sizeof(event_out.start)); // tm initialization

            auto ret = strptime(event_out.start_str.c_str(), "\"%Y-%m-%d %H:%M:%S", &event_out.start);
            if (ret == NULL)
            {
                printf("\nstrptime failed\n");
            }
            else
            {
                // event_out.start.tm_year = 1900 + event_out.start.tm_year;
                // event_out.start.tm_mon = 1 + event_out.start.tm_mon;
                // printf("tm_hour:  %d\n", event_out.start.tm_hour);
                // printf("tm_min:  %d\n", event_out.start.tm_min);
                // printf("tm_sec:  %d\n", event_out.start.tm_sec);
                // printf("tm_mon:  %d\n", event_out.start.tm_mon);
                // printf("tm_mday:  %d\n", event_out.start.tm_mday);
                // printf("tm_year:  %d\n", event_out.start.tm_year);
                // printf("tm_yday:  %d\n", event_out.start.tm_yday);
                // printf("tm_wday:  %d\n", event_out.start.tm_wday);
            }
        }
    }

    /*
    return number of bytes readden from stream
    */
    void get_event_end_str(WiFiClient *stream, Event &event_out)
    {
        // Serial.println(__PRETTY_FUNCTION__);
        if (stream->find("end\": "))
        {
            stream->find("dateTime\": ");
            event_out.end_str = stream->readStringUntil('+'); // Cut the +02:00 GMT offset

            // I want to replace the "T" that is stuck between date and time, with a space
            // the ASCII code for a blank space is the decimal number 32, or the binary number 0010 00002.
            event_out.end_str.setCharAt(11, 32);

            memset(&event_out.end, 0, sizeof(event_out.end)); // tm initialization

            auto ret = strptime(event_out.end_str.c_str(), "\"%Y-%m-%d %H:%M:%S", &event_out.end);
            if (ret == NULL)
            {
                printf("\nstrptime failed\n");
            }
            else
            {
                event_out.duration = difftime(mktime(&event_out.end), mktime(&event_out.start));
                // event_out.end.tm_year = 1900 + event_out.end.tm_year;
                // event_out.end.tm_mon = 1 + event_out.end.tm_mon;
            }
        }
    }

    void get_event_acceptance_status(WiFiClient *stream, bool &is_accepted_out)
    {
        if (stream->find("attendees"))
        {
            if (stream->find((token_data::USER_NAME + token_data::USER_DOMAIN).c_str())) // TODO change to variable
            {
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

    int parse_calendar(HTTPClient &http, Event events[], int events_len)
    {
        // get length of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();

        // get tcp stream
        WiFiClient *stream = http.getStreamPtr();

        int eventNum{0};
        // read all data from server
        while (http.connected() && (len > 0 || len == -1))
        {
            // get available data size
            size_t size = stream->available();

            if (size)
            {

                get_event_summary_str(stream, events[eventNum].name);
                get_event_start_str(stream, events[eventNum]);
                get_event_end_str(stream, events[eventNum]);
                get_event_acceptance_status(stream, events[eventNum].accepted);

                Serial.println("\n*******************************************\n");
                Serial.println("Found todays event: ");
                events[eventNum++].Print();
                Serial.println("\n*******************************************\n");
            }
            else if (eventNum > events_len)
            {
                Serial.println("Readden from stream maximum number of events.");
                return eventNum;
            }

            else
            {
                Serial.println("No more events in stream.");
                return eventNum;
            }

            delay(1);
        }

        Serial.printf("\n[HTTP] connection closed or file end.\n");
        return 0;
    }

    bool should_fetch_calendar()
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
        case 8:
        case 9:
        case 12:
        case 16:
        case 19:
            if (timeinfo.tm_min < 1) // fetch for calendar only at the first minute of these hours
            {
                printf("\nHo, its time for fetching the calendar\n");
                return true;
            }

        default:
            return false;
        }
    }

} // namespace calendar
