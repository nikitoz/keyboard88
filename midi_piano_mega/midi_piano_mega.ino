#include <MIDI.h>
#include <Keypad.h>

// keyboard 15x6, columns are for big connector

 // Created and binds the MIDI interface to the default hardware Serial port
 MIDI_CREATE_DEFAULT_INSTANCE();

const byte rows = 6;
const byte cols = 15;

char keys[rows][cols] = {
  {108,102,96,90,84,78,72,66,60,54,48,42,36,30,24},
  {107,101,95,89,83,77,71,65,59,53,47,41,35,29,23},
  {106,100,94,88,82,76,70,64,58,52,46,40,34,28,22},
  {105, 99,93,87,81,75,69,63,57,51,45,39,33,27,21},
  {104, 98,92,86,80,74,68,62,56,50,44,38,32,26,20},
  {103, 97,91,85,79,73,67,61,55,49,43,37,31,25,19}
};

const int fade_away_time_ms = 5000;
const int can_release_time_ms = 100;

byte row_pins[rows] = {24,25,26,27,28,29};
byte row_aftertouch_pins[rows] = {18,19,20,21,22,23};
byte col_pins[cols] = {30,31,32,33,34,35,36,37,38,39,40,41,42,43,44};

Keypad kpd = Keypad(makeKeymap(keys), row_pins, col_pins, rows, cols);
Keypad kpd_aftertouch = Keypad(makeKeymap(keys), row_aftertouch_pins, col_pins, rows, cols);

void setup()
{
  MIDI.begin(4);        // will listen incoming MIDI at channel 4
  Serial.begin(115200); // will change baud rate of MIDI traffic from 31250 to 115200
  kpd.setDebounceTime(0);
  kpd.setHoldTime(fade_away_time_ms);
}

void loop()
{
    if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if (!kpd.key[i].stateChanged)   // Only find keys that have changed state.
              continue;
            
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                        MIDI.sendNoteOn(kpd.key[i].kchar, 49, 1);
                        break;
                    case HOLD: 
                    break;
                    case RELEASED:
                        MIDI.sendNoteOff(kpd.key[i].kchar, 49, 1);
                        break;
                    case IDLE:
                    break;
                }
        }
    }
    if (kpd_aftertouch.getKeys()) {
        for (int i=0; i<LIST_MAX; i++)
        {
            if (!kpd_aftertouch.key[i].stateChanged)
              continue;
            
                switch (kpd_aftertouch.key[i].kstate) {
                    case PRESSED:
                        MIDI.sendAfterTouch(kpd_aftertouch.key[i].kchar, 49, 1);
                        break;
                    case HOLD: 
                        break;
                    case RELEASED:
                        break;
                    case IDLE:
                        break;
                }
        }
    }
}
