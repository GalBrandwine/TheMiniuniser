#pragma once
#include </home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp>
#include <FastLED.h>

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 12
// #define CLOCK_PIN 13

#define NUM_LEDS 6
// Define the array of leds
CRGB leds[NUM_LEDS];

namespace ledstools
{
    void show_event_progress(calendar::Event &event)
    {
        while (event.time_left > 0)
        {
            // Turn the LED on, then pause
            leds[0] = CRGB::Red;
            FastLED.show();
            delay(500);
            // Now turn the LED off, then pause
            leds[0] = CRGB::Black;
            FastLED.show();
            delay(500);
            event.UpdateTimeLeft();
        }
    }
    // Blocking until the meeting is over
    void simple_handle_event(const calendar::Event &event)
    {
        // TODO: fix this to turn on the LEDS for the remaining of the meeting
        event.Print();

        digitalWrite(4, HIGH);
        delay(event.time_left * 1000);
        digitalWrite(4, LOW);
    }
    void init_leds()
    {
        FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); // GRB ordering is assumed
    }

} // ledstools