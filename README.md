# TheMinioniser

A friendly & helpful google-calendar meeting tracker

## Pinout

| Component    | PIN |
| ------------ | --- |
| Reset button | 2   |
| Leds         | 12  |
| Speaker      | 13  |

## Dependencies

* <https://github.com/tzapu/WiFiManager>
* <https://github.com/FastLED/FastLED>

```shell
sudo apt install python3-serial
```

## Fixing USB serial TTL converter on Ubuntu 22.04

I use USB serial TTL converter, and for some reading; Ubuntu 22.04 couldn't recognize it - got **no /dev/ttyUSB0**
After reading [this](https://unix.stackexchange.com/questions/696001/dev-ttyusb0-is-available-but-after-try-to-call-its-gone) & viewing `dmesg` logs, fixed it by doing:

```shell
sudo apt remove brltty
```
