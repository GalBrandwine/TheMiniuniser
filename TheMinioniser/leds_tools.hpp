#pragma once
#include </home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp>

namespace ledstools
{
    // Blocking until the meeting is over
    void simple_handle_event(const calendar::Event &event)
    {
        // TODO: fix this to turn on the LEDS for the remaining of the meeting
        event.Print();

        digitalWrite(4, HIGH);
        delay(event.time_left * 1000);
        digitalWrite(4, LOW);
    }
} // ledstools