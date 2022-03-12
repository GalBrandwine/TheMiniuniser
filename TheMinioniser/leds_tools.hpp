#pragma once
#include </home/gal/dev/TheMiniuniser/TheMinioniser/calendar_parsers.hpp>
#include <FastLED.h>

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 12
// #define CLOCK_PIN 13

#define NUM_LEDS 8
// Define the array of leds
CRGB leds[NUM_LEDS];

namespace ledstools
{
    bool continues_effect_connecting_to_wifi_flag = false;
    BaseType_t continues_effect_connecting_to_wifiReturned;
    TaskHandle_t continues_effect_connecting_to_wifiHandle = NULL;
    enum STATES
    {
        CONNECTED_TO_WIFI,
        CONNECTING_TO_WIFI_FAILED,
        CONNECTING_TO_WIFI_CONTINUES,
        GETTING_CALENDAR,
        GETTING_CALENDAR_FAILED
    };

    void turn_off_leds()
    {
        FastLED.clear(true);
        if (continues_effect_connecting_to_wifi_flag && continues_effect_connecting_to_wifiReturned == pdPASS)
        {
            /* The task was created.  Use the task's handle to delete the task. */
            vTaskDelete(continues_effect_connecting_to_wifiHandle);
            continues_effect_connecting_to_wifi_flag = false;
        }
    }

    void connecting_to_wifi_continues_effect(void *pvParameters)
    {
        for (;;)
        {
            if (continues_effect_connecting_to_wifi_flag)
            {
                for (size_t i = 0; i < NUM_LEDS / 2; i++)
                {
                    leds[i] = CRGB::Orange;
                    leds[NUM_LEDS - 1 - i] = CRGB::Orange;

                    FastLED.show();
                    vTaskDelay(750);
                    leds[i] = CRGB::Black;
                    leds[NUM_LEDS - 1 - i] = CRGB::Black;
                    FastLED.show();
                    vTaskDelay(750);
                }
            }
            else
                vTaskDelay(3000);
        }
        vTaskDelete(NULL);
    }
    // Blocking, until meeting is over
    void communicate_status(const STATES state)
    {
        CRGB::HTMLColorCode html_color;
        switch (state)
        {
        case STATES::GETTING_CALENDAR:
            html_color = CRGB::Navy;
            break;
        case STATES::CONNECTED_TO_WIFI:
            html_color = CRGB::SeaGreen;
            for (size_t i = 0; i < 2; i++)
            {
                FastLED.clear(true);
                FastLED.delay(750);
                for (size_t i = 0; i < NUM_LEDS / 2; i++)
                {
                    leds[i] = html_color;
                    leds[NUM_LEDS - 1 - i] = html_color;
                }
                FastLED.show();
                FastLED.delay(750);
            }
            return;
        case STATES::CONNECTING_TO_WIFI_CONTINUES:
            continues_effect_connecting_to_wifi_flag = true;
            continues_effect_connecting_to_wifiReturned = xTaskCreate(
                connecting_to_wifi_continues_effect,   // Function that should be called
                "connecting_to_wifi_continues_effect", // Name of the task (for debugging)
                10000,                                 // Stack size (bytes)
                // (void *)&settings,    // Parameter to pass
                NULL,
                1,                                         // Task priority
                &continues_effect_connecting_to_wifiHandle // Task handle
            );
            return;
        case STATES::CONNECTING_TO_WIFI_FAILED:
        case STATES::GETTING_CALENDAR_FAILED:
            html_color = CRGB::Red;
            break;

        default:
            break;
        }

        FastLED.clear(true);
        for (size_t i = 0; i < NUM_LEDS / 2; i++)
        {
            leds[i] = html_color;
            leds[NUM_LEDS - 1 - i] = html_color;
        }
        FastLED.show();
    };
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
            }
            FastLED.show();
            delay(1000 * 5);
            event.UpdateTimeLeft();
        }

        // Delay in ms
        int event_ending_blinking_delay = 1000;
        while (event.time_left > 1)
        {
            // Event is 30 seconds to its end.
            // Turn the LED on, then pause
            leds[(NUM_LEDS / 2) - 1] = CRGB::Red;
            leds[(NUM_LEDS / 2)] = CRGB::Red;
            FastLED.show();
            delay(event_ending_blinking_delay);
            // Now turn the LED off, then pause
            leds[(NUM_LEDS / 2) - 1] = CRGB::Black;
            leds[(NUM_LEDS / 2)] = CRGB::Black;
            FastLED.show();

            if (event_ending_blinking_delay > 10)
            {
                event_ending_blinking_delay -= event_ending_blinking_delay * 0.08;
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