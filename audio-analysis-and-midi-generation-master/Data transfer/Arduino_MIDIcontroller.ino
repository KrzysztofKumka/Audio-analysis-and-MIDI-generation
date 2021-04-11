#include <stdlib.h>
#define ATMEGA328 1
#ifdef ATMEGA328
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#elif ATMEGA32U4
#include "MIDIUSB.h"

#endif


// BUTTONS
const int N_BUTTONS = 2;                    
const int BUTTON_ARDUINO_PIN[N_BUTTONS] = {2,3};
int buttonCState[N_BUTTONS] = {};
int buttonPState[N_BUTTONS] = {};

#define pin13 1
byte pin13index = 12;

// DEBOUNCE
unsigned long lastDebounceTime[N_BUTTONS] = {0};
unsigned long debounceDelay = 5;


// MIDI
byte midiCh = 1; //* MIDI channel to be used
byte note = 0; //* Lowest note to be used
byte cc = 1; //* Lowest MIDI CC to be used


// SETUP
void setup() {
  Serial.begin(115200);

  // BUTTONS
  for (int i = 0; i < N_BUTTONS; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

#ifdef pin13
pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
#endif
}

// LOOP
void loop() {
  buttons();
  
}


// BUTTONS
void buttons() {
  for (int i = 0; i < N_BUTTONS; i++) {
    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);

#ifdef pin13
if (i == pin13index) {
buttonCState[i] = !buttonCState[i];
}
#endif

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();
        if (buttonCState[i] == LOW) {






#ifdef ATMEGA328  
int x = 6;
int y = 4;
int time_resolution = 1000;
int midi_notation[x][y] = {{62, 65, 67, 71}, {0, 67, 0, 0}, {60, 64, 67, 0}, {64, 67, 71, 0}, {0, 0, 62, 0}, {0, 0, 60, 0}};




// PLAY MIDI
for(int i = 0; i <= x-1; i++) 
{
  for(int j = 0; j <= y-1; j++)
  {
    if (midi_notation[i][j] == 0) 
    {
      MIDI.sendNoteOn(note + midi_notation[i][j], 0, midiCh);
    }
    else if (midi_notation[i][j] > 0)
    {
      MIDI.sendNoteOn(note + midi_notation[i][j], 127, midiCh);
    }
  }
  delay(time_resolution);
  for(int k = 0; k <=y-1; k++)
    {
      MIDI.sendNoteOn(note + midi_notation[i][k], 0, midiCh);
    }
}

#endif


        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}
