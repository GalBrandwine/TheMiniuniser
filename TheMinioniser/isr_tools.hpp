#pragma once
#include <Esp.h>
namespace isr_tools
{

    struct Button
    {
        const uint8_t PIN;
        uint32_t numberKeyPresses;
        bool pressed;
    };

    Button button1 = {2, 0, false};

    void IRAM_ATTR isr()
    {
        button1.numberKeyPresses += 1;
        button1.pressed = true;
    }
    void init_isr()
    {
        pinMode(button1.PIN, INPUT_PULLUP);
        attachInterrupt(button1.PIN, isr, FALLING);
    }
}
