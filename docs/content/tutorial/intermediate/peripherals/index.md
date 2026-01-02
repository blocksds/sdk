---
title: Peripherals
weight: 11
---

## 1. Introduction

The Nintendo DS and DSi has several peripherals that you're unlikely to use in
most applications, but can be very useful in some specific applications.

## 2. Battery

You can check the status of the battery easily, but the information you can
obtain depends on the DS model:

- Original DS: You can only check if the battery level is high or low.
- DS Lite: You can also check if the charger is connected or not. Some of them
  support reading 15 charge levels.
- DSi: You can check if the charger is connected or not, and 15 different charge
  levels.

Libnds abstracts the different consoles and it provides one single function to
check this information from both the ARM9 and ARM7:

```c
u32 value = getBatteryLevel();
unsigned int battery_level = value & BATTERY_LEVEL_MASK;
bool charger_connected = value & BATTERY_CHARGER_CONNECTED;

printf("Current charge level: %u\n", battery_level);
printf("Charger connected: %s\n", charger_connected ? "Yes" : "No");
```

You can check this code in action in this example:
[`examples/peripherals/battery_status`](https://github.com/blocksds/sdk/tree/master/examples/peripherals/battery_status)

## 3. Hardware divider and square root

The ARM9 has access to some registers that can calculate integer divisions and
square roots by hardware. This can help you optimize applications because the
CPUs of the DS don't support division or square root and they need to be
calculated by software. The registers work asynchronously, so you can even let
the hardware calculate something while you do other things:

```c
div32_asynch(7000, 3); // Start division
u32 sqrt_out = sqrt64(12345 * 12345);
u32 div_out = div32_result(); // Read division result
```

You can check all helpers provided by libnds in the documentation
[here](https://blocksds.skylyrac.net/libnds/math_8h.html). There are some
helpers to do regular integer division and square root, as well as some helpers
to do fixed point divisions and square roots.

You can check this code in action in this example:
[`examples/peripherals/maths_coprocessor`](https://github.com/blocksds/sdk/tree/master/examples/peripherals/maths_coprocessor)

## 4. Real Time Clock

You can get the current time by using the standard C function `time()`. You can
use its returned value the same way you'd use it in any C program. For example:

```c
#include <time.h>

// ...

char str[100];

time_t t = time(NULL);
struct tm *tmp = localtime(&t);

if (strftime(str, sizeof(str), "%Y-%m-%dT%H:%M:%S%z", tmp) == 0)
    snprintf(str, sizeof(str), "Failed to get time");

printf("%s", str);
```

You can also set the date and time in the RTC chip. This must be done from the
ARM7, libnds doesn't provide any helper to do it from the ARM9 at the moment.
You can set the time and date simultaneously or just the time:

```c
rtcTimeAndDate rtc_time_date = { ... };

if (rtcTimeAndDateSet(&rtc_time_date) != 0)
{
    // Error
}

rtcTime rtc_time = { ... };

if (rtcTimeet(&rtc_time) != 0)
{
    // Error
}
```

Remember to subtract 2000 from the real year when writing the values of a
`rtcTimeAndDate` struct!

{{< callout type="tip" >}}
Games like Animal Crossing detect manual changes of the time/date in the
firmware settings because the firmware settings store the fact that you've
modified the RTC settings (`PersonalData->rtcOffset`). Changing the values with
libnds won't affect that flag, so the change won't be detected!
{{< /callout >}}

You can check this code in action in this example:
[`examples/time/rtc_set_get`](https://github.com/blocksds/sdk/tree/master/examples/time/rtc_set_get)

## 5. Firmware user settings

Players can set settings like their name, birthday, etc, in a small flash memory
present in all DS models. This memory also stores the Wi-Fi connection settings,
for example.

{{< callout type="warning" >}}
The default ARM7 core in BlocksDS reads the settings with function
`readUserSettings()`, which is called at the beginning of `main()`. You may have
to wait for a couple of frames for the function to run and load the settings.
{{< /callout >}}

You can access them with the `PersonalData` pointer. You can see information
about its members [here](https://blocksds.skylyrac.net/libnds/structtPERSONAL__DATA.html).
For example, you can just type `PersonalData->birthDay` to read the user birth
day.

The name and personal message are encoded as UTF-16LE format, not UTF-8. You can
use function `utf16_to_utf8()` of libnds to convert it.

Check the following example to see how to do this in practice:
[`examples/firmware/user_settings`](https://github.com/blocksds/sdk/tree/master/examples/firmware/user_settings)

## 6. LCD backlight

The backlight of the LCD screens can be adjusted by the application. This is
useful when entering sleep mode, for example. However, different DS models have
different capabilities, Libnds provides `systemSetBacklightLevel()`, which takes
a value between 0 and 5, and this is how it behaves in different models:

- DSi: 5 levels of brightness (1 to 5, or `PM_BACKLIGHT_MIN` to
  `PM_BACKLIGHT_MAX`).
- DS Lite: 4 levels of brightness (2 to 5). Level 1 is internally set to
  level 2.
- DS: The screen can be turned off or on. Levels 1 to 5 are internally set
  to level 5 (full brightness). Some models of the DS support the same
  levels of brightness of the DS Lite. In them, the function behaves the
  same way as on DS Lite.

Level 0 (`PM_BACKLIGHT_OFF`) turns the backlight off.

DS and DS Lite consoles support turning on and off individual screens, but
`systemSetBacklightLevel()` doesn't support controlling the two screens
independently.

{{< callout type="warning" >}}
On DSi the brightness setting is persistent and it will be the setting used the
next time the console is turned on.
{{< /callout >}}

You can check this code in action in this example:
[`examples/peripherals/backlight_level`](https://github.com/blocksds/sdk/tree/master/examples/peripherals/backlight_level)

{{< callout type="error" >}}
This chapter is a work in progress...
{{< /callout >}}
