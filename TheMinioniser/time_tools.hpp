#pragma once
#include </home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp>

namespace timetools
{
    const String dayEndTime = "20";
    const String dayStartTime = "06";
    String dateDay = "2";
    String dateMonth = "03";
    String dateYear = "2022";

    // Ntp information
    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 2 * 60 * 60;
    const int daylightOffset_sec = 3600;

    // Update Local time using NTP server
    // Update query day,month,year time-variables
    bool updateLocalTime()
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time");
            return false;
        }
        // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
        dateDay = String(timeinfo.tm_mday);
        dateMonth = String(1 + timeinfo.tm_mon); // 0-11
        dateYear = String(1900 + timeinfo.tm_year);
        return true;
        // printf("Updated query day\\month\\year: %s\\%s\\%s\n", dateDay.c_str(), dateMonth.c_str(), dateYear.c_str());
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

            auto secs_since_start_time = difftime(mktime(&timeinfo), mktime(&events[i].start));
            auto secs_until_end_time = difftime(mktime(&timeinfo), mktime(&events[i].end)); // If end time is in the future this will result in negative number.

            if (secs_since_start_time >= 0 && secs_until_end_time < 0)
            {

                printf("There's an event going on right now\n");
                if (events[i].accepted)
                {
                    printf("Were participating this event\n");
                    events[i].time_left = fabs(secs_until_end_time);
                    printf("Event time left %lf[secs]\n", events[i].time_left);
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