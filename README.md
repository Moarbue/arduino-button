# Button Arduino Library

A simple Arduino library to interact with buttons.

# Example

```C++
#include <Arduino.h>
#include "button.h"

const uint8_t BUTTON_PIN = 2;

Button b(BUTTON_PIN);

// callback functions for all the button events
void onPress(void)       { digitalWrite(LED_BUILTIN, HIGH); }
void onRelease(void)     { digitalWrite(LED_BUILTIN, LOW); }
void onSinglePress(void) { Serial.println("single press action detected"); }
void onDoublePress(void) { Serial.println("double press action detected"); }
void onLongPress(void)   { Serial.println("long press action detected"); }

void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    // Use the internal pullup resistor for the button
    // other options are ACTIVE_LOW and ACTIVE_HIGH
    b.begin(ACTIVE_LOW_INTERNAL);

    // Set our callback functions
    // For more info about each event you can check out each function
    b.setOnPressFunc(onPress);
    b.setOnReleaseFunc(onRelease);
    b.setOnSinglePressFunc(onSinglePress);
    b.setOnDoublePressFunc(onDoublePress);
    b.setOnLongPressFunc(onLongPress);

    // Set the debounce time to prevent bouncing
    b.setDebounceTime(50);
    // Set the minimal long press duration
    b.setLongPressTime(3000);
    // Set the time interval between two button presses
    b.setDoublePressInterval(200);
}

void loop()
{
    // Update the button and check if any event occured
    b.update();
}
```

You can also check the [examples](examples) directory for examples.