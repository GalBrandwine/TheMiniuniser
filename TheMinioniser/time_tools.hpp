#pragma once
#include </home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp>

namespace timetools
{
    const String dayEndTime = "20";
    const String dayStartTime = "01";
    String dateDay = "2";
    String dateMonth = "03";
    String dateYear = "2022";

    // Ntp information
    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 2 * 60 * 60;
    const int daylightOffset_sec = 3600;

    // Update Local time using NTP server
    // Update query day,month,year time-variables
    void printAndUpdateLocalTime()
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
    };

    /*
    return the index of an currently happening accepted event, -1 otherwise
    */
    int get_currently_ocuring_accepted_meeting(calendar::Event events[], int today_num_of_events)
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time");
            return -1;
        }
        // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

        for (size_t i = 0; i < today_num_of_events; i++)
        {
            Serial.println(events[i].name);
            printf("events[i] tm_hour:  %d\n", events[i].start.tm_hour);
            printf("events[i] tm_min:  %d\n", events[i].start.tm_min);
            printf("events[i] tm_sec:  %d\n", events[i].start.tm_sec);
            printf("events[i] tm_mon:  %d\n", events[i].start.tm_mon);
            printf("events[i] tm_mday:  %d\n", events[i].start.tm_mday);
            printf("events[i] tm_year:  %d\n", events[i].start.tm_year);
            printf("events[i] tm_yday:  %d\n", events[i].start.tm_yday);
            printf("events[i] tm_wday:  %d\n", events[i].start.tm_wday);
            printf("\n*\n");
            printf("events[i] tm_hour:  %d\n", events[i].end.tm_hour);
            printf("events[i] tm_min:  %d\n", events[i].end.tm_min);
            printf("events[i] tm_sec:  %d\n", events[i].end.tm_sec);
            printf("events[i] tm_mon:  %d\n", events[i].end.tm_mon);
            printf("events[i] tm_mday:  %d\n", events[i].end.tm_mday);
            printf("events[i] tm_year:  %d\n", events[i].end.tm_year);
            printf("events[i] tm_yday:  %d\n", events[i].end.tm_yday);
            printf("events[i] tm_wday:  %d\n", events[i].end.tm_wday);
            printf("\n*\n");
            printf("timeinfo.tm_hour:  %d\n", timeinfo.tm_hour);
            printf("timeinfo.tm_min:  %d\n", timeinfo.tm_min);
            printf("timeinfo.tm_sec:  %d\n", timeinfo.tm_sec);
            printf("timeinfo.tm_mon:  %d\n", timeinfo.tm_mon);
            printf("timeinfo.tm_mday:  %d\n", timeinfo.tm_mday);
            printf("timeinfo.tm_year:  %d\n", timeinfo.tm_year);
            printf("timeinfo.tm_yday:  %d\n", timeinfo.tm_yday);
            printf("timeinfo.tm_wday:  %d\n", timeinfo.tm_wday);
            // printf("Event duration in secs: %lf\n", difftime(mktime(&events[i].end), mktime(&events[i].start)));

            if (events[i].start.tm_hour <= timeinfo.tm_hour)
            { // Were in an event
                printf("Now is bigger than some event start time\n");
                bool in_event = false;

                if (timeinfo.tm_hour < events[i].end.tm_hour)
                {
                    printf("Were in an event\n");
                    in_event = true;
                }
                else if (timeinfo.tm_min < events[i].end.tm_min)
                {
                    printf("Were in an event\n");
                    in_event = true;
                }

                if (in_event && events[i].accepted)
                {
                    printf("Were participating this event\n");
                    return i;
                }
            }
        }
        return -1;
    }

    void track_active_event(const calendar::Event &event)
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time");
            return;
        }

        printf("Tracking current event: \n");
        event.Print();
    }
} // timetools