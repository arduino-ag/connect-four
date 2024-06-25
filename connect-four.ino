#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define ROW_COUNT 6
#define COLUMN_COUNT 7

#define LEFT 7
#define ENTER 6
#define RIGHT 5
#define START 4
#define RESET 3

#define LED_PIN 11
#define LED_COUNT 42

#define CELL_COUNT 42

Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

enum State
{
    Empty,
    Selecting,
    NotSelectable,
    TeamOne,
    TeamTwo,
};

enum ColorState
{
    ColorEmpty = 0x000000,
    ColorSelecting = 0xFFFFFF,
    ColorNotSelectable = 0xAAAAAA,
    ColorTeamOne = 0x0000FF,
    ColorTeamTwo = 0xFF0000,
};

static State game[CELL_COUNT] = {};

void setup();
void loop();
void print_state();
boolean areFourConnected(State player);
State get_board(int x, int y);
void set_board(int x, int y, State state);
int turn_first_player();
int turn_second_player();
int play_a_chip(State player);
ColorState color_from_state(State state);

void setup()
{
    Serial.begin(9600);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif

    pinMode(LEFT, INPUT);
    pinMode(ENTER, INPUT);
    pinMode(RIGHT, INPUT);
    pinMode(START, INPUT);

    pinMode(RESET, OUTPUT);

    pixels.begin();

    pixels.clear();

    print_state();
}

// TODO add a wining animation
void loop()
{
    static int status = 0;

    status = status | turn_first_player();

    if (status != 0) {
        for (int i = 0; i < CELL_COUNT; i++) {
            game[i] = State::TeamOne;
        }
        
        while (true) {}
    }

    status = status | turn_second_player();
    
    if (status != 0) {
        for (int i = 0; i < CELL_COUNT; i++) {
            game[i] = State::TeamTwo;
        }
        
        while (true) {}
    }
}

void print_state()
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        pixels.setPixelColor(i, (uint32_t)color_from_state(game[i]));
    }

    pixels.show();
}

ColorState color_from_state(State state)
{
    switch (state)
    {
    case State::Empty:
        return ColorState::ColorEmpty;
    case State::Selecting:
        return ColorState::ColorSelecting;
    case State::NotSelectable:
        return ColorState::ColorNotSelectable;
    case State::TeamOne:
        return ColorState::ColorTeamOne;
    case State::TeamTwo:
        return ColorState::ColorTeamTwo;
    default:
        return ColorState::ColorEmpty;
    }
}

boolean areFourConnected(State player)
{

    // horizontalCheck
    for (int j = 0; j < ROW_COUNT - 3; j++)
    {
        for (int i = 0; i < COLUMN_COUNT; i++)
        {
            if (get_board(i, j) == player && get_board(i, j + 1) == player && get_board(i, j + 2) == player && get_board(i, j + 3) == player)
            {
                return true;
            }
        }
    }
    
    // verticalCheck
    for (int i = 0; i < COLUMN_COUNT - 3; i++)
    {
        for (int j = 0; j < ROW_COUNT; j++)
        {
            if (get_board(i, j) == player && get_board(i + 1, j) == player && get_board(i + 2, j) == player && get_board(i + 3, j) == player)
            {
                return true;
            }
        }
    }

    // ascendingDiagonalCheck
    for (int i = 3; i < COLUMN_COUNT; i++)
    {
        for (int j = 0; j < ROW_COUNT; j++)
        {
            if (get_board(i, j) == player && get_board(i - 1, j + 1) == player && get_board(i - 2, j + 2) == player && get_board(i - 3, j + 3) == player)
                return true;
        }
    }
    // descendingDiagonalCheck
    for (int i = 3; i < COLUMN_COUNT; i++)
    {
        for (int j = 3; j < ROW_COUNT; j++)
        {
            if (get_board(i, j) == player && get_board(i - 1, j - 1) == player && get_board(i - 2, j - 2) == player && get_board(i - 3, j - 3) == player)
                return true;
        }
    }
    return false;
}

// helper function to access an 1-dimensional array as a 2-dimensional one
State get_board(int x, int y)
{
    return game[x + y * COLUMN_COUNT];
}

// helper function to write to an 1-dimensional array as a 2-dimensional one
void set_board(int x, int y, State state)
{
    game[x + y * COLUMN_COUNT] = state;
}

int turn_first_player() {
    if (areFourConnected(State::TeamOne)) {
        return 1;
    }

    play_a_chip(State::TeamOne);

    return 0;
}

int turn_second_player() {
    if (areFourConnected(State::TeamOne)) {
        return 2;
    }

    play_a_chip(State::TeamTwo);

    return 0;
}

int play_a_chip(State player) {
    int column = 3;
    int row = ROW_COUNT;
    State old_state;

    while (true) {
        // get button states
        int enter_status = digitalRead(ENTER);
        int right_status = digitalRead(RIGHT);
        int left_status = digitalRead(LEFT);

        if (enter_status && get_board(column, 0) == State::Empty) {
            break;
        }

        if (right_status && column < 6) {
            column++;
        }

        if (left_status && column > 0) {
            column++;
        }

        old_state = get_board(column, 0);

        // check if a cell is already occupied or not
        if (old_state == State::TeamOne || old_state == State::TeamTwo) {
            set_board(column, 0, State::NotSelectable);
        } else {
            set_board(column, 0, State::Selecting);
        }

        // print state with currently selected cell
        print_state();

        set_board(column, 0, old_state);

        delay(400);
    }

    // check which is the last free row
    for (row = ROW_COUNT - 1; row >= 0; row--) {
        if (get_board(column, row) == State::Empty) {
            break;
        }
    }
    
    // TODO add animation for falling piece
    set_board(column, row, player);
}
