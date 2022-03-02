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

        for (size_t i = 0; i < today_num_of_events; i++)
        {
            events[i].Print();
            if (events[i].start.tm_hour < timeinfo.tm_hour && events[i].end.tm_hour > timeinfo.tm_hour)
            { // Were in an event
                if (events[i].accepted)
                {
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