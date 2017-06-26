#include <MIDI.h>
#include <Keypad.h>

/*
    This is Arduino Mega program which implements midi controller for
    Korg C-15S digital piano.
    I implemented it after my Korg motherboard died.
    Keyboard's matrix outputs and inputs are directly connected to Arduino Mega
    inputs.
    Keyboard matrix is 15x6. Bigger connector is for columns.
    There are actually two matrices, the second one (at least in my
    implementation) is for aftertouch.

    I connect arduino to GNU/Linux, running ttymidi and lmms (I use soundfonts
    for sound generation).

    TODO:   base velocity and its control
            pedals
            pitch switch
*/

// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

const byte rows = 6;
const byte cols = 15;

char keys[rows][cols] = {{108, 102, 96, 90, 84, 78, 72, 66, 60, 54, 48, 42, 36, 30, 24},
                         {107, 101, 95, 89, 83, 77, 71, 65, 59, 53, 47, 41, 35, 29, 23},
                         {106, 100, 94, 88, 82, 76, 70, 64, 58, 52, 46, 40, 34, 28, 22},
                         {105, 99, 93, 87, 81, 75, 69, 63, 57, 51, 45, 39, 33, 27, 21},
                         {104, 98, 92, 86, 80, 74, 68, 62, 56, 50, 44, 38, 32, 26, 20},
                         {103, 97, 91, 85, 79, 73, 67, 61, 55, 49, 43, 37, 31, 25, 19}};

const int fade_away_time_ms = 5000;
const int can_release_time_ms = 100;
const int base_velocity = 79;
const int aftertouch_velocity_bump = 10;
int pedal_added_velocity = 0;
// Mega pins for matrix rows, controlling note on/off
byte row_pins[rows] = {24, 25, 26, 27, 28, 29};
// Mega pins for matrix rows, controlling aftertouch
byte row_aftertouch_pins[rows] = {18, 19, 20, 21, 22, 23};
// Mega pins for matrix columns
byte col_pins[cols] = {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44};

Keypad kpd = Keypad(makeKeymap(keys), row_pins, col_pins, rows, cols);
Keypad kpd_aftertouch = Keypad(makeKeymap(keys), row_aftertouch_pins, col_pins, rows, cols);

void setup() {
    // Listen incoming MIDI at channel 4
    MIDI.begin(4);
    // Change baud rate of MIDI traffic from 31250 to 115200
    Serial.begin(115200);
    kpd.setDebounceTime(0);
    kpd.setHoldTime(fade_away_time_ms);
}

void process_key_pressed() {
    if (!kpd.getKeys())
        return;
    const int velocity = base_velocity + pedal_added_velocity;
    for (int i = 0; i < LIST_MAX; ++i) {
        if (!kpd.key[i].stateChanged)
            continue;
        switch (kpd.key[i].kstate) {
        case PRESSED:
            MIDI.sendNoteOn(kpd.key[i].kchar, velocity, 1);
            break;
        case RELEASED:
            MIDI.sendNoteOff(kpd.key[i].kchar, velocity, 1);
            break;
        case IDLE:
        case HOLD:
            break;
        }
    }
}

void process_pedals() {}

void process_volume_control() {}

void process_aux_buttons() {}

void process_aftertouch() {
    if (!kpd_aftertouch.getKeys())
        return;
    const int velocity = base_velocity + pedal_added_velocity + aftertouch_velocity_bump;
    for (int i = 0; i < LIST_MAX; i++) {
        if (!kpd_aftertouch.key[i].stateChanged)
            continue;

        if (kpd_aftertouch.key[i].kstate == PRESSED) {
            MIDI.sendAfterTouch(kpd_aftertouch.key[i].kchar, velocity, 1);
        }
    }
}

void loop() {
    process_pedals();
    process_volume_control();
    process_key_pressed();
    process_aftertouch();
}
