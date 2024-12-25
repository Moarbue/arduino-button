#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Arduino.h>

typedef void (* ButtonOnPressFunc)(void);
typedef void (* ButtonOnReleaseFunc)(void);
typedef void (* ButtonOnSinglePressFunc)(void);
typedef void (* ButtonOnDoublePressFunc)(void);
typedef void (* ButtonOnLongPressFunc)(void);

typedef enum {
    ACTIVE_LOW,             // Button is configured active low  externally
    ACTIVE_HIGH,            // Button is configured active high externally
    ACTIVE_LOW_INTERNAL,    // Button will be configured active low internally (no pullup resistor required)
} Button_Mode;

class Button {
public:
    Button(uint8_t pin);

    void begin(Button_Mode mode);
    bool isPressed(void);
    void setDebounceTime(uint16_t ms);
    void setDoublePressInterval(uint16_t ms);
    void setLongPressTime(uint16_t ms);
    void setOnPressFunc(ButtonOnPressFunc onPressFunc);
    void setOnReleaseFunc(ButtonOnReleaseFunc onReleaseFunc);
    void setOnSinglePressFunc(ButtonOnSinglePressFunc onSinglePressFunc);
    void setOnDoublePressFunc(ButtonOnDoublePressFunc onDoublePressFunc);
    void setOnLongPressFunc(ButtonOnLongPressFunc onLongPressFunc);
    void update(void);

private:
    uint8_t pin;                        // button pin
    uint8_t activeState;                // HIGH or LOW, depending on wheter the button is acitve high/low
    uint8_t state;                      // the current state of the button HIGH or LOW

    ButtonOnPressFunc onPressFunc;              // function that gets called once when a button press event is registered
    ButtonOnReleaseFunc onReleaseFunc;          // function that gets called once when a button release event is registered
    ButtonOnSinglePressFunc onSinglePressFunc;  // function that gets called once when a button single press action is registered
    ButtonOnDoublePressFunc onDoublePressFunc;  // function that gets called once when a button double press action is registered
    ButtonOnLongPressFunc onLongPressFunc;      // function that gets called once when a button long press action is registered

    uint16_t debounceTime;              // how long a button must be pressed/released until it is registered as a button press/release event
    unsigned long lastDebounceTime;     // timestamp since last debounce occured
    int lastState;                      // last state of the button

    uint16_t doublePressInterval;       // how long the gap between the two presses can be
    uint16_t longPressTime;             // how long the minimal press time for a long press event can be
    unsigned long lastPressTime;        // timestamp since last press event
    unsigned long lastReleaseTime;      // timestamp since last release event
    int action;                         // the action that is currently performed
};

#endif // _BUTTON_H_