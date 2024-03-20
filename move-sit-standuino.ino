////////////////////////////////////////////////////////////////////////////////
//!
//! @file             move-sit-standuino.ino
//!
//! @author           Jim Konish
//!
//! @date             2024.03.18
//!
//! @brief            Brief description
//!
//! @details          Detailed description
//!
//! @copyright        2010-2024. Jim Konish
//!                   MIT License
//!
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//  Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>


////////////////////////////////////////////////////////////////////////////////
//  Private Macros
////////////////////////////////////////////////////////////////////////////////

#define MILLISECONDS_PER_MINUTE (60000)

////////////////////////////////////////////////////////////////////////////////
//  Private Types
////////////////////////////////////////////////////////////////////////////////

//! Pin index type
typedef enum
{
    PIN_SIT_YELLOW,
    PIN_STAND_BLUE,
    PIN_MOVE_GREEN,
    PIN_BUTTON,
    NUM_PINS
} pinIndex_t;


//! comment
typedef enum
{
    STATE_SIT,
    STATE_STAND,
    STATE_MOVE,
    NUM_STATES
} displayState_t;


////////////////////////////////////////////////////////////////////////////////
//  Private Function Prototypes
////////////////////////////////////////////////////////////////////////////////

//! Sets one color output to be on, others off
static void setColor(pinIndex_t colorPin);

//! Do a color dance so the change might get noticed before the audio implementation
static void colorDance(void);

//! Display state machine
static void updateDisplay(uint32_t time);

////////////////////////////////////////////////////////////////////////////////
//  Public Data
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
//  Private Data
////////////////////////////////////////////////////////////////////////////////


static uint8_t pinNumbers[NUM_PINS] =
{
    [PIN_SIT_YELLOW] = 4,
    [PIN_STAND_BLUE] = 5,
    [PIN_MOVE_GREEN] = 3,
    [PIN_BUTTON] = 10
};

static displayState_t displayState = STATE_MOVE;

static uint32_t nextStateTime = 0;

////////////////////////////////////////////////////////////////////////////////
//  Public Function Implementations
////////////////////////////////////////////////////////////////////////////////

void setup()
{
    Serial.begin(9600);
    pinMode(pinNumbers[PIN_SIT_YELLOW], OUTPUT);
    pinMode(pinNumbers[PIN_STAND_BLUE], OUTPUT);
    pinMode(pinNumbers[PIN_MOVE_GREEN], OUTPUT);
    pinMode(pinNumbers[PIN_BUTTON], INPUT_PULLUP);
}

void loop()
{
    uint32_t time = millis();

    updateDisplay(time);
}



////////////////////////////////////////////////////////////////////////////////
//  Private Function Implementations
////////////////////////////////////////////////////////////////////////////////

static void setColor(pinIndex_t colorPin)
{
    for (uint8_t i = 0; i < sizeof(pinNumbers)/sizeof(pinNumbers[0]); ++i)
    {
        uint8_t pinState = (i == colorPin) ? HIGH : LOW;

        digitalWrite(pinNumbers[i], pinState);
    }
}

static void colorDance(void)
{
    // TODO refactor to non-blocking
    for (size_t i = 0; i < 10; ++i)
    {
        setColor(PIN_SIT_YELLOW);

        delay(100);

        setColor(PIN_STAND_BLUE);

        delay(100);

        setColor(PIN_MOVE_GREEN);

        delay(100);
    }
}

static void advanceDisplayState(void)
{
    displayState = (displayState_t)((displayState + 1) % NUM_STATES);
}

static void updateDisplay(uint32_t time)
{


    // If we timed out on the current state or the button was pressed
    // TODO debounce handling
    bool timeToAdvance = (time >= nextStateTime);
    bool buttonPressed = (digitalRead(pinNumbers[PIN_BUTTON]) == LOW);

    if(timeToAdvance)
    {
        Serial.println("Advancing state due to time");
    }

    if(buttonPressed)
    {
        Serial.println("Advancing state due to button");
        delay(200); // TODO this is a hack in place of real debounce
    }

    if(timeToAdvance || buttonPressed)
    {
        Serial.println(displayState);
        advanceDisplayState();

        if(!buttonPressed)
        {
            colorDance();
        }

        switch(displayState)
        {
            case STATE_SIT:
                setColor(PIN_SIT_YELLOW);
                nextStateTime = time + (20 * MILLISECONDS_PER_MINUTE);
                break;

            case STATE_STAND:
                setColor(PIN_STAND_BLUE);
                nextStateTime = time + (8 * MILLISECONDS_PER_MINUTE);
                break;

            case STATE_MOVE:
                setColor(PIN_MOVE_GREEN);
                nextStateTime = time + (2 * MILLISECONDS_PER_MINUTE);
                break;

            default:
                break;
        }
    }
}







