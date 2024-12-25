#include "button.h"

void defaultOnPressFunc(void) {}
void defaultOnReleaseFunc(void) {}
void defaultOnSinglePressFunc(void) {}
void defaultOnDoublePressFunc(void) {}
void defaultOnLongPressFunc(void) {}

typedef enum {
    ACTION_SINGLE_PRESS,
    ACTION_DOUBLE_PRESS,
    ACTION_LONG_PRESS,
    ACTION_NONE,
} Button_Action;


/// @brief Initialize a new button
/// @param pin the pin to witch the button is attached
/// @param isActiveLow wheter the button is active low (true) or active high (false)
Button::Button(uint8_t pin) 
    : pin(pin), 
      activeState(0), 
      state(0), 
      onPressFunc(defaultOnPressFunc), 
      onReleaseFunc(defaultOnReleaseFunc), 
      onSinglePressFunc(defaultOnSinglePressFunc),
      onDoublePressFunc(defaultOnDoublePressFunc), 
      onLongPressFunc(defaultOnLongPressFunc),
      debounceTime(0), 
      lastDebounceTime(0), 
      lastState(0),
      doublePressInterval(0),
      longPressTime(0),
      lastPressTime(0),
      lastReleaseTime(0),
      action(ACTION_NONE)
{}

/// @brief Setup the button pin
/// @param mode the mode of the button (active low, active high, active low internally)
void Button::begin(Button_Mode mode)
{
    switch (mode) {
        case ACTIVE_LOW:
            pinMode(pin, INPUT);
            activeState = LOW;
        break;

        case ACTIVE_HIGH:
            pinMode(pin, INPUT);
            activeState = HIGH;
        break;
        
        case ACTIVE_LOW_INTERNAL:
            pinMode(pin, INPUT_PULLUP);
            activeState = LOW;
        break;
    }

    // set the inital state to current button reading without debouncing
    state = digitalRead(pin);
    lastState = state;
}

/// @brief Check wheter the button is currently pressed
/// @return true if the button is pressed, otherwise false
bool Button::isPressed(void)
{
    return (state == activeState);
}

/// @brief Set how long a button must be pressed/released until it is registered as a button press/release event
/// @param ms the time in milliseconds
void Button::setDebounceTime(uint16_t ms)
{
    debounceTime = ms;
}

/// @brief Set time interval between the two button presses for a double press action to be scheduled
/// @param ms the time in milliseconds
void Button::setDoublePressInterval(uint16_t ms)
{
    doublePressInterval = ms;
}

/// @brief Set the minimal button press duration for a long press action to be scheduled
/// @param ms the time in milliseconds
void Button::setLongPressTime(uint16_t ms)
{
    longPressTime = ms;
}

/// @brief Set the function that is called when a button press event is registered
/// @param onPressFunc the function to call
void Button::setOnPressFunc(ButtonOnPressFunc onPressFunc)
{
    this->onPressFunc = onPressFunc;
}

/// @brief Set the function that is called when a button release event is registered
/// @param onReleaseFunc the function to call
void Button::setOnReleaseFunc(ButtonOnReleaseFunc onReleaseFunc)
{
    this->onReleaseFunc = onReleaseFunc;
}

/// @brief Set the function that is called when a button single press event is registered
/// A single press action is defined as a button `press` followed by a button `release` where the time between the press and release
/// is no longer than `longPressTime`. After the release a time period of at least `doublePressInterval` must elapse. Only after that
/// this function is called.
/// @note This action is not triggered if a double press action or a long press action is triggered
/// @param onSinglePressFunc the function to call
void Button::setOnSinglePressFunc(ButtonOnSinglePressFunc onSinglePressFunc)
{
    this->onSinglePressFunc = onSinglePressFunc;
}

/// @brief Set the function that is called when a button double press event is registered. 
/// A double press action is defined as a button `press` followed by a button `release`, 
/// where the time between press and release is no longer than `longPressTime`, 
/// after that a press must occur in a shorter time perdiod than `doublePressInterval`. The function is called on the second press event
/// but the action ends only after the button was released again.
/// @note This action prevents the single press action to trigger
/// @param onDoublePressFunc the function to call
void Button::setOnDoublePressFunc(ButtonOnDoublePressFunc onDoublePressFunc)
{
    this->onDoublePressFunc = onDoublePressFunc;
}

/// @brief Set the function that is called when a button long press event is registered
/// A long press action is defined as a button `press` for at least `longPressTime`. The function is called after `longPressTime` time has elapsed.
/// @note This action prevents the single press action to trigger
/// @param onLongPressFunc the function to call
void Button::setOnLongPressFunc(ButtonOnLongPressFunc onLongPressFunc)
{
    this->onLongPressFunc = onLongPressFunc;
}

/// @brief Update the button. Check wheter the button state changed or any actions were triggered etc.
void Button::update(void)
{
    unsigned long mills;
    int newState;

    // place millis inside local variable to ensure it doesn't change throughout the function
    mills = millis();

    // get current button state
    newState = digitalRead(pin);

    // detect falling/rising edge
    if (newState != lastState) {
        // reset time since last debounce action
        lastDebounceTime = mills;
    }

    // check wheter debounce time is over and button state has changed
    if (mills - lastDebounceTime >= debounceTime && newState != state) {
        // update button state
        state = newState;

        // check if button was pressed or released
        if (state == activeState) {
            onPressFunc();
            lastPressTime = mills;
            
            if (action == ACTION_SINGLE_PRESS) {
                // if a single press action was scheduled but another press event occured in the meantime it's a double press action
                // double press action is executed immediately but only finishes once the button is released again
                onDoublePressFunc();
                action = ACTION_DOUBLE_PRESS;
            } else {
                // schedule long press action
                action = ACTION_LONG_PRESS;
            }
        } else {
            onReleaseFunc();
            lastReleaseTime = mills;

            // finish double press action by resetting the action variable
            if (action == ACTION_DOUBLE_PRESS) {
                action = ACTION_NONE;
            } else if (lastReleaseTime - lastPressTime < longPressTime) {
                // schedule single press action
                action = ACTION_SINGLE_PRESS;
            }
        }
    }

    // single press action was scheduled and we waited until we are sure it is not a double press event
    if (action == ACTION_SINGLE_PRESS && mills - lastReleaseTime > doublePressInterval) {
        onSinglePressFunc();
        action = ACTION_NONE;
    }

    // long press action was scheduled and we waited until we are sure it is a long press event
    if (action == ACTION_LONG_PRESS && mills - lastPressTime >= longPressTime) {
        onLongPressFunc();
        action = ACTION_NONE;
    }

    // newState is now lastState
    lastState = newState;
}
