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
    void turn_off_leds()
    {
        FastLED.clear(true);
    }

    // Blocking, until meeting is over
    void show_event_progress(calendar::Event &event)
    {

        while (event.time_left > 30)
        {

            auto progress_precentage = 1 - (event.time_left / event.duration);
            printf("Event progress %lf% \n", progress_precentage);
            auto leds_to_turn_on = (NUM_LEDS / 2) * progress_precentage;
            printf("raw #_of_leds_to_turn_on: %lf\n", leds_to_turn_on);
            if (leds_to_turn_on - (int)leds_to_turn_on < 0.5)
            {
                leds_to_turn_on = floor(leds_to_turn_on);
                leds_to_turn_on = (leds_to_turn_on == 0) ? 1 : leds_to_turn_on; // Turn leds at the begining of an event
            }
            else
                leds_to_turn_on = ceil(leds_to_turn_on);

            printf("Turning %lf leds on.\n", leds_to_turn_on);
            for (size_t i = 0; i < leds_to_turn_on; i++)
            {
                leds[i] = CRGB::Red;
                leds[NUM_LEDS - 1 - i] = CRGB::Red;
                FastLED.show();
            }
            delay(1000 * 5);
            event.UpdateTimeLeft();
        }

        // Delay in ms
        int event_ending_blinking_delay = 1000;
        while (event.time_left > 0)
        {
            // Event is 30 seconds to tis end.
            // Turn the LED on, then pause
            leds[NUM_LEDS - 1] = CRGB::Red;
            FastLED.show();
            delay(event_ending_blinking_delay);
            // Now turn the LED off, then pause
            leds[NUM_LEDS - 1] = CRGB::Black;
            FastLED.show();

            if (event_ending_blinking_delay > 10)
            {
                event_ending_blinking_delay -= 15;
            }
            delay(event_ending_blinking_delay);
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
        FastLED.clear(true);
    }

} // ledstools